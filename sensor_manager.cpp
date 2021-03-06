#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "pico/multicore.h"
#include "json.hpp"
#include "Tricorder_AS7341.h"
#include "Tricorder_BME688.h"
#include "Tricorder_LTR390.h"
#include "Tricorder_PMSA003I.h"
#include "Tricorder_TLV493D.h"
#include "Tricorder_SCD41.h"
#include "Tricorder_Sensor.h"

#define GPIO_ON 1
#define GPIO_OFF 0

#define LED_PIN 25

#define MAX_BUFFER_SIZE 256
#define MAX_INITIALIZATION_RETRIES 2
#define INITIALIZATION_RETRY_DELAY 10
	
// Sensor Array
Tricorder_AS7341 _as7341 = Tricorder_AS7341();
Tricorder_BME688 _bme688 = Tricorder_BME688();
Tricorder_LTR390 _ltr390 = Tricorder_LTR390();
Tricorder_PMSA003I _pmsa003i = Tricorder_PMSA003I();
Tricorder_TLV493D _tlv493d = Tricorder_TLV493D();
Tricorder_SCD41 _scd41 = Tricorder_SCD41();
Tricorder_Sensor *sensors[] = {
	&_as7341,
	&_bme688,
	&_ltr390,
	&_pmsa003i,
	//&_tlv493d,
	//&_scd41
};

void send_json(nlohmann::json frame){
	printf("<%s>\n", frame.dump().c_str());
}

void async_sensor_update_loop(){
	while(true){
		for(Tricorder_Sensor *sensor : sensors){
			sensor->async_update_sensor();
		}
	}
}
	

int main(){
	// Global Variables
	bool led_state = false;
	uint64_t led_stamp = time_us_64();
	//char nam_buf[MAX_BUFFER_SIZE], prop_buf[MAX_BUFFER_SIZE], val_buf[MAX_BUFFER_SIZE];
	bool read_ready = false;
	int read_state = 0;
	int read_idx = 0;
	
	char input_buffer[MAX_BUFFER_SIZE];
	
	// Basic Initialization
	stdio_init_all();
	gpio_set_function(2, GPIO_FUNC_UART);
	gpio_set_function(3, GPIO_FUNC_UART);
	uart_set_hw_flow(uart0, true, true);
	//if(watchdog_caused_reboot()){ printf("<Rebooted By Watchdog>\n"); }
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);
	
	// Initialize Sensors
	//printf("<Initializing Sensors>\n");
	for(Tricorder_Sensor *sensor : sensors){
		gpio_put(LED_PIN, GPIO_ON);
		led_state = true;
		int counter = 0;
		bool success = sensor->sensor_init();
		while(!success && counter < MAX_INITIALIZATION_RETRIES){
			sleep_ms(INITIALIZATION_RETRY_DELAY);
			counter++;
			success = sensor->sensor_init();
			//printf("<Retrying: %s>\n", sensor->sensor_name);
		}
		if(success){
			//printf("<Initialized: %s>\n", sensor->sensor_name);
		}else{
			//printf("<Failed To Initialize: %s>\n", sensor->sensor_name);
		}
		gpio_put(LED_PIN, GPIO_OFF);
		led_state = false;
		sleep_ms(10);
	}
	
	// Blinky Show
	gpio_put(LED_PIN, GPIO_OFF);
	led_state = false;
	sleep_ms(500);
	for(int i = 0; i < 20; i++){
		gpio_put(LED_PIN, led_state ? GPIO_ON : GPIO_OFF);
		led_state = !led_state;
		sleep_ms(50);
	}
	//printf("<Finished Initializing Sensors>\n");
	
	// Send Ready Message
	printf("<Sensor Manager Ready>\n");
	
	// Launch Asynchronous Sensor Reader
	multicore_launch_core1(async_sensor_update_loop);
	
	// Main Loop
	watchdog_enable(1000, 1);
	while(true){
		watchdog_update();
		//printf("Start Loop\n");
		uint64_t current_time = time_us_64();
		
		// Read Input
		while(uart_is_readable(uart0) && !read_ready){
			if(read_idx >= MAX_BUFFER_SIZE){
				read_ready = false;
				read_idx = 0;
				read_state = 0;
			}
			char tmp = uart_getc(uart0);
			switch(read_state){
				case 0:
					if(tmp == '<'){
						read_state = 1;
						read_idx = 0;
					}
					break;
				case 1:
					if(tmp == '>'){
						input_buffer[read_idx] = '\0';
						read_state = 0;
						read_idx = 0;
						read_ready = true;
					}else{
						input_buffer[read_idx] = tmp;
						read_idx++;
					}
					break;
			}
		}
		
		// Handle Input
		if(read_ready){
			//printf("\n<RECV: %s>\n\n", input_buffer);
			read_ready = false;
			auto jinp = nlohmann::json::parse(input_buffer, nullptr, false);
			if(!jinp.is_discarded()){
				const char *cmd = jinp.at("command").get<std::string>().c_str();
				if(strcmp(cmd, "list_sensors") == 0){
					//printf("<EXECUTE: List Sensors>\n");
					nlohmann::json sensor_list = nlohmann::json::array();
					for(Tricorder_Sensor *sensor : sensors){
						nlohmann::json entry = {
							{"name", sensor->sensor_name},
							{"Initialized", sensor->initialized},
							{"enabled", sensor->enabled}
						};
						sensor_list.push_back(entry);
					}
					nlohmann::json report_frame = {
						{"report", "sensor_list"},
						{"sensors", sensor_list}
					};
					send_json(report_frame);
				}else if(strcmp(cmd, "set_parameter") == 0){
					//printf("<EXECUTE: Set Parameter>\n");
					//printf("<Getting target...>\n");
					const char *target = jinp.at("sensor").get<std::string>().c_str();
					//printf("<Getting parameter...>\n");
					const char *param = jinp.at("param").get<std::string>().c_str();
					//printf("<Getting value...>\n");
					const char *val = jinp.at("value").get<std::string>().c_str();
					//printf("<Searching for sensor...>\n");
					for(Tricorder_Sensor *sensor : sensors){
						if(strcmp(target, sensor->sensor_name) == 0){
							//printf("<Setting property...>\n");
							sensor->set_property((char*)param, (char*)val);
							//printf("<Property set!>\n");
							break;
						}
					}
				}else if(strcmp(cmd, "list_options") == 0){
					//printf("<EXECUTE: List Options>\n");
					//printf("<Getting target...>\n");
					const char *target = jinp.at("sensor").get<std::string>().c_str();
					//printf("<Searching for sensor...>\n");
					for(Tricorder_Sensor *sensor : sensors){
						if(strcmp(target, sensor->sensor_name) == 0){
							//printf("<Building message...>\n");
							nlohmann::json opts_frame = sensor->report_options();
							send_json(opts_frame);
							//printf("<Message sent!>\n");
							break;
						}
					}
				}else if(strcmp(cmd, "reset") == 0){
					//printf("<EXECUTE: Reset>\n");
					printf("<Rebooting...>\n");
					while(true){ sleep_ms(10); }
				}else if(strcmp(cmd, "initialize") == 0){
					//printf("<EXECUTE: Initialize>\n");
					//printf("<Getting target...>\n");
					const char *target = jinp.at("sensor").get<std::string>().c_str();
					//printf("<Searching for sensor...>\n");
					for(Tricorder_Sensor *sensor : sensors){
						if(strcmp(target, sensor->sensor_name) == 0){
							//printf("<Initializing sensor...>\n");
							sensor->sensor_init();
							//printf("<Sensor initialized!>\n");
							break;
						}
					}
				}else if(strcmp(cmd, "enable") == 0){
					//printf("<EXECUTE: Enable>\n");
					//printf("<Getting target...>\n");
					const char *target = jinp.at("sensor").get<std::string>().c_str();
					//printf("<Searching for sensor...>\n");
					for(Tricorder_Sensor *sensor : sensors){
						if(strcmp(target, sensor->sensor_name) == 0){
							//printf("<Enabling sensor...>\n");
							sensor->set_property("enabled", "true");
							//printf("<Sensor Enabled!>\n");
							break;
						}
					}
				}else if(strcmp(cmd, "disable") == 0){
					//printf("<EXECUTE: Disable>\n");
					//printf("<Getting target...>\n");
					const char *target = jinp.at("sensor").get<std::string>().c_str();
					//printf("<Searching for sensor...>\n");
					for(Tricorder_Sensor *sensor : sensors){
						if(strcmp(target, sensor->sensor_name) == 0){
							//printf("<Disabling sensor...>\n");
							sensor->set_property("enabled", "false");
							//printf("<Sensor Disabled!>\n");
							break;
						}
					}
				}else if(strcmp(cmd, "enable_all") == 0){
					for(Tricorder_Sensor *sensor : sensors){
						sensor->set_property("enabled", "true");
					}
				}else if(strcmp(cmd, "disable_all") == 0){
					for(Tricorder_Sensor *sensor : sensors){
						sensor->set_property("enabled", "false");
					}
				}
			}
		}
		
		// Update LED
		//printf("Updating LED...\n");
		gpio_put(LED_PIN, led_state ? GPIO_ON : GPIO_OFF);
		led_state = !led_state;
		//sleep_ms(1000);
		
		// Run through sensors
		//printf("Reporting Sensors...\n");
		for(Tricorder_Sensor *sensor : sensors){
			//printf("  Updating %s...\n", sensor->sensor_name);
			//bool sensor_updated = sensor->update_data();
			//printf("  Finished updating %s\n", sensor->sensor_name);
			//if(sensor_updated){
			//	//printf("  Reporting %s...\n", sensor->sensor_name);
			//	nlohmann::json report_frame = sensor->build_json();
			//	send_json(report_frame);
			//	//printf("  Finished reporting %s\n", sensor->sensor_name);
			//}
			//sensor->async_update_sensor();
			if(sensor->async_poll_sensor()){
				send_json(sensor->async_get_frame());
			}
		}
	}
}

#include <stdio.h>
#include "pico/stdlib.h"
#include "json.hpp"
#include "Tricorder_AS7341.h"
#include "Tricorder_BME688.h"
#include "Tricorder_LTR390.h"
#include "Tricorder_PMSA003I.h"
#include "Tricorder_Sensor.h"

#define GPIO_ON 1
#define GPIO_OFF 0

#define LED_PIN 25

#define MAX_BUFFER_SIZE 1024
#define MAX_INITIALIZATION_RETRIES 2
#define INITIALIZATION_RETRY_DELAY 10

void send_json(nlohmann::json frame){
	printf("<%s>\n", frame.dump().c_str());
}

int main(){
	// Basic Initialization
	stdio_init_all();
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);
	
	// Sensor Array
	Tricorder_AS7341 as7341 = Tricorder_AS7341();
	Tricorder_BME688 bme688 = Tricorder_BME688();
	Tricorder_LTR390 ltr390 = Tricorder_LTR390();
	Tricorder_PMSA003I pmsa003i = Tricorder_PMSA003I();
	Tricorder_Sensor *sensors[] = {
		&as7341,
		&bme688,
		&ltr390,
		&pmsa003i
	};
	
	// Global Variables
	bool led_state = false;
	uint64_t led_stamp = time_us_64();
	//char nam_buf[MAX_BUFFER_SIZE], prop_buf[MAX_BUFFER_SIZE], val_buf[MAX_BUFFER_SIZE];
	bool read_ready = false;
	int read_state = 0;
	int read_idx = 0;
	
	char input_buffer[MAX_BUFFER_SIZE];
	
	// Initialize Sensors
	printf("<Initializing Sensors>\n");
	for(Tricorder_Sensor *sensor : sensors){
		int counter = 0;
		bool success = sensor->sensor_init();
		while(!success && counter < MAX_INITIALIZATION_RETRIES){
			sleep_ms(INITIALIZATION_RETRY_DELAY);
			counter++;
			success = sensor->sensor_init();
			printf("<Retrying: %s>\n", sensor->sensor_name);
		}
		if(success){
			printf("<Initialized: %s>\n", sensor->sensor_name);
		}else{
			printf("<Failed To Initialize: %s>\n", sensor->sensor_name);
		}
	}
	printf("<Finished Initializing Sensors>\n");
	
	while(true){
		//printf("Start Loop\n");
		uint64_t current_time = time_us_64();
		
		// Read Input
		if(uart_is_readable(uart0)){
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
			read_ready = false;
			auto jinp = nlohmann::json::parse(input_buffer);
			const char *cmd = jinp.at("command").get<std::string>().c_str();
			if(strcmp(cmd, "list_sensors") == 0){
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
				const char *target = jinp.at("sensor").get<std::string>().c_str();
				const char *param = jinp.at("param").get<std::string>().c_str();
				const char *val = jinp.at("value").get<std::string>().c_str();
				for(Tricorder_Sensor *sensor : sensors){
					if(strcmp(target, sensor->sensor_name) == 0){
						sensor->set_property((char*)param, (char*)val);
						break;
					}
				}
			}else if(strcmp(cmd, "list_options") == 0){
				const char *target = jinp.at("sensor").get<std::string>().c_str();
				for(Tricorder_Sensor *sensor : sensors){
					if(strcmp(target, sensor->sensor_name) == 0){
						nlohmann::json opts_frame = sensor->report_options();
						send_json(opts_frame);
						break;
					}
				}
			}
		}
		
		// Handle input
		//printf("Reading Input..\n");
		/*if(uart_is_readable(uart0)){
			if(read_idx >= MAX_BUFFER_SIZE){
				read_ready = false;
				read_idx = 0;
				read_state = 0;
			}
			//printf("  Grabbing char...\n");
			char tmp = uart_getc(uart0);
			//printf("  Finished grabbing char.\n");
			
			switch(read_state){
				case 0:
					if(tmp == '<'){
						read_state = 1;
						read_idx = 0;
					}
					break;
				case 1:
					if(tmp == ':'){
						nam_buf[read_idx] = '\0';
						read_state = 2;
						read_idx = 0;
					}else{
						nam_buf[read_idx] = tmp;
						read_idx++;
					}
					break;
				case 2:
					if(tmp == '='){
						prop_buf[read_idx] = '\0';
						read_state = 3;
						read_idx = 0;
					}else{
						prop_buf[read_idx] = tmp;
						read_idx++;
					}
					break;
				case 3:
					if(tmp == '>'){
						val_buf[read_idx] = '\0';
						read_state = 0;
						read_idx = 0;
						read_ready = true;
					}else{
						val_buf[read_idx] = tmp;
						read_idx++;
					}
					break;
			}
		}
		
		// Handle Commands
		//printf("Handling Input..\n");
		if(read_ready){
			//printf("Name: %s\nProperty: %s\nValue: %s\n", nam_buf, prop_buf, val_buf);
			bool hit = false;
			if(strcmp(nam_buf, "CONTROL") == 0){
				if(strcmp(prop_buf, "SENSORS") == 0){
					if(strcmp(val_buf, "LIST") == 0){
						nlohmann::json sensor_list = nlohmann::json::array();
						for(Tricorder_Sensor *sensor : sensors){
							sensor_list.push_back(sensor->sensor_name);
						}
						nlohmann::json frame = {
							{"name", "SENSORS"},
							{"sensors", sensor_list}
						};
						send_json(frame);
						hit = true;
					}
				}else if(strcmp(val_buf, "OPTS") == 0){
					for(Tricorder_Sensor *sensor : sensors){
						if(strcmp(prop_buf, sensor->sensor_name) == 0){
							nlohmann::json opts_frame = sensor->report_options();
							send_json(opts_frame);
							hit = true;
							break;
						}
					}
				}
			}else{
				for(Tricorder_Sensor *sensor : sensors){
					if(strcmp(nam_buf, sensor->sensor_name) == 0){
						//printf("  Setting property...\n");
						sensor->set_property(prop_buf, val_buf);
						//printf("  Finished setting property.\n");
						//Serial.println("Property Set");
						hit = true;
						break;
					}
				}
			}
			read_ready = false;
		}*/
		
		// Update LED
		//printf("Updating LED...\n");
		gpio_put(LED_PIN, led_state ? GPIO_ON : GPIO_OFF);
		led_state = !led_state;
		//sleep_ms(1000);
		
		// Run through sensors
		//printf("Reporting Sensors...\n");
		for(Tricorder_Sensor *sensor : sensors){
			//printf("  Updating %s...\n", sensor->sensor_name);
			bool sensor_updated = sensor->update_data();
			//printf("  Finished updating %s\n", sensor->sensor_name);
			if(sensor_updated){
				//printf("  Reporting %s...\n", sensor->sensor_name);
				nlohmann::json report_frame = sensor->build_json();
				send_json(report_frame);
				//printf("  Finished reporting %s\n", sensor->sensor_name);
			}
		}
	}
}

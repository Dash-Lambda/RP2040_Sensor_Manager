#include "Tricorder_PMSA003I.h"

Tricorder_PMSA003I::Tricorder_PMSA003I(){
	sensor_name = "PMSA003I";
}

bool Tricorder_PMSA003I::sensor_init(){
	sensor_obj = Adafruit_PM25AQI();
	initialized = sensor_obj.begin_I2C();
	
	if(initialized){
		report_rate = 1000;
	}
	
	return initialized;
}

bool Tricorder_PMSA003I::set_property(char *nam, char *val){
	if(strcmp(nam, "enabled") == 0){
		if(strcmp(val, "true") == 0){
			enabled = true;
		}else if(strcmp(val, "false") == 0){
			enabled = false;
		}
	}else if(strcmp(nam, "report_rate") == 0){
		report_rate = atoi(val);
	}
	
	return true;
}

bool Tricorder_PMSA003I::read_sensor(){
	unsigned long cur_time = to_us_since_boot(get_absolute_time());
	if(cur_time - timestamp > 1000*report_rate){
		return sensor_obj.read(&current_data);
	}else{
		return false;
	}
}

nlohmann::json Tricorder_PMSA003I::populate_data(){
	nlohmann::json data_frame = {
		{"0.3um", current_data.particles_03um},
		{"0.5um", current_data.particles_05um},
		{"1.0um", current_data.particles_10um},
		{"2.5um", current_data.particles_25um},
		{"5.0um", current_data.particles_50um},
		{"10um", current_data.particles_100um},
	};
	
	return data_frame;
}

nlohmann::json Tricorder_PMSA003I::populate_options(){
	nlohmann::json opts_frame = {"enabled"};
	
	return opts_frame;
}

nlohmann::json Tricorder_PMSA003I::query_state(char *opnam){
	nlohmann::json frame;
	if(strcmp(opnam, "enabled") == 0){
		frame["state"] = enabled;
	}
	
	return frame;
}

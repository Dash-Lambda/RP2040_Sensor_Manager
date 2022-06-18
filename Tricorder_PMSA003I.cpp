#include "Tricorder_PMSA003I.h"

Tricorder_PMSA003I::Tricorder_PMSA003I(){
	sensor_name = "PMSA003I";
	
	sensor_options.push_back(sensor_option(
		"enabled",
		OP_Boolean,
		[&](nlohmann::json val)->bool{			
			enabled = val.get<bool>();
			return true;
		},
		[&]()->nlohmann::json{ return enabled; }
	));
	sensor_options.push_back(sensor_option(
		"report_rate",
		OP_Double,
		[&](nlohmann::json val)->bool{
			report_rate = val.get<double>();
			return true;
		},
		[&]()->nlohmann::json{
			return report_rate; }
	));
}

bool Tricorder_PMSA003I::sensor_init(){
	sensor_obj = Adafruit_PM25AQI();
	initialized = sensor_obj.begin_I2C();
	
	if(initialized){
		report_rate = 1000;
	}
	
	return initialized;
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

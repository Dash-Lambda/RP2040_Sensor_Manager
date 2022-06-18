#include "Tricorder_LTR390.h"

Tricorder_LTR390::Tricorder_LTR390(){
	sensor_name = "LTR390";
}

bool Tricorder_LTR390::sensor_init(){
	sensor_obj = Adafruit_LTR390();
	initialized = sensor_obj.begin();
	int counter = 0;
	while(!initialized && counter < 10){
		initialized = sensor_obj.begin();
		counter++;
		sleep_ms(10);
	}

	// Defaults
	if(initialized){
		cur_mode = LTR390_MODE_UVS;
		sensor_obj.setMode(LTR390_MODE_UVS);
		sensor_obj.setGain(LTR390_GAIN_3);
		sensor_obj.setResolution(LTR390_RESOLUTION_16BIT);
		sensor_obj.setThresholds(100, 1000);
		sensor_obj.configInterrupt(true, LTR390_MODE_UVS);
	}
	
	return initialized;
}

bool Tricorder_LTR390::set_property(char *nam, char *val){
	if(strcmp(nam, "enabled") == 0){
		if(strcmp(val, "true") == 0){
			sensor_obj.enable(true);
			enabled = true;
		}else if(strcmp(val, "false") == 0){
			sensor_obj.enable(false);
			enabled = false;
		}
	}else if(strcmp(nam, "reset") == 0){
		if(strcmp(val, "true") == 0){
			sensor_obj.reset();
		}
	}else if(strcmp(nam, "mode") == 0){
		if(strcmp(val, "ALS") == 0){
			cur_mode = LTR390_MODE_ALS;
		}else if(strcmp(val, "UVS") == 0){
			cur_mode = LTR390_MODE_UVS;
		}
		sensor_obj.setMode(cur_mode);
		data_updated = false;
	}else if(strcmp(nam, "gain") == 0){
		ltr390_gain_t gain = static_cast<ltr390_gain_t>(atoi(val));
		sensor_obj.setGain(gain);
	}else if(strcmp(nam, "res") == 0){
		ltr390_resolution_t res = static_cast<ltr390_resolution_t>(atoi(val));
		sensor_obj.setResolution(res);
	}
	
	return true;
}

bool Tricorder_LTR390::read_sensor(){
	if(sensor_obj.newDataAvailable()){
		switch(cur_mode){
			case LTR390_MODE_ALS:
				cur_data = sensor_obj.readALS();
				break;
			case LTR390_MODE_UVS:
				cur_data = sensor_obj.readUVS();
				break;
		}
		return true;
	}
	return false;
}

nlohmann::json Tricorder_LTR390::populate_data(){
	nlohmann::json data_ltr = nlohmann::json::object();
	switch(cur_mode){
		case LTR390_MODE_ALS:
			data_ltr["ALS"] = cur_data;
			break;
		case LTR390_MODE_UVS:
			data_ltr["UVS"] = cur_data;
			break;
	}
	
	return data_ltr;
}

nlohmann::json Tricorder_LTR390::populate_options(){
	nlohmann::json opts_frame = {"enabled"};
	return opts_frame;
}

nlohmann::json Tricorder_LTR390::query_state(char *opnam){
	nlohmann::json frame = nlohmann::json::object();
	if(strcmp(opnam, "enabled") == 0){
		frame["state"] = enabled;
	}
	
	return frame;
}

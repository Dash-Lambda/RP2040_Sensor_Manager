#include "Tricorder_LTR390.h"

Tricorder_LTR390::Tricorder_LTR390(){
	sensor_name = "LTR390";
	
	sensor_options.push_back(sensor_option(
		"enabled",
		OP_Boolean,
		[&](nlohmann::json val)->bool{			
			enabled = val.get<bool>();
			sensor_obj.enable(enabled);
			return true;
		},
		[&]()->nlohmann::json{ return enabled; }
	));
	
	sensor_options.push_back(sensor_option(
		"reset",
		OP_Boolean,
		[&](nlohmann::json val)->bool{
			if(val.get<bool>()){ sensor_obj.reset(); }
			return true;
		},
		[&]()->nlohmann::json{ return "unreadable"; }
	));
	
	sensor_options.push_back(sensor_option(
		"mode",
		OP_String,
		[&](nlohmann::json val)->bool{
			const char *tmp = val.get<std::string>().c_str();
			if(strcmp(tmp, "ALS") == 0){
				cur_mode = LTR390_MODE_ALS;
			}else if(strcmp(tmp, "UVS") == 0){
				cur_mode = LTR390_MODE_UVS;
			}else{
				return false;
			}
			sensor_obj.setMode(cur_mode);
			return true;
		},
		[&]()->nlohmann::json{
			switch(cur_mode){
				case LTR390_MODE_ALS: return "ALS"; break;
				case LTR390_MODE_UVS: return "UVS"; break;
			}
		}
	));
	
	sensor_options.push_back(sensor_option(
		"gain",
		OP_Integer,
		[&](nlohmann::json val)->bool{
			ltr390_gain_t gain = static_cast<ltr390_gain_t>(val.get<int>());
			sensor_obj.setGain(gain);
			return true;
		},
		[&]()->nlohmann::json{ return "unimplemented"; }
	));
	
	sensor_options.push_back(sensor_option(
		"resolution",
		OP_Integer,
		[&](nlohmann::json val)->bool{
			ltr390_resolution_t res = static_cast<ltr390_resolution_t>(val.get<int>());
			sensor_obj.setResolution(res);
			return true;
		},
		[&]()->nlohmann::json{ return "unimplemented"; }
	));
}

bool Tricorder_LTR390::sensor_init(){
	sensor_obj = Adafruit_LTR390();
	initialized = sensor_obj.begin();

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
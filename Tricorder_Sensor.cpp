#include "Tricorder_Sensor.h"

Tricorder_Sensor::Tricorder_Sensor(){
	enabled = false;
	data_updated = false;
	error_state = false;
	timestamp = 0;
	report_rate = 0;
	
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

bool Tricorder_Sensor::sensor_init(){return false;}

bool Tricorder_Sensor::read_sensor(){return false;}

nlohmann::json Tricorder_Sensor::populate_data(){return nlohmann::json();}

bool Tricorder_Sensor::set_property(char *nam, char *val){
	for(sensor_option op : sensor_options){
		if(strcmp(nam, op.op_name) == 0){
			op.set(val);
			break;
		}
	}
	
	return true;
}

nlohmann::json Tricorder_Sensor::build_json(){
	nlohmann::json data_frame = populate_data();
	nlohmann::json report_frame = {
		{"rep", "DATA"},
		{"time", timestamp},
		{"sensor", sensor_name},
		{"data", data_frame}
	};
	
	return report_frame;
}

nlohmann::json Tricorder_Sensor::populate_options(){
	nlohmann::json opt_frame = nlohmann::json::array();
	for(sensor_option op : sensor_options){
		opt_frame.push_back(op.op_name);
	}
	return opt_frame;
}

nlohmann::json Tricorder_Sensor::report_options(){
	nlohmann::json ops_frame = populate_options();
	nlohmann::json report_frame = {
		{"rep", "OPTIONS"},
		{"name", sensor_name},
		{"opts", ops_frame}
	};
	
	return report_frame;
}

nlohmann::json Tricorder_Sensor::query_state(char *opnam){
	nlohmann::json frame;
	for(sensor_option op : sensor_options){
		if(strcmp(opnam, op.op_name) == 0){
			frame["state"] = op.get();
			break;
		}
	}
	return frame;
}

nlohmann::json Tricorder_Sensor::report_state(char *opnam){
	nlohmann::json state_frame = query_state(opnam);
	nlohmann::json report_frame = {
		{"rep", "STATE"},
		{"sensor", sensor_name},
		{"report", state_frame}
	};
	
	return report_frame;
}

bool Tricorder_Sensor::update_data(){
	unsigned long cur_time = to_us_since_boot(get_absolute_time());
	if(enabled && !error_state && read_sensor() && cur_time - timestamp > 1000*report_rate){
		timestamp = to_us_since_boot(get_absolute_time());
		data_updated = true;
		return true;
	}else{
		data_updated = false;
		return false;
	}
}

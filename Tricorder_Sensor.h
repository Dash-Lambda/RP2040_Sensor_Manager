#ifndef Tricorder_Sensor_h
#define Tricorder_Sensor_h

#include "pico/stdlib.h"
//#include <functional>
#include "json.hpp"

enum OptionType {OP_Boolean, OP_Integer, OP_Double, OP_String};

typedef struct sensor_option{
	char *op_name;
	OptionType op_type;
	nlohmann::json default_value;
	std::function<bool(nlohmann::json)> setter;
	std::function<nlohmann::json()> getter;
	
	bool set(char *val){
		nlohmann::json jval;
		switch(op_type){
			case OP_Boolean: {
				if(strcmp(val, "true") == 0){
					bool tmp = true;
					jval = tmp;
				}else if(strcmp(val, "false") == 0){
					bool tmp = false;
					jval = tmp;
				}else{
					return false;
				}
				break;
			}
			case OP_Integer: {
				char *chk;
				long tmp = strtol(val, &chk, 10);
				if(chk){ jval = tmp; }
				else{ return false; }
				break;
			}
			case OP_Double: {
				char *chk;
				double tmp = strtod(val, &chk);
				if(chk){ jval = tmp; }
				else{ return false; }
				break;
			}
			case OP_String: {
				jval = val;
				break;
			}
		}
		return setter(jval);
	}
	nlohmann::json get(){
		return getter();
	}
	
	sensor_option(char *inName, OptionType inType, std::function<bool(nlohmann::json)> inSet, std::function<nlohmann::json()> inGet){
		op_name = inName;
		op_type = inType;
		setter = inSet;
		getter = inGet;
	}
} sensor_option;

class Tricorder_Sensor
{
	public:
		Tricorder_Sensor();

		// Variables
		//String sensor_name;
		char *sensor_name;
		unsigned long timestamp;
		bool initialized;
		bool enabled;
		bool data_updated;
		bool error_state;

		// External Use
		virtual bool sensor_init();
		//virtual void enable();
		//virtual void disable();
		bool set_property(char *nam, char *val);
		nlohmann::json build_json();
		nlohmann::json report_options();
		nlohmann::json report_state(char *opnam);
		bool update_data();

		// Internal Use
		nlohmann::json populate_options();
		virtual nlohmann::json populate_data();
		nlohmann::json query_state(char *opnam);
		virtual bool read_sensor();
	protected:
		std::vector<sensor_option> sensor_options;
		double report_rate;
};
#endif

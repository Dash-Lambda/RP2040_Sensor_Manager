#include "Tricorder_SCD41.h"

Tricorder_SCD41::Tricorder_SCD41(){
	sensor_name = "SCD41";
	
	sensor_options.push_back(sensor_option(
		"enabled",
		OP_Boolean,
		[&](nlohmann::json val)->bool{			
			enabled = val.get<bool>();
			if(enabled){
				sensor_obj.startPeriodicMeasurement();
			}else{
				sensor_obj.stopPeriodicMeasurement();
			}
			return true;
		},
		[&]()->nlohmann::json{ return enabled; }
	));
}

bool Tricorder_SCD41::sensor_init(){
	//sensor_obj = SensirionI2CScd4x();
	sensor_obj.begin(Wire);
	uint16_t err = sensor_obj.stopPeriodicMeasurement();
	initialized = err != 0;
	
	return initialized;
}

bool Tricorder_SCD41::read_sensor(){
	uint16_t dataReadyRaw = 0;
	sensor_obj.getDataReadyStatus(dataReadyRaw);
	bool dataReady = (dataReadyRaw & 0x07FF) != 0;
	if(dataReady){
		sensor_obj.readMeasurement(co2, temperature, humidity);
	}
	return dataReady;
}

nlohmann::json Tricorder_SCD41::populate_data(){
	nlohmann::json data_frame = {
		{"CO2", co2},
		{"Temperature", temperature},
		{"Humidity", humidity}
	};
	
	return data_frame;
}

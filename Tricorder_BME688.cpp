#include "Tricorder_BME688.h"

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Tricorder_BME688::Tricorder_BME688(){
	sensor_name = "BME688";
	read_after = 0;
	heaterTemp = 0;
	heaterTime = 0;
}

bool Tricorder_BME688::sensor_init(){
	sensor_obj = Adafruit_BME680();
	initialized = sensor_obj.begin();
	
	// Set up oversampling and filter initialization
	if(initialized){
		sensor_obj.setTemperatureOversampling(BME680_OS_8X);
		sensor_obj.setHumidityOversampling(BME680_OS_2X);
		sensor_obj.setPressureOversampling(BME680_OS_4X);
		sensor_obj.setIIRFilterSize(BME680_FILTER_SIZE_3);
		sensor_obj.setGasHeater(320, 150); // 320*C for 150 ms
	}
	
	return initialized;
}

bool Tricorder_BME688::read_sensor(){
	if(read_after == 0){
		read_after = sensor_obj.beginReading();
	}else if(millis() > read_after){
		if(sensor_obj.endReading()){
			read_after = sensor_obj.beginReading();
			return true;
		}
	}
	return false;
}

bool Tricorder_BME688::set_property(char *nam, char *val){
	if(strcmp(nam, "enabled") == 0){
		if(strcmp(val, "true") == 0){
			enabled = true;
		}else if(strcmp(val, "false") == 0){
			enabled = false;
		}
	}else if(strcmp(nam, "TempOS") == 0){
		sensor_obj.setTemperatureOversampling(atoi(val));
	}else if(strcmp(nam, "PresOS") == 0){
		sensor_obj.setPressureOversampling(atoi(val));
	}else if(strcmp(nam, "HumOS") == 0){
		sensor_obj.setHumidityOversampling(atoi(val));
	}else if(strcmp(nam, "IIRFS") == 0){
		sensor_obj.setIIRFilterSize(atoi(val));
	}else if(strcmp(nam, "HeatTemp") == 0){
		heaterTemp = atoi(val);
	}else if(strcmp(nam, "HeatTime") == 0){
		heaterTime = atoi(val);
	}else if(strcmp(nam, "RunHeat") == 0){
		sensor_obj.setGasHeater(heaterTemp, heaterTime);
	}else if(strcmp(nam, "ODR") == 0){
		sensor_obj.setODR(atoi(val));
	}
	
	return true;
}

nlohmann::json Tricorder_BME688::populate_data(){
	nlohmann::json data_frame = {
		{"temperature", sensor_obj.temperature},
		{"pressure", sensor_obj.pressure/100.0},
		{"humidity", sensor_obj.humidity},
		{"gas", sensor_obj.gas_resistance/1000.0},
		//{"altitude", sensor_obj.readAltitude(SEALEVELPRESSURE_HPA)}
	};
	
	return data_frame;
}

nlohmann::json Tricorder_BME688::populate_options(){
	nlohmann::json opts_frame = {"enabled", "TempOS", "PresOS", "HumOS", "IIRFS", "HeatTemp", "HeatTime", "RunHeat", "ODR"};
	return opts_frame;
}

nlohmann::json Tricorder_BME688::query_state(char *opnam){
	nlohmann::json frame;
	if(strcmp(opnam, "enabled") == 0){
		frame["state"] = enabled;
	}
	
	return frame;
}

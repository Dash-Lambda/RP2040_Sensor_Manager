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
		"tempOverSampling",
		OP_Integer,
		[&](nlohmann::json val)->bool{			
			sensor_obj.setTemperatureOversampling(val.get<long>());
			return true;
		},
		[&]()->nlohmann::json{ return "unimplemented"; }
	));
	
	sensor_options.push_back(sensor_option(
		"pressureOverSampling",
		OP_Integer,
		[&](nlohmann::json val)->bool{			
			sensor_obj.setPressureOversampling(val.get<long>());
			return true;
		},
		[&]()->nlohmann::json{ return "unimplemented"; }
	));
	
	sensor_options.push_back(sensor_option(
		"humidityOverSampling",
		OP_Integer,
		[&](nlohmann::json val)->bool{			
			sensor_obj.setHumidityOversampling(val.get<long>());
			return true;
		},
		[&]()->nlohmann::json{ return "unimplemented"; }
	));
	
	sensor_options.push_back(sensor_option(
		"IIRFilterSize",
		OP_Integer,
		[&](nlohmann::json val)->bool{			
			sensor_obj.setIIRFilterSize(val.get<long>());
			return true;
		},
		[&]()->nlohmann::json{ return "unimplemented"; }
	));
	
	sensor_options.push_back(sensor_option(
		"heaterTemp",
		OP_Integer,
		[&](nlohmann::json val)->bool{			
			heaterTemp = val.get<long>();
			return true;
		},
		[&]()->nlohmann::json{ return "unimplemented"; }
	));
	
	sensor_options.push_back(sensor_option(
		"heaterTime",
		OP_Integer,
		[&](nlohmann::json val)->bool{			
			heaterTime = val.get<long>();
			return true;
		},
		[&]()->nlohmann::json{ return "unimplemented"; }
	));
	
	sensor_options.push_back(sensor_option(
		"runHeat",
		OP_Boolean,
		[&](nlohmann::json val)->bool{			
			if(val.get<bool>()){ sensor_obj.setGasHeater(heaterTemp, heaterTime); }
			return true;
		},
		[&]()->nlohmann::json{ return "unreadable"; }
	));
	
	sensor_options.push_back(sensor_option(
		"ODR",
		OP_Integer,
		[&](nlohmann::json val)->bool{			
			sensor_obj.setODR(val.get<long>());
			return true;
		},
		[&]()->nlohmann::json{ return "unimplemented"; }
	));
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
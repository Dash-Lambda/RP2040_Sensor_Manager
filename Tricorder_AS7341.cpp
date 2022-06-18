#include "Tricorder_AS7341.h"

Tricorder_AS7341::Tricorder_AS7341(){
	sensor_name = "AS7341";
	sensor_reading = false;
	spectral_enabled = false;
	led_enabled = false;
	
	sensor_options.push_back(sensor_option(
		"enabled",
		OP_Boolean,
		[&](nlohmann::json val)->bool{			
			enabled = val.get<bool>();
			sensor_obj.powerEnable(enabled);
			return true;
		},
		[&]()->nlohmann::json{ return enabled; }
	));
	
	sensor_options.push_back(sensor_option(
		"ASTEP",
		OP_Integer,
		[&](nlohmann::json val)->bool{			
			sensor_obj.setASTEP(val.get<int>());
			return true;
		},
		[&]()->nlohmann::json{ return "unimplemented"; }
	));
	
	sensor_options.push_back(sensor_option(
		"ATIME",
		OP_Integer,
		[&](nlohmann::json val)->bool{			
			sensor_obj.setATIME(val.get<int>());
			return true;
		},
		[&]()->nlohmann::json{ return "unimplemented"; }
	));
	
	sensor_options.push_back(sensor_option(
		"Gain",
		OP_Integer,
		[&](nlohmann::json val)->bool{			
			as7341_gain_t gain = static_cast<as7341_gain_t>(val.get<int>());
			sensor_obj.setGain(gain);
			return true;
		},
		[&]()->nlohmann::json{ return "unimplemented"; }
	));
	
	sensor_options.push_back(sensor_option(
		"SpectralMeasurement",
		OP_Boolean,
		[&](nlohmann::json val)->bool{			
			spectral_enabled = val.get<bool>();
			sensor_obj.enableSpectralMeasurement(spectral_enabled);
			return true;
		},
		[&]()->nlohmann::json{ return "unimplemented"; }
	));
	
	sensor_options.push_back(sensor_option(
		"LED",
		OP_Boolean,
		[&](nlohmann::json val)->bool{			
			led_enabled = val.get<bool>();
			sensor_obj.enableLED(led_enabled);
			return true;
		},
		[&]()->nlohmann::json{ return "unimplemented"; }
	));
	
	sensor_options.push_back(sensor_option(
		"LEDCurrent",
		OP_Integer,
		[&](nlohmann::json val)->bool{			
			sensor_obj.setLEDCurrent(val.get<int>());
			return true;
		},
		[&]()->nlohmann::json{ return "unimplemented"; }
	));
}

bool Tricorder_AS7341::sensor_init(){
	sensor_obj = Adafruit_AS7341();
	initialized = sensor_obj.begin();
	
	// Defaults
	if(initialized){
		sensor_obj.setATIME(100);
		sensor_obj.setASTEP(999);
		sensor_obj.setGain(AS7341_GAIN_256X);
		sensor_obj.enableSpectralMeasurement(false);
		spectral_enabled = false;
		sensor_obj.enableLED(false);
		led_enabled = false;
	}
	
	return initialized;
}

bool Tricorder_AS7341::read_sensor(){
	if(!sensor_reading){
		sensor_obj.startReading();
		sensor_reading = true;
	}
	if(sensor_obj.checkReadingProgress()){
		if(sensor_obj.getAllChannels(readings)){
			spectral_channels[0] = readings[0];
			spectral_channels[1] = readings[1];
			spectral_channels[2] = readings[2];
			spectral_channels[3] = readings[3];
			spectral_channels[4] = readings[6];
			spectral_channels[5] = readings[7];
			spectral_channels[6] = readings[8];
			spectral_channels[7] = readings[9];
			spectral_channels[8] = readings[10];
			spectral_channels[9] = readings[11];
			
			sensor_obj.startReading();
			return true;
		}
	}
	return false;
}

nlohmann::json Tricorder_AS7341::populate_data(){
	nlohmann::json data_frame = {
		{"415", spectral_channels[0]},
		{"445", spectral_channels[1]},
		{"480", spectral_channels[2]},
		{"515", spectral_channels[3]},
		{"555", spectral_channels[4]},
		{"590", spectral_channels[5]},
		{"630", spectral_channels[6]},
		{"680", spectral_channels[7]},
		{"CLR", spectral_channels[8]},
		{"NIR", spectral_channels[9]}
	};
	
	return data_frame;
}
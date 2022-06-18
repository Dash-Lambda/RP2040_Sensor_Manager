#include "Tricorder_AS7341.h"

Tricorder_AS7341::Tricorder_AS7341(){
	sensor_name = "AS7341";
	sensor_reading = false;
	spectral_enabled = false;
	led_enabled = false;
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
	
	//set_property("enabled", "true");
	//set_property("SpecMeas", "true");
	
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

bool Tricorder_AS7341::set_property(char *nam, char *val){
	if(strcmp(nam, "enabled") == 0){
		if(strcmp(val, "true") == 0){
			sensor_obj.powerEnable(true);
			enabled = true;
			//if(sensor_obj.powerEnable()){
				//sensor_obj.setATIME(100);
				//sensor_obj.setASTEP(100);
				//sensor_obj.setGain(AS7341_GAIN_256X);
				//error_state = false;
				//enabled = true;
			//}else{
				//error_state = true;
				//enabled = false;
			//}
		}else if(strcmp(val, "false") == 0){
			sensor_obj.powerEnable(false);
			enabled = false;
		}
	}else if(strcmp(nam, "ASTEP") == 0){
		sensor_obj.setASTEP(atoi(val));
	}else if(strcmp(nam, "ATIME") == 0){
		sensor_obj.setATIME(atoi(val));
	}else if(strcmp(nam, "Gain") == 0){
		as7341_gain_t gain = static_cast<as7341_gain_t>(atoi(val));
		sensor_obj.setGain(gain);
	}else if(strcmp(nam, "SpecMeas") == 0){
		spectral_enabled = strcmp(val, "true") == 0;
		sensor_obj.enableSpectralMeasurement(spectral_enabled);
	}else if(strcmp(nam, "FlckrDet") == 0){
		//sensor_obj.enableFlickerDetection(strcmp(val, "true") == 0);
	}else if(strcmp(nam, "HThresh") == 0){
		sensor_obj.setHighThreshold(atoi(val));
	}else if(strcmp(nam, "LThresh") == 0){
		sensor_obj.setLowThreshold(atoi(val));
	}else if(strcmp(nam, "SpecInt") == 0){
		sensor_obj.enableSpectralInterrupt(strcmp(val, "true") == 0);
	}else if(strcmp(nam, "SysInt") == 0){
		sensor_obj.enableSystemInterrupt(strcmp(val, "true") == 0);
	}else if(strcmp(nam, "APERS") == 0){
		as7341_int_cycle_count_t count = static_cast<as7341_int_cycle_count_t>(atoi(val));
		sensor_obj.setAPERS(count);
	}else if(strcmp(nam, "SpecThrC") == 0){
		as7341_adc_channel_t channel = static_cast<as7341_adc_channel_t>(atoi(val));
		sensor_obj.setSpectralThresholdChannel(channel);
	}else if(strcmp(nam, "LED") == 0){
		led_enabled = strcmp(val, "true") == 0;
		sensor_obj.enableLED(led_enabled);
	}else if(strcmp(nam, "LEDCur") == 0){
		sensor_obj.setLEDCurrent(atoi(val));
	}
	return true;
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

nlohmann::json Tricorder_AS7341::populate_options(){
	nlohmann::json state_frame = {"enabled", "ASTEP", "ATIME", "Gain", "SpecMeas", "LED", "LEDCur"};
	return state_frame;
}

nlohmann::json Tricorder_AS7341::query_state(char *opnam){
	nlohmann::json frame;
	if(strcmp(opnam, "enabled") == 0){
		frame["state"] = enabled;
	}else if(strcmp(opnam, "ASTEP") == 0){
		frame["state"] = sensor_obj.getASTEP();
	}else if(strcmp(opnam, "ATIME") == 0){
		frame["state"] = sensor_obj.getATIME();
	}else if(strcmp(opnam, "Gain") == 0){
		frame["state"] = sensor_obj.getGain();
	}else if(strcmp(opnam, "SpecMeas") == 0){
		frame["state"] = spectral_enabled;
	}else if(strcmp(opnam, "LED") == 0){
		frame["state"] = led_enabled;
	}else if(strcmp(opnam, "LEDCur") == 0){
		frame["state"] = "unreadable";
	}
	return frame;
}

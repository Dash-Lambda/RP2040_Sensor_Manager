#ifndef Tricorder_AS7341_h
#define Tricorder_AS7341_h

#include "pico/stdlib.h"
#include "Adafruit_AS7341.h"
#include "Tricorder_Sensor.h"

class Tricorder_AS7341 : public Tricorder_Sensor
{
	public:
		Tricorder_AS7341();
		bool sensor_init();
		bool set_property(char *nam, char *val);
		bool read_sensor();
		nlohmann::json populate_data();
		nlohmann::json populate_options();
		nlohmann::json populate_state();
		nlohmann::json query_state(char *opnam);
	private:
		Adafruit_AS7341 sensor_obj;
		uint16_t readings[12];
		uint16_t spectral_channels[10];
		bool sensor_reading;
		bool spectral_enabled;
		bool led_enabled;
};
#endif

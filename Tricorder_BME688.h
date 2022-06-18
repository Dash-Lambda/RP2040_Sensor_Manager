#ifndef Tricorder_BME688_h
#define Tricorder_BME688_h

#include "pico/stdlib.h"
//#include <Wire.h>
//#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include "Tricorder_Sensor.h"

class Tricorder_BME688 : public Tricorder_Sensor
{
	public:
		Tricorder_BME688();
		bool sensor_init();
		bool set_property(char *nam, char *val);
		bool read_sensor();
		nlohmann::json populate_data();
		nlohmann::json populate_options();
		nlohmann::json populate_state();
		nlohmann::json query_state(char *opnam);
	private:
		Adafruit_BME680 sensor_obj;
		unsigned long read_after;
		uint16_t heaterTemp, heaterTime;
};
#endif

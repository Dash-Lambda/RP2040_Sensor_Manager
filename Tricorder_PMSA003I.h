#ifndef Tricorder_PMSA003I_h
#define Tricorder_PMSA003I_h

#include "pico/stdlib.h"
#include "Adafruit_PM25AQI.h"
#include "Tricorder_Sensor.h"

class Tricorder_PMSA003I : public Tricorder_Sensor
{
	public:
		Tricorder_PMSA003I();
		bool sensor_init();
		bool read_sensor();
		nlohmann::json populate_data();
		nlohmann::json populate_state();
	private:
		Adafruit_PM25AQI sensor_obj;
		PM25_AQI_Data current_data;
};
#endif

#ifndef Tricorder_LTR390_h
#define Tricorder_LTR390_h

#include "pico/stdlib.h"
#include "Adafruit_LTR390.h"
#include "Tricorder_Sensor.h"

class Tricorder_LTR390 : public Tricorder_Sensor
{
	public:
		Tricorder_LTR390();
		bool sensor_init();
		bool read_sensor();
		nlohmann::json populate_data();
		nlohmann::json populate_state();
	private:
		Adafruit_LTR390 sensor_obj;
		uint32_t cur_data;
		ltr390_mode_t cur_mode;
};
#endif

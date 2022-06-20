#ifndef Tricorder_SCD41_h
#define Tricorder_SCD41_h

#include "pico/stdlib.h"
#include "SensirionI2CScd4x.h"
#include "Tricorder_Sensor.h"

class Tricorder_SCD41 : public Tricorder_Sensor
{
	public:
		Tricorder_SCD41();
		bool sensor_init();
		bool read_sensor();
		nlohmann::json populate_data();
		nlohmann::json populate_state();
	private:
		SensirionI2CScd4x sensor_obj;
		uint16_t co2;
		float temperature;
		float humidity;
};
#endif

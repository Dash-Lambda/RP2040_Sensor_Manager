#ifndef Tricorder_TLV493D_h
#define Tricorder_TLV493D_h

#include "pico/stdlib.h"
#include "Tlv493d.h"
#include "Tricorder_Sensor.h"

class Tricorder_TLV493D : public Tricorder_Sensor
{
	public:
		Tricorder_TLV493D();
		bool sensor_init();
		bool read_sensor();
		nlohmann::json populate_data();
		nlohmann::json populate_state();
	private:
		Tlv493d sensor_obj;
};
#endif

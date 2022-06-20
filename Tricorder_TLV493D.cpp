#include "Tricorder_TLV493D.h"

Tricorder_TLV493D::Tricorder_TLV493D(){
	sensor_name = "TLV493D";
	
	sensor_options.push_back(sensor_option(
		"enabled",
		OP_Boolean,
		[&](nlohmann::json val)->bool{			
			enabled = val.get<bool>();
			return true;
		},
		[&]()->nlohmann::json{ return enabled; }
	));
}

bool Tricorder_TLV493D::sensor_init(){
	sensor_obj = Tlv493d();
	sensor_obj.begin();
	initialized = true;
	
	return initialized;
}

bool Tricorder_TLV493D::read_sensor(){
	sensor_obj.updateData();
	return true;
}

nlohmann::json Tricorder_TLV493D::populate_data(){
	nlohmann::json data_frame = {
		{"X", sensor_obj.getX()},
		{"Y", sensor_obj.getY()},
		{"Z", sensor_obj.getZ()}
	};
	
	return data_frame;
}

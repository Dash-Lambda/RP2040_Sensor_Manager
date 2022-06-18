#ifndef Tricorder_Sensor_h
#define Tricorder_Sensor_h

#include "pico/stdlib.h"
#include "json.hpp"

class Tricorder_Sensor
{
  public:
    Tricorder_Sensor();

    // Variables
    //String sensor_name;
    char *sensor_name;
    unsigned long timestamp;
    bool initialized;
    bool enabled;
    bool data_updated;
    bool error_state;

    // External Use
    virtual bool sensor_init();
    //virtual void enable();
    //virtual void disable();
    virtual bool set_property(char *nam, char *val);
    nlohmann::json build_json();
    nlohmann::json report_options();
    nlohmann::json report_state(char *opnam);
    bool update_data();

    // Internal Use
    virtual nlohmann::json populate_options();
    virtual nlohmann::json populate_data();
    virtual nlohmann::json query_state(char *opnam);
    virtual bool read_sensor();
};
#endif

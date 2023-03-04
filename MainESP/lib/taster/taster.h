#pragma once

#include "digital_sensor.h"
#include "../../include/pin_definitions.h"

class taster_class
{
    public:
        // taster_class();
        enum taster_name {
            front_left,
            front_right,
            reset_nvs
        };
        bool get_state(taster_name name);
    private:
        digital_sensor* front_right_taster = new digital_sensor(PIN_SENS3, INPUT_PULLUP, true);
        digital_sensor* front_left_taster = new digital_sensor(PIN_SENS4, INPUT_PULLUP, true);
        digital_sensor* reset_nvs_taster = new digital_sensor(0, INPUT_PULLUP, true);
}; 

#pragma once

#include "digital_sensor.h"
#include "../../include/pin_definitions.h"

class taster_class
{
    public:
        // taster_class();
        enum taster_name {
            front
        };
        bool get_state(taster_name name);
    private:
        digital_sensor* front_taster = new digital_sensor(PIN_SENS4, INPUT_PULLUP, true);
}; 

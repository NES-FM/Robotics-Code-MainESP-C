#pragma once
#include "analog_sensor.h"

class IR {
    public:
        IR(int pin) { as = new analog_sensor(pin, false); };
        float get_raw() { return as->get_state(); };
        float get_cm() { return 66.655466 * exp(-0.00281495 * as->get_state()); }
    private:
        analog_sensor* as;

};
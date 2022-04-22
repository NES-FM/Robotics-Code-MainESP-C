#pragma once
#include "analog_sensor.h"

class IR {
    public:
        IR(int pin) { as = new analog_sensor(pin, false); };
        int get_raw() { return as->get_state(); };
        float get_cm() { return 46.68567 * exp(-0.001145 * as->get_state()); }
    private:
        analog_sensor* as;

};
#pragma once
#include "analog_sensor.h"

class IR {
    public:
        IR(int pin) { as = new analog_sensor(pin, false); };
        int get_raw() { return as->get_state(); };
        float get_cm() { float state = 46.68567 * exp(-0.001145 * as->get_state()); last_state = state; return state; }
        float change_between_last_time() { float state = get_cm(); if (state > 45) {return 0;}float diff = abs(state - last_state); last_state = state; return diff; }
    private:
        analog_sensor* as;
        float last_state = 0;
};
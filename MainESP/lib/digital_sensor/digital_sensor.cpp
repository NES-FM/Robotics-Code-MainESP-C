#include "digital_sensor.h"

uint8_t valid_analog_pins[20] = {0,2,4,12,13,14,15,25,26,27,32,33,34,35,36,37,38,39};

digital_sensor::digital_sensor(int p)
{
    pin = p;
}

bool digital_sensor::get_state_dont_set_last_state()
{
    return digitalRead(pin);
}

bool digital_sensor::get_state()
{
    bool state = get_state_dont_set_last_state();
    last_state = state;
    return state;
}

bool digital_sensor::state_changed()
{
    bool state = get_state_dont_set_last_state();
    bool changed = (state != last_state);
    last_state = state;
    return changed;
}

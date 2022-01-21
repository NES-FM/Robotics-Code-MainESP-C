#include "digital_sensor.h"

digital_sensor::digital_sensor(int p, int input_type, bool inverted)
{
    pin = p;
    pinMode(pin, input_type);
    _inverted = inverted;
}

bool digital_sensor::get_state_dont_set_last_state()
{
    return digitalRead(pin) ^ _inverted; // XOring inverts the input, if _inverted is true
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

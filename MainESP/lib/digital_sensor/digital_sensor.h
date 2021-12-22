#ifndef DIGITAL_SENSOR_H
#define DIGITAL_SENSOR_H

#include <Arduino.h>

class digital_sensor {
    public:
        digital_sensor(int p);
        bool get_state();
        bool state_changed();
    private:
        uint8_t pin;
        bool last_state = LOW;
        bool get_state_dont_set_last_state();
};

#endif

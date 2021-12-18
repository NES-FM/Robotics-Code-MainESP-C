#ifndef ANALOG_SENSOR_H
#define ANALOG_SENSOR_H

#include <Arduino.h>

class analog_sensor {
    public:
        analog_sensor(int p, bool av8);
        analog_sensor(int p);
        uint16_t get_state();
        uint16_t diff_to_last_state();
        float convert_to_battery_voltage();
        void setAverage8(bool is_on) {average8_enabled = is_on; };
    private:
        uint8_t pin;
        uint16_t last_state = 0;
        bool average8_enabled = false;
        uint16_t average8_list[9] = {0,0,0,0,0,0,0,0,0};
        int average8_index = 0;
        void checkValidPin(uint8_t pin);
};

#endif

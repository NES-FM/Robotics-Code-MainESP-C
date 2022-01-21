#ifndef DIP_H
#define DIP_H

#include "digital_sensor.h"
#include "../../include/pin_definitions.h"

class DIP
{
    public:
        DIP() { this->get_state(); };
        enum dips {
            wettkampfmodus = 0,
            dip1 = 1,
            dip2 = 2
        };

        bool get_state(dips index);
        char get_state();
        bool get_wettkampfmodus();
        bool has_changed(dips index);
        bool has_changed();
    private:
        digital_sensor* _wettk = new digital_sensor(PIN_DIP_WETTKAMPFMODUS, INPUT_PULLUP, true);
        digital_sensor* _dip1 = new digital_sensor(PIN_DIP1, INPUT_PULLUP, true);
        digital_sensor* _dip2 = new digital_sensor(PIN_DIP2, INPUT_PULLUP, true);
};

#endif

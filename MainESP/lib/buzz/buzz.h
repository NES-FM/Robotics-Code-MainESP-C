#pragma once

#include <Arduino.h>
#include "dip.h"

class buzz {
    public:
        buzz(int p, int vol, DIP* dipref);
        void tone(note_t note, uint8_t octave, unsigned long duration = 0);
        void noTone();
        void setVol(int vol) { duty = vol; }
        void startCrossingSound() { tone(NOTE_A, 4); }
        void turnLeftSound() { tone(NOTE_F, 4); }
        void turnRightSound() { tone(NOTE_D, 4); }
    private:
        int channel;
        int pin;
        int duty;
        DIP* _dip;
};

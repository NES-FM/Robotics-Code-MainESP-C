#pragma once

#include <Arduino.h>

class buzz {
    public:
        buzz(int p, int vol);
        void tone(note_t note, uint8_t octave, unsigned long duration = 0);
        void noTone();
        void setVol(int vol) { duty = vol; }
    private:
        int channel;
        int pin;
        int duty;
};

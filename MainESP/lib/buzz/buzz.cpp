#include "buzz.h"

buzz::buzz(int p, int vol)
{
    for (int i = 15; i >= 0; i--)
    {
        if (!ledcRead(i))
        {
            channel = i;
            break;
        }
    }
    pin = p;
    ledcSetup(channel, 2000, 8);
    ledcAttachPin(pin, channel);
    this->noTone();
    duty = vol;
}

void buzz::tone(note_t note, uint8_t octave, unsigned long duration)
{
    ledcWriteNote(channel, note, octave);
    ledcWrite(channel, duty);
    if (duration) {
        delay(duration);
        this->noTone();
    }    
}

void buzz::noTone()
{
    ledcWrite(channel, 0);
}

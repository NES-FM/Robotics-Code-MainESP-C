#include "buzz.h"

buzz::buzz(int p, int vol, DIP* dipref)
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
    _dip = dipref;
}

void buzz::tone(note_t note, uint8_t octave, unsigned long duration)
{
    if (_dip->get_state(_dip->dip2))
    {
        ledcWriteNote(channel, note, octave);
        ledcWrite(channel, duty);
        if (duration) {
            delay(duration);
            this->noTone();
        }    
    }
}

void buzz::noTone()
{
    ledcWrite(channel, 0);
}

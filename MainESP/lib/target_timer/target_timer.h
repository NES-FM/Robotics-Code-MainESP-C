#pragma once

#include "Arduino.h"

class target_timer
{
    public:
        target_timer();
        target_timer(int target);
        void reset();
        void set_target(int t);
        bool has_reached_target();
    private:
        int target = 0;
        int zero_time = 0;
};

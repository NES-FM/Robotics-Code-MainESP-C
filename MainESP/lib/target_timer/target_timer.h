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
        int time_elapsed();
        int time_left();
    private:
        int target = 0;
        int zero_time = 0;
};

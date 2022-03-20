#pragma once

class timer
{
    public:
        timer();
        void reset();
        void set_target(int t);
        bool has_reached_target();
    private:
        int target = 0;
        int zero_time = 0;
};

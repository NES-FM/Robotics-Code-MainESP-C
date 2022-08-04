#pragma once
#include <Arduino.h>

#include "io_extender.h"

class lc02
{
    public:
        lc02(int offset_x, int offset_y, int offset_a) { _offset_x = offset_x; _offset_y = offset_y; _offset_a = offset_a; };
        int getDistance_mm();
        uint8_t getErrorCode();
    private:
        void getData();
        lc02_answer data;
        int _offset_x = 0;
        int _offset_y = 0;
        int _offset_a = 0;
    
    friend class Robot;
};

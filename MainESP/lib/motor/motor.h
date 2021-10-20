#ifndef MOTOR_H
#define MOTOR_H

#include <Wire.h>
#include "../../include/pin_definitions.h"
#include "../../include/motor_directions.h"

class motor 
{
    public:
        motor();
        void init(char mnum);
        void move(int speed);
        void move_direction(int speed, int direction);
        void stop();
        void off();
    private:
        const char _i2c_set_speed = 0x00;
        const char _i2c_set_direc = 0x01;
        const char _i2c_offset_motor_1 = 0x30;
        const char _i2c_offset_motor_2 = 0x35;
        const char _i2c_address = 0x08;

        signed char motor_num = -1;
        char current_i2c_offset = 0;

        char current_speed = 0;
        char current_direc = MOTOR_DIREC_STOP;

};

#endif /* MOTOR_H */

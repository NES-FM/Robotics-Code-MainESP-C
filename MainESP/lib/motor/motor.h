#ifndef MOTOR_H
#define MOTOR_H

#include <Wire.h>
#include "../../include/pin_definitions.h"
#include "../../include/motor_directions.h"
#include "../../include/i2c_addresses.h"
// #include "../../lib/i2c_scanner/i2c_scanner.h"
#include <HardwareSerial.h>

class motor 
{
    public:
        motor();
        void init(char mnum);
        void move(int speed);
        void move_direction(int speed, int direction);
        void stop();
        void off();
        void enable(bool enabled);
        bool is_enabled() { return _motor_i2c_enabled; }
        int motor_speed = 0;
    private:
        const char _i2c_set_speed = 0x00;
        const char _i2c_set_direc = 0x01;
        const char _i2c_offset_motor_1 = 0x30;
        const char _i2c_offset_motor_2 = 0x35;
        unsigned char _i2c_address = I2C_ADDRESSS_MOTOR_CONTROLLER;

        bool _motor_i2c_enabled = false;

        signed char motor_num = -1;
        char current_i2c_offset = 0;

        char current_speed = 0;
        char current_direc = MOTOR_DIREC_STOP;

        void writeto_mem(char addr, char reg, char data);

};

#endif /* MOTOR_H */

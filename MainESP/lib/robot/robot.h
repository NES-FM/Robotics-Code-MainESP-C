#pragma once

#include "motor.h"
#include "accel.h"
#include "compass.h"
#include "analog_sensor.h"
#include "dip.h"
#include "taster.h"
#include <Servo.h>
#include "servo_angles.h"
#include "buzz.h"
#include "debug_disp.h"
#include "i2c_addresses.h"
#include "i2c_scanner.h"

class Robot
{
    public:
        Robot();
        void enable(debug_disp* disp);
        void init();

        void PlayBeginSound();

        void move(int speed_left, int speed_right);
        void greifer_home();
    private:
        debug_disp* display;

        Servo* greifer_up;
        Servo* greifer_zu;
        DIP* dip;
        buzz* main_buzzer = new buzz(PIN_BUZZ1, 128, dip);
        taster_class* taster;
        
        accel* accel_sensor;
        compass_hmc* compass;

        motor* motor_left;
        motor* motor_right;

        analog_sensor* bat_voltage = new analog_sensor(PIN_BATPROBE, true);

    friend class debug_disp;
};

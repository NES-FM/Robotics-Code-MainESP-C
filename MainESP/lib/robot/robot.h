#pragma once

#include "motor.h"
#include "accel.h"
#include "compass.h"
#include "analog_sensor.h"
#include "dip.h"
#include "taster.h"
#include <Servo.h>
#include "../../include/servo_angles.h"
#include "buzz.h"
#include "../../include/i2c_addresses.h"
#include "tof.h"

class Robot
{
    public:
        Robot();
        void init();

        void PlayBeginSound();

        void move(int speed_left, int speed_right);
        void greifer_home();

        void calculate_position();
        void setRoomBeginningAngle() { room_beginning_angle = compass->get_angle(); }

        Servo* greifer_up = new Servo();
        Servo* greifer_zu = new Servo();
        DIP* dip = new DIP();
        buzz* main_buzzer = new buzz(PIN_BUZZ1, 128, dip);
        taster_class* taster = new taster_class();
        
        accel* accel_sensor = new accel();
        compass_hmc* compass = new compass_hmc();

        motor* motor_left = new motor();
        motor* motor_right = new motor();

        tof* tof_right = new tof(PIN_SENS1, 95, 22, 90);
        tof* tof_left = new tof(PIN_SERVO2, -95, 22, -90);

        analog_sensor* bat_voltage = new analog_sensor(PIN_BATPROBE, true);

        const int width = 180;
        const int height = 175;

        const int room_width = 1235;
        const int room_height = 950;

        float room_beginning_angle = 0.0f;
        float angle = 0.0f;
        int posx = 500;
        int posy = 400;

        struct point
        {
            int x = 0;
            int y = 0;
        };
        point points[90];
};

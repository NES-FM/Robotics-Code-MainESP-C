#pragma once

#ifndef ROBOT_H
#define ROBOT_H
#endif

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
#include "io_extender.h"
#include "lc02.h"

#include "logger.h"
#include "command_parser.h"

class Robot
{
    public:
        Robot();
        void init_tof_xshut();
        void init();

        void tick();
        void tickPinnedMain();

        void PlayBeginSound();

        void move(int speed_left, int speed_right);
        void greifer_home();

        void calculate_position();
        void setRoomBeginningAngle() { room_beginning_angle = compass->get_angle(); }

        void startRoom();

        Servo* greifer_up = new Servo();
        Servo* greifer_zu = new Servo();
        DIP* dip = new DIP();
        buzz* main_buzzer = new buzz(PIN_BUZZ1, 128, dip);
        taster_class* taster = new taster_class();
        
        accel* accel_sensor = new accel();
        compass_hmc* compass = new compass_hmc();

        motor* motor_left = new motor();
        motor* motor_right = new motor();

        tof* tof_right = new tof(TOF_SENSOR_VL53L0X, 95, -60, 90, io_ext_pins::EXT_D6);
        tof* tof_left = new tof(TOF_SENSOR_VL53L0X, -95, -60, -90, io_ext_pins::EXT_D4);
        tof* tof_back = new tof(TOF_SENSOR_VL53L0X, 0, -90, 180, io_ext_pins::EXT_D2);
        tof* tof_front = new tof(TOF_SENSOR_VL53L0X, 0, 90, 0, io_ext_pins::EXT_D7);

        tof* tof_closerange = new tof(TOF_SENSOR_VL6180X, 90, 80, 90, io_ext_pins::EXT_D8);

        // lc02* lc02_right = new lc02(90, -76, 90);

        analog_sensor* bat_voltage = new analog_sensor(PIN_BATPROBE, true);

        const int width = 180;
        const int height = 175;

        const int room_width = 1235;
        const int room_height = 950;

        struct point
        {
            int x_mm = 0;
            int y_mm = 0;
        };

        point point_cloud[120];

        float room_beginning_angle = 0.0f;
        float angle = 0.0f;
        point pos;
        static point rotate_point(point point_to_rotate, point pivot, float angle);

        enum ROBOT_DRIVE_MODE
        {
            ROBOT_DRIVE_MODE_LINE,
            ROBOT_DRIVE_MODE_ROOM
            //TBD: Different Steps of room
        };
        ROBOT_DRIVE_MODE cur_drive_mode = ROBOT_DRIVE_MODE_LINE; // NEEDS TO BE CHANGED (Sets default drive mode)

        bool is_control_on_user = false;

        bool compass_calibration_background_task_enabled = false;
        void compass_start_calibration_background_task();
        void compass_stop_calibration_background_task();

        bool serial_lidar_mode = false;
    private:
        void parse_command(String command);
        String help_command();
        String get_command(String sensor, String subsensor);
        String move_command(String left, String right, String third_arg);
        String control_command(String on_off);
        String set_command(String first_arg, String second_arg, String third_arg);
        String comamnd_template(String arg);
};

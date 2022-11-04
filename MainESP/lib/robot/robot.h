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
#include "../../include/drive_speeds.h"
#include "../../include/i2c_addresses.h"
#include "../../include/room_stuff.h"
#include "buzz.h"
#include "tof.h"
#include "io_extender.h"
#include "lc02.h"
#include "cuart.h"

#include "logger.h"
#include "command_parser.h"

#include <deque>

class Robot
{
    public:
        Robot(CUART_class* cuart);
        void init_tof_xshut();
        void init();

        void tick();
        void tickPinnedMain();

        void PlayBeginSound();

        void move(int speed_left, int speed_right);
        void greifer_home();

        CUART_class* cuart_ref;

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

        const int room_width = 1150;
        const int room_height = 850;

        int room_rotation = 0;

        struct point
        {
            int x_mm = 0;
            int y_mm = 0;
        };

        float angle = 0.0f;
        point pos;
        static point rotate_point(point point_to_rotate, point pivot, float angle_degrees);

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


        // Room
        void calculate_position();

        float room_beginning_angle = 0.0f;
        void setRoomBeginningAngle(float offset = 0.0f) { room_beginning_angle = compass->keep_in_360_range(compass->get_angle() + offset); }

        float room_search_balls_beginning_angle = 0.0f;
        void setRoomSearchBallsBeginningAngle() { room_search_balls_beginning_angle = compass->keep_in_360_range(compass->get_angle() - 90); }
        
        bool searching_balls_moving_backward = false;
        
        void startRoom();

        void room_move_along_wall();

        enum room_end_types {
            ROOM_HAS_NOT_REACHED_END,
            ROOM_HAS_REACHED_TASTER_RIGHT,
            ROOM_HAS_REACHED_TASTER_LEFT,
            ROOM_HAS_REACHED_SILVER_LINE,
            ROOM_HAS_REACHED_GREEN_LINE
        };
        room_end_types room_has_reached_end();

        bool serial_lidar_mode = false;

        void room_set_cur_pos(int x, int y);

        void room_rotate_to_degrees(float degrees, bool rotate_right);
        void room_rotate_relative_degrees(float degrees);

        unsigned long room_time_measure_start_time = 0u;
        void room_time_measure_start() { room_time_measure_start_time = millis(); }
        unsigned long room_time_measure_stop() { return millis() - room_time_measure_start_time; }

        const unsigned int robot_millis_per_360_at_30_speed = 2650;

        point room_entry_pos;
        bool room_entry_found = false;
        point room_exit_pos;
        bool room_exit_found = false;
        point room_corner_pos;
        bool room_corner_found = false;

        bool last_time_was_corner = false;

        enum room_wall_types
        {
            WALL_FIRST_UNKNOWN_WALL,
            WALL_2_LONG,
            WALL_2_SHORT,
            WALL_3_LONG,
            WALL_3_SHORT,
            WALL_4_LONG,
            WALL_4_SHORT,
            WALL_1_LONG,
            WALL_1_SHORT,
        };
        room_wall_types cur_moving_wall;

        enum room_states
        {
            ROOM_STATE_INITAL_MOVE_AROUND_WALLS,
            ROOM_STATE_SCAN_FOR_BALLS,
            // ROOM_STATE_COLLECT_BALL, etc...
        };
        room_states cur_room_state;

        point room_tof_to_relative_point(tof* tof_sensor, float angle_degrees);

        struct room_search_balls_points
        {
            int_least16_t x;
            int_least16_t y;
        };

        std::deque<room_search_balls_points> room_search_balls_left_values;
        std::deque<room_search_balls_points> room_search_balls_right_values;
        // room_search_balls_vector left_values[1000];

        void roomSetEntryPos(int x, int y);
        void roomSetExitPos(int x, int y);
        void roomSetCornerPos(int x, int y);


        // Bluetooth app stuff
        bool bluetooth_app_enabled = false;
        void roomSendNewPoints();
        void roomSendNewEntry();
        void roomSendNewExit();
        void roomSendNewCorner();
        void roomSendRobotData();

    private:
        void parse_command(String command);
        String help_command();
        String get_command(String sensor, String subsensor);
        String move_command(String left, String right, String third_arg);
        String rotate_command(String degrees);
        String control_command(String on_off);
        String set_command(String first_arg, String second_arg, String third_arg);
        String comamnd_template(String arg);
        String heartbeat_command(String arg);
        String bluetooth_app_command(String arg);
};

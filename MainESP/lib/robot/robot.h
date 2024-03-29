#pragma once

#ifndef ROBOT_H
#define ROBOT_H

#include "motor.h"
#include "analog_sensor.h"
#include "dip.h"
#include "../../include/servo_angles.h"
#include "../../include/drive_speeds.h"
#include "../../include/i2c_addresses.h"
#include "../../include/room_stuff.h"
#include "buzz.h"
#include "io_extender.h"
#include "cuart.h"
#include "bcuart.h"
#include "claw.h"
#include <Preferences.h>

#include "logger.h"
#include "command_parser.h"

#include <deque>
#include <vector>

class Robot
{
    public:
        Robot(CUART_class* cuart, BCUART_class* bcuart);
        void init();

        void tick();

        void PlayBeginSound();

        void move(int speed_left, int speed_right);

        CUART_class* cuart_ref;
        BCUART_class* bcuart_ref;

        DIP* dip = new DIP();
        buzz* main_buzzer = new buzz(PIN_BUZZ1, 128, dip);
        
        motor* motor_left = new motor();
        motor* motor_right = new motor();

        Claw* claw = new Claw();

        io_extender* io_ext = new io_extender();

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
        point Origin;

        // float angle = 0.0f;

        static point rotate_point(point point_to_rotate, point pivot, float angle_degrees);
        static point rotate_point_around_origin(point point_to_rotate, float angle_degrees);
        static float distance_between_points(point p1, point p2);
        static float x_distance_between_points(point p1, point p2);
        static float y_distance_between_points(point p1, point p2);
        static point midpoint_between_points(point p1, point p2);

        enum ROBOT_DRIVE_MODE
        {
            ROBOT_DRIVE_MODE_LINE,
            ROBOT_DRIVE_MODE_ROOM
            //TBD: Different Steps of room
        };
        ROBOT_DRIVE_MODE cur_drive_mode = ROBOT_DRIVE_MODE_LINE; // NEEDS TO BE CHANGED (Sets default drive mode)

        bool is_control_on_user = false;

        // Room
        // float room_beginning_angle = 0.0f;
        // void setRoomBeginningAngle(float offset = 0.0f) { room_beginning_angle = compass->keep_in_360_range(compass->get_angle() + offset); }

        void startRoom();

        struct ball {
            float distance = 0.0;
            float conf = 0.0;
            uint16_t num_hits = 0;
            bool black = false;
        };

        ball detected_balls[30];
        uint8_t num_detected_balls = 0;
        bool detectingBallsEnabled = false;
        ball moving_to_balls_target;
        void print_balls();

        struct corner {
            float distance = 0.0;
            float conf = 0.0;
            uint16_t num_hits = 0;
        };
        
        std::vector<corner*> possible_corners;
        bool detectingCornerEnabled = false;
        corner* most_likely_corner = new corner();
        void print_corners();

        // void room_move_along_wall();

        enum room_end_types {
            ROOM_HAS_NOT_REACHED_END = 0,
            ROOM_HAS_REACHED_TASTER_RIGHT = 1,
            ROOM_HAS_REACHED_TASTER_LEFT = 2,
            ROOM_HAS_REACHED_SILVER_LINE = 3,
            ROOM_HAS_REACHED_GREEN_LINE = 4
        };
        room_end_types room_has_reached_end();

        bool serial_lidar_mode = false;

        // void room_rotate_to_degrees(float degrees, bool rotate_right);
        // void room_rotate_relative_degrees(float degrees);

        unsigned long room_time_measure_start_time = 0u;
        void room_time_measure_start() { room_time_measure_start_time = millis(); }
        unsigned long room_time_measure_stop() { return millis() - room_time_measure_start_time; }

        const unsigned int robot_millis_per_360_at_30_speed = 2650;

        enum room_states
        {
            ROOM_STATE_DEFAULT,
            ROOM_STATE_MOVE_IN_ROOM,
            ROOM_STATE_FIND_WALL_DRIVE_TO_CENTER,
            ROOM_STATE_ROTATE_TO_FIND_BALLS,
            ROOM_STATE_PUT_BALL_IN_CORNER_STEP_1,
            ROOM_STATE_PUT_BALL_IN_CORNER_STEP_2,
            ROOM_STATE_SEARCHING_EXIT
        };
        room_states cur_room_state;
        room_states prev_room_state;

        bool has_seen_black_ball_before = false;

        // tof* tof_side = new tof(TOF_SENSOR_VL53l1X, 90, 10, 90, EXT_P1);

        const float millimeters_per_millisecond_40_speed = 0.1770833333;
        const float degrees_per_millisecond_20_speed = 360 / 4050;

        // Preferences* room_prefs = new Preferences();

    private:
        void parse_command(String command);
        String help_command();
        String get_command(String sensor, String subsensor);
        String move_command(String left, String right, String third_arg);
        // String rotate_command(String degrees);
        String control_command(String on_off);
        String set_command(String first_arg, String second_arg, String third_arg);
        String comamnd_template(String arg);
        String heartbeat_command(String arg);
        String balls_command(String arg);
};

#endif

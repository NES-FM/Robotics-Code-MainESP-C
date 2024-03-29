#ifndef CLAW_H
#define CLAW_H

#include "Arduino.h"
#include <Servo.h>
#include "../../include/pin_definitions.h"
#include "logger.h"
#include <Preferences.h>

class Claw
{
    public:
        enum State
        {
            BOTTOM_OPEN = 0,
            BOTTOM_MID = 1,
            BOTTOM_CLOSED = 2,
            SIDE_CLOSED = 3,
            TOP_CLOSED = 4,
            TOP_OPEN = 5
        };

        Claw();
        void init();
        void set_state(State state, bool force = false);
        State get_state();

        void _set_raw_servo_up_state(int degrees) {claw_up_servo->write(degrees);}
        void _set_raw_servo_close_state(int degrees) {claw_close_servo->write(degrees);}
        void _set_raw_servo_blue_cube_state(int degrees) {blue_cube_servo->write(degrees);}

        void hold_blue_cube();
        void throw_blue_cube();
        
        bool has_blue_cube = true;

        void disable_close_servo();
        void enable_close_servo();
    private:
        State _last_state;
        Servo* claw_up_servo = new Servo();
        Servo* claw_close_servo = new Servo();
        Servo* blue_cube_servo = new Servo();
        void open_claw();
        void claw_bottom_mid();
        void claw_to_down_pos();
        void claw_to_side_pos();
        void claw_to_up_pos();
        void close_claw();

        int safety_delay = 100;

        const uint8_t initial_angle_close_servo = 110;
        const uint8_t initial_angle_up_servo = 0;

        const uint8_t servo_close_open = 110;
        const uint8_t servo_close_open_first_step = 115;
        const uint8_t servo_close_open_second_step = 110;
        const uint8_t servo_close_closed_first_step = 40;
        const uint8_t servo_close_closed_second_step = 43;
        const uint8_t servo_close_mid = 80;

        const uint8_t servo_up_down = 0;
        const uint8_t servo_up_side = 90;
        const uint8_t servo_up_up = 180;

        const uint8_t servo_blue_cube_holding = 0;
        const uint8_t servo_blue_cube_throw = 130;

        Preferences* claw_prefs = new Preferences();
};

#endif
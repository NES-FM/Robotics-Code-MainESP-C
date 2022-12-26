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
            BOTTOM_CLOSED = 1,
            SIDE_CLOSED = 2,
            TOP_CLOSED = 3,
            TOP_OPEN = 4
        };

        Claw();
        void init();
        void set_state(State state);
        State get_state();

        void _set_raw_servo_up_state(int degrees) {claw_up_servo->write(degrees);}
        void _set_raw_servo_close_state(int degrees) {claw_close_servo->write(degrees);}

    private:
        State _last_state;
        Servo* claw_up_servo = new Servo();
        Servo* claw_close_servo = new Servo();
        void open_claw();
        void claw_to_down_pos();
        void claw_to_side_pos();
        void claw_to_up_pos();
        void close_claw();
        void disable_close_servo();
        void enable_close_servo();

        int safety_delay = 100;

        const uint8_t initial_angle_close_servo = 110;
        const uint8_t initial_angle_up_servo = 0;

        const uint8_t servo_close_open = 110;
        const uint8_t servo_close_open_first_step = 115;
        const uint8_t servo_close_open_second_step = 110;
        const uint8_t servo_close_closed_first_step = 45;
        const uint8_t servo_close_closed_second_step = 50;

        const uint8_t servo_up_down = 0;
        const uint8_t servo_up_side = 90;
        const uint8_t servo_up_up = 180;

        Preferences* claw_prefs = new Preferences();
};

#endif

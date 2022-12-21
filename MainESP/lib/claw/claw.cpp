#include "claw.h"

Claw::Claw()
{
    _state = TOP_OPEN;
}

void Claw::init()
{
    enable_close_servo();
    // _set_raw_servo_close_state(initial_angle_close_servo);
    claw_up_servo->attach(PIN_SERVO2, -1, 0, 180, 530, 2450); // default: pin, -1, 0, 544, 2400
    // _set_raw_servo_up_state(initial_angle_up_servo);
    this->set_state(Claw::BOTTOM_OPEN);
}

void Claw::set_state(State target_state)
{
    if (target_state == _state)  // Same state requested, that claw already is at
    {
        goto end;
    }
    else if (target_state > _state)  // Claw should go up in hirarchie
    {
        if (_state == BOTTOM_OPEN)
        {
            enable_close_servo();
            close_claw();
            if (target_state == BOTTOM_CLOSED)
                goto end;
            else
            {
                claw_moving_up_bottom_already_closed:
                if (target_state == SIDE_CLOSED)
                {
                    claw_to_side_pos();
                    goto end;
                }
                else
                {
                    claw_moving_up_target_not_side_pos:
                    claw_to_up_pos();
                    if (target_state == TOP_CLOSED)
                        goto end;
                    else
                    {
                        claw_moving_up_target_top_open:
                        open_claw();
                        goto end;
                    }
                }
            }
        }
        else if (_state == BOTTOM_CLOSED)
        {
            goto claw_moving_up_bottom_already_closed;
        }
        else if (_state == SIDE_CLOSED)
        {
            goto claw_moving_up_target_not_side_pos;
        }
        else if (_state == TOP_CLOSED)
        {
            goto claw_moving_up_target_top_open;
        }
        else
        {
            logln("[target_state > _state][else] ERROR! THIS SHOULD NEVER BE REACHED!");
        }
    }
    else if (target_state < _state)
    {
        if (_state == TOP_OPEN)
        {
            close_claw();
            if (target_state == TOP_CLOSED)
                goto end;
            else
            {
                claw_moving_down_top_already_closed:
                if (target_state == SIDE_CLOSED)
                {
                    claw_to_side_pos();
                    goto end;
                }
                else
                {
                    claw_moving_down_target_not_side_pos:
                    claw_to_down_pos();
                    if (target_state == BOTTOM_CLOSED)
                        goto end;
                    else
                    {
                        claw_moving_down_target_bottom_open:
                        open_claw();
                        disable_close_servo();
                        goto end;
                    }
                }
            }
        }
        else if (_state == TOP_CLOSED)
        {
            goto claw_moving_down_top_already_closed;
        }
        else if (_state == SIDE_CLOSED)
        {
            goto claw_moving_down_target_not_side_pos;
        }
        else if (_state == BOTTOM_CLOSED)
        {
            goto claw_moving_down_target_bottom_open;
        }
        else
        {
            logln("[target_state < _state][else] ERROR! THIS SHOULD NEVER BE REACHED!");
        }
    }
    else
    {
        logln("[else] ERROR! THIS SHOULD NEVER BE REACHED!");
    }


    end:
    _state = target_state;
}

Claw::State Claw::get_state()
{
    return _state;
}

void Claw::open_claw()
{
    logln("Opening claw...");
    uint8_t angle = 0;
    for (int i = 0; i < 500; i += 5)
    {
        angle = map(i, 0, 500, servo_close_closed_second_step, servo_close_open);
        _set_raw_servo_close_state(angle);
        delay(5);
    }
    _set_raw_servo_close_state(servo_close_open);
    delay(100);
    // _set_raw_servo_close_state(servo_close_open_second_step);
    
    delay(safety_delay);
}

void Claw::close_claw()
{
    logln("Closing claw...");
    uint8_t angle = 0;
    for (int i = 0; i < 500; i += 5)
    {
        angle = map(i, 0, 500, servo_close_open_second_step, servo_close_closed_first_step);
        _set_raw_servo_close_state(angle);
        delay(5);
    }
    _set_raw_servo_close_state(servo_close_closed_first_step);
    delay(100);
    _set_raw_servo_close_state(servo_close_closed_second_step);
    
    delay(safety_delay);
}

void Claw::claw_to_down_pos()
{
    logln("Moving claw to down position...");
    uint8_t angle = 0;
    uint8_t start_angle = claw_up_servo->read();
    int time_to_take = abs(start_angle-servo_up_down)*(750/180);
    for (int i = 0; i < time_to_take; i += 5)
    {
        angle = map(i, 0, time_to_take, start_angle, servo_up_down);
        _set_raw_servo_up_state(angle);
        delay(5);
    }
    _set_raw_servo_up_state(servo_up_down);

    delay(safety_delay);
}

void Claw::claw_to_side_pos()
{
    logln("Moving claw to side position...");
    uint8_t angle = 0;
    uint8_t start_angle = claw_up_servo->read();
    int time_to_take = abs(start_angle-servo_up_side)*(750/180);
    for (int i = 0; i < time_to_take; i += 5)
    {
        angle = map(i, 0, time_to_take, start_angle, servo_up_side);
        _set_raw_servo_up_state(angle);
        delay(5);
    }
    _set_raw_servo_up_state(servo_up_side);

    delay(safety_delay);
}

void Claw::claw_to_up_pos()
{
    logln("Moving claw to up position...");
    uint8_t angle = 0;
    uint8_t start_angle = claw_up_servo->read();
    int time_to_take = abs(start_angle-servo_up_up)*(750/180);
    for (int i = 0; i < time_to_take; i += 5)
    {
        angle = map(i, 0, time_to_take, start_angle, servo_up_up);
        _set_raw_servo_up_state(angle);
        delay(5);
    }
    _set_raw_servo_up_state(servo_up_up);

    delay(safety_delay);
}

void Claw::disable_close_servo()
{
    claw_close_servo->detach();
}

void Claw::enable_close_servo()
{
    claw_close_servo->attach(PIN_SERVO1);
}


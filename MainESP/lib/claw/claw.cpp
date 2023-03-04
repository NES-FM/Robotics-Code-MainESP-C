#include "claw.h"

Claw::Claw()
{
    _last_state = TOP_OPEN;
}

void Claw::init()
{
    claw_prefs->begin("claw");
    _last_state = (State)claw_prefs->getUChar("last_state", TOP_OPEN);

    enable_close_servo();
    claw_up_servo->attach(PIN_SERVO2, -1, 0, 180, 530, 2450); // default: pin, -1, 0, 544, 2400
    blue_cube_servo->attach(PIN_SERVO3);
    hold_blue_cube();
    
    if (_last_state == BOTTOM_OPEN)
    {
        _set_raw_servo_up_state(servo_up_down);
        delay(safety_delay);
        _set_raw_servo_close_state(servo_close_open);
        delay(500);
        disable_close_servo();
    }
    else
    {
        this->set_state(Claw::BOTTOM_OPEN);
    }
}

void Claw::set_state(State target_state, bool force)
{
    if (target_state == _last_state)  // Same state requested, that claw already is at
    {
        if (force)
        {
            enable_close_servo();
            if (target_state == BOTTOM_OPEN)
            {
                _set_raw_servo_close_state(servo_close_open);
                claw_to_down_pos();
                disable_close_servo();
            }
            else if (target_state == BOTTOM_MID)
            {
                _set_raw_servo_close_state(servo_close_mid);
                claw_to_down_pos();
            }
            else if (target_state == BOTTOM_CLOSED)
            {
                _set_raw_servo_close_state(servo_close_closed_second_step);
                claw_to_down_pos();
            }
            else if (target_state == SIDE_CLOSED)
            {
                _set_raw_servo_close_state(servo_close_closed_second_step);
                claw_to_side_pos();
            }
            else if (target_state == TOP_CLOSED)
            {
                _set_raw_servo_close_state(servo_close_closed_second_step);
                claw_to_up_pos();
            }
            else if (target_state == TOP_OPEN)
            {
                _set_raw_servo_close_state(servo_close_open);
                claw_to_up_pos();
            }
            delay(safety_delay);
        }
        goto end;
    }
    else if (target_state > _last_state)  // Claw should go up in hirarchie
    {
        if (_last_state == BOTTOM_OPEN)
        {
            enable_close_servo();
            if (target_state == BOTTOM_MID)
            {
                claw_bottom_mid();
                goto end;
            }
            else
            {
                claw_from_bottom_mid_to_closed:
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
        }
        else if (_last_state == BOTTOM_MID)
        {
            goto claw_from_bottom_mid_to_closed;
        }
        else if (_last_state == BOTTOM_CLOSED)
        {
            goto claw_moving_up_bottom_already_closed;
        }
        else if (_last_state == SIDE_CLOSED)
        {
            goto claw_moving_up_target_not_side_pos;
        }
        else if (_last_state == TOP_CLOSED)
        {
            goto claw_moving_up_target_top_open;
        }
        else
        {
            logln("[target_state > _state][else] ERROR! THIS SHOULD NEVER BE REACHED!");
        }
    }
    else if (target_state < _last_state)
    {
        if (_last_state == TOP_OPEN)
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

                    claw_already_bottom_only_open:
                    if (target_state == BOTTOM_MID)
                    {
                        claw_bottom_mid();
                        goto end;
                    }
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
        else if (_last_state == TOP_CLOSED)
        {
            goto claw_moving_down_top_already_closed;
        }
        else if (_last_state == SIDE_CLOSED)
        {
            goto claw_moving_down_target_not_side_pos;
        }
        else if (_last_state == BOTTOM_CLOSED)
        {
            goto claw_already_bottom_only_open;
        }
        else if (_last_state == BOTTOM_MID)
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
    _last_state = target_state;
    claw_prefs->putUChar("last_state", (uint8_t)_last_state);
}

Claw::State Claw::get_state()
{
    return _last_state;
}

void Claw::open_claw()
{
    logln("Opening claw...");
    uint8_t angle = 0;
    uint8_t start_angle = claw_close_servo->read();
    int time_to_take = abs(start_angle-servo_close_open)*(750/180);
    for (int i = 0; i < time_to_take; i += 5)
    {
        angle = map(i, 0, time_to_take, start_angle, servo_close_open);
        _set_raw_servo_close_state(angle);
        delay(5);
    }
    _set_raw_servo_close_state(servo_close_open);

    delay(safety_delay);
}

void Claw::claw_bottom_mid()
{
    logln("Claw bottom mid...");
    uint8_t angle = 0;
    uint8_t start_angle = claw_close_servo->read();
    int time_to_take = abs(start_angle-servo_close_mid)*(750/180);
    for (int i = 0; i < time_to_take; i += 5)
    {
        angle = map(i, 0, time_to_take, start_angle, servo_close_mid);
        _set_raw_servo_close_state(angle);
        delay(5);
    }
    _set_raw_servo_close_state(servo_close_mid);

    delay(safety_delay);
}

void Claw::close_claw()
{
    logln("Closing claw...");
    uint8_t angle = 0;
    uint8_t start_angle = claw_close_servo->read();
    int time_to_take = abs(start_angle-servo_close_closed_first_step)*(750/180);
    for (int i = 0; i < time_to_take; i += 5)
    {
        angle = map(i, 0, time_to_take, start_angle, servo_close_closed_first_step);
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
    claw_close_servo->detach(); // TODO: Sometimes closes the servo a little bit
    digitalWrite(PIN_SERVO1, HIGH);
}

void Claw::enable_close_servo()
{
    claw_close_servo->attach(PIN_SERVO1);
}

uint16_t Claw::get_ball_distance()
{
    uint16_t dis = tof_claw->getMeasurement();

    if (tof_claw->getMeasurementError() == tof::TOF_ERROR_MAX_DISTANCE)
        dis = 65535;
    else if (tof_claw->getMeasurementError() == tof::TOF_ERROR_MIN_DISTANCE)
        dis = 0;
    else if (tof_claw->getMeasurementError() != tof::TOF_ERROR_NONE)
        dis = 65534;
        
    return dis;
}

void Claw::hold_blue_cube()
{
    _set_raw_servo_blue_cube_state(servo_blue_cube_holding);
    delay(50);
}

void Claw::throw_blue_cube()
{
    _set_raw_servo_blue_cube_state(servo_blue_cube_throw);
    delay(50);
}

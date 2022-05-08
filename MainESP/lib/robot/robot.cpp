#include "robot.h"

Robot::Robot()
{
}

void Robot::init()
{
    // TOF
    tof_right->enable(true);
    tof_left->enable(true);
    tof_left->init(); // Setting Pinmodes
    tof_right->init();
    delay(10);
    tof_left->holdReset(); // Resetting both sensors
    tof_right->holdReset();
    delay(10);

    tof_right->releaseReset(); // Unresetting right, so that right can be initialized
    delay(10);
    tof_right->begin(I2C_ADDRESS_TOF_RIGHT);
    delay(10);

    tof_left->releaseReset(); // Unresetting left, so that left can be initialized
    delay(10);
    tof_left->begin(I2C_ADDRESS_TOF_LEFT);
    delay(10);

    tof_right->setLongRangeMode(true);
    tof_right->setContinuous(true);
    tof_right->setHighAccuracy(true);

    tof_left->setLongRangeMode(true);
    tof_left->setContinuous(true);
    tof_right->setHighAccuracy(true);

    // Others
    motor_left->init(1);
    motor_right->init(2);

    accel_sensor->init();
    compass->init(accel_sensor);
}

void Robot::PlayBeginSound()
{
    main_buzzer->tone(NOTE_C, 4, 100);
    main_buzzer->tone(NOTE_D, 4, 100);
    main_buzzer->tone(NOTE_E, 4, 100);
    main_buzzer->tone(NOTE_F, 4, 100);
    main_buzzer->tone(NOTE_G, 4, 100);
    main_buzzer->tone(NOTE_A, 4, 100);
    main_buzzer->tone(NOTE_B, 4, 100);
}

void Robot::move(int speed_left, int speed_right)
{
    #ifdef EXTENSIVE_DEBUG
    Serial.printf("[Move] L:%d, R:%d\r\n", speed_left, speed_right);
    #endif
    motor_left->move(speed_left);
    motor_right->move(speed_right);
    // resend_motor_timer.reset();
}

void Robot::greifer_home()
{
    greifer_zu->attach(PIN_SERVO1);
    greifer_zu->write(ANGLE_GREIFER_CLOSE_CUBE);
    delay(300);
    greifer_up->attach(PIN_SERVO2);
    greifer_up->write(ANGLE_GREIFER_UP); //NEEDS TO BE CHANGED
}

void Robot::calculate_position()
{
    this->angle = compass->keep_in_360_range(compass->get_angle() - room_beginning_angle);

    float left_dis = tof_left->getMeasurement() + abs(tof_left->_offset_x);
    float right_dis = tof_right->getMeasurement() + abs(tof_right->_offset_x);

    if (tof_left->getMeasurementError() == tof_left->TOF_ERROR_NONE)
    {
        float measurement_angle = compass->keep_in_360_range(this->angle + tof_left->_offset_a);
        point temp_point;
        temp_point.x = (left_dis * cos( (measurement_angle - 90) * DEG_TO_RAD )) + posx;
        temp_point.y = (left_dis * sin( (measurement_angle - 90) * DEG_TO_RAD )) + posy;
        points[int(measurement_angle / 4)] = temp_point;
    }
    if (tof_right->getMeasurementError() == tof_right->TOF_ERROR_NONE)
    {
        float measurement_angle = compass->keep_in_360_range(this->angle + tof_right->_offset_a);
        point temp_point;
        temp_point.x = (right_dis * cos( (measurement_angle - 90) * DEG_TO_RAD )) + posx;
        temp_point.y = (right_dis * sin( (measurement_angle - 90) * DEG_TO_RAD )) + posy;
        points[int(measurement_angle / 4)] = temp_point;
    }
}


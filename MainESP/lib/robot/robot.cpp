#include "robot.h"

Robot::Robot()
{
}

void Robot::init()
{
    // TOF
    tof_left->enable(true); // Enabling both sensors by default
    tof_right->enable(true);

    tof_left->init(); // Setting Pinmodes
    tof_right->init();

    tof_left->holdReset(); // Resetting both sensors
    tof_right->holdReset();
    delay(10);
    tof_left->releaseReset();
    tof_right->releaseReset();
    delay(10);

    tof_left->holdReset(); // Resetting left, so that right can be initialized
    tof_right->begin(I2C_ADDRESS_TOF_RIGHT);
    tof_right->setContinuous(true);
    tof_right->setLongRangeMode(true);

    tof_left->releaseReset(); // Unresetting left, so that left can be initialized
    tof_left->begin(I2C_ADDRESS_TOF_LEFT);
    tof_left->setContinuous(true);
    tof_left->setLongRangeMode(true);

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

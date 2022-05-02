#include "robot.h"

Robot::Robot()
{
}

void Robot::enable(debug_disp* disp)
{
    display = disp;
    
    // I2C Enable
    scan_i2c_addresses();
    print_i2c_addresses();

    motor_left->enable(check_device_enabled(I2C_ADDRESSS_MOTOR_CONTROLLER, "motor_left", "ML"));
    motor_right->enable(check_device_enabled(I2C_ADDRESSS_MOTOR_CONTROLLER, "motor_right", "MR"));

    display->enable(check_device_enabled(I2C_ADDRESS_DISPLAY, "display", "DI"));
    // check_device_enabled(I2C_ADDRESS_IO_EXTENDER, "io-extender", "IO");

    compass->enable(check_device_enabled(I2C_ADDRESS_COMPASS, "compass", "CO"));
    accel_sensor->enable(check_device_enabled(I2C_ADDRESS_ACCELEROMETER, "accelerometer", "AC"));
}

void Robot::init()
{
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

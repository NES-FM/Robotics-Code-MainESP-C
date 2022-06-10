#include "robot.h"

Robot::Robot()
{
}

void Robot::init_tof_xshut()
{
    tof_right->enable(true);
    tof_left->enable(true);
    tof_back->enable(true);
    tof_left->init(); // Setting Pinmodes
    tof_right->init();
    tof_back->init();
    tof_left->holdReset(); // Resetting all sensors
    tof_right->holdReset();
    tof_back->holdReset();
    delay(20);
}

void Robot::init()
{
    // TOF
    log_inline_begin();
    log_inline(" Left: ");
    tof_left->releaseReset(); // Unresetting left, so that left can be initialized
    delay(1000);
    tof_left->begin(I2C_ADDRESS_TOF_LEFT);

    log_inline_begin();
    log_inline("Right: ");
    tof_right->releaseReset(); // Unresetting right, so that right can be initialized
    delay(1000);
    tof_right->begin(I2C_ADDRESS_TOF_RIGHT);

    log_inline_begin();
    log_inline("Back: ");
    tof_back->releaseReset(); // Unresetting back, so that back can be initialized
    delay(1000);
    tof_back->begin(I2C_ADDRESS_TOF_BACK);

    // tof_right->setLongRangeMode(true);
    tof_right->setContinuous(true);
    tof_right->setHighSpeed(true);

    // tof_left->setLongRangeMode(true);
    tof_left->setContinuous(true);
    tof_left->setHighSpeed(true);

    // tof_back->setLongRangeMode(true);
    tof_back->setContinuous(true);
    tof_back->setHighSpeed(true);

    // Others
    motor_left->init(1);
    motor_right->init(2);

    accel_sensor->init();
    compass->init(accel_sensor);

    pos.x_mm = 500;
    pos.y_mm = 400;
}

void Robot::tick()
{
    if (cur_drive_mode == ROBOT_DRIVE_MODE_LINE)
    {

    }
    else if (cur_drive_mode == ROBOT_DRIVE_MODE_ROOM)
    {
        // this->calculate_position();
    }
    logger_tick();
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
    logln("L:%d, R:%d", speed_left, speed_right);
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

    int left_dis = tof_left->getMeasurement() + abs(tof_left->_offset_x);
    int right_dis = tof_right->getMeasurement() + abs(tof_right->_offset_x);
    int back_dis = tof_back->getMeasurement() + abs(tof_back->_offset_x);

    float measurement_angle = 0.0f;
    int point_cloud_index = 0;
    point measurement;
    point measurement_old;

    if (tof_right->getMeasurementError() == tof_right->TOF_ERROR_NONE)
    {
        measurement_angle = compass->keep_in_360_range(this->angle + tof_right->_offset_a);
        point_cloud_index = int(measurement_angle / 3);

        measurement_old = point_cloud[point_cloud_index];
        measurement.x_mm = right_dis + pos.x_mm;
        measurement.y_mm = tof_right->_offset_y + pos.y_mm;
        measurement = rotate_point(measurement, pos, (measurement_angle - 90) * DEG_TO_RAD);

        if (measurement_old.x_mm != 0 || measurement_old.y_mm != 0) // If the Value already exists, taking the average of current and old
        {
            measurement.x_mm = (measurement_old.x_mm + measurement.x_mm) / 2;
            measurement.y_mm = (measurement_old.y_mm + measurement.y_mm) / 2;
        }

        point_cloud[point_cloud_index] = measurement;
    }
    else
    {
        logln("Error with Right Sensor: %s", tof_right->getMeasurementErrorString().c_str());
    }

    if (tof_left->getMeasurementError() == tof_left->TOF_ERROR_NONE)
    {
        measurement_angle = compass->keep_in_360_range(this->angle + tof_left->_offset_a);
        point_cloud_index = int(measurement_angle / 3);

        measurement_old = point_cloud[point_cloud_index];
        measurement.x_mm = left_dis + pos.x_mm;
        measurement.y_mm = tof_left->_offset_y + pos.y_mm;
        measurement = rotate_point(measurement, pos, (measurement_angle - 90) * DEG_TO_RAD);

        if (measurement_old.x_mm != 0 || measurement_old.y_mm != 0) // If the Value already exists, taking the average of current and old
        {
            measurement.x_mm = (measurement_old.x_mm + measurement.x_mm) / 2;
            measurement.y_mm = (measurement_old.y_mm + measurement.y_mm) / 2;
        }

        point_cloud[point_cloud_index] = measurement;
    }
    else
    {
        logln("Error with Left Sensor: %s", tof_left->getMeasurementErrorString().c_str());
    }

    if (tof_back->getMeasurementError() == tof_back->TOF_ERROR_NONE)
    {
        measurement_angle = compass->keep_in_360_range(this->angle + tof_back->_offset_a);
        point_cloud_index = int(measurement_angle / 3);

        measurement_old = point_cloud[point_cloud_index];
        measurement.x_mm = back_dis + pos.x_mm;
        measurement.y_mm = tof_back->_offset_y + pos.y_mm;
        measurement = rotate_point(measurement, pos, (measurement_angle - 90) * DEG_TO_RAD);

        if (measurement_old.x_mm != 0 || measurement_old.y_mm != 0) // If the Value already exists, taking the average of current and old
        {
            measurement.x_mm = (measurement_old.x_mm + measurement.x_mm) / 2;
            measurement.y_mm = (measurement_old.y_mm + measurement.y_mm) / 2;
        }

        point_cloud[point_cloud_index] = measurement;
    }
    else
    {
        logln("Error with Back Sensor: %s", tof_back->getMeasurementErrorString().c_str());
    }
}

Robot::point Robot::rotate_point(point point_to_rotate, point pivot, float angle)
{
    float s = sin(angle);
    float c = cos(angle);

    // translate point back to origin:
    point_to_rotate.x_mm -= pivot.x_mm;
    point_to_rotate.y_mm -= pivot.y_mm;

    // rotate point
    float xnew = point_to_rotate.x_mm * c + point_to_rotate.y_mm * s;
    float ynew = -point_to_rotate.x_mm * s + point_to_rotate.y_mm * c;

    // translate point back:
    point_to_rotate.x_mm = xnew + pivot.x_mm;
    point_to_rotate.y_mm = ynew + pivot.y_mm;
    return point_to_rotate;
}



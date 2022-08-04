#include "robot.h"

Robot::Robot()
{
}

void Robot::init_tof_xshut()
{
    tof_right->enable(true);
    tof_left->enable(true);
    tof_back->enable(true);
    tof_front->enable(false);
    tof_left->init(); // Setting Pinmodes
    tof_right->init();
    tof_back->init();
    tof_front->init();
    tof_left->holdReset(); // Resetting all sensors
    tof_right->holdReset();
    tof_back->holdReset();
    tof_front->holdReset();
    delay(20);
}

void Robot::init()
{
    delay(500); // Let IO Extender fully initialize
    // TOF
    log_inline_begin();
    log_inline("Right: ");
    tof_right->releaseReset(); // Unresetting right, so that right can be initialized
    delay(500);
    tof_right->begin(I2C_ADDRESS_TOF_RIGHT);

    log_inline_begin();
    log_inline("Back: ");
    tof_back->releaseReset(); // Unresetting back, so that back can be initialized
    delay(500);
    tof_back->begin(I2C_ADDRESS_TOF_BACK);

    log_inline_begin();
    log_inline("Left: ");
    tof_left->releaseReset(); // Unresetting left, so that left can be initialized
    delay(500);
    tof_left->begin(I2C_ADDRESS_TOF_LEFT);

    log_inline_begin();
    log_inline("Front: ");
    tof_front->releaseReset(); // Unresetting left, so that left can be initialized
    delay(1000);
    tof_front->begin(I2C_ADDRESS_TOF_FRONT);

    tof_right->setLongRangeMode(true);
    tof_right->setContinuous(false); 
    // tof_right->setHighSpeed(true);

    tof_left->setLongRangeMode(true);
    tof_left->setContinuous(true);
    // tof_left->setHighSpeed(true);

    tof_back->setLongRangeMode(true);
    tof_back->setContinuous(true);
    // tof_back->setHighSpeed(true);

    tof_front->setLongRangeMode(true);
    tof_front->setContinuous(true);
    // tof_front->setHighSpeed(true);

    // Others
    motor_left->init(1);
    motor_right->init(2);

    accel_sensor->init();
    logln("After accel");
    compass->init(accel_sensor);

    pos.x_mm = 500;
    pos.y_mm = 400;
}

void Robot::tick()
{
    // logln("Tick");
    if (cur_drive_mode == ROBOT_DRIVE_MODE_LINE)
    {
        
    }
    else if (cur_drive_mode == ROBOT_DRIVE_MODE_ROOM)
    {
        this->calculate_position();
    }

    String logger_tick_return = logger_tick();
    if (logger_tick_return != "")
    {
        this->parse_command(logger_tick_return);
    }

    if (compass_calibration_background_task_enabled)
    {
        compass->calibrate_background_task();
    }
}

void Robot::tickPinnedMain()
{
    this->calculate_position();
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
    if (!is_control_on_user)
    {
        #ifdef EXTENSIVE_DEBUG
        logln("L:%d, R:%d", speed_left, speed_right);
        #endif
        motor_left->move(speed_left);
        motor_right->move(speed_right);
        // resend_motor_timer.reset();
    }
    else
    {
        #ifdef EXTENSIVE_DEBUG
        logln("move function not activated because is_control_on_user is true");
        #endif
    }
}

void Robot::greifer_home()
{
    greifer_zu->attach(PIN_SERVO1);
    greifer_zu->write(ANGLE_GREIFER_CLOSE_CUBE);
    delay(300);
    greifer_up->attach(PIN_SERVO2);
    greifer_up->write(ANGLE_GREIFER_UP); //NEEDS TO BE CHANGED
}

void Robot::compass_start_calibration_background_task()
{
    if (!compass_calibration_background_task_enabled)
    {
        compass_calibration_background_task_enabled = true;
        compass->start_calibrate_background_task();
    }
}

void Robot::compass_stop_calibration_background_task()
{
    if (compass_calibration_background_task_enabled)
    {
        compass_calibration_background_task_enabled = false;
        compass->stop_calibrate_background_task();
    }
}


void Robot::calculate_position()
{
    this->angle = compass->keep_in_360_range(compass->get_angle() - room_beginning_angle);

    int left_dis = tof_left->getMeasurement() + abs(tof_left->_offset_x);
    int right_dis = tof_right->getMeasurement() + abs(tof_right->_offset_x);
    int back_dis = tof_back->getMeasurement() + abs(tof_back->_offset_x);

    // int lc02_dis = lc02_right->getDistance_mm();

    float measurement_angle = 0.0f;
    int point_cloud_index = 0;
    point measurement;
    point measurement_old;

    if (serial_lidar_mode)
    {
        if (tof_right->getMeasurementError() != tof_right->TOF_ERROR_NONE)
            right_dis = -1;
        if (tof_back->getMeasurementError() != tof_back->TOF_ERROR_NONE)
            back_dis = -1;
        if (tof_left->getMeasurementError() != tof_left->TOF_ERROR_NONE)
            left_dis = -1;
        // if (lc02_right->getErrorCode() != 0)
            // lc02_dis = -1;

        Serial.printf("%f;%d;%d;%d\r\n", this->angle, left_dis, back_dis, right_dis);
    }
    else
    {
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
            // logln("Error with Right Sensor: %s", tof_right->getMeasurementErrorString().c_str());
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
            // logln("Error with Left Sensor: %s", tof_left->getMeasurementErrorString().c_str());
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
            // logln("Error with Back Sensor: %s", tof_back->getMeasurementErrorString().c_str());
        }
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


String Robot::help_command()
{
    String ret = "\r\n";
    ret += "command [required arg] [<optional arg>] [<option_1/option_2>]\r\n";
    ret += "get [sensor] [<subsensor>]\r\n";
    ret += "move [left] [right]\r\n";
    ret += "control [<on/off>]\r\n";
    ret += "set [sensor] [<subsensor>] [value]\r\n";
    ret += "calibrate_compass\r\n";
    ret += "serial_lidar_mode\r\n";
    return ret;
}

String Robot::get_command(String sensor, String subsensor)
{
    char out[255];
    sprintf(out, "unknown error");

    if (sensor == "")
        sprintf(out, "too few arguments");
    else if (sensor == "help")
    {
        String ret = "";
        ret += "--get--\r\n";
        ret += "battery\r\n";
        ret += "dip [<dip1/dip2/wettkampfmodus>]\r\n";
        ret += "accel\r\n";
        ret += "compass\r\n";
        ret += "tof [<back/left/right>]\r\n";
        ret += "drive mode\r\n";
        ret += "-------\r\n";

        sprintf(out, "%s", ret.c_str());
    }
    else if (sensor == "battery")
        sprintf(out, "%fV", bat_voltage->convert_to_battery_voltage());
    else if (sensor == "dip")
    {
        if (subsensor == "")
            sprintf(out, "%s%s%s", dip->get_state(DIP::dip1) ? "1": "0", dip->get_state(DIP::dip2) ? "1": "0", dip->get_state(DIP::wettkampfmodus) ? "1": "0");
        else if (subsensor == "dip1")
            sprintf(out, "%s", dip->get_state(DIP::dip1) ? "1": "0");
        else if (subsensor == "dip2")
            sprintf(out, "%s", dip->get_state(DIP::dip2) ? "1": "0");
        else if (subsensor == "wettkampfmodus")
            sprintf(out, "%s", dip->get_state(DIP::wettkampfmodus) ? "1": "0");
        else
            sprintf(out, "subsensor not found");
    }
    else if (sensor == "accel")
    {
        if (subsensor == "")
            sprintf(out, "%f", accel_sensor->get_roll_degrees());
        else
            sprintf(out, "subsensor not found");
        // ToDo: Other Axis + Ramp modes
    }
    else if (sensor == "compass")
        sprintf(out, "%f", compass->get_angle());
    else if (sensor == "tof")
    {
        if (subsensor == "")
            sprintf(out, "Specifying of subsensor needed");
        else if (subsensor == "right")
            sprintf(out, "%d", tof_right->getMeasurement());
        else if (subsensor == "left")
            sprintf(out, "%d", tof_left->getMeasurement());
        else if (subsensor == "back")
            sprintf(out, "%d", tof_back->getMeasurement());
        // else if (subsensor == "lc02")
            // sprintf(out, "%d", lc02_right->getDistance_mm());
        else
            sprintf(out, "subsensor not found");
    }
    else if (sensor == "drive" && subsensor == "mode")
    {
        if (cur_drive_mode == Robot::ROBOT_DRIVE_MODE_LINE)
            sprintf(out, "line");
        else if (cur_drive_mode == Robot::ROBOT_DRIVE_MODE_ROOM)
            sprintf(out, "room");
        else
            sprintf(out, "unknown drive mode");
    }
    // ToDo: Taster
    return out;
}

String Robot::move_command(String first_arg, String second_arg, String third_arg)
{
    if (!is_control_on_user)
    {
        return "User is not in control! Type \"control on\"";
    }

    char out[64];

    if (first_arg == "help")
    {
        String ret = "\r\n";
        ret += "--move--\r\n";
        ret += "[l speed] [r speed]\r\n";
        ret += "steps [speed] [steps]";
        ret += "--------\r\n";
        return ret;
    }
    else if (first_arg == "steps")
    {
        sprintf(out, "Moving in steps mode with speed: %d and steps: %d", second_arg.toInt(), third_arg.toInt());
        this->motor_left->move_steps(second_arg.toInt(), third_arg.toInt());
        this->motor_right->move_steps(second_arg.toInt(), third_arg.toInt());
        return out;
    }
    
    this->motor_left->move(first_arg.toInt());
    this->motor_right->move(second_arg.toInt());

    sprintf(out, "Succesfully moved with L:%d, R:%d", first_arg.toInt(), second_arg.toInt());
    return out;
}

String Robot::control_command(String on_off)
{
    if (on_off == "")
    {
        is_control_on_user = !is_control_on_user;
        if (is_control_on_user)
            return "User is now in control of robot";
        else
            return "Robot is now in control of itself, User not anymore!";
    }
    else if (on_off == "on")
    {
        is_control_on_user = true;
        return "User is now in control of robot";
    }
    else
    {
        is_control_on_user = false;
        return "Robot is now in control of itself, User not anymore!";
    }
}

String Robot::set_command(String first_arg, String second_arg, String third_arg)
{
    char out[64];
    if (first_arg == "help")
    {
        String ret = "\r\n";
        ret += "--set--\r\n";
        ret += "drive mode [line/room]\r\n";
        ret += "compass calib(ration) [on/off]\r\n";
        ret += "tof <left/back/right> <highspeed/highaccuracy/longrange>\r\n";
        ret += "-------\r\n";
        return ret;
    }
    else if (first_arg == "drive" && second_arg == "mode")
    {
        if (third_arg == "room")
        {
            sprintf(out, "%s", "Setting Drive Mode to Room");
            cur_drive_mode = ROBOT_DRIVE_MODE_ROOM;
        }
        else if (third_arg == "line")
        {
            sprintf(out, "%s", "Setting Drive Mode to Line");
            cur_drive_mode = ROBOT_DRIVE_MODE_LINE;
        }
        else
        {
            sprintf(out, "%s", "Invalid Drive mode");
        }
    }
    else if (first_arg == "compass" && (second_arg == "calib" || second_arg == "calibration"))
    {
        if (third_arg == "")
        {
            if (compass_calibration_background_task_enabled)
                third_arg = "off";
            else
                third_arg = "on";
        }

        if (third_arg == "on")
        {
            sprintf(out, "%s", "Setting calibration background task to ON");
            compass_start_calibration_background_task();
        }
        else if (third_arg == "off")
        {
            sprintf(out, "%s", "Setting calibration background task to OFF");
            compass_stop_calibration_background_task();
        }
    }
    else if (first_arg == "tof")
    {
        if (second_arg == "left")
        {
            if (third_arg == "longrange")
            {
                tof_left->setLongRangeMode(!tof_left->getLongRangeMode());
                sprintf(out, "Sensor.longrange is now %s", tof_left->getLongRangeMode() ? "true" : "false");
            }
            else if (third_arg == "highaccuracy")
            {
                tof_left->setHighAccuracy(!tof_left->getHighAccuracy());
                sprintf(out, "Sensor.highaccuracy is now %s", tof_left->getHighAccuracy() ? "true" : "false");
            }
            else if (third_arg == "highspeed")
            {
                tof_left->setHighSpeed(!tof_left->getHighSpeed());
                sprintf(out, "Sensor.highspeed is now %s", tof_left->getHighSpeed() ? "true" : "false");
            }
        }
        else if (second_arg == "back")
        {
            if (third_arg == "longrange")
            {
                tof_back->setLongRangeMode(!tof_back->getLongRangeMode());
                sprintf(out, "Sensor.longrange is now %s", tof_back->getLongRangeMode() ? "true" : "false");
            }
            else if (third_arg == "highaccuracy")
            {
                tof_back->setHighAccuracy(!tof_back->getHighAccuracy());
                sprintf(out, "Sensor.highaccuracy is now %s", tof_back->getHighAccuracy() ? "true" : "false");
            }
            else if (third_arg == "highspeed")
            {
                tof_back->setHighSpeed(!tof_back->getHighSpeed());
                sprintf(out, "Sensor.highspeed is now %s", tof_back->getHighSpeed() ? "true" : "false");
            }
        }
        else if (second_arg == "right")
        {
            if (third_arg == "longrange")
            {
                tof_right->setLongRangeMode(!tof_right->getLongRangeMode());
                sprintf(out, "Sensor.longrange is now %s", tof_right->getLongRangeMode() ? "true" : "false");
            }
            else if (third_arg == "highaccuracy")
            {
                tof_right->setHighAccuracy(!tof_right->getHighAccuracy());
                sprintf(out, "Sensor.highaccuracy is now %s", tof_right->getHighAccuracy() ? "true" : "false");
            }
            else if (third_arg == "highspeed")
            {
                tof_right->setHighSpeed(!tof_right->getHighSpeed());
                sprintf(out, "Sensor.highspeed is now %s", tof_right->getHighSpeed() ? "true" : "false");
            }
        }
    }
    return out;
}

String Robot::comamnd_template(String arg)
{
    char out[64];
    if (arg == "")
    {
        sprintf(out, "%s", "do sth");
    }
    return out;
}

void Robot::parse_command(String command)
{
    command.trim();
    // logln("Parsing Command: %s", command.c_str());

    splitstring splitted = split_string_at_space(command);

    String top_level_command = splitted.data[0];
    String first_arg = "";
    String second_arg = "";
    String third_arg = "";

    if (splitted.length > 1)
        first_arg = splitted.data[1];
    if (splitted.length > 2)
        second_arg = splitted.data[2];
    if (splitted.length > 3)
        third_arg = splitted.data[3];

    String out;

    top_level_command.toLowerCase();
    first_arg.toLowerCase();
    second_arg.toLowerCase();

    if (top_level_command == "help")
        out = this->help_command();
    else if (top_level_command == "get")
        out = this->get_command(first_arg, second_arg);
    else if (top_level_command == "move")
        out = this->move_command(first_arg, second_arg, third_arg);
    else if (top_level_command == "control")
        out = this->control_command(first_arg);
    else if (top_level_command == "set")
        out = this->set_command(first_arg, second_arg, third_arg);
    else if (top_level_command == "calibrate_compass")
    {
        this->compass->calibrate();
        out = "Done!";
    }
    else if (top_level_command == "serial_lidar_mode")
    {
        serial_lidar_mode = !serial_lidar_mode;
        if (serial_lidar_mode)
        {
            move(10, 10);
            out = "Serial lidar mode is now enabled!";
        }
        else
        {
            out = "Serial lidar mode is now disabled";
        }
    }
    else
        out = "Invalid command: " + top_level_command;

    logln("%s", out.c_str());
}


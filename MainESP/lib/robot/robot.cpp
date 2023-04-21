#include "robot.h"

Robot::Robot(CUART_class* cuart, BCUART_class* bcuart)
{
    cuart_ref = cuart;
    bcuart_ref = bcuart;
}

#define TOF_DELAY_BETWEEN_BEGIN 100

void Robot::init()
{
    claw->init(); // Initializing Claw Servos

    // delay(500); // Let IO Extender fully initialize
    // digitalRead(io_ext_pins::EXT_D13); // Let IO Extender fully initialize

    // Others
    motor_left->init(1);
    motor_right->init(2);

    // accel_sensor->init();
    // logln("After accel");
    // compass->init();

    room_prefs->begin("room", false);
}

uint32_t last_compass_millis = 0;

void Robot::tick()
{
    // logln("Tick");
    if (cur_drive_mode == ROBOT_DRIVE_MODE_LINE)
    {
        
    }
    else if (cur_drive_mode == ROBOT_DRIVE_MODE_ROOM)
    {
        
    }

    String logger_tick_return = logger_tick();
    if (logger_tick_return != "")
    {
        this->parse_command(logger_tick_return);
    }

    if (taster->get_state(taster_class::reset_nvs))
    {
        logln("reset");
        room_prefs->putBool("silver_1", false);
        room_prefs->putBool("silver_2", false);
        room_prefs->putBool("black", false);
        room_prefs->putBool("blue", false);
        logln("reset done");
    }
}

void Robot::print_balls()
{
    logln("----- Balls -----");
    for (int i = 0; i < num_detected_balls; i++)
    {
        ball b = detected_balls[i];
        logln("%d: D=%.3f C=%.2f N=%d %s", i, b.distance, b.conf, b.num_hits, b.black ? "black" : "silver");
    }
    logln("-----------------");
}

void Robot::print_corners()
{
    logln("---- Corners ----");
    for (corner* c : possible_corners)
    {
        logln("D=%.3f C=%.2f N=%d", c->distance, c->conf, c->num_hits);
    }
    logln("-----------------");
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

Robot::point Robot::rotate_point(point point_to_rotate, point pivot, float angle_degrees)
{
    // translate point back to rotate around origin:
    point_to_rotate.x_mm -= pivot.x_mm;
    point_to_rotate.y_mm -= pivot.y_mm;

    point pnew = rotate_point_around_origin(point_to_rotate, angle_degrees);

    // translate point back:
    point_to_rotate.x_mm = pnew.x_mm + pivot.x_mm;
    point_to_rotate.y_mm = pnew.y_mm + pivot.y_mm;
    return point_to_rotate;
}

Robot::point Robot::rotate_point_around_origin(point point_to_rotate, float angle_degrees)
{
    float angle = DEG_TO_RAD * angle_degrees;
    float s = sin(angle);
    float c = cos(angle);

    // rotate point
    float xnew = float(point_to_rotate.x_mm) * c + float(point_to_rotate.y_mm) * s;
    float ynew = float(-point_to_rotate.x_mm) * s + float(point_to_rotate.y_mm) * c;

    point pnew;
    pnew.x_mm = xnew;
    pnew.y_mm = ynew;

    return pnew;
}

float Robot::distance_between_points(point p1, point p2)
{
    float dx = p1.x_mm - p2.x_mm;
    float dy = p1.y_mm - p2.y_mm;
    return sqrtf(dx*dx + dy*dy);
}

float Robot::x_distance_between_points(point p1, point p2)
{
    return abs(p1.x_mm - p2.x_mm);
}
float Robot::y_distance_between_points(point p1, point p2)
{
    return abs(p1.y_mm - p2.y_mm);
}

Robot::point Robot::midpoint_between_points(point p1, point p2)
{
    point return_point;
    return_point.x_mm = (p1.x_mm + p2.x_mm) / 2;
    return_point.y_mm = (p1.y_mm + p2.y_mm) / 2;

    return return_point;
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
        ret += "tof [<back/left/right/closerange>]\r\n";
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
            sprintf(out, "%f", io_ext->get_roll_degrees());
        else
            sprintf(out, "subsensor not found");
        // ToDo: Other Axis + Ramp modes
    }
    else if (sensor == "tof")
    {
        if (subsensor == "")
        {
            sprintf(out, "Specifying of subsensor needed");
        }
        else if (subsensor == "claw")
        {
            uint16_t measurement = io_ext->claw_getMeasurement();
            sprintf(out, "%d", measurement);
        }
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
    #ifdef comm_v2
    else if (first_arg == "steps")
    {
        sprintf(out, "Moving in steps mode with speed: %d and steps: %d", second_arg.toInt(), third_arg.toInt());
        this->motor_left->move_steps(second_arg.toInt(), third_arg.toInt());
        this->motor_right->move_steps(second_arg.toInt(), third_arg.toInt());
        return out;
    }
    #endif
    
    this->motor_left->move(first_arg.toInt());
    this->motor_right->move(second_arg.toInt());

    sprintf(out, "Succesfully moved with L:%d, R:%d", first_arg.toInt(), second_arg.toInt());
    return out;
}

// String Robot::rotate_command(String degrees)
// {
//     if (!is_control_on_user)
//     {
//         return "User is not in control! Type \"control on\"";
//     }

//     is_control_on_user = false;
//     this->room_rotate_relative_degrees(degrees.toFloat());
//     move(0, 0);
//     is_control_on_user = true;

//     char out[64];
//     sprintf(out, "Succesfully rotated %f degrees", degrees.toFloat());
//     return out;
// }

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
    char out[256];
    if (first_arg == "help")
    {
        String ret = "\r\n";
        ret += "--set--\r\n";
        ret += "drive mode [line/room]\r\n";
        ret += "compass calib(ration) [on/off]\r\n";
        ret += "corner <TL/TR/BL/BR>\r\n";
        ret += "claw <servo_up/servo_close/blue_cube> angle\r\n";
        ret += "claw state <states>\r\n";
        ret += "-------\r\n";
        return ret;
    }
    else if (first_arg == "drive" && second_arg == "mode")
    {
        if (third_arg == "room")
        {
            sprintf(out, "%s", "Setting Drive Mode to Room");
            cur_drive_mode = ROBOT_DRIVE_MODE_ROOM;
            cur_room_state = ROOM_STATE_DEFAULT;
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
    else if (first_arg == "claw")
    {
        if (second_arg == "servo_up")
        {
            int angle = third_arg.toInt();
            claw->_set_raw_servo_up_state(angle);
            return "Successfully set Claw Up Servo!\r\n";
        }
        else if (second_arg == "servo_close")
        {
            int angle = third_arg.toInt(); 
            claw->_set_raw_servo_close_state(angle);
            return "Successfully set Claw Close Servo!\r\n";
        }
        else if (second_arg == "blue_cube")
        {
            int angle = third_arg.toInt(); 
            claw->_set_raw_servo_blue_cube_state(angle);
            return "Successfully set Blue Cube Servo!\r\n";
        }
        else if (second_arg == "state")
        {
            // Map the string to the corresponding enum value
            Claw::State state;
            if (third_arg == "bottom_open")
                state = Claw::BOTTOM_OPEN;
            else if (third_arg == "bottom_mid")
                state = Claw::BOTTOM_MID;
            else if (third_arg == "bottom_closed")
                state = Claw::BOTTOM_CLOSED;
            else if (third_arg == "side_closed")
                state = Claw::SIDE_CLOSED;
            else if (third_arg == "top_closed")
                state = Claw::TOP_CLOSED;
            else if (third_arg == "top_open")
                state = Claw::TOP_OPEN;
            else
            {
                return "\r\nInvalid Input! Valid states are: BOTTOM_OPEN, BOTTOM_CLOSED, SIDE_CLOSED, TOP_CLOSED, TOP_OPEN\r\n";
            }

            // Set the claw state
            claw->set_state(state);
            return "Successfully set new Claw State!\r\n";
        }
        else
        {
            return "\r\nInvalid command! See Help:\r\n" + set_command("help", "", "");
        }
    }
    else
    {
        return "\r\nInvalid command! See Help:\r\n" + set_command("help", "", "");
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

String Robot::heartbeat_command(String arg)
{
    return "heartbeat " + arg;
}

String Robot::balls_command(String arg)
{
    if (arg == "on")
        detectingBallsEnabled = true;
    else if (arg == "off")
        detectingBallsEnabled = false;
    else if (arg == "print")
        this->print_balls();
    else if (arg == "auto")
        cur_room_state = ROOM_STATE_ROTATE_TO_FIND_BALLS;
    else
    {
        String ret = "";
        ret += "-- balls --\r\n";
        ret += "balls <on/off>\r\n";
        ret += "balls print\r\n";
        ret += "balls auto\r\n";
        ret += "\r\n";
        return ret;
    }
    return "Success!";
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
    third_arg.toLowerCase();

    if (top_level_command == "help")
        out = this->help_command();
    else if (top_level_command == "get")
        out = this->get_command(first_arg, second_arg);
    else if (top_level_command == "move")
        out = this->move_command(first_arg, second_arg, third_arg);
    // else if (top_level_command == "rotate")
    //     out = this->rotate_command(first_arg);
    else if (top_level_command == "control")
        out = this->control_command(first_arg);
    else if (top_level_command == "set")
        out = this->set_command(first_arg, second_arg, third_arg);
    else if (top_level_command == "balls")
        out = this->balls_command(first_arg);
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
    else if (top_level_command == "heartbeat")
        heartbeat_command(first_arg);
    else
        out = "Invalid command: " + top_level_command;

    logln("%s", out.c_str());
}


// Room

// #define ROOM_MOVE_ALONG_WALL_DISTANCE 80
// #define ROOM_MOVE_ALONG_WALL_LINEAR_FACTOR 0.43f

// void Robot::room_move_along_wall()
// {
//     //////// General todo: tof_closereange average

//     uint16_t closerange_dis = tof_closerange->getMeasurement();
//     if (tof_closerange->getMeasurementError() != tof::TOF_ERROR_MAX_DISTANCE) // Closerange needs to see the wall, or else it cant work
//     {
//         int motor_l_val = DRIVE_SPEED_RAUM;
//         int motor_r_val = DRIVE_SPEED_RAUM;

//         if (tof_closerange->getMeasurementError() == tof::TOF_ERROR_NONE)
//         {
//             float error = ROOM_MOVE_ALONG_WALL_DISTANCE - closerange_dis;

//             motor_l_val = float(DRIVE_SPEED_RAUM) - ROOM_MOVE_ALONG_WALL_LINEAR_FACTOR * error;
//             motor_r_val = float(DRIVE_SPEED_RAUM) + ROOM_MOVE_ALONG_WALL_LINEAR_FACTOR * error;
//         }

//         move(motor_l_val, motor_r_val);
//     }
// }

void Robot::startRoom()
{
    move(0, 0);
    delay(1000);

    if (cuart_ref->array_total < 5)
    {
        cur_drive_mode = ROBOT_DRIVE_MODE_ROOM;
        main_buzzer->noTone();
        cuart_ref->silver_line = false;
        cuart_ref->green_line = false;

        // setRoomBeginningAngle();

        cur_room_state = ROOM_STATE_FIND_WALL_DRIVE_TO_CENTER;
        prev_room_state = ROOM_STATE_DEFAULT;
    }
}

Robot::room_end_types Robot::room_has_reached_end()
{
    if (taster->get_state(taster->front_right))
        return room_end_types::ROOM_HAS_REACHED_TASTER_RIGHT;
    else if (taster->get_state(taster->front_left))
        return room_end_types::ROOM_HAS_REACHED_TASTER_LEFT;
    else if (cuart_ref->silver_line)
        return room_end_types::ROOM_HAS_REACHED_SILVER_LINE;
    else if (cuart_ref->green_line)
        return room_end_types::ROOM_HAS_REACHED_GREEN_LINE;
    return room_end_types::ROOM_HAS_NOT_REACHED_END;
}

// void Robot::room_rotate_to_degrees(float degrees, bool rotate_right)
// {
//     if (rotate_right) // rotate right
//     {
//         move(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
//     }
//     else // rotate left
//     {
//         move(-DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
//     }

//     while(abs(compass->get_angle() - degrees) > 10)
//     {
//         delay(5);
//     }
// }

// void Robot::room_rotate_relative_degrees(float degrees)
// {
//     // move(0, 0);

//     // float starting_point = compass->get_angle();

//     // int num_values = 1;
    
//     // for (int i = 0; i < 200; i += 10)
//     // {
//     //     starting_point += compass->get_angle();
//     //     num_values += 1;
//     //     delay(10);
//     // }

//     // starting_point = (starting_point / num_values);

//     // float target_degrees = compass->keep_in_360_range(starting_point + degrees);

//     // logln("Rotating relative degrees: %f from start %f and target %f", degrees, starting_point, target_degrees);

//     // room_rotate_to_degrees(target_degrees, degrees > 0); // Rotate right if degrees > 0

//     if (degrees > 0) // rotate right
//     {
//         move(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
//     }
//     else // rotate left
//     {
//         move(-DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
//     }

//     delay((abs(degrees) / 360) * robot_millis_per_360_at_30_speed);
// }

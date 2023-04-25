#include "moving_in_room.h"

bool moving_in_room_follow_ball::tick(uint32_t delta_time)
{
    logln("Follow ball tick");
    _robot->claw->set_state(Claw::BOTTOM_MID);
    
    if (!moving_to_balls_ball_too_close)
    {
        if (_bcuart->num_balls_in_array > 0)
        {
            // Parse all received balls to find most likely to be the same as target and get x_offset
            float closest_distance = 99999.9;
            float closest_ball_x_offset = 0.0;
            float closest_ball_y_offset = 0.0;
            Robot::point irl_pos;
            for (int i = 0; i < _bcuart->num_balls_in_array; i++)
            {
                BCUART_class::ball received_ball = _bcuart->received_balls[i];
                irl_pos.x_mm = -received_ball.x_offset*10; // *10, because x_offset is in cm, while irl_pos is in mm  // - because negative x_offset means right of robot, if robot is pointing forward
                irl_pos.y_mm = -received_ball.distance*10 - (0.5*_robot->height); // - because the robot detects balls towards the back  // - (0.5*height) because b.distance counts starting from the back edge of the robot
                // irl_pos = Robot::rotate_point_around_origin(irl_pos, _robot->angle);

                float dist = abs(_robot->moving_to_balls_target.distance - received_ball.distance); // TODO: Use conf instead of distance
                if (dist < closest_distance)
                {
                    closest_distance = dist;
                    closest_ball_x_offset = -received_ball.x_offset;
                    closest_ball_y_offset = received_ball.distance;
                }
            }
            _bcuart->reset_balls();

            float robot_to_ball_dis = closest_ball_y_offset;

            logln("Following Ball with x_off: %f, y_off: %f, distance to predicted: %f, Robot to ball dis: %f", closest_ball_x_offset, closest_ball_y_offset, closest_distance, robot_to_ball_dis);

            if (closest_ball_y_offset <= 12) // Ball is too close to be detected
            {
                logln("\r\n\r\nGoing into balls too close mode\r\n");
                moving_to_balls_ball_too_close = true;
            }

            _robot->moving_to_balls_target.distance = closest_ball_y_offset;

            // TODO: Correct less if far away

            if (closest_ball_x_offset > 3) // If more than 2 cm deviation from middle line, correct for it
            {
                logln("Correcting with -5 5, because x_offset:%.3f is > 3", closest_ball_x_offset);
                _robot->move(-5, 5);
            }
            else if (closest_ball_x_offset < -3) // If more than 2 cm deviation from middle line, correct for it
            {
                logln("Correcting with 5 -5, because x_offset:%.3f is < -3", closest_ball_x_offset);
                _robot->move(5, -5);
            }
            else
            {
                logln("Driving with %d %d", motor_left_speed, motor_right_speed);
                _robot->move(motor_left_speed, motor_right_speed);
            }
        }
    }
    else
    {
        _robot->move(motor_left_speed, motor_right_speed);   
    }

    uint16_t ball_sensor_distance = _robot->io_ext->claw_getMeasurement();
    logln("Ball Sensor Distance: %d", ball_sensor_distance);
    if (ball_sensor_distance < 18)
    {
        logln("Ball is close enough -> end");
        delay(200);
        return true;
    }

    return false;
}

bool moving_in_room_follow_corner::tick(uint32_t delta_time)
{
    logln("Follow corner tick");

    if (!moving_to_corner_corner_too_close)
    {
        if (_bcuart->corner_valid)
        {
            auto recieved_corner = _bcuart->received_corner;
            Robot::point irl_pos;
            irl_pos.x_mm = -recieved_corner.x_offset*10; // *10, because x_offset is in cm, while irl_pos is in mm  // - because negative x_offset means right of robot, if robot is pointing forward
            irl_pos.y_mm = -recieved_corner.distance*10 - (0.5*_robot->height); // - because the robot detects corner towards the back  // - (0.5*height) because b.distance counts starting from the back edge of the robot
            // irl_pos = Robot::rotate_point_around_origin(irl_pos, _robot->angle);

            float closest_corner_x_offset = -recieved_corner.x_offset;
            float closest_corner_y_offset = recieved_corner.distance;

            _bcuart->reset_corner();

            float robot_to_corner_dis = Robot::distance_between_points(_robot->Origin, irl_pos);

            logln("Following Corner with x_off: %f, y_off: %f, screen_w: %d, Robot to corner dis: %f", closest_corner_x_offset, closest_corner_y_offset, recieved_corner.screen_w, robot_to_corner_dis);

            if (recieved_corner.screen_w > 200 || closest_corner_y_offset <= 20) // Corner is too close to be detected
            {
                logln("\r\n\r\nGoing into corner too close mode\r\n");
                moving_to_corner_corner_too_close = true;
            }

            
            if (closest_corner_x_offset > 4)//4+4) // If more than 2 cm deviation from 4cm line, correct for it
            {
                logln("Correcting with -5 5, because x_offset:%.3f is > 3", closest_corner_x_offset);
                _robot->move(-5, 5);
            }
            else if (closest_corner_x_offset < -4)//4-4) // If more than 2 cm deviation from 4cm line, correct for it
            {
                logln("Correcting with 5 -5, because x_offset:%.3f is < -3", closest_corner_x_offset);
                _robot->move(5, -5);
            }
            else
            {
                logln("Driving with %d %d", motor_left_speed, motor_right_speed);
                _robot->move(motor_left_speed, motor_right_speed);
            }
        }
    }
    else
    {
        _robot->claw->set_state(Claw::TOP_CLOSED);
        _robot->move(motor_left_speed, motor_right_speed);

        if (time_after <= delta_time) // Done!
            return true;
        else
            time_after -= delta_time;
        return false;
    }

    return false;
}

// bool moving_in_room_rotate_to_deg::tick(uint32_t delta_time)
// {
//     _robot->move(motor_left_speed, motor_right_speed);
//     logln("Rotate to deg tick (Cur: %f Target: %f) with speed %d %d", _robot->angle, target_angle, motor_left_speed, motor_right_speed);
//     bool ret = (abs(_robot->angle - target_angle) < ROTATE_TO_ANGLE_TOLERANCE);
//     if ((ROTATE_TO_ANGLE_TOLERANCE - target_angle) > 0)
//         ret = ret || _robot->angle > 360 - (ROTATE_TO_ANGLE_TOLERANCE - target_angle);
//     return ret; // Angle Reached
// }

// bool moving_in_room_rotate_relative_degrees::tick(uint32_t delta_time)
// {
//     if (first_time)
//     {
//         first_time = false;
//         rotate_to_deg = new moving_in_room_rotate_to_deg();
//         rotate_to_deg->_robot = _robot;
//         rotate_to_deg->motor_left_speed = motor_left_speed;
//         rotate_to_deg->motor_right_speed = motor_right_speed;
//         rotate_to_deg->target_angle = compass_bmm::keep_in_360_range(_robot->angle + target_relative_angle);
//     }

//     return rotate_to_deg->tick(delta_time);
// }

bool moving_in_room_distance_by_time::tick(uint32_t delta_time)
{
    _robot->move(motor_left_speed, motor_right_speed);
    logln("distance by time tick. Time left: %d, delta Time: %d with speed %d %d", time_left, delta_time, motor_left_speed, motor_right_speed);
    if (time_left <= delta_time) // Done!
    {
        return true;
    }
    else
        time_left -= delta_time;
    return false;
}
void moving_in_room_distance_by_time::calculate_time_by_distance(int distance)
{
    float speed_scale = (float)(abs(motor_left_speed) + abs(motor_right_speed)) / 2.0 / 40.0; // Adjust for not driving with 40 speed
    time_left = constrain(distance / (_robot->millimeters_per_millisecond_40_speed * speed_scale), 0, UINT32_MAX);
}

bool moving_in_room_pick_up_ball::tick(uint32_t delta_time)
{
    _robot->move(0, 0);
    _robot->claw->set_state(Claw::BOTTOM_CLOSED);
    _robot->move(10, 10);
    delay(1000);
    _robot->move(-20, -20);
    delay(500);
    // _robot->claw->set_state(Claw::SIDE_CLOSED);
    _robot->move(0, 0);

    return true;
}

bool moving_in_room_goto_room_state::tick(uint32_t delta_time)
{
    _robot->move(0, 0);
    _robot->cur_room_state = target_room_state;
    return true;
}

bool moving_in_room_set_claw::tick(uint32_t delta_time)
{
    _robot->move(0, 0);
    _robot->claw->set_state(claw_state, force);
    return true;
}

bool moving_in_room_until_taster::tick(uint32_t delta_time)
{
    _robot->move(motor_left_speed, motor_right_speed);
    logln("move until taster tick with speed %d %d", motor_left_speed, motor_right_speed);

    Robot::room_end_types room_end = _robot->room_has_reached_end();
    taster_activated = room_end == Robot::ROOM_HAS_REACHED_TASTER_LEFT || room_end == Robot::ROOM_HAS_REACHED_TASTER_RIGHT;

    if (taster_activated)
    {
        if (continue_afterwards <= delta_time) // Done!
        {
            return true;
        }
        else
            continue_afterwards -= delta_time;
    }

    return false;
}

std::vector<moving_in_room_step*> moving_in_room_queue;

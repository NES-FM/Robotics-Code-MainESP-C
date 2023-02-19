#include "moving_in_room.h"

bool moving_in_room_follow_ball::tick(uint32_t delta_time)
{
    logln("Follow ball tick with speeds %d %d", motor_left_speed, motor_right_speed);
    _robot->claw->set_state(Claw::BOTTOM_MID);
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
            irl_pos = Robot::rotate_point_around_origin(irl_pos, _robot->angle);

            float dist = Robot::distance_between_points(irl_pos, _robot->moving_to_balls_target.pos);
            if (dist < closest_distance)
            {
                closest_distance = dist;
                closest_ball_x_offset = received_ball.x_offset;
                closest_ball_y_offset = received_ball.distance;
            }
        }
        _bcuart->reset_balls();

        float robot_to_ball_dis = Robot::distance_between_points(_robot->Origin, _robot->moving_to_balls_target.pos);

        logln("Following Ball with x_off: %f, y_off: %f, distance to predicted: %f, Robot to ball dis: %f", closest_ball_x_offset, closest_ball_y_offset, closest_distance, robot_to_ball_dis);

        if (!moving_to_balls_ball_too_close && robot_to_ball_dis <= 250) // Ball is too close to be detected
        {
            logln("\r\n\r\nGoing into balls too close mode\r\n");
            moving_to_balls_ball_too_close = true;
        }

        if (moving_to_balls_ball_too_close)
        {
            uint16_t ball_sensor_distance = _robot->claw->get_ball_distance();
            logln("Ball Sensor Distance: %d", ball_sensor_distance);
            if (ball_sensor_distance < 18)  // TODO: Some weird bug, where ball is placed perfectly but distance is always ~60, until it jumps to 0 way too late
            {
                logln("Ball is close enough -> end");
                delay(200);
                return true;
            }
        }
        else
        {
            _robot->moving_to_balls_target.pos = irl_pos;

            if (closest_ball_x_offset > 2) // If more than 2 cm deviation from middle line, correct for it
            {
                _robot->move(-5, 5);
            }
            else if (closest_ball_x_offset < -2) // If more than 2 cm deviation from middle line, correct for it
            {
                _robot->move(5, -5);
            }
        }
    }

    return false;
}

bool moving_in_room_rotate_to_deg::tick(uint32_t delta_time)
{
    logln("Rotate to deg tick (Cur: %f Target: %f) with speed %d %d", _robot->angle, target_angle, motor_left_speed, motor_right_speed);
    return (abs(_robot->angle - target_angle) < ROTATE_TO_ANGLE_TOLERANCE); // Angle Reached
}

bool moving_in_room_distance_by_time::tick(uint32_t delta_time)
{
    logln("distance by time tick. Time left: %d, delta Time: %d with speed %d %d", time_left, delta_time, motor_left_speed, motor_right_speed);
    if (time_left <= delta_time) // Done!
    {
        return true;
    }
    else
        time_left -= delta_time;
    return false;
}

bool moving_in_room_pick_up_ball::tick(uint32_t delta_time)
{
    _robot->move(0, 0);
    _robot->claw->set_state(Claw::BOTTOM_CLOSED);
    _robot->move(10, 10);
    delay(50);
    _robot->claw->set_state(Claw::SIDE_CLOSED);
    _robot->move(0, 0);

    return true;
}

bool moving_in_room_goto_room_state::tick(uint32_t delta_time)
{
    _robot->cur_room_state = target_room_state;
    return true;
}

std::vector<moving_in_room_step*> moving_in_room_queue;

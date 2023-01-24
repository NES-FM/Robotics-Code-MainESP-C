// mm/ms at 40 speed
#define MILLIMETERS_PER_MILLISECOND 0.1770833333

#define ROTATE_TO_ANGLE_TOLERANCE 5

float rotate_balls_360_start_angle = 0;
bool rotated_balls_over_10_degrees = false;

void adjust_moving_to_balls_target(uint32_t delta_time);

bool moving_to_balls_ball_too_close = false;

uint32_t last_millis;
void drive_room()
{
    if (robot.cur_room_state == robot.ROOM_STATE_DEFAULT)
    {
        if (robot.prev_room_state != robot.ROOM_STATE_DEFAULT)
        {
            robot.move(0, 0);
            robot.prev_room_state = robot.ROOM_STATE_DEFAULT;
        }
    }
    else if (robot.cur_room_state == robot.ROOM_STATE_ROTATE_TO_FIND_BALLS)
    {
        float angle = robot.compass->get_angle();
        if (robot.prev_room_state != robot.ROOM_STATE_ROTATE_TO_FIND_BALLS)
        {
            robot.move(-5, 5);
            robot.prev_room_state = robot.ROOM_STATE_ROTATE_TO_FIND_BALLS;
            robot.detectingBallsEnabled = true;
            rotate_balls_360_start_angle = angle;
            rotated_balls_over_10_degrees = false;
            robot.moving_to_balls_queue.clear();
        }

        if (abs(angle - rotate_balls_360_start_angle) > 10)
            rotated_balls_over_10_degrees = true;

        if (rotated_balls_over_10_degrees && abs(angle - rotate_balls_360_start_angle) < ROTATE_TO_ANGLE_TOLERANCE)  // Rotated approx. 360 degrees -> goto next step
        {
            robot.detectingBallsEnabled = false;
            robot.move(0, 0);
            robot.print_balls();

            last_millis = millis();
            robot.cur_room_state = robot.ROOM_STATE_MOVING_TO_BALL;

            float max_conf = 0.0;
            int max_conf_index;

            for (int i = 0; i < robot.num_detected_balls; i++)
            {
                float ball_conf = robot.detected_balls[i].conf * (float)robot.detected_balls[i].num_hits;
                if (ball_conf > max_conf)
                {
                    max_conf = ball_conf;
                    max_conf_index = i;
                }
            }
            robot.moving_to_balls_target = robot.detected_balls[max_conf_index]; // TODO: remember how many silver & black balls were put down -> only get black ball if 2 silver were put down

            // if (abs(robot.moving_to_balls_target.pos.y_mm) <= 200) // If ball is less than 20cm away of middle, go directly to ball
            // {
                // Step 1: Rotate to ball
                Robot::moving_to_balls_step rotate_to_ball;
                rotate_to_ball.motor_left_speed = 20;
                rotate_to_ball.motor_right_speed = -20;
                float target_angle_rad = abs(atan(robot.moving_to_balls_target.pos.x_mm / robot.moving_to_balls_target.pos.y_mm));
                if (robot.moving_to_balls_target.pos.x_mm < 0)
                {
                    if (robot.moving_to_balls_target.pos.y_mm > 0) // TL
                    {
                        target_angle_rad = PI - target_angle_rad;
                    }
                }
                else
                {
                    if (robot.moving_to_balls_target.pos.y_mm > 0) // TR
                    {
                        target_angle_rad = PI + target_angle_rad;
                    }
                    else // BR
                    {
                        target_angle_rad = 2*PI - target_angle_rad;
                    }
                }
                rotate_to_ball.target_angle = target_angle_rad * RAD_TO_DEG;
                robot.moving_to_balls_queue.push_back(rotate_to_ball);
                logln("Rotating to ball with %d|%d (Target: %f°)", 20, -20, target_angle_rad * RAD_TO_DEG);

                // Step 2: Move to ball using cam
                Robot::moving_to_balls_step move_to_ball;
                move_to_ball.motor_left_speed = -20;
                move_to_ball.motor_right_speed = -20;
                move_to_ball.follow_ball = true;
                robot.moving_to_balls_queue.push_back(move_to_ball);
            // }
            // TODO: else if ball is not within 20cm of center line (ie next to wall)

            logln("Detecting balls finnished, waiting for 5 secs before continuing...");
            for (int i = 0; i < 5000; i+=100)
            {
                robot.tick();
                display.tick();
                delay(100);
            }
        }
    }
    else if (robot.cur_room_state == robot.ROOM_STATE_MOVING_TO_BALL)
    {
        if (robot.prev_room_state != robot.ROOM_STATE_MOVING_TO_BALL)
        {
            last_millis = millis();
            robot.prev_room_state = robot.cur_room_state;
            moving_to_balls_ball_too_close = false;
        }

        uint32_t delta_time = millis() - last_millis;
        last_millis = millis();
        adjust_moving_to_balls_target(delta_time);

        if (!robot.moving_to_balls_queue.empty())
        {
            Robot::moving_to_balls_step* step = &robot.moving_to_balls_queue.front();
            robot.move(step->motor_left_speed, step->motor_right_speed);
            if (step->follow_ball) // Follow Ball with Cam mode
            {
                robot.claw->set_state(Claw::BOTTOM_MID);
                if (bcuart.num_balls_in_array > 0)
                {
                    // Parse all received balls to find most likely to be the same as target and get x_offset
                    float closest_distance = 99999.9;
                    float closest_ball_x_offset = 0.0;
                    float closest_ball_y_offset = 0.0;
                    Robot::point irl_pos;
                    for (int i = 0; i < bcuart.num_balls_in_array; i++)
                    {
                        BCUART_class::ball received_ball = bcuart.received_balls[i];
                        irl_pos.x_mm = -received_ball.x_offset*10; // *10, because x_offset is in cm, while irl_pos is in mm  // - because negative x_offset means right of robot, if robot is pointing forward
                        irl_pos.y_mm = -received_ball.distance*10 - (0.5*robot.height); // - because the robot detects balls towards the back  // - (0.5*height) because b.distance counts starting from the back edge of the robot
                        irl_pos = Robot::rotate_point_around_origin(irl_pos, robot.angle);

                        float dist = Robot::distance_between_points(irl_pos, robot.moving_to_balls_target.pos);
                        if (dist < closest_distance)
                        {
                            closest_distance = dist;
                            closest_ball_x_offset = received_ball.x_offset;
                            closest_ball_y_offset = received_ball.distance;
                        }
                    }
                    bcuart.reset_balls();

                    float robot_to_ball_dis = Robot::distance_between_points(robot.Origin, robot.moving_to_balls_target.pos);

                    logln("Following Ball with x_off: %f, y_off: %f, distance to predicted: %f, Robot to ball dis: %f", closest_ball_x_offset, closest_ball_y_offset, closest_distance, robot_to_ball_dis);

                    if (!moving_to_balls_ball_too_close && robot_to_ball_dis <= 250) // Ball is too close to be detected
                    {
                        logln("\r\n\r\nGoing into balls too close mode\r\n");
                        moving_to_balls_ball_too_close = true;
                    }

                    if (moving_to_balls_ball_too_close)
                    {
                        uint16_t ball_sensor_distance = robot.claw->get_ball_distance();
                        logln("Ball Sensor Distance: %d", ball_sensor_distance);
                        if (ball_sensor_distance < 18)  // TODO: Some weird bug, where ball is placed perfectly but distance is always ~60, until it jumps to 0 way too late
                        {
                            logln("Ball is close enough -> end");
                            delay(200);
                            robot.moving_to_balls_queue.pop_front();
                            if (robot.moving_to_balls_queue.empty())
                                robot.move(0, 0);
                        }
                    }
                    else
                    {
                        robot.moving_to_balls_target.pos = irl_pos;

                        if (closest_ball_x_offset > 2) // If more than 2 cm deviation from middle line, correct for it
                        {
                            robot.move(-5, 5);
                        }
                        else if (closest_ball_x_offset < -2) // If more than 2 cm deviation from middle line, correct for it
                        {
                            robot.move(5, -5);
                        }
                        else
                        {
                            robot.move(step->motor_left_speed, step->motor_right_speed);
                        }
                    }
                }
            }
            else if (step->target_angle != -1) // Rotate to Target Angle Mode
            {
                if (abs(robot.angle - step->target_angle) < ROTATE_TO_ANGLE_TOLERANCE) // Done!
                {
                    robot.moving_to_balls_queue.pop_front();
                    if (robot.moving_to_balls_queue.empty())
                        robot.move(0, 0);
                }
            }
            else // Move straight Distance (time) mode
            {
                logln("Time left: %d, delta Time: %d", step->time_left, delta_time);
                if (step->time_left <= delta_time) // Done!
                {
                    robot.moving_to_balls_queue.pop_front();
                    if (robot.moving_to_balls_queue.empty())
                        robot.move(0, 0);
                }
                else
                    step->time_left -= delta_time;
            }
        }
        else
        {
            robot.move(0, 0);
            robot.claw->set_state(Claw::BOTTOM_CLOSED);
            robot.move(10, 10);
            delay(50);
            robot.claw->set_state(Claw::SIDE_CLOSED);
            robot.move(0, 0);

            robot.cur_room_state = robot.ROOM_STATE_PUTTING_BALL_IN_CORNER;
        }
    }
    else if (robot.cur_room_state == robot.ROOM_STATE_PUTTING_BALL_IN_CORNER)
    {
        if (robot.prev_room_state != robot.ROOM_STATE_PUTTING_BALL_IN_CORNER)
        {
            robot.move(0, 0);
            robot.prev_room_state = robot.cur_room_state;
        }

        // TODO: Bring ball to corner
    }
}

void adjust_moving_to_balls_target(uint32_t delta_time)
{
    int motor_left_speed = robot.motor_left->motor_speed;
    int motor_right_speed = robot.motor_right->motor_speed;
    if (motor_left_speed == motor_right_speed && motor_left_speed != 0)
    {
        float speed_scale = abs((float)motor_left_speed) / 40.0; // Adjust for not driving with 40 speed
        float delta_distance = (double)delta_time * MILLIMETERS_PER_MILLISECOND * speed_scale;

        // convert angle to radians
        float angle_rad = robot.angle * M_PI / 180.0;

        if (motor_left_speed > 0) // If moving forward, move the ball backwards
        {
            // calculate the opposite angle by adding 180 degrees
            angle_rad = angle_rad + M_PI;
        }

        // calculate new x and y coordinates
        robot.moving_to_balls_target.pos.x_mm = robot.moving_to_balls_target.pos.x_mm + delta_distance * cos(angle_rad);
        robot.moving_to_balls_target.pos.y_mm = robot.moving_to_balls_target.pos.y_mm + delta_distance * sin(angle_rad);
    }
}
 
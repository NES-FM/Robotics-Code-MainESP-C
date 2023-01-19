// mm/ms at 40 speed
#define MILLIMETERS_PER_MILLISECOND 0.1770833333

#define ROTATE_360_TOLERANCE 5

float rotate_balls_360_start_angle = 0;
bool rotated_balls_over_10_degrees = false;

void adjust_moving_to_balls_target(uint32_t delta_time);

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
            logln("Detecting balls finnished, waiting for 5 secs before continuing...");
            for (int i = 0; i < 5000; i+=100)
            {
                robot.tick();
                display.tick();
                delay(100);
            }

            last_millis = millis();
            robot.cur_room_state = robot.ROOM_STATE_MOVING_TO_BALL;
            robot.moving_to_balls_target = robot.detected_balls[0]; // TODO: Select most confident ball instead + remember how many silver & black balls were put down -> only get black ball if 2 silver were put down

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

                // Step 2: Move to ball using cam
                // Robot::moving_to_balls_step move_to_ball;  // TODO: Actually use this, instead of just guessing and hoping
                // move_to_ball.motor_left_speed = -20;
                // move_to_ball.motor_right_speed = -20;
                // move_to_ball.follow_ball = true;
                // robot.moving_to_balls_queue.push_back(move_to_ball);

                Robot::moving_to_balls_step move_to_ball;
                move_to_ball.motor_left_speed = -20;
                move_to_ball.motor_right_speed = -20;
                move_to_ball.time_left = ( robot.distance_between_points(robot.moving_to_balls_target.pos, robot.Origin) - (robot.height / 2) ) / (0.5 * MILLIMETERS_PER_MILLISECOND); // Distance to travel / constant for speed 20 (for speed 40 / 2)
                robot.moving_to_balls_queue.push_back(move_to_ball);
            // }
            // TODO: else if ball is not within 20cm of center line (ie next to wall)
        }
    }
    else if (robot.cur_room_state == robot.ROOM_STATE_MOVING_TO_BALL)
    {
        if (robot.prev_room_state != robot.ROOM_STATE_MOVING_TO_BALL)
        {
            last_millis = millis();
            robot.prev_room_state = robot.cur_room_state;
        }

        uint32_t delta_time = millis() - last_millis;
        last_millis = millis();
        adjust_moving_to_balls_target(delta_time);

        if (!robot.moving_to_balls_queue.empty())
        {
            Robot::moving_to_balls_step step = robot.moving_to_balls_queue.front();
            robot.move(step.motor_left_speed, step.motor_right_speed);
            if (step.follow_ball) // Follow Ball with Cam mode
            {
                logln("ERROR! FOLLOW BALL NOT IMPLEMENTED YET!"); // TODO: Implement Follow Ball
            }
            else if (step.target_angle != -1) // Rotate to Target Angle Mode
            {
                if (abs(robot.angle - step.target_angle) < ROTATE_TO_ANGLE_TOLERANCE) // Done!
                    robot.moving_to_balls_queue.pop_front();
            }
            else // Move straight Distance (time) mode
            {
                if (step.time_left <= delta_time) // Done!
                    robot.moving_to_balls_queue.pop_front();
                else
                    step.time_left -= delta_time;
            }
        }
        else
        {
            robot.claw->set_state(Claw::BOTTOM_OPEN);
            robot.move(DRIVE_SPEED_RAUM, DRIVE_SPEED_RAUM);
            delay(50);
            robot.claw->set_state(Claw::BOTTOM_CLOSED);
            delay(50);
            robot.move(0, 0);
            robot.claw->set_state(CLAW::SIDE_CLOSED);

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
 
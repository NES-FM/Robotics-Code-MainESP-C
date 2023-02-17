// mm/ms at 40 speed
#define MILLIMETERS_PER_MILLISECOND 0.1770833333

#include "moving_in_room.h"
#include "target_timer.h"

#define ROTATE_TO_ANGLE_TOLERANCE 2

#define MOVE_TO_CENTER_TIME 1200

float rotate_balls_360_start_angle = 0;
bool rotated_balls_was_at_180_degrees = false;

void adjust_moving_to_balls_target(uint32_t delta_time);
void rotate_to_angle(float target, bool turn_right);

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
    else if (robot.cur_room_state == robot.ROOM_STATE_ORIENT_ON_SILVER)
    {
        if (robot.prev_room_state != robot.ROOM_STATE_ORIENT_ON_SILVER)
        {
            robot.move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
            robot.prev_room_state = robot.ROOM_STATE_ORIENT_ON_SILVER;
            cuart.silver_line = false;
        }

        if (cuart.silver_line)
        {
            delay(200); // move further behind silver line
            
            robot.move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL); // Move until Silver line in middle
            while (!cuart.sensor_array[0])
            {
                display.tick();
                delay(5);
            }
            robot.move(0, 0);
            delay(1000);

            while (abs(cuart.line_angle) > 4) // Move until straight
            {
                if (cuart.line_angle > 0)
                    robot.move(-5, 5);
                else
                    robot.move(5, -5);
            }
            robot.move(0, 0);

            robot.setRoomBeginningAngle();
            robot.cur_room_state = robot.ROOM_STATE_FIND_WALL_DRIVE_TO_CENTER;
        }
    }
    else if (robot.cur_room_state == robot.ROOM_STATE_FIND_WALL_DRIVE_TO_CENTER) // TODO: Maybe make async
    {
        if (robot.prev_room_state != robot.ROOM_STATE_FIND_WALL_DRIVE_TO_CENTER)
        {
            robot.prev_room_state = robot.ROOM_STATE_FIND_WALL_DRIVE_TO_CENTER;
            robot.claw->set_state(Claw::BOTTOM_CLOSED);
            delay(1000);
        }
        logln("Find Wall");
        target_timer time_until_wall_found(2000);

        logln("Rotate to 45");
        rotate_to_angle(45, true);
        robot.move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);

        logln("room_has_reached_end()=%d", (int)robot.room_has_reached_end());

        time_until_wall_found.reset();

        while (robot.room_has_reached_end() != robot.ROOM_HAS_REACHED_TASTER_RIGHT 
            && robot.room_has_reached_end() != robot.ROOM_HAS_REACHED_TASTER_LEFT  // While no taster pressed
            && !time_until_wall_found.has_reached_target()) // And timer not reached
        {
            logln("in while -> room_has_reached_end()=%d", (int)robot.room_has_reached_end());
            display.tick();
            delay(5);
        }

        if (robot.room_has_reached_end() == robot.ROOM_HAS_REACHED_TASTER_RIGHT || robot.room_has_reached_end() == robot.ROOM_HAS_REACHED_TASTER_LEFT) // Wall to the right -> Drive to the Left
        {
            logln("Taster pressed -> drive to left");
            robot.move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
            delay(time_until_wall_found.time_elapsed());

            rotate_to_angle(315, false);

            robot.move(DRIVE_SPEED_RAUM, DRIVE_SPEED_RAUM);
            delay(MOVE_TO_CENTER_TIME);
        }
        else // No Wall to the right -> Drive to the Right
        {
            logln("No taster pressed");
            robot.move(DRIVE_SPEED_RAUM, DRIVE_SPEED_RAUM);
            delay(MOVE_TO_CENTER_TIME-500);
        }

        robot.cur_room_state = robot.ROOM_STATE_ROTATE_TO_FIND_BALLS;
    }
    else if (robot.cur_room_state == robot.ROOM_STATE_ROTATE_TO_FIND_BALLS)
    {
        float angle = robot.compass->get_angle();
        if (robot.prev_room_state != robot.ROOM_STATE_ROTATE_TO_FIND_BALLS)
        {
            robot.move(-5, 5);
            robot.prev_room_state = robot.ROOM_STATE_ROTATE_TO_FIND_BALLS;
            robot.num_detected_balls = 0;  // TODO: Find some way to remember old ball positions, so that we dont have to rotate 360 next time, but only check where we believe a ball should be
            robot.possible_corners.clear();
            robot.detectingBallsEnabled = true;
            robot.detectingCornerEnabled = true;
            rotate_balls_360_start_angle = angle;
            rotated_balls_was_at_180_degrees = false;
            moving_in_room_queue.clear();
        }

        if (abs(angle - rotate_balls_360_start_angle - 180) < 10)
            rotated_balls_was_at_180_degrees = true;

        if (rotated_balls_was_at_180_degrees && abs(angle - rotate_balls_360_start_angle) < ROTATE_TO_ANGLE_TOLERANCE)  // Rotated approx. 360 degrees -> goto next step
        {
            robot.detectingBallsEnabled = false;
            robot.detectingCornerEnabled = false;
            robot.move(0, 0);
            robot.print_balls();
            robot.print_corners();

            last_millis = millis();
            robot.cur_room_state = robot.ROOM_STATE_MOVING_TO_BALL;

            // Selecting Ball to follow
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

            logln("Selected Ball (%d|%d), conf=%d*%f=%f as a target!", robot.moving_to_balls_target.pos.x_mm, robot.moving_to_balls_target.pos.y_mm, robot.moving_to_balls_target.num_hits, robot.moving_to_balls_target.conf, max_conf);

            // Selecting Corner
            robot.most_likely_corner = *std::max_element(robot.possible_corners.begin(), robot.possible_corners.end(), [](const Robot::corner& a, const Robot::corner& b) { return a.conf*(float)a.num_hits < b.conf*(float)b.num_hits; });

            logln("Selected Corner (%d|%d)!", robot.most_likely_corner.center_pos.x_mm, robot.most_likely_corner.center_pos.y_mm);

            // if (abs(robot.moving_to_balls_target.pos.y_mm) <= 200) // If ball is less than 20cm away of middle, go directly to ball
            // {
                // Step 1: Rotate to ball
                moving_in_room_rotate_to_deg* rotate_to_ball = new moving_in_room_rotate_to_deg();
                rotate_to_ball->_bcuart = &bcuart;
                rotate_to_ball->_robot = &robot;
                rotate_to_ball->motor_left_speed = 20;
                rotate_to_ball->motor_right_speed = -20;

                if (robot.moving_to_balls_target.pos.y_mm != 0)
                    robot.moving_to_balls_target.pos.y_mm = 1;
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
                rotate_to_ball->target_angle = target_angle_rad * RAD_TO_DEG;

                moving_in_room_queue.push_back(rotate_to_ball);
                logln("Step1: Rotating to ball with motors %d|%d (Target angle: %f°)", 20, -20, target_angle_rad * RAD_TO_DEG);

                // Step 2: Move to ball using cam
                moving_in_room_follow_ball* move_to_ball = new moving_in_room_follow_ball();
                move_to_ball->_bcuart = &bcuart;
                move_to_ball->_robot = &robot;
                move_to_ball->motor_left_speed = -20;
                move_to_ball->motor_right_speed = -20;
                moving_in_room_queue.push_back(move_to_ball);
                logln("Step2: Moving to Ball using cam");
            // }
            // TODO: else if ball is not within 20cm of center line (ie next to wall)
            // TODO: else if ball is too close to robot

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
            robot.claw->setTofContinuous(true);
        }

        uint32_t delta_time = millis() - last_millis;
        last_millis = millis();
        adjust_moving_to_balls_target(delta_time);

        if (!moving_in_room_queue.empty())
        {
            moving_in_room_step* step = moving_in_room_queue.front();
            robot.move(step->motor_left_speed, step->motor_right_speed);
            
            if (step->tick(delta_time))
            {
                delete moving_in_room_queue[0];
                moving_in_room_queue.erase(moving_in_room_queue.begin());
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
            robot.claw->setTofContinuous(false);
        }

        // TODO: Bring ball to corner
    }
}

void adjust_moving_to_balls_target(uint32_t delta_time)
{
    int motor_left_speed = robot.motor_left->motor_speed;
    int motor_right_speed = robot.motor_right->motor_speed;

    if (motor_left_speed == 0 && motor_right_speed == 0)
        return; // Nothing to adjust
    
    if (motor_left_speed == -motor_right_speed)
        return; // Rotating while standing still

    if (motor_left_speed != motor_right_speed) // If motors not on same speed, make a rough estimate using average.
    { // Note: This becomes less accurate the more the speeds diverge
        int average_speed = (motor_left_speed + motor_right_speed) / 2;
        motor_left_speed = average_speed;
        motor_right_speed = average_speed;
    }

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
    robot.most_likely_corner.center_pos.x_mm = robot.most_likely_corner.center_pos.x_mm + delta_distance * cos(angle_rad);
    robot.most_likely_corner.center_pos.y_mm = robot.most_likely_corner.center_pos.y_mm + delta_distance * sin(angle_rad);
}
 
void rotate_to_angle(float target, bool turn_right)
{
    if (turn_right)
        robot.move(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    else
        robot.move(-DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);

    while(abs(robot.compass->keep_in_360_range(robot.compass->get_angle() - robot.room_beginning_angle) - target) > ROTATE_TO_ANGLE_TOLERANCE)
    {
        delay(5);
        display.tick();
    }
}

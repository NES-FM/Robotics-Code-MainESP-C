#include "moving_in_room.h"
#include "target_timer.h"

#define ROTATE_TO_ANGLE_TOLERANCE 2

#define MOVE_TO_CENTER_TIME 1200

#define TARGET_2_CORNER_DISTANCE 350

#define TURN_90_DEG_DELAY 500

#define ROOM_MOVE_ALONG_WALL_DISTANCE 60
#define ROOM_MOVE_ALONG_WALL_LINEAR_FACTOR 0.43f

float rotate_balls_360_start_angle = 0;
bool rotated_balls_was_at_180_degrees = false;

float find_corner_again_target_angle = 0;
Robot::point start_pos_of_moving_to_ball;

void adjust_moving_to_balls_target(uint32_t delta_time);
void rotate_to_angle(float target, bool turn_right);
void clear_queue();
void clear_possible_corners();
void turn_90_while_next_to_wall();

target_timer find_wall_timer;
int find_wall_right_distance_avg = -1;

uint16_t follow_wall_last_tof_value = 0;

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
    else if (robot.cur_room_state == robot.ROOM_STATE_MOVE_IN_ROOM)
    {
        if (robot.prev_room_state != robot.ROOM_STATE_MOVE_IN_ROOM)
        {
            last_millis = millis();
            robot.prev_room_state = robot.cur_room_state;
        }
        uint32_t delta_time = millis() - last_millis;
        last_millis = millis();
        adjust_moving_to_balls_target(delta_time);

        if (!moving_in_room_queue.empty())
        {
            moving_in_room_step* step = moving_in_room_queue.front();
            
            if (step->tick(delta_time))
            {
                delete moving_in_room_queue[0];
                moving_in_room_queue.erase(moving_in_room_queue.begin());
                last_millis = millis();
            }
        }
        else
        {
            logln("ERROR! THIS SHOULD NOT BE REACHED! MOVE_IN_ROOM_QUEUE BECAME EMPTY!");
        }
    }
    else if (robot.cur_room_state == robot.ROOM_STATE_FIND_WALL_DRIVE_TO_CENTER) // TODO: Maybe make async
    {
        if (robot.prev_room_state != robot.ROOM_STATE_FIND_WALL_DRIVE_TO_CENTER)
        {
            logln("Find Wall");
            robot.prev_room_state = robot.ROOM_STATE_FIND_WALL_DRIVE_TO_CENTER;

            robot.move(-DRIVE_SPEED_RAUM, -DRIVE_SPEED_RAUM);
            delay(500);
            find_wall_timer.set_target(2000);
            find_wall_timer.reset();
            robot.move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
        }

        int measurement = robot.tof_side->getMeasurement();
        if (robot.tof_side->getMeasurementError() == tof::TOF_ERROR_NONE)
        {
            if (find_wall_right_distance_avg == -1) // First Time -> Reset
                find_wall_right_distance_avg = measurement;
            else
                find_wall_right_distance_avg = (find_wall_right_distance_avg + measurement) / 2;
        }

        if (find_wall_timer.has_reached_target())
        {
            logln("Found Wall at Distance of %d", find_wall_right_distance_avg);

            if (find_wall_right_distance_avg >= 550 || find_wall_right_distance_avg == -1) // If Robot is on Left half of Room
            {
                moving_in_room_rotate_to_deg* rotate_to_middle = new moving_in_room_rotate_to_deg();
                rotate_to_middle->_robot = &robot;
                rotate_to_middle->motor_left_speed = 20;
                rotate_to_middle->motor_right_speed = -20;
                rotate_to_middle->target_angle = 90;
                moving_in_room_queue.push_back(rotate_to_middle);

                moving_in_room_distance_by_time* move_to_middle = new moving_in_room_distance_by_time();
                move_to_middle->_robot = &robot;
                move_to_middle->motor_left_speed = 40;
                move_to_middle->motor_right_speed = 40;
                move_to_middle->calculate_time_by_distance(find_wall_right_distance_avg-320);
                moving_in_room_queue.push_back(move_to_middle);
            }
            else if (find_wall_right_distance_avg <= 150) // If Robot is on Right side of Room
            {
                moving_in_room_rotate_to_deg* rotate_to_middle = new moving_in_room_rotate_to_deg();
                rotate_to_middle->_robot = &robot;
                rotate_to_middle->motor_left_speed = -20;
                rotate_to_middle->motor_right_speed = 20;
                rotate_to_middle->target_angle = 270;
                moving_in_room_queue.push_back(rotate_to_middle);

                moving_in_room_distance_by_time* move_to_middle = new moving_in_room_distance_by_time();
                move_to_middle->_robot = &robot;
                move_to_middle->motor_left_speed = 40;
                move_to_middle->motor_right_speed = 40;
                move_to_middle->calculate_time_by_distance(320-find_wall_right_distance_avg);
                moving_in_room_queue.push_back(move_to_middle);
            }
            //else // If Robot is in Middle of Room

            moving_in_room_goto_room_state* goto_rotate_to_find_balls = new moving_in_room_goto_room_state();
            goto_rotate_to_find_balls->_robot = &robot;
            goto_rotate_to_find_balls->target_room_state = Robot::ROOM_STATE_ROTATE_TO_FIND_BALLS;
            moving_in_room_queue.push_back(goto_rotate_to_find_balls);

            robot.cur_room_state = Robot::ROOM_STATE_MOVE_IN_ROOM;
        }
    }
    else if (robot.cur_room_state == robot.ROOM_STATE_ROTATE_TO_FIND_BALLS)
    {
        float angle = robot.compass->get_angle();
        if (robot.prev_room_state != robot.ROOM_STATE_ROTATE_TO_FIND_BALLS)
        {
            robot.move(-5, 5);
            robot.prev_room_state = robot.ROOM_STATE_ROTATE_TO_FIND_BALLS;
            robot.num_detected_balls = 0;  // TODO: Find some way to remember old ball positions, so that we dont have to rotate 360 next time, but only check where we believe a ball should be
            clear_possible_corners();
            robot.detectingBallsEnabled = true;
            // robot.detectingCornerEnabled = true;
            rotate_balls_360_start_angle = angle;
            rotated_balls_was_at_180_degrees = false;
            clear_queue();
            robot.claw->set_state(Claw::BOTTOM_OPEN, true);
        }

        if (abs(angle - robot.compass->keep_in_360_range(rotate_balls_360_start_angle - 180)) < 10)
            rotated_balls_was_at_180_degrees = true;

        if (rotated_balls_was_at_180_degrees && abs(angle - rotate_balls_360_start_angle) < ROTATE_TO_ANGLE_TOLERANCE)  // Rotated approx. 360 degrees -> goto next step
        {
            robot.detectingBallsEnabled = false;
            robot.detectingCornerEnabled = false;
            robot.move(0, 0);
            robot.print_balls();
            // robot.print_corners();

            clear_queue();

            last_millis = millis();
            robot.cur_room_state = robot.ROOM_STATE_MOVE_IN_ROOM;

            // Selecting Ball to follow^
            bool searching_for_black = robot.room_prefs->getBool("silver_1", false) && robot.room_prefs->getBool("silver_2", false);
            float max_conf = 0.0;
            int max_conf_index;
            for (int i = 0; i < robot.num_detected_balls; i++)
            {
                if (robot.detected_balls[i].black == searching_for_black)
                {
                    float ball_conf = robot.detected_balls[i].conf * (float)robot.detected_balls[i].num_hits;
                    if (ball_conf > max_conf)
                    {
                        max_conf = ball_conf;
                        max_conf_index = i;
                    }
                }
            }
            robot.moving_to_balls_target = robot.detected_balls[max_conf_index]; // TODO: remember how many silver & black balls were put down -> only get black ball if 2 silver were put down

            logln("Selected Ball (%d|%d), conf=%d*%f=%f as a target!", robot.moving_to_balls_target.pos.x_mm, robot.moving_to_balls_target.pos.y_mm, robot.moving_to_balls_target.num_hits, robot.moving_to_balls_target.conf, max_conf);

            // // Find the corner with the highest value of conf * num_hits
            // auto max_corner_it = std::max_element(robot.possible_corners.begin(), robot.possible_corners.end(),
            //     [](Robot::corner* a, Robot::corner* b) { return a->conf * (float)a->num_hits < b->conf * (float)b->num_hits; });

            // // Dereference the iterator to get the max element
            // robot.most_likely_corner = *max_corner_it;

            // logln("Selected Corner (%d|%d)!", robot.most_likely_corner->center_pos.x_mm, robot.most_likely_corner->center_pos.y_mm);

            // if (abs(robot.moving_to_balls_target.pos.y_mm) <= 200) // If ball is less than 20cm away of middle, go directly to ball
            // {
                // Step 1: Rotate to ball
                moving_in_room_rotate_to_deg* rotate_to_ball = new moving_in_room_rotate_to_deg();
                rotate_to_ball->_robot = &robot;
                rotate_to_ball->motor_left_speed = 20;
                rotate_to_ball->motor_right_speed = -20;

                if (robot.moving_to_balls_target.pos.y_mm == 0)
                    robot.moving_to_balls_target.pos.y_mm = 1;
                float target_angle_rad = abs(atan((float)robot.moving_to_balls_target.pos.x_mm / (float)robot.moving_to_balls_target.pos.y_mm));
                logln("Result of atan: %f using atan(%d/%d)", target_angle_rad, robot.moving_to_balls_target.pos.x_mm, robot.moving_to_balls_target.pos.y_mm);
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
                logln("After If: %f", target_angle_rad);
                rotate_to_ball->target_angle = target_angle_rad * RAD_TO_DEG;

                moving_in_room_queue.push_back(rotate_to_ball);
                logln("Step1: Rotating to ball with motors %d|%d (Target angle: %f°)", 20, -20, target_angle_rad * RAD_TO_DEG);

                moving_in_room_distance_by_time* wait_1_sec = new moving_in_room_distance_by_time();
                wait_1_sec->_robot = &robot;
                wait_1_sec->motor_left_speed = 0;
                wait_1_sec->motor_right_speed = 0;
                wait_1_sec->time_left = 1000;
                moving_in_room_queue.push_back(wait_1_sec);

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

            moving_in_room_pick_up_ball* pick_up_ball = new moving_in_room_pick_up_ball();
            pick_up_ball->_robot = &robot;
            moving_in_room_queue.push_back(pick_up_ball);

            moving_in_room_goto_room_state* goto_next_step = new moving_in_room_goto_room_state();
            goto_next_step->_robot = &robot;
            goto_next_step->target_room_state = Robot::ROOM_STATE_PUT_BALL_IN_CORNER_STEP_1;
            moving_in_room_queue.push_back(goto_next_step);

            start_pos_of_moving_to_ball.x_mm = 0;
            start_pos_of_moving_to_ball.y_mm = 0;

            // logln("Detecting balls finnished, waiting for 5 secs before continuing...");
            // for (int i = 0; i < 5000; i+=100)
            // {
            //     robot.tick();
            //     display.tick();
            //     delay(100);
            // }
        }
    }
    else if (robot.cur_room_state == robot.ROOM_STATE_PUT_BALL_IN_CORNER_STEP_1)
    {
        float angle = robot.compass->get_angle();
        if (robot.prev_room_state != robot.ROOM_STATE_PUT_BALL_IN_CORNER_STEP_1)
        {
            robot.move(0, 0);
            robot.prev_room_state = robot.cur_room_state;
            // robot.claw->setTofContinuous(false);
            clear_queue();

            float time_to_move = Robot::distance_between_points(robot.Origin, start_pos_of_moving_to_ball) / robot.millimeters_per_millisecond_40_speed;
            robot.move(40, 40);
            delay(time_to_move);

            robot.move(-5, 5);
            clear_possible_corners();
            robot.detectingCornerEnabled = true;
            rotate_balls_360_start_angle = angle;
            rotated_balls_was_at_180_degrees = false;
            clear_queue();

            robot.claw->set_state(Claw::SIDE_CLOSED);
        }

        if (abs(angle - robot.compass->keep_in_360_range(rotate_balls_360_start_angle - 180)) < 15)
            rotated_balls_was_at_180_degrees = true;

        if (rotated_balls_was_at_180_degrees && abs(angle - rotate_balls_360_start_angle) < ROTATE_TO_ANGLE_TOLERANCE)  // Rotated approx. 360 degrees -> goto next step
        {
            robot.detectingCornerEnabled = false;

            auto max_corner_it = std::max_element(robot.possible_corners.begin(), robot.possible_corners.end(),
                [](Robot::corner* a, Robot::corner* b) { return a->conf * (float)a->num_hits < b->conf * (float)b->num_hits; });

            // Dereference the iterator to get the max element
            robot.most_likely_corner = *max_corner_it;

            robot.print_corners();

            log_inline_begin();
            log_inline("\r\n---------------------\r\n");
            log_inline("Selected Corner (%d|%d)!\r\n", robot.most_likely_corner->center_pos.x_mm, robot.most_likely_corner->center_pos.y_mm);

            Robot::point first_p = robot.most_likely_corner->first_pos;
            Robot::point mid_p = robot.most_likely_corner->center_pos;//Robot::midpoint_between_points(first_p, last_p);
            Robot::point last_p = robot.most_likely_corner->last_pos;

            // Slopes
            float slope_corner = ((float)last_p.y_mm - (float)first_p.y_mm) / ((float)last_p.x_mm - (float)first_p.x_mm);
            float slope_90_to_corner = -1.0 / slope_corner;
            float angle_slope_90_to_corner = atan(slope_90_to_corner);

            // Target 2
            Robot::point target_2;
            if (mid_p.x_mm < 0)
            {
                target_2.x_mm = mid_p.x_mm + (cos(angle_slope_90_to_corner) * TARGET_2_CORNER_DISTANCE);
                target_2.y_mm = mid_p.y_mm + (sin(angle_slope_90_to_corner) * TARGET_2_CORNER_DISTANCE);
            }
            else
            {
                target_2.x_mm = mid_p.x_mm - (cos(angle_slope_90_to_corner) * TARGET_2_CORNER_DISTANCE);
                target_2.y_mm = mid_p.y_mm - (sin(angle_slope_90_to_corner) * TARGET_2_CORNER_DISTANCE);
            }

            log_inline("Calculated all Targets: slope_corner=%.3f slope_90_to_corner=%.3f->%.3f°\r\n", slope_corner, slope_90_to_corner, angle_slope_90_to_corner);
            log_inline("Using Corner: first=(%d|%d) mid=(%d|%d) last=(%d|%d)\r\n", first_p.x_mm, first_p.y_mm, mid_p.x_mm, mid_p.y_mm, last_p.x_mm, last_p.y_mm);
            log_inline("Robot angle: %.3f°\r\n", robot.angle);
            log_inline("Targets: target_2(%d|%d)\r\n", target_2.x_mm, target_2.y_mm);

            moving_in_room_rotate_to_deg* rotate_on_target1 = new moving_in_room_rotate_to_deg();
            rotate_on_target1->_robot = &robot;
            rotate_on_target1->target_angle = 90.0 - (atan((float)target_2.y_mm / (float)target_2.x_mm) * RAD_TO_DEG);
            if (target_2.x_mm < 0)
                rotate_on_target1->target_angle += 180.0;
            rotate_on_target1->motor_left_speed = 20;
            rotate_on_target1->motor_right_speed = -20;
            moving_in_room_queue.push_back(rotate_on_target1);
            log_inline("Step 1: rotate_on_target1 with %d|%d speed and target_angle %.1f\r\n", rotate_on_target1->motor_left_speed, rotate_on_target1->motor_right_speed, rotate_on_target1->target_angle);

            moving_in_room_distance_by_time* move_to_target2 = new moving_in_room_distance_by_time();
            move_to_target2->_robot = &robot;
            move_to_target2->motor_left_speed = 40;
            move_to_target2->motor_right_speed = 40;
            move_to_target2->calculate_time_by_distance(Robot::distance_between_points(robot.Origin, target_2) - 100);
            moving_in_room_queue.push_back(move_to_target2);
            log_inline("Step 2: move_to_target_2 with 40|40 speed and time %d\r\n", move_to_target2->time_left);

            moving_in_room_rotate_to_deg* rotate_on_target2 = new moving_in_room_rotate_to_deg();
            rotate_on_target2->_robot = &robot;
            rotate_on_target2->motor_left_speed = -20;
            rotate_on_target2->motor_right_speed = 20;
            rotate_on_target2->target_angle = 90.0 - (angle_slope_90_to_corner * RAD_TO_DEG);
            if (mid_p.x_mm > 0) // Make sure back of robot points to corner
                rotate_on_target2->target_angle += 180.0;
            moving_in_room_queue.push_back(rotate_on_target2);
            log_inline("Step 3: rotate_on_target2 with %d|%d speed and target_angle %.1f\r\n", rotate_on_target2->motor_left_speed, rotate_on_target2->motor_right_speed, rotate_on_target2->target_angle);

            // moving_in_room_set_claw* set_claw_side = new moving_in_room_set_claw();
            // set_claw_side->_robot = &robot;
            // set_claw_side->claw_state = Claw::SIDE_CLOSED;
            // moving_in_room_queue.push_back(set_claw_side);
            // log_inline("Step 4: set_claw_side\r\n");

            // moving_in_room_until_taster* move_to_corner = new moving_in_room_until_taster();
            // move_to_corner->_robot = &robot;
            // move_to_corner->motor_left_speed = 20;
            // move_to_corner->motor_right_speed = 20;
            // move_to_corner->continue_afterwards = 500;
            // moving_in_room_queue.push_back(move_to_corner);
            // log_inline("Step 4: move_to_corner until taster pressed with %d|%d speed \r\n", move_to_corner->motor_left_speed, move_to_corner->motor_right_speed);
            moving_in_room_follow_corner* move_to_corner = new moving_in_room_follow_corner();
            move_to_corner->_robot = &robot;
            move_to_corner->_bcuart = &bcuart;
            move_to_corner->motor_left_speed = -20;
            move_to_corner->motor_right_speed = -20;
            move_to_corner->time_after = 3000;
            moving_in_room_queue.push_back(move_to_corner);
            log_inline("Step 5: move_to_corner with %d|%d speed \r\n", move_to_corner->motor_left_speed, move_to_corner->motor_right_speed);

            // moving_in_room_distance_by_time* move_to_turn_around = new moving_in_room_distance_by_time();
            // move_to_turn_around->_robot = &robot;
            // move_to_turn_around->motor_left_speed = -20;
            // move_to_turn_around->motor_right_speed = -20;
            // move_to_turn_around->calculate_time_by_distance(100);
            // moving_in_room_queue.push_back(move_to_turn_around);
            // log_inline("Step 5: move_to_turn_around with 40|40 speed and time %d\r\n", move_to_turn_around->time_left);

            // moving_in_room_rotate_relative_degrees* turn_around = new moving_in_room_rotate_relative_degrees();
            // turn_around->_robot = &robot;
            // turn_around->motor_left_speed = 20;
            // turn_around->motor_right_speed = -20;
            // turn_around->target_relative_angle = 180;
            // moving_in_room_queue.push_back(turn_around);
            // log_inline("Step 6: turn_around with %d|%d speed with relative angle 180°\r\n", turn_around->motor_left_speed, turn_around->motor_right_speed);

            // moving_in_room_set_claw* set_claw_top = new moving_in_room_set_claw();
            // set_claw_top->_robot = &robot;
            // set_claw_top->claw_state = Claw::TOP_CLOSED;
            // moving_in_room_queue.push_back(set_claw_top);
            // log_inline("Step 7: set_claw_top\r\n");

            // moving_in_room_distance_by_time* move_to_corner_backwards = new moving_in_room_distance_by_time();
            // move_to_corner_backwards->_robot = &robot;
            // move_to_corner_backwards->motor_left_speed = -20;
            // move_to_corner_backwards->motor_right_speed = -20;
            // move_to_corner_backwards->calculate_time_by_distance(120);
            // moving_in_room_queue.push_back(move_to_corner_backwards);
            // log_inline("Step 7: move_to_corner_backwards with -20|-20 speed and time %d\r\n", move_to_corner_backwards->time_left);

            moving_in_room_set_claw* set_claw_open = new moving_in_room_set_claw();
            set_claw_open->_robot = &robot;
            set_claw_open->claw_state = Claw::TOP_OPEN;
            moving_in_room_queue.push_back(set_claw_open);
            log_inline("Step 6: set_claw_open\r\n");

            moving_in_room_goto_room_state* goto_state = new moving_in_room_goto_room_state();
            goto_state->_robot = &robot;
            goto_state->target_room_state = Robot::ROOM_STATE_PUT_BALL_IN_CORNER_STEP_2;
            moving_in_room_queue.push_back(goto_state);
            log_inline("Step 7: goto_room_state\r\n");

            robot.cur_room_state = Robot::ROOM_STATE_MOVE_IN_ROOM;

            log_inline("---------------------\r\n");
            log_inline_end();
        }
    }
    else if (robot.cur_room_state == robot.ROOM_STATE_PUT_BALL_IN_CORNER_STEP_2)
    {
        if (robot.prev_room_state != robot.cur_room_state)
        {
            robot.prev_room_state = robot.cur_room_state;

            // Put down blue Cube if still in claw
            if (true)//!robot.room_prefs->getBool("blue", false))
            {
                robot.move(20, 20);
                delay(800);
                robot.move(0, 0);
                robot.claw->set_state(Claw::BOTTOM_OPEN);
                delay(100);
                robot.move(-20, -20);
                delay(1500);
                robot.claw->throw_blue_cube();
                delay(1000);
                robot.claw->hold_blue_cube();
                robot.room_prefs->putBool("blue", true);
            }

            // // Save Balls that were put down
            // if (!robot.room_prefs->getBool("silver_1", false))
            //     robot.room_prefs->putBool("silver_1", true);
            // else if (!robot.room_prefs->getBool("silver_2", false))
            //     robot.room_prefs->putBool("silver_2", true);
            // else
            //     robot.room_prefs->putBool("black", true);

            if (true)//!robot.room_prefs->getBool("silver_1", false) || !robot.room_prefs->getBool("silver_2", false) || !robot.room_prefs->getBool("black", false))
            {
                robot.move(40, 40);
                delay(TARGET_2_CORNER_DISTANCE / robot.millimeters_per_millisecond_40_speed);
                robot.move(0, 0);
                delay(2000); // Lag of Progress safety delay
                // save amount of put down balls
                robot.cur_room_state = Robot::ROOM_STATE_ROTATE_TO_FIND_BALLS;
            }
            else
            {
                robot.cur_room_state = robot.ROOM_STATE_SEARCHING_EXIT;
            }
        }
    }

    else if (robot.cur_room_state == robot.ROOM_STATE_SEARCHING_EXIT)
    {
        uint16_t tof_dis = robot.tof_side->getMeasurement();
        if (robot.prev_room_state != robot.cur_room_state)
        {
            robot.prev_room_state = robot.cur_room_state;
            follow_wall_last_tof_value = tof_dis;
        }

        if (robot.tof_side->getMeasurementError() != tof::TOF_ERROR_MAX_DISTANCE) // Closerange needs to see the wall, or else it cant work
        {
            int motor_l_val = DRIVE_SPEED_RAUM;
            int motor_r_val = DRIVE_SPEED_RAUM;

            if (robot.tof_side->getMeasurementError() == tof::TOF_ERROR_NONE)
            {
                float error = ROOM_MOVE_ALONG_WALL_DISTANCE - tof_dis;

                motor_l_val = float(DRIVE_SPEED_RAUM) - ROOM_MOVE_ALONG_WALL_LINEAR_FACTOR * error;
                motor_r_val = float(DRIVE_SPEED_RAUM) + ROOM_MOVE_ALONG_WALL_LINEAR_FACTOR * error;
            }

            robot.move(motor_l_val, motor_r_val);
        }

        if (robot.tof_side->getMeasurementError() == tof::TOF_ERROR_MAX_DISTANCE || (robot.tof_side->getMeasurementError() == tof::TOF_ERROR_NONE && abs(follow_wall_last_tof_value - tof_dis) > 100))
        {
            // Maybe Hole
            robot.move(20, 20);
            delay(400);
            robot.move(20, -20);
            delay(TURN_90_DEG_DELAY);
            robot.room_has_reached_end(); // Just Reset everything

            robot.move(20, 20);

            Robot::room_end_types end_type = Robot::ROOM_HAS_NOT_REACHED_END;
            while (end_type == Robot::ROOM_HAS_NOT_REACHED_END)
            {
                end_type = robot.room_has_reached_end();
                display.tick();
                delay(10);
            }

            if (end_type == Robot::ROOM_HAS_REACHED_SILVER_LINE)
            {
                robot.move(-20, -20);
                delay(200);
                cuart.silver_line = false;
            }
            else if (end_type == Robot::ROOM_HAS_REACHED_GREEN_LINE)
            {
                robot.move(20, 20);
                delay(1000);
                robot.move(0, 0);
                cuart.green_line = false;
                robot.cur_drive_mode = Robot::ROBOT_DRIVE_MODE_LINE;
                delay(1000);
                robot.move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
                return;
            }

            turn_90_while_next_to_wall();
            cuart.silver_line = false;
        }

        Robot::room_end_types end_type = robot.room_has_reached_end();
        if (end_type != Robot::ROOM_HAS_NOT_REACHED_END)
        {
            if (end_type == Robot::ROOM_HAS_REACHED_SILVER_LINE)
            {
                robot.move(-20, -20);
                delay(200);
                cuart.silver_line = false;
            }
            else if (end_type == Robot::ROOM_HAS_REACHED_GREEN_LINE)
            {
                robot.move(20, 20);
                delay(1000);
                robot.move(0, 0);
                cuart.green_line = false;
                robot.cur_drive_mode = Robot::ROBOT_DRIVE_MODE_LINE;
                delay(1000);
                robot.move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
                return;
            }
            turn_90_while_next_to_wall();
        }
        
        if (robot.tof_side->getMeasurementError() == tof::TOF_ERROR_NONE)
            follow_wall_last_tof_value = tof_dis;
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
    float delta_distance = (double)delta_time * robot.millimeters_per_millisecond_40_speed * speed_scale;

    // convert angle to radians
    float angle_rad = robot.angle * M_PI / 180.0;

    if (motor_left_speed > 0) // If moving forward, move the ball backwards
    {
        // calculate the opposite angle by adding 180 degrees
        angle_rad = angle_rad + M_PI;
    }

    float s = sin(angle_rad);
    float c = cos(angle_rad);

    // calculate new x and y coordinates
    robot.moving_to_balls_target.pos.x_mm = robot.moving_to_balls_target.pos.x_mm + delta_distance * c;
    robot.moving_to_balls_target.pos.y_mm = robot.moving_to_balls_target.pos.y_mm + delta_distance * s;

    start_pos_of_moving_to_ball.x_mm = start_pos_of_moving_to_ball.x_mm + delta_distance * c;
    start_pos_of_moving_to_ball.y_mm = start_pos_of_moving_to_ball.y_mm + delta_distance * s;

    // robot.most_likely_corner->center_pos.x_mm = robot.most_likely_corner->center_pos.x_mm + delta_distance * c;
    // robot.most_likely_corner->center_pos.y_mm = robot.most_likely_corner->center_pos.y_mm + delta_distance * s;
    // robot.most_likely_corner->first_pos.x_mm = robot.most_likely_corner->first_pos.x_mm + delta_distance * c;
    // robot.most_likely_corner->first_pos.y_mm = robot.most_likely_corner->first_pos.y_mm + delta_distance * s;
    // robot.most_likely_corner->last_pos.x_mm = robot.most_likely_corner->last_pos.x_mm + delta_distance * c;
    // robot.most_likely_corner->last_pos.y_mm = robot.most_likely_corner->last_pos.y_mm + delta_distance * s;
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

void clear_possible_corners()
{
    for (auto el : robot.possible_corners)
    {
        delete el;
    }
    robot.possible_corners.clear();
}
void clear_queue()
{
    for (auto el : moving_in_room_queue)
    {
        delete el;
    }
    moving_in_room_queue.clear();
}

void turn_90_while_next_to_wall()
{
    robot.move(0, -DRIVE_SPEED_NORMAL-5);
    delay(300);
    robot.move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    delay(200);
    robot.move(-DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
    delay(TURN_90_DEG_DELAY+160);
    robot.move(DRIVE_SPEED_RAUM, DRIVE_SPEED_RAUM);
}

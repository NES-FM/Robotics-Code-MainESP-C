#include "moving_in_room.h"
#include "target_timer.h"

#define ROTATE_TO_ANGLE_TOLERANCE 4

#define MOVE_TO_CENTER_TIME 1200

#define TARGET_2_CORNER_DISTANCE 350

#define ROOM_MOVE_ALONG_WALL_DISTANCE 60
#define ROOM_MOVE_ALONG_WALL_LINEAR_FACTOR 0.43f

float rotate_balls_360_start_angle = 0;
bool rotated_balls_was_at_180_degrees = false;

float find_corner_again_target_angle = 0;
Robot::point start_pos_of_moving_to_ball;
target_timer find_corner_again_timeout;

// void adjust_moving_to_balls_target(uint32_t delta_time);
// void rotate_to_angle(float target, bool turn_right);
void clear_queue();
void clear_possible_corners();

target_timer find_wall_timer;
int find_wall_right_distance_avg = -1;

uint16_t follow_wall_last_tof_value = 0;

Robot::ball temp_ball;
Robot::ball temp_black_ball;
target_timer last_time_was_ball_timer;
bool has_seen_black_ball_before = false;

Robot::corner temp_corner;
target_timer last_time_was_corner_timer;

uint32_t last_millis;
void drive_room()
{
    if (robot.cur_room_state == robot.ROOM_STATE_DEFAULT)
    {
        if (robot.prev_room_state != robot.ROOM_STATE_DEFAULT)
        {
            robot.move(0, 0);
            // robot.move(40, -40);
            robot.prev_room_state = robot.ROOM_STATE_DEFAULT;
        }
        // logln("%.4f", robot.angle);
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
        // adjust_moving_to_balls_target(delta_time);

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
    else if (robot.cur_room_state == robot.ROOM_STATE_FIND_WALL_DRIVE_TO_CENTER)
    {
        if (robot.prev_room_state != robot.ROOM_STATE_FIND_WALL_DRIVE_TO_CENTER)
        {
            logln("Find Wall");
            robot.prev_room_state = robot.ROOM_STATE_FIND_WALL_DRIVE_TO_CENTER;

            // robot.move(-DRIVE_SPEED_RAUM, -DRIVE_SPEED_RAUM);
            // delay(500);
            find_wall_timer.set_target(500);
            find_wall_timer.reset();
            robot.move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
        }

        // int measurement = robot.tof_side->getMeasurement();
        // if (robot.tof_side->getMeasurementError() == tof::TOF_ERROR_NONE)
        // {
        //     if (find_wall_right_distance_avg == -1) // First Time -> Reset
        //         find_wall_right_distance_avg = measurement;
        //     else
        //         find_wall_right_distance_avg = (find_wall_right_distance_avg + measurement) / 2;
        // }

        if (find_wall_timer.has_reached_target())
        {
            // logln("Found Wall at Distance of %d", find_wall_right_distance_avg);

            // if (find_wall_right_distance_avg >= 550 || find_wall_right_distance_avg == -1) // If Robot is on Left half of Room
            // {
            //     moving_in_room_rotate_to_deg* rotate_to_middle = new moving_in_room_rotate_to_deg();
            //     rotate_to_middle->_robot = &robot;
            //     rotate_to_middle->motor_left_speed = 20;
            //     rotate_to_middle->motor_right_speed = -20;
            //     rotate_to_middle->target_angle = 90;
            //     moving_in_room_queue.push_back(rotate_to_middle);

            //     moving_in_room_distance_by_time* move_to_middle = new moving_in_room_distance_by_time();
            //     move_to_middle->_robot = &robot;
            //     move_to_middle->motor_left_speed = 40;
            //     move_to_middle->motor_right_speed = 40;
            //     move_to_middle->calculate_time_by_distance(find_wall_right_distance_avg-320);
            //     moving_in_room_queue.push_back(move_to_middle);
            // }
            // else if (find_wall_right_distance_avg <= 150) // If Robot is on Right side of Room
            // {
            //     moving_in_room_rotate_to_deg* rotate_to_middle = new moving_in_room_rotate_to_deg();
            //     rotate_to_middle->_robot = &robot;
            //     rotate_to_middle->motor_left_speed = -20;
            //     rotate_to_middle->motor_right_speed = 20;
            //     rotate_to_middle->target_angle = 270;
            //     moving_in_room_queue.push_back(rotate_to_middle);

            //     moving_in_room_distance_by_time* move_to_middle = new moving_in_room_distance_by_time();
            //     move_to_middle->_robot = &robot;
            //     move_to_middle->motor_left_speed = 40;
            //     move_to_middle->motor_right_speed = 40;
            //     move_to_middle->calculate_time_by_distance(320-find_wall_right_distance_avg);
            //     moving_in_room_queue.push_back(move_to_middle);
            // }
            // //else // If Robot is in Middle of Room

            // moving_in_room_goto_room_state* goto_rotate_to_find_balls = new moving_in_room_goto_room_state();
            // goto_rotate_to_find_balls->_robot = &robot;
            // goto_rotate_to_find_balls->target_room_state = Robot::ROOM_STATE_ROTATE_TO_FIND_BALLS;
            // moving_in_room_queue.push_back(goto_rotate_to_find_balls);

            // robot.cur_room_state = Robot::ROOM_STATE_MOVE_IN_ROOM;

            // robot.move(0, 0);
            // delay(2000);
            robot.cur_room_state = Robot::ROOM_STATE_ROTATE_TO_FIND_BALLS;
        }
    }
    else if (robot.cur_room_state == robot.ROOM_STATE_ROTATE_TO_FIND_BALLS)
    {
        if (robot.prev_room_state != robot.ROOM_STATE_ROTATE_TO_FIND_BALLS)
        {
            robot.move(-5, 5);
            robot.prev_room_state = robot.ROOM_STATE_ROTATE_TO_FIND_BALLS;
            clear_queue();
            robot.claw->set_state(Claw::BOTTOM_OPEN, true);
            last_time_was_ball_timer.set_target(250);

            temp_ball.conf = 0.0;
            temp_ball.distance = 0;
            temp_ball.num_hits = 0;
            temp_ball.black = false;

            temp_black_ball.conf = 0.0;
            temp_black_ball.distance = 0;
            temp_black_ball.num_hits = 0;
            temp_black_ball.black = false;

            has_seen_black_ball_before = false;
        }
        
        if (bcuart.num_balls_in_array > 0)  // TODO: Black Ball
        {
            logln("Ball in array");
            int min_idx = -1;
            float min_dis = 99999.9f;
            int max_idx = -1;
            float max_conf = 0;

            int min_idx_black = -1;
            float min_dis_black = 99999.9f;
            int max_idx_black = -1;
            float max_conf_black = 0;

            for (int i = 0; i < bcuart.num_balls_in_array; ++i) 
            {
                if (bcuart.received_balls[i].black)
                {
                    if (temp_black_ball.distance != 0)
                    {
                        if (abs(bcuart.received_balls[i].distance - temp_black_ball.distance) < min_dis_black) 
                        {
                            min_dis_black = abs(bcuart.received_balls[i].distance - temp_black_ball.distance);
                            min_idx_black = i;
                        }
                    }
                    else
                    {
                        if (bcuart.received_balls[i].conf > max_conf_black) 
                        {
                            max_conf_black = bcuart.received_balls[i].conf;
                            max_idx_black = i;
                        }
                    }
                }
                else
                {
                    if (temp_ball.distance != 0)
                    {
                        if (abs(bcuart.received_balls[i].distance - temp_ball.distance) < min_dis) 
                        {
                            min_dis = abs(bcuart.received_balls[i].distance - temp_ball.distance);
                            min_idx = i;
                        }
                    }
                    else
                    {
                        if (bcuart.received_balls[i].conf > max_conf) 
                        {
                            max_conf = bcuart.received_balls[i].conf;
                            max_idx = i;
                        }
                    }
                }
            }

            if (max_idx != -1)
            {
                temp_ball.conf = bcuart.received_balls[max_idx].conf;
                temp_ball.distance = bcuart.received_balls[max_idx].distance;
                temp_ball.num_hits = 1;
                temp_ball.black = false;
            }
            else if (min_idx != -1)
            {
                temp_ball.conf = max(bcuart.received_balls[min_idx].conf, temp_ball.conf);
                temp_ball.distance = bcuart.received_balls[min_idx].distance;
                temp_ball.num_hits += 1;
                temp_ball.black = false;
            }

            if (max_idx_black != -1)
            {
                temp_black_ball.conf = bcuart.received_balls[max_idx_black].conf;
                temp_black_ball.distance = bcuart.received_balls[max_idx_black].distance;
                temp_black_ball.num_hits = 1;
                temp_black_ball.black = true;
            }
            else if (min_idx_black != -1)
            {
                temp_black_ball.conf = max(bcuart.received_balls[min_idx_black].conf, temp_black_ball.conf);
                temp_black_ball.distance = bcuart.received_balls[min_idx_black].distance;
                temp_black_ball.num_hits += 1;
                temp_black_ball.black = true;
            }

            if (((float)temp_ball.num_hits * temp_ball.conf > 4) || (has_seen_black_ball_before && (float)temp_black_ball.num_hits * temp_black_ball.conf > 4))
            {
                robot.move(0, 0);
                robot.claw->set_state(Claw::BOTTOM_MID);

                if (has_seen_black_ball_before && (float)temp_black_ball.num_hits * temp_black_ball.conf > 4)
                    temp_ball = temp_black_ball;

                robot.moving_to_balls_target = temp_ball;

                moving_in_room_follow_ball* move_to_ball = new moving_in_room_follow_ball();
                move_to_ball->_bcuart = &bcuart;
                move_to_ball->_robot = &robot;
                move_to_ball->motor_left_speed = -20;
                move_to_ball->motor_right_speed = -20;
                moving_in_room_queue.push_back(move_to_ball);

                moving_in_room_pick_up_ball* pick_up_ball = new moving_in_room_pick_up_ball();
                pick_up_ball->_robot = &robot;
                moving_in_room_queue.push_back(pick_up_ball);

                moving_in_room_distance_by_time* move_back_to_center = new moving_in_room_distance_by_time();
                move_back_to_center->_robot = &robot;
                move_back_to_center->motor_left_speed = 20;
                move_back_to_center->motor_right_speed = 20;
                move_back_to_center->calculate_time_by_distance(min(temp_ball.distance*10 - 150, 800.0f));
                moving_in_room_queue.push_back(move_back_to_center);

                moving_in_room_goto_room_state* goto_next_step = new moving_in_room_goto_room_state();
                goto_next_step->_robot = &robot;
                goto_next_step->target_room_state = Robot::ROOM_STATE_PUT_BALL_IN_CORNER_STEP_1;
                moving_in_room_queue.push_back(goto_next_step);

                robot.cur_room_state = robot.ROOM_STATE_MOVE_IN_ROOM;
            }
            

            last_time_was_ball_timer.reset();
            bcuart.reset_balls();
        }
        else if (last_time_was_ball_timer.has_reached_target())
        {
            logln("Hasnt seen ball again");
            temp_ball.conf = 0.0;
            temp_ball.distance = 0;
            temp_ball.num_hits = 0;
            temp_ball.black = false;

            if ((float)temp_black_ball.num_hits * temp_black_ball.conf > 4)
            {
                logln("There was a black ball that is now gone -> has_seen_black_ball");
                has_seen_black_ball_before = true;
            }

            temp_black_ball.conf = 0.0;
            temp_black_ball.distance = 0;
            temp_black_ball.num_hits = 0;
            temp_black_ball.black = false;

            last_time_was_ball_timer.reset();
        }
    }
    else if (robot.cur_room_state == robot.ROOM_STATE_PUT_BALL_IN_CORNER_STEP_1)
    {
        if (robot.prev_room_state != robot.ROOM_STATE_PUT_BALL_IN_CORNER_STEP_1)
        {
            robot.move(5, -5);
            robot.prev_room_state = robot.cur_room_state;
            clear_queue();
            robot.claw->set_state(Claw::SIDE_CLOSED);
            last_time_was_corner_timer.set_target(250);

            temp_corner.conf = 0.0;
            temp_corner.distance = 0;
            temp_corner.num_hits = 0;
        }
        
        if (bcuart.corner_valid)
        {
            logln("Corner received");
            if (temp_corner.distance != 0)
            {
                logln("Same Corner");

                temp_corner.conf = max(bcuart.received_corner.conf, temp_corner.conf);
                temp_corner.distance = bcuart.received_corner.distance;
                temp_corner.num_hits += 1;

                logln("... at %.3f distance with %d hits and %.3f conf", temp_corner.distance, temp_corner.num_hits, temp_corner.conf);
            }
            else
            {
                logln("New Corner");
                
                temp_corner.conf = bcuart.received_corner.conf;
                temp_corner.distance = bcuart.received_corner.distance;
                temp_corner.num_hits = 1;
            }

            if ((float)temp_corner.num_hits * temp_corner.conf > 4) // TODO: Find correct value here
            {
                robot.move(0, 0);
                robot.most_likely_corner = &temp_corner;

                moving_in_room_follow_corner* move_to_corner = new moving_in_room_follow_corner();
                move_to_corner->_robot = &robot;
                move_to_corner->_bcuart = &bcuart;
                move_to_corner->motor_left_speed = -20;
                move_to_corner->motor_right_speed = -20;
                move_to_corner->time_after = 6000;
                moving_in_room_queue.push_back(move_to_corner);
                log_inline("Step 1: move_to_corner with %d|%d speed \r\n", move_to_corner->motor_left_speed, move_to_corner->motor_right_speed);

                moving_in_room_set_claw* set_claw_open = new moving_in_room_set_claw();
                set_claw_open->_robot = &robot;
                set_claw_open->claw_state = Claw::TOP_OPEN;
                moving_in_room_queue.push_back(set_claw_open);
                log_inline("Step 2: set_claw_open\r\n");

                moving_in_room_goto_room_state* goto_state = new moving_in_room_goto_room_state();
                goto_state->_robot = &robot;
                goto_state->target_room_state = Robot::ROOM_STATE_PUT_BALL_IN_CORNER_STEP_2;
                moving_in_room_queue.push_back(goto_state);
                log_inline("Step 3: goto_room_state\r\n");

                robot.cur_room_state = Robot::ROOM_STATE_MOVE_IN_ROOM;
            }
            

            last_time_was_corner_timer.reset();
            bcuart.reset_corner();
        }
        else if (last_time_was_corner_timer.has_reached_target())
        {
            logln("Hasnt seen corner again");
            temp_corner.conf = 0.0;
            temp_corner.distance = 0;
            temp_corner.num_hits = 0;

            last_time_was_corner_timer.reset();
        }
    }
    else if (robot.cur_room_state == robot.ROOM_STATE_PUT_BALL_IN_CORNER_STEP_2)
    {
        if (robot.prev_room_state != robot.cur_room_state)
        {
            robot.prev_room_state = robot.cur_room_state;
            clear_queue();

            // Move back to Corner to be straight
            robot.move(20, 20);
            delay(1000);
            robot.move(0, 0);
            robot.claw->set_state(Claw::BOTTOM_OPEN);
            delay(100);
            robot.move(-20, -20);

            robot.io_ext->tick();
            while(!(robot.io_ext->get_taster_state(io_extender::back_left) || robot.io_ext->get_taster_state(io_extender::back_right)))
            {
                robot.io_ext->tick();
                display.tick();
                delay(10);
            }

            delay(200);
            robot.move(0, 0);

            // Put down blue Cube if still in claw
            if (true) //TODO: Remember if blue cube has been put down (!robot.room_prefs->getBool("blue", false)))
            {
                robot.claw->throw_blue_cube();
                delay(300);
                robot.claw->hold_blue_cube();
                delay(200);
                // robot.room_prefs->putBool("blue", true);
            }

            // TODO: Only continue searching if balls left -> //TODO: Find Exit
            moving_in_room_distance_by_time* move_back_to_center = new moving_in_room_distance_by_time();
            move_back_to_center->_robot = &robot;
            move_back_to_center->motor_left_speed = 20;
            move_back_to_center->motor_right_speed = 20;
            move_back_to_center->calculate_time_by_distance(300);
            moving_in_room_queue.push_back(move_back_to_center);

            moving_in_room_goto_room_state* goto_next_step = new moving_in_room_goto_room_state();
            goto_next_step->_robot = &robot;
            goto_next_step->target_room_state = Robot::ROOM_STATE_ROTATE_TO_FIND_BALLS;
            moving_in_room_queue.push_back(goto_next_step);

            robot.cur_room_state = Robot::ROOM_STATE_MOVE_IN_ROOM;
        }
    }
}

// void adjust_moving_to_balls_target(uint32_t delta_time)
// {
//     int motor_left_speed = robot.motor_left->motor_speed;
//     int motor_right_speed = robot.motor_right->motor_speed;

//     if (motor_left_speed == 0 && motor_right_speed == 0)
//         return; // Nothing to adjust
    
//     if (motor_left_speed == -motor_right_speed)
//         return; // Rotating while standing still

//     if (motor_left_speed != motor_right_speed) // If motors not on same speed, make a rough estimate using average.
//     { // Note: This becomes less accurate the more the speeds diverge
//         int average_speed = (motor_left_speed + motor_right_speed) / 2;
//         motor_left_speed = average_speed;
//         motor_right_speed = average_speed;
//     }

//     float speed_scale = abs((float)motor_left_speed) / 40.0; // Adjust for not driving with 40 speed
//     float delta_distance = (double)delta_time * robot.millimeters_per_millisecond_40_speed * speed_scale;

//     // convert angle to radians
//     float angle_rad = robot.angle * M_PI / 180.0;

//     if (motor_left_speed > 0) // If moving forward, move the ball backwards
//     {
//         // calculate the opposite angle by adding 180 degrees
//         angle_rad = angle_rad + M_PI;
//     }

//     float s = sin(angle_rad);
//     float c = cos(angle_rad);

//     // calculate new x and y coordinates
//     // robot.moving_to_balls_target.pos.x_mm = robot.moving_to_balls_target.pos.x_mm + delta_distance * c;
//     // robot.moving_to_balls_target.pos.y_mm = robot.moving_to_balls_target.pos.y_mm + delta_distance * s;

//     start_pos_of_moving_to_ball.x_mm = start_pos_of_moving_to_ball.x_mm + delta_distance * c;
//     start_pos_of_moving_to_ball.y_mm = start_pos_of_moving_to_ball.y_mm + delta_distance * s;

//     // robot.most_likely_corner->center_pos.x_mm = robot.most_likely_corner->center_pos.x_mm + delta_distance * c;
//     // robot.most_likely_corner->center_pos.y_mm = robot.most_likely_corner->center_pos.y_mm + delta_distance * s;
//     // robot.most_likely_corner->first_pos.x_mm = robot.most_likely_corner->first_pos.x_mm + delta_distance * c;
//     // robot.most_likely_corner->first_pos.y_mm = robot.most_likely_corner->first_pos.y_mm + delta_distance * s;
//     // robot.most_likely_corner->last_pos.x_mm = robot.most_likely_corner->last_pos.x_mm + delta_distance * c;
//     // robot.most_likely_corner->last_pos.y_mm = robot.most_likely_corner->last_pos.y_mm + delta_distance * s;
// }
 
// void rotate_to_angle(float target, bool turn_right)
// {
//     if (turn_right)
//         robot.move(10, -10);
//     else
//         robot.move(-10, 10);

//     while(abs(robot.angle - target) > ROTATE_TO_ANGLE_TOLERANCE)
//     {
//         delay(5);
//         display.tick();
//         robot.tick();
//         logln("Rotate to Angle. Cur: %.3f Target:%.3f", robot.angle, target);
//     }
// }

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

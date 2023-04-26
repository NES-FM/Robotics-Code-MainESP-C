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

Robot::corner temp_corner;
target_timer last_time_was_corner_timer;

target_timer search_exit_timer;

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

            robot.has_seen_black_ball_before = false;
        }
        
        if (bcuart.num_balls_in_array > 0)
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

            if (((float)temp_ball.num_hits * temp_ball.conf > 4) || (robot.has_seen_black_ball_before && (float)temp_black_ball.num_hits * temp_black_ball.conf > 4))
            {
                robot.move(0, 0);
                robot.claw->set_state(Claw::BOTTOM_MID);

                if (robot.has_seen_black_ball_before && (float)temp_black_ball.num_hits * temp_black_ball.conf > 4)
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

                moving_in_room_distance_by_time* move_back_to_center_part1 = new moving_in_room_distance_by_time();
                move_back_to_center_part1->_robot = &robot;
                move_back_to_center_part1->motor_left_speed = 20;
                move_back_to_center_part1->motor_right_speed = 20;
                move_back_to_center_part1->calculate_time_by_distance(100);
                moving_in_room_queue.push_back(move_back_to_center_part1);

                moving_in_room_set_claw* set_claw_side = new moving_in_room_set_claw();
                set_claw_side->_robot = &robot;
                set_claw_side->claw_state = Claw::SIDE_CLOSED;
                moving_in_room_queue.push_back(set_claw_side);

                moving_in_room_distance_by_time* move_back_to_center_part2 = new moving_in_room_distance_by_time();
                move_back_to_center_part2->_robot = &robot;
                move_back_to_center_part2->motor_left_speed = 20;
                move_back_to_center_part2->motor_right_speed = 20;
                move_back_to_center_part2->calculate_time_by_distance(min(temp_ball.distance*10 - 250, 400.0f));  // - 150
                moving_in_room_queue.push_back(move_back_to_center_part2);

                moving_in_room_goto_room_state* goto_next_step = new moving_in_room_goto_room_state();
                goto_next_step->_robot = &robot;
                goto_next_step->target_room_state = Robot::ROOM_STATE_PUT_BALL_IN_CORNER_STEP_1;
                moving_in_room_queue.push_back(goto_next_step);

                robot.move(-5, 5);
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
                robot.has_seen_black_ball_before = true;
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

                robot.move(5, -5);
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
            if (robot.claw->has_blue_cube) //TODO: Remember if blue cube has been put down (!robot.room_prefs->getBool("blue", false)))
            {
                robot.claw->throw_blue_cube();
                delay(800);
                robot.claw->hold_blue_cube();
                delay(200);
                robot.claw->throw_blue_cube();
                delay(500);
                robot.claw->hold_blue_cube();
                robot.claw->has_blue_cube = false;
                // robot.room_prefs->putBool("blue", true);
            }

            if (robot.moving_to_balls_target.black)
            {
                robot.cur_room_state = Robot::ROOM_STATE_SEARCHING_EXIT;
            }
            else
            {
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
    else if (robot.cur_room_state == robot.ROOM_STATE_SEARCHING_EXIT)
    {
        if (robot.prev_room_state != robot.cur_room_state)
        {
            robot.prev_room_state = robot.cur_room_state;
            robot.move(20, 20);
            delay(100);
            robot.move(20, -20);
            delay(500);
            robot.move(40, 20);

            cuart.green_line = false;
            cuart.silver_line = false;

            search_exit_timer.set_target(4000);
            search_exit_timer.reset();
        }

        if (robot.room_has_reached_end() != Robot::ROOM_HAS_NOT_REACHED_END || search_exit_timer.has_reached_target())
        {
            if (robot.room_has_reached_end() == Robot::ROOM_HAS_REACHED_GREEN_LINE)
            {
                robot.move(20, -20);
                delay(100);
                robot.move(20, 20);
                delay(500);
                robot.move(0, 0);

                robot.cur_drive_mode = Robot::ROBOT_DRIVE_MODE_LINE;
                return;
            }
            else if (robot.room_has_reached_end() == Robot::ROOM_HAS_REACHED_SILVER_LINE)
            {
                robot.move(-20, -20);
                delay(400);
                cuart.silver_line = false;
            }

            // robot.move(-10, -40);
            // delay(500);
            robot.move(-20, 20);
            delay(800);
            robot.move(40, 10);

            search_exit_timer.reset();
        }
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

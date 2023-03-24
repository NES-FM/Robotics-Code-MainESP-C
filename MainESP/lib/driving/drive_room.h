#include "moving_in_room.h"
#include "target_timer.h"

#define ROTATE_TO_ANGLE_TOLERANCE 4

#define MOVE_TO_CENTER_TIME 1200

#define TARGET_2_CORNER_DISTANCE 350

#define TURN_90_DEG_DELAY 500

#define ROOM_MOVE_ALONG_WALL_DISTANCE 60
#define ROOM_MOVE_ALONG_WALL_LINEAR_FACTOR 0.43f

float rotate_balls_360_start_angle = 0;
bool rotated_balls_was_at_180_degrees = false;

float find_corner_again_target_angle = 0;
Robot::point start_pos_of_moving_to_ball;
target_timer find_corner_again_timeout;

void adjust_moving_to_balls_target(uint32_t delta_time);
void rotate_to_angle(float target, bool turn_right);
void clear_queue();
void clear_possible_corners();
void turn_90_while_next_to_wall();

target_timer find_wall_timer;
int find_wall_right_distance_avg = -1;

uint16_t follow_wall_last_tof_value = 0;

Robot::ball temp_ball;
target_timer last_time_was_ball_timer;

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
        }
        
        if (bcuart.num_balls_in_array > 0)  // TODO: Black Ball
        {
            logln("Ball in array");
            if (temp_ball.distance != 0)
            {
                logln("Same Ball");
                int min_idx = 0;
                float min_dis = 99999.9f;
                for (int i = 1; i < bcuart.num_balls_in_array; ++i) {
                    if (abs(bcuart.received_balls[i].distance - temp_ball.distance) < min_dis) {
                        min_dis = abs(bcuart.received_balls[i].distance - temp_ball.distance);
                        min_idx = i;
                    }
                }

                temp_ball.conf = max(bcuart.received_balls[min_idx].conf, temp_ball.conf);
                temp_ball.distance = bcuart.received_balls[min_idx].distance;
                temp_ball.num_hits += 1;
                temp_ball.black = bcuart.received_balls[min_idx].black;

                logln("... at %.3f distance with %d hits and %.3f conf", temp_ball.distance, temp_ball.num_hits, temp_ball.conf);
            }
            else
            {
                logln("New Ball");

                int max_idx = 0;
                for (int i = 1; i < bcuart.num_balls_in_array; ++i) {
                    if (bcuart.received_balls[i].conf > bcuart.received_balls[max_idx].conf) {
                        max_idx = i;
                    }
                }
                
                temp_ball.conf = bcuart.received_balls[max_idx].conf;
                temp_ball.distance = bcuart.received_balls[max_idx].distance;
                temp_ball.num_hits = 1;
                temp_ball.black = bcuart.received_balls[max_idx].black;
            }

            if ((float)temp_ball.num_hits * temp_ball.conf > 4) // TODO: Find correct value here
            {
                robot.move(0, 0);
                robot.claw->set_state(Claw::BOTTOM_MID);

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
                move_back_to_center->calculate_time_by_distance(min(temp_ball.distance, 800.0f));
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

            last_time_was_ball_timer.reset();
        }
    }
    else if (robot.cur_room_state == robot.ROOM_STATE_PUT_BALL_IN_CORNER_STEP_1)
    {
        if (robot.prev_room_state != robot.ROOM_STATE_PUT_BALL_IN_CORNER_STEP_1)
        {
            robot.move(0, 0);
            robot.prev_room_state = robot.cur_room_state;
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

    // else if (robot.cur_room_state == robot.ROOM_STATE_SEARCHING_EXIT)
    // {
    //     uint16_t tof_dis = robot.tof_side->getMeasurement();
    //     if (robot.prev_room_state != robot.cur_room_state)
    //     {
    //         robot.prev_room_state = robot.cur_room_state;
    //         follow_wall_last_tof_value = tof_dis;
    //     }

    //     if (robot.tof_side->getMeasurementError() != tof::TOF_ERROR_MAX_DISTANCE) // Closerange needs to see the wall, or else it cant work
    //     {
    //         int motor_l_val = DRIVE_SPEED_RAUM;
    //         int motor_r_val = DRIVE_SPEED_RAUM;

    //         if (robot.tof_side->getMeasurementError() == tof::TOF_ERROR_NONE)
    //         {
    //             float error = ROOM_MOVE_ALONG_WALL_DISTANCE - tof_dis;

    //             motor_l_val = float(DRIVE_SPEED_RAUM) - ROOM_MOVE_ALONG_WALL_LINEAR_FACTOR * error;
    //             motor_r_val = float(DRIVE_SPEED_RAUM) + ROOM_MOVE_ALONG_WALL_LINEAR_FACTOR * error;
    //         }

    //         robot.move(motor_l_val, motor_r_val);
    //     }

    //     if (robot.tof_side->getMeasurementError() == tof::TOF_ERROR_MAX_DISTANCE || (robot.tof_side->getMeasurementError() == tof::TOF_ERROR_NONE && abs(follow_wall_last_tof_value - tof_dis) > 100))
    //     {
    //         // Maybe Hole
    //         robot.move(20, 20);
    //         delay(400);
    //         robot.move(20, -20);
    //         delay(TURN_90_DEG_DELAY);
    //         robot.room_has_reached_end(); // Just Reset everything

    //         robot.move(20, 20);

    //         Robot::room_end_types end_type = Robot::ROOM_HAS_NOT_REACHED_END;
    //         while (end_type == Robot::ROOM_HAS_NOT_REACHED_END)
    //         {
    //             end_type = robot.room_has_reached_end();
    //             display.tick();
    //             delay(10);
    //         }

    //         if (end_type == Robot::ROOM_HAS_REACHED_SILVER_LINE)
    //         {
    //             robot.move(-20, -20);
    //             delay(200);
    //             cuart.silver_line = false;
    //         }
    //         else if (end_type == Robot::ROOM_HAS_REACHED_GREEN_LINE)
    //         {
    //             robot.move(20, 20);
    //             delay(1000);
    //             robot.move(0, 0);
    //             cuart.green_line = false;
    //             robot.cur_drive_mode = Robot::ROBOT_DRIVE_MODE_LINE;
    //             delay(1000);
    //             robot.move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
    //             return;
    //         }

    //         turn_90_while_next_to_wall();
    //         cuart.silver_line = false;
    //     }

    //     Robot::room_end_types end_type = robot.room_has_reached_end();
    //     if (end_type != Robot::ROOM_HAS_NOT_REACHED_END)
    //     {
    //         if (end_type == Robot::ROOM_HAS_REACHED_SILVER_LINE)
    //         {
    //             robot.move(-20, -20);
    //             delay(200);
    //             cuart.silver_line = false;
    //         }
    //         else if (end_type == Robot::ROOM_HAS_REACHED_GREEN_LINE)
    //         {
    //             robot.move(20, 20);
    //             delay(1000);
    //             robot.move(0, 0);
    //             cuart.green_line = false;
    //             robot.cur_drive_mode = Robot::ROBOT_DRIVE_MODE_LINE;
    //             delay(1000);
    //             robot.move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
    //             return;
    //         }
    //         turn_90_while_next_to_wall();
    //     }
        
    //     if (robot.tof_side->getMeasurementError() == tof::TOF_ERROR_NONE)
    //         follow_wall_last_tof_value = tof_dis;
    // }
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
    // robot.moving_to_balls_target.pos.x_mm = robot.moving_to_balls_target.pos.x_mm + delta_distance * c;
    // robot.moving_to_balls_target.pos.y_mm = robot.moving_to_balls_target.pos.y_mm + delta_distance * s;

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
        robot.move(10, -10);
    else
        robot.move(-10, 10);

    while(abs(robot.angle - target) > ROTATE_TO_ANGLE_TOLERANCE)
    {
        delay(5);
        display.tick();
        robot.tick();
        logln("Rotate to Angle. Cur: %.3f Target:%.3f", robot.angle, target);
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

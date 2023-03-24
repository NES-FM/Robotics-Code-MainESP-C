// Backups:

#pragma region Old Robot.tick
if (detectingBallsEnabled && bcuart_ref->num_balls_in_array > 0)  // TO-DO: Bugs if balls are too close to robot
{
    logln("Balls: ");
    for (int i = 0; i < bcuart_ref->num_balls_in_array; i++)
    {
        log_inline_begin();
        auto received_ball = bcuart_ref->received_balls[i];
        point irl_pos;
        irl_pos.x_mm = -received_ball.x_offset*10; // *10, because x_offset is in cm, while irl_pos is in mm  // - because negative x_offset means right of robot, if robot is pointing forward
        irl_pos.y_mm = -received_ball.distance*10 - (0.5*height); // - because the robot detects balls towards the back  // - (0.5*height) because b.distance counts starting from the back edge of the robot
        irl_pos = rotate_point_around_origin(irl_pos, angle);
        // log_inline("x%d y%d c%.3f %s | ", irl_pos.x_mm, irl_pos.y_mm, b.conf, b.black ? "black" : "silver");

        bool ball_found_before = false;       

        if (num_detected_balls == (sizeof(detected_balls) / sizeof(detected_balls[0])))
        {
            log_inline("ERROR! TOO MANY DETECTED BALLS!!!");
        }
        else
        {
            for (int b_num = 0; b_num < num_detected_balls; b_num++)
            {
                ball old_ball = detected_balls[b_num];
                if (distance_between_points(old_ball.pos, irl_pos) < (60 + old_ball.num_hits*5) && old_ball.black == received_ball.black)
                {
                    log_inline("Ball was found before!");
                    detected_balls[b_num].conf = max(old_ball.conf, received_ball.conf);
                    detected_balls[b_num].num_hits += 1;
                    detected_balls[b_num].pos = irl_pos;
                    ball_found_before = true;
                    break;
                }
            }
            if (!ball_found_before)
            {
                log_inline("Ball not found before! Appending...");

                int b_num = num_detected_balls;
                num_detected_balls++;
                detected_balls[b_num].black = received_ball.black;
                detected_balls[b_num].conf = received_ball.conf;
                detected_balls[b_num].num_hits = 1;
                detected_balls[b_num].pos = irl_pos;
                log_inline(" Done!");
            }
        }
        log_inline_end();
    }
    bcuart_ref->reset_balls();
}

if (detectingCornerEnabled && bcuart_ref->corner_valid)
{
    log_inline_begin();log_inline("Corner  ");
    auto recieved_corner = bcuart_ref->received_corner;
    point irl_pos;
    irl_pos.x_mm = -recieved_corner.x_offset*10; // *10, because x_offset is in cm, while irl_pos is in mm  // - because negative x_offset means right of robot, if robot is pointing forward
    irl_pos.y_mm = -recieved_corner.distance*10 - (0.5*height); // - because the robot detects balls towards the back  // - (0.5*height) because b.distance counts starting from the back edge of the robot
    irl_pos = rotate_point_around_origin(irl_pos, angle);

    bool corner_found_before = false;

    for (corner* old_corner : possible_corners)
    {
        if (distance_between_points(old_corner->center_pos, irl_pos) < 250)
        {
            logln("Corner Found before!");
            old_corner->last_pos = irl_pos;
            old_corner->center_pos = midpoint_between_points(old_corner->first_pos, old_corner->last_pos);
            old_corner->conf = max(old_corner->conf, recieved_corner.conf);
            old_corner->num_hits += 1;
            corner_found_before = true;
            break;
        }
    }

    if (!corner_found_before)
    {
        logln("New Corner -> Appending!");
        corner* new_corner = new corner();
        new_corner->first_pos = irl_pos;
        new_corner->center_pos = irl_pos;
        new_corner->last_pos = irl_pos;
        new_corner->conf = recieved_corner.conf;
        new_corner->num_hits = 1;

        possible_corners.push_back(new_corner);
    }

    bcuart_ref->reset_corner();
}
#pragma endregion

#pragma region Old drive_room rotated 360 degrees
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

    // Selecting Ball to follow
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
    robot.moving_to_balls_target = robot.detected_balls[max_conf_index]; // TO-DO: remember how many silver & black balls were put down -> only get black ball if 2 silver were put down

    logln("Selected Ball (%d|%d), conf=%d*%f=%f as a target!", robot.moving_to_balls_target.pos.x_mm, robot.moving_to_balls_target.pos.y_mm, robot.moving_to_balls_target.num_hits, robot.moving_to_balls_target.conf, max_conf);

    // Find the corner with the highest value of conf * num_hits
    auto max_corner_it = std::max_element(robot.possible_corners.begin(), robot.possible_corners.end(),
        [](Robot::corner* a, Robot::corner* b) { return a->conf * (float)a->num_hits < b->conf * (float)b->num_hits; });

    // Dereference the iterator to get the max element
    robot.most_likely_corner = *max_corner_it;

    logln("Selected Corner (%d|%d)!", robot.most_likely_corner->center_pos.x_mm, robot.most_likely_corner->center_pos.y_mm);

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
    // TO-DO: else if ball is not within 20cm of center line (ie next to wall)
    // TO-DO: else if ball is too close to robot

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
#pragma endregion

#pragma region Old Put Ball In Corner Step 1
float angle = robot.angle;
if (robot.prev_room_state != robot.ROOM_STATE_PUT_BALL_IN_CORNER_STEP_1)
{
    robot.move(0, 0);
    robot.prev_room_state = robot.cur_room_state;
    // robot.claw->setTofContinuous(false);
    clear_queue();

    float time_to_move = Robot::distance_between_points(robot.Origin, start_pos_of_moving_to_ball) / robot.millimeters_per_millisecond_40_speed / 2;
    robot.move(20, 20);
    delay((float)time_to_move);
    adjust_moving_to_balls_target((float)time_to_move);

    float angle_to_center_pos = 90 - (atan((float)robot.most_likely_corner->center_pos.y_mm / (float)robot.most_likely_corner->center_pos.x_mm) * RAD_TO_DEG);
    if (robot.most_likely_corner->center_pos.x_mm > 0)
        angle_to_center_pos += 180.0;

    float begin_angle = angle_to_center_pos - 90;
    find_corner_again_target_angle = angle_to_center_pos + 60;

    // robot.move(-20, 20);
    rotate_to_angle(begin_angle, false);


    robot.move(5, -5);
    clear_possible_corners();
    robot.detectingCornerEnabled = true;
    rotate_balls_360_start_angle = angle;
    rotated_balls_was_at_180_degrees = false;
    clear_queue();

    robot.claw->set_state(Claw::SIDE_CLOSED);

    angle = robot.angle;
    find_corner_again_timeout.set_target(7000);
    find_corner_again_timeout.reset();
}

// if (abs(angle - robot.compass->keep_in_360_range(rotate_balls_360_start_angle - 180)) < 15)
//     rotated_balls_was_at_180_degrees = true;

if (abs(angle-find_corner_again_target_angle) < ROTATE_TO_ANGLE_TOLERANCE || find_corner_again_timeout.has_reached_target())//rotated_balls_was_at_180_degrees && abs(angle - rotate_balls_360_start_angle) < ROTATE_TO_ANGLE_TOLERANCE)  // Rotated approx. 360 degrees -> goto next step
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
    rotate_on_target1->motor_left_speed = 10;
    rotate_on_target1->motor_right_speed = -10;
    moving_in_room_queue.push_back(rotate_on_target1);
    log_inline("Step 1: rotate_on_target1 with %d|%d speed and target_angle %.1f\r\n", rotate_on_target1->motor_left_speed, rotate_on_target1->motor_right_speed, rotate_on_target1->target_angle);

    moving_in_room_distance_by_time* delay_1 = new moving_in_room_distance_by_time();
    delay_1->_robot = &robot;
    delay_1->motor_left_speed = 0;
    delay_1->motor_right_speed = 0;
    delay_1->time_left = 300;
    moving_in_room_queue.push_back(delay_1);

    moving_in_room_distance_by_time* move_to_target2 = new moving_in_room_distance_by_time();
    move_to_target2->_robot = &robot;
    move_to_target2->motor_left_speed = 20;
    move_to_target2->motor_right_speed = 20;
    move_to_target2->calculate_time_by_distance(Robot::distance_between_points(robot.Origin, target_2) - 100);
    moving_in_room_queue.push_back(move_to_target2);
    log_inline("Step 2: move_to_target_2 with 40|40 speed and time %d\r\n", move_to_target2->time_left);

    moving_in_room_distance_by_time* delay_2 = new moving_in_room_distance_by_time();
    delay_2->_robot = &robot;
    delay_2->motor_left_speed = 0;
    delay_2->motor_right_speed = 0;
    delay_2->time_left = 300;
    moving_in_room_queue.push_back(delay_2);

    moving_in_room_rotate_to_deg* rotate_on_target2 = new moving_in_room_rotate_to_deg();
    rotate_on_target2->_robot = &robot;
    rotate_on_target2->motor_left_speed = -10;
    rotate_on_target2->motor_right_speed = 10;
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
    move_to_corner->time_after = 6000;
    moving_in_room_queue.push_back(move_to_corner);
    log_inline("Step 5: move_to_corner with %d|%d speed \r\n", move_to_corner->motor_left_speed, move_to_corner->motor_right_speed);

    moving_in_room_distance_by_time* delay_3 = new moving_in_room_distance_by_time();
    delay_3->_robot = &robot;
    delay_3->motor_left_speed = 0;
    delay_3->motor_right_speed = 0;
    delay_3->time_left = 300;
    moving_in_room_queue.push_back(delay_3);

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
#pragma endregion

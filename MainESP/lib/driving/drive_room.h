#include "Timer.h"

Timer wall_timer;
Timer corner_timer;

#define SHORTWALL (wall_time < 4000)
#define LONGWALL (wall_time >= 4000)
#define CORNER (corner_time >= 1000)
#define NOCORNER (corner_time < 1000)

#define TIME_ADD_FOR_CORNER 1080
#define MILLIMETERS_PER_MILLISECOND 0.1770833333

uint16_t how_far_have_i_traveled();

void drive_room()
{
    if (robot.cur_room_state == Robot::ROOM_STATE_INITAL_MOVE_AROUND_WALLS)
    {
        if (wall_timer.state() == STOPPED)
        {
            wall_timer.start();
        }
        robot.room_move_along_wall();
        Robot::room_end_types has_reached_end = robot.room_has_reached_end();

        if (has_reached_end != Robot::room_end_types::ROOM_HAS_NOT_REACHED_END)
        {
            if (has_reached_end == Robot::room_end_types::ROOM_HAS_REACHED_TASTER_LEFT || has_reached_end == Robot::room_end_types::ROOM_HAS_REACHED_TASTER_RIGHT)
            {
                wall_timer.stop();
                unsigned long wall_time = wall_timer.read();
                logln("Reached end with time of: %u", wall_time);

                if (robot.last_time_was_corner)
                {
                    logln("Last wall was corner (+1080)");
                    robot.last_time_was_corner = false;
                    wall_time += TIME_ADD_FOR_CORNER; // Adds one tile of time to compensate for the corner
                }

                if (!robot.room_corner_found)
                {
                    logln("Corner not found");
                    robot.move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL); // Rotate 45 deg to be paralel to the might be corner
                    delay(400);
                    robot.room_rotate_relative_degrees(-50);
                    robot.move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);

                    corner_timer.start(); // Measuring the time it takes to reach wall again
                    has_reached_end = robot.room_has_reached_end();
                    while (has_reached_end != Robot::room_end_types::ROOM_HAS_REACHED_TASTER_LEFT && has_reached_end != Robot::room_end_types::ROOM_HAS_REACHED_TASTER_RIGHT) // While no taster is hit
                    {
                        has_reached_end = robot.room_has_reached_end();
                        delay(5);
                    }
                    corner_timer.stop();
                    unsigned long corner_time = corner_timer.read();
                    logln("Corner took %u ms", corner_time);

                    if (CORNER) // Was corner
                    {
                        logln("Was Corner (+1080)");
                        wall_time += TIME_ADD_FOR_CORNER; // Adds one tile of time to compensate for the corner
                        robot.room_corner_found = true;

                        if (robot.cur_moving_wall == Robot::WALL_FIRST_UNKNOWN_WALL)
                        {
                            logln("Unknown wall");
                            if (LONGWALL)
                            {
                                logln("Corner in TL");
                                robot.room_corner_pos.x_mm = ROOM_CORNER_POS_TL_X;
                                robot.room_corner_pos.y_mm = ROOM_CORNER_POS_TL_Y;
                            }
                            else
                            {
                                logln("Corner in TR");
                                robot.room_corner_pos.x_mm = ROOM_CORNER_POS_TR_X;
                                robot.room_corner_pos.y_mm = ROOM_CORNER_POS_TR_Y;
                            }
                        }
                        else if (robot.cur_moving_wall == Robot::WALL_2_LONG || robot.cur_moving_wall == Robot::WALL_1_LONG)
                        {
                            logln("Corner in TL");
                            robot.room_corner_pos.x_mm = ROOM_CORNER_POS_TL_X;
                            robot.room_corner_pos.y_mm = ROOM_CORNER_POS_TL_Y;
                        }
                        else if (robot.cur_moving_wall == Robot::WALL_3_SHORT || robot.cur_moving_wall == Robot::WALL_2_SHORT)
                        {
                            logln("Corner in BL");
                            robot.room_corner_pos.x_mm = ROOM_CORNER_POS_BL_X;
                            robot.room_corner_pos.y_mm = ROOM_CORNER_POS_BL_Y;
                        }
                        else if (robot.cur_moving_wall == Robot::WALL_4_LONG || robot.cur_moving_wall == Robot::WALL_3_LONG)
                        {
                            logln("Corner in BR");
                            robot.room_corner_pos.x_mm = ROOM_CORNER_POS_BR_X;
                            robot.room_corner_pos.y_mm = ROOM_CORNER_POS_BR_Y;
                        }
                        else if (robot.cur_moving_wall == Robot::WALL_1_SHORT || robot.cur_moving_wall == Robot::WALL_4_SHORT)
                        {
                            logln("Corner in TR");
                            robot.room_corner_pos.x_mm = ROOM_CORNER_POS_TR_X;
                            robot.room_corner_pos.y_mm = ROOM_CORNER_POS_TR_Y;
                        }
                    }

                    robot.move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL); // Rotate 45 deg to be paralel with wall again
                    delay(400);
                    robot.room_rotate_relative_degrees(-45);
                    robot.move(DRIVE_SPEED_RAUM, DRIVE_SPEED_RAUM);
                }
                else
                {
                    logln("Corner was already found -> 90 degrees");
                    robot.move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
                    delay(500);
                    robot.room_rotate_relative_degrees(-90);
                    robot.move(DRIVE_SPEED_RAUM, DRIVE_SPEED_RAUM);
                }
                
                if (robot.cur_moving_wall == Robot::WALL_FIRST_UNKNOWN_WALL)
                {
                    if (SHORTWALL)
                    {
                        logln("Entry wall was short, setting entry pos to 1025|0");
                        robot.room_entry_found = true;
                        robot.room_entry_pos.x_mm = 1025/* - offset of room entry*/;
                        robot.room_entry_pos.y_mm = 0;
                        robot.cur_moving_wall = Robot::WALL_1_SHORT; // Was moving along short wall 1
                    }
                    else
                    {
                        logln("Entry wall was long, setting entry pos to width|725");
                        robot.room_entry_found = true;
                        robot.room_entry_pos.x_mm = robot.room_width;
                        robot.room_entry_pos.y_mm = 725/* - offset of room entry*/;
                        robot.cur_moving_wall = Robot::WALL_1_LONG; // Was moving along long wall 1
                    }
                }


                // -------------- Setting next cur_moving_wall --------------
                if (robot.cur_moving_wall == Robot::WALL_1_SHORT)
                    robot.cur_moving_wall = Robot::WALL_2_LONG;
                else if (robot.cur_moving_wall == Robot::WALL_1_LONG)
                    robot.cur_moving_wall = Robot::WALL_2_SHORT;

                else if (robot.cur_moving_wall == Robot::WALL_2_SHORT)
                    robot.cur_moving_wall = Robot::WALL_3_LONG;
                else if (robot.cur_moving_wall == Robot::WALL_2_LONG)
                    robot.cur_moving_wall = Robot::WALL_3_SHORT;

                else if (robot.cur_moving_wall == Robot::WALL_3_SHORT)
                    robot.cur_moving_wall = Robot::WALL_4_LONG;
                else if (robot.cur_moving_wall == Robot::WALL_3_LONG)
                    robot.cur_moving_wall = Robot::WALL_4_SHORT;

                else if (robot.cur_moving_wall == Robot::WALL_4_SHORT)
                    robot.cur_moving_wall = Robot::WALL_1_LONG;
                else if (robot.cur_moving_wall == Robot::WALL_4_LONG)
                    robot.cur_moving_wall = Robot::WALL_1_SHORT;

                wall_timer.start();
            }
            else if (has_reached_end == Robot::room_end_types::ROOM_HAS_REACHED_GREEN_LINE || has_reached_end == Robot::room_end_types::ROOM_HAS_REACHED_SILVER_LINE) // For now just ignore silver and green lines
            {
                // Here should be marked as entry / exit
                cuart.green_line = false;
                cuart.silver_line = false;
                logln("Some line was triggered");
            }
        }
        else
        {
            // if (max_distance_of_closerange)
            // { -> pause time measurement and explore hole in wall
            // }

            if (robot.cur_moving_wall == Robot::WALL_2_LONG || robot.cur_moving_wall == Robot::WALL_1_LONG) // Top Wall
            {
                robot.pos.x_mm = robot.room_width - (0.5*robot.height) - 80 - how_far_have_i_traveled();
                robot.pos.y_mm = robot.room_height - (0.5*robot.width) - 80;
            }
            else if (robot.cur_moving_wall == Robot::WALL_3_SHORT || robot.cur_moving_wall == Robot::WALL_2_SHORT) // Left Wall
            {
                robot.pos.x_mm = 0 + (0.5*robot.width) + 80;
                robot.pos.y_mm = robot.room_height - (0.5*robot.height) - 80 - how_far_have_i_traveled();
            }
            else if (robot.cur_moving_wall == Robot::WALL_4_LONG || robot.cur_moving_wall == Robot::WALL_3_LONG) // Bottom Wall
            {
                robot.pos.x_mm = 0 + (0.5*robot.height) + 80 + how_far_have_i_traveled();
                robot.pos.y_mm = 0 + (0.5*robot.width) + 80;
            }
            else if (robot.cur_moving_wall == Robot::WALL_1_SHORT || robot.cur_moving_wall == Robot::WALL_4_SHORT) // Right Wall
            {
                robot.pos.x_mm = robot.room_width - (0.5*robot.width) - 80;
                robot.pos.y_mm = 0 + (0.5*robot.height) + 80 + how_far_have_i_traveled();
            }

            if (robot.cur_moving_wall == Robot::WALL_1_SHORT || robot.cur_moving_wall == Robot::WALL_4_SHORT) // Right Wall
            {
                if (robot.pos.y_mm > 420) // Reached center tile of right wall
                {
                    robot.room_rotate_relative_degrees(-90);
                    robot.move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
                    int back_dis = robot.tof_back->getMeasurement();
                    while (back_dis > 40)
                    {
                        back_dis = robot.tof_back->getMeasurement();
                        delay(5);
                    }
                    delay(200); // Move back a little more
                    robot.move(0, 0);
                    delay(100);
                    robot.setRoomSearchBallsBeginningAngle();
                    robot.move(DRIVE_SPEED_HALF, DRIVE_SPEED_HALF);
                    robot.cur_room_state = Robot::ROOM_STATE_SCAN_FOR_BALLS;
                }
            }
        }
    }
    else if (robot.cur_room_state == Robot::ROOM_STATE_SCAN_FOR_BALLS)
    {
        float angle = robot.compass->keep_in_360_range(robot.compass->get_angle() - robot.room_search_balls_beginning_angle);
        int left_dis = robot.tof_left->getMeasurement();
        int right_dis = robot.tof_right->getMeasurement();
        int back_dis = robot.tof_back->getMeasurement();

        Robot::room_end_types has_reached_end = robot.room_has_reached_end();
        if (has_reached_end != Robot::ROOM_HAS_NOT_REACHED_END)
        {
            // idk lol
            robot.move(0, 0);
        }
        else if (back_dis > 600) // After half
        {
            // Turn around
            robot.room_rotate_relative_degrees(180);
            robot.move(-DRIVE_SPEED_HALF, -DRIVE_SPEED_HALF);
            robot.searching_balls_moving_backward = true;
        }
        else
        {
            // Add points
        }
    }
}

uint16_t how_far_have_i_traveled()
{
    uint32_t millisec = wall_timer.read();
    if (robot.last_time_was_corner)
        millisec += TIME_ADD_FOR_CORNER;
    
    return millisec * MILLIMETERS_PER_MILLISECOND;
}

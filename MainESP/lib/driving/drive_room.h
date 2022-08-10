#define SHORTWALL (wall_time < 4000)
#define LONGWALL (wall_time >= 4000)
#define CORNER (corner_time >= 1000)
#define NOCORNER (corner_time < 1000)

void drive_room()
{
    robot.room_move_along_wall();
    Robot::room_end_types has_reached_end = robot.room_has_reached_end();

    if (has_reached_end != Robot::room_end_types::ROOM_HAS_NOT_REACHED_END)
    {
        if (has_reached_end == Robot::room_end_types::ROOM_HAS_REACHED_TASTER_LEFT || has_reached_end == Robot::room_end_types::ROOM_HAS_REACHED_TASTER_RIGHT)
        {
            unsigned long wall_time = robot.room_time_measure_stop();
            logln("Reached end with time of: %u", wall_time);

            if (robot.last_time_was_corner)
            {
                robot.last_time_was_corner = false;
                wall_time += 1080; // Adds one tile of time to compensate for the corner
            }

            if (!robot.room_corner_found)
            {
                robot.move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL); // Rotate 45 deg to be paralel to the might be corner
                delay(400);
                robot.room_rotate_relative_degrees(-50);
                robot.move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);

                robot.room_time_measure_start(); // Measuring the time it takes to reach wall again
                has_reached_end = robot.room_has_reached_end();
                while (has_reached_end != Robot::room_end_types::ROOM_HAS_REACHED_TASTER_LEFT && has_reached_end != Robot::room_end_types::ROOM_HAS_REACHED_TASTER_RIGHT) // While no taster is hit
                {
                    has_reached_end = robot.room_has_reached_end();
                    delay(5);
                }
                unsigned long corner_time = robot.room_time_measure_stop();
                logln("Corner took %u ms", corner_time);

                if (CORNER) // Was corner
                {
                    wall_time += 1080; // Adds one tile of time to compensate for the corner
                    robot.room_corner_found = true;

                    if (robot.cur_moving_wall == Robot::WALL_FIRST_UNKNOWN_WALL)
                    {
                        if (LONGWALL)
                        {
                            robot.room_corner_pos.x_mm = ROOM_CORNER_POS_TL_X;
                            robot.room_corner_pos.y_mm = ROOM_CORNER_POS_TL_Y;
                        }
                        else
                        {
                            robot.room_corner_pos.x_mm = ROOM_CORNER_POS_TR_X;
                            robot.room_corner_pos.y_mm = ROOM_CORNER_POS_TR_Y;
                        }
                    }
                    else if (robot.cur_moving_wall == Robot::WALL_2_LONG || robot.cur_moving_wall == Robot::WALL_1_LONG)
                    {
                        robot.room_corner_pos.x_mm = ROOM_CORNER_POS_TL_X;
                        robot.room_corner_pos.y_mm = ROOM_CORNER_POS_TL_Y;
                    }
                    else if (robot.cur_moving_wall == Robot::WALL_3_SHORT || robot.cur_moving_wall == Robot::WALL_2_SHORT)
                    {
                        robot.room_corner_pos.x_mm = ROOM_CORNER_POS_BL_X;
                        robot.room_corner_pos.y_mm = ROOM_CORNER_POS_BL_Y;
                    }
                    else if (robot.cur_moving_wall == Robot::WALL_4_LONG || robot.cur_moving_wall == Robot::WALL_3_LONG)
                    {
                        robot.room_corner_pos.x_mm = ROOM_CORNER_POS_BR_X;
                        robot.room_corner_pos.y_mm = ROOM_CORNER_POS_BR_Y;
                    }
                    else if (robot.cur_moving_wall == Robot::WALL_1_SHORT || robot.cur_moving_wall == Robot::WALL_4_SHORT)
                    {
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
                robot.move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
                delay(500);
                robot.room_rotate_relative_degrees(-90);
                robot.move(DRIVE_SPEED_RAUM, DRIVE_SPEED_RAUM);
            }
            
            if (robot.cur_moving_wall == Robot::WALL_FIRST_UNKNOWN_WALL)
            {
                if (LONGWALL)
                {
                    robot.room_entry_found = true;
                    robot.room_entry_pos.x_mm = 1025/* - offset of room entry*/;
                    robot.room_entry_pos.y_mm = 0;
                    robot.cur_moving_wall = Robot::WALL_1_SHORT; // Was moving along short wall 1
                }
                else
                {
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


            robot.room_time_measure_start();
        }
        else if (has_reached_end == Robot::room_end_types::ROOM_HAS_REACHED_GREEN_LINE || has_reached_end == Robot::room_end_types::ROOM_HAS_REACHED_SILVER_LINE) // For now just ignore silver and green lines
        {
            // Here should be marked as entry / exit
            cuart.green_line = false;
            cuart.silver_line = false;
        }
        // else if (max_distance_of_closerange -> pause time measurement and explore hole in wall)
    }
}
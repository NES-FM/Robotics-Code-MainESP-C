#pragma once

int old_left_speed = 0;
int old_right_speed = 0;

bool check_left_right()  // Returns true if line has been found by this method
{
    white_timer.set_target(1000);
    white_timer.reset();
    robot.move(-DRIVE_SPEED_CORNER, DRIVE_SPEED_CORNER);
    while(cuart.array_total <= 2 && !white_timer.has_reached_target())
    {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    if (!white_timer.has_reached_target()) // If exit before target reached: line found
    {
        return true;
    }
    
    white_timer.set_target(2000);
    white_timer.reset();
    robot.move(DRIVE_SPEED_CORNER, -DRIVE_SPEED_CORNER);
    while(cuart.array_total <= 2 && !white_timer.has_reached_target())
    {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    if (!white_timer.has_reached_target()) // If exit before target reached: line found
    {
        return true;
    }

    white_timer.set_target(1000);
    white_timer.reset();
    robot.move(-DRIVE_SPEED_CORNER, DRIVE_SPEED_CORNER);
    while(cuart.array_total <= 2 && !white_timer.has_reached_target())
    {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    if (!white_timer.has_reached_target()) // If exit before target reached: line found
    {
        return true;
    }

    return false;
}

void center_line()
{
    bool rotating_left = (robot.motor_left->motor_speed < robot.motor_right->motor_speed);
    while(cuart.array_total >= 2 && cuart.array_mid_sensor <= 2) // While there are some pixels continue rotating until mid sensor is reached
    {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    if (cuart.array_mid_sensor >= 2) // If MidSensor is reached -> done
    {
        robot.move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
        return;
    }

    robot.move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL); // If not, move forward until there are pixels
    while(cuart.array_total <= 2)
    {
        display.tick();
        vTaskDelay(watchdog_delay);
    }

    if (rotating_left)
        robot.move(-DRIVE_SPEED_CORNER, DRIVE_SPEED_CORNER);
    else
        robot.move(DRIVE_SPEED_CORNER, -DRIVE_SPEED_CORNER);

    center_line(); // continue until done
}

void check_backwards()
{
    bool result = check_left_right();
    if (result)
    {
        center_line();
        return;
    }
    
    white_timer.set_target(2000);
    white_timer.reset();
    robot.move(-old_left_speed, -old_right_speed);
    while(cuart.array_total <= 2 && !white_timer.has_reached_target())
    {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    if (white_timer.has_reached_target()) // If it has reached the target, no line has been found -> check left right again
    {
        check_backwards();
    }
    else // Line has been found
    {
        delay(100);
        robot.move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
        //center_line();
    }
}

void find_line()
{
    old_left_speed = robot.motor_left->motor_speed;
    old_right_speed = robot.motor_right->motor_speed;
    check_backwards();
    white_timer.set_target(1500);
}

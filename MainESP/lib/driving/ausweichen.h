#pragma once

target_timer ausweichen_failsave(30000);

void ausweichen()
{
    robot.move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    delay(500);
    robot.move(DRIVE_SPEED_CORNER, -DRIVE_SPEED_CORNER);
    delay(900);
    robot.move(15, 35);
    for (int x = 0; x < 1500; x+=10)
    {
        display.tick();
        delay(10);
    }
    ausweichen_failsave.reset();
    while(cuart.array_total < 5)
    {
        display.tick();
        vTaskDelay(watchdog_delay);
        // if (ausweichen_failsave.has_reached_target())
        // {
        //     robot.move(-20, -20);
        //     delay(2000);
        //     robot.startRoom();
        //     return;
        // }
    }
    robot.move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
    while(!cuart.sensor_array[0])
    {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    robot.move(DRIVE_SPEED_CORNER, -DRIVE_SPEED_CORNER);
    while(cuart.array_mid_sensor < 3)
    {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    robot.move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
    white_timer.reset();
}

#pragma once

void ausweichen()
{
    robot.move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    delay(500);
    robot.move(DRIVE_SPEED_CORNER, -DRIVE_SPEED_CORNER);
    delay(900);
    robot.move(15, 35);
    for (int x = 0; x < 3000; x+=10)
    {
        display.tick();
        delay(10);
    }
    while(cuart.array_mid_sensor < 3)
    {
        display.tick();
        vTaskDelay(watchdog_delay);
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

#pragma once

void ausweichen()
{
    move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    delay(200);
    move(DRIVE_SPEED_CORNER, -DRIVE_SPEED_CORNER);
    delay(500);
    move(8, 35);
    delay(500);
    while(cuart.array_mid_sensor < 3)
    {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
    while(!cuart.sensor_array[0])
    {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    move(DRIVE_SPEED_CORNER, -DRIVE_SPEED_CORNER);
    while(cuart.array_mid_sensor < 3)
    {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
}

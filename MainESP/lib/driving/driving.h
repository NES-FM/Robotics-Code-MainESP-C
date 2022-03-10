#ifndef DRIVING_H
#define DRIVING_H

#define DRIVING_TYPE 0

#include "../../include/drive_speeds.h"

#define DEBUG_MOTOR_VALUES 0

TickType_t watchdog_delay = pdMS_TO_TICKS(5);

void move(int speed_left, int speed_right)
{
    motor_left.move(speed_left);
    motor_right.move(speed_right);
}

#if DRIVING_TYPE == 0
#include "drive_sensor_array.h"
#elif DRIVING_TYPE == 1
#include "drive_new.h"
#endif

void drive()
{
    #if DRIVING_TYPE == 0
    drive_sensor_array();
    #elif DRIVING_TYPE == 1
    drive_new();
    #endif
}

#endif /* DRIVING_H */

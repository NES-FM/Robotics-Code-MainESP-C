#ifndef DRIVING_H
#define DRIVING_H

#define DRIVING_TYPE 0

#include "../../include/drive_speeds.h"
#include "timer.h"

timer resend_motor_timer(100);

#define DEBUG_MOTOR_VALUES 0

TickType_t watchdog_delay = pdMS_TO_TICKS(5);

bool in_raum = true; //NEEDS TO BE CHANGED

#if DRIVING_TYPE == 0
#include "drive_sensor_array.h"
#elif DRIVING_TYPE == 1
#include "drive_new.h"
#endif

void drive()
{
    // #if DRIVING_TYPE == 0
    if (!in_raum)
    {
        drive_sensor_array();
    }
    else
    {
        robot.calculate_position();
        robot.move(10, -10);
    }
    // #elif DRIVING_TYPE == 1
    // drive_new();
    // #endif

    // if (resend_motor_timer.has_reached_target())
    // {
    //     resend_motor_timer.reset();
    //     motor_left.force_resend();
    //     motor_right.force_resend();
    // }
}

#endif /* DRIVING_H */

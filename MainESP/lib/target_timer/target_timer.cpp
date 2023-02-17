#include "target_timer.h"

target_timer::target_timer()
{
    reset();
}

target_timer::target_timer(int target)
{
    set_target(target);
    reset();
}

void target_timer::reset()
{
    zero_time = millis();
}

void target_timer::set_target(int t)
{
    target = t;
}

bool target_timer::has_reached_target()
{
    return (millis() - target) > zero_time;
}

int target_timer::time_elapsed()
{
    return millis() - zero_time;
}

int target_timer::time_left()
{
    return zero_time - (millis() - target);
}

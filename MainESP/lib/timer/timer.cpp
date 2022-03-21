#include "timer.h"

timer::timer()
{
    reset();
}

timer::timer(int target)
{
    set_target(target);
    reset();
}

void timer::reset()
{
    zero_time = millis();
}

void timer::set_target(int t)
{
    target = t;
}

bool timer::has_reached_target()
{
    return (millis() - target) > zero_time;
}

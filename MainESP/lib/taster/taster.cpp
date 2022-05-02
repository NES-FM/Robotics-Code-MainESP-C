#include "taster.h"

bool taster_class::get_state(taster_name name)
{
    if (name == front_left)
        return front_left_taster->get_state();
    else if (name == front_right)
        return front_right_taster->get_state();

    return false;
}

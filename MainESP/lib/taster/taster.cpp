#include "taster.h"

bool taster_class::get_state(taster_name name)
{
    if (name == front)
        return front_taster->get_state();

    return false;
}

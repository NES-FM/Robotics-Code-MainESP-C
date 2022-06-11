#include "dip.h"

bool DIP::get_state(dips index)
{
    switch (index)
    {
        case wettkampfmodus:
            return _wettk->get_state();
        case dip1:
            return _dip1->get_state();
        case dip2:
            return _dip2->get_state();
    }
    return false;
}

char DIP::get_state()
{
    return (get_state(wettkampfmodus) << 2) || (get_state(dip1) << 1) || (get_state(dip2));
}

bool DIP::get_wettkampfmodus()
{
    return this->get_state(wettkampfmodus);
}

bool DIP::has_changed(dips index)
{
    switch (index)
    {
        case wettkampfmodus:
            return _wettk->state_changed();
        case dip1:
            return _dip1->state_changed();
        case dip2:
            return _dip2->state_changed();
    }
    return false;
}

bool DIP::has_changed()
{
    return _wettk->state_changed() || _dip1->state_changed() || _dip2->state_changed();
}


#include "io_extender.h"

io_extender::io_extender()
{

}

io_extender::ramp_types io_extender::getCurrentRampState()
{
    float angle = accel_roll_degrees;
    if (angle < -10)
        return up;
    else if (angle > 10)
        return down;
    return level;
}

uint16_t io_extender::claw_getMeasurement()
{
    if (_claw_tof_error == TOF_ERROR_NONE)
        return _claw_tof_distance;
    
    return UINT16_MAX;
}

String io_extender::claw_getMeasurementErrorString()
{
    switch (_claw_tof_error)
    {
        case TOF_ERROR_NONE:
            return F("None");
        case TOF_ERROR_NOT_ENABLED:
            return F("Not enabled");
        case TOF_ERROR_TIMEOUT:
            return F("Timeout");
        case TOF_ERROR_MAX_DISTANCE:
            return F("Max distance exceeded");
        case TOF_ERROR_FAILED_TO_INITIALIZE:
            return F("Failed to initialize");
        case TOF_ERROR_MIN_DISTANCE:
            return F("Min distance exceeded");
        case TOF_ERROR_SYSERROR:
            return F("System error");
        case TOF_ERROR_ECE_ERROR:
            return F("ECE failure");
        case TOF_ERROR_NOCONVERGENCE:
            return F("No convergence");
        case TOF_ERROR_SNR_ERROR:
            return F("Signal/Noise Ratio Error");
    }
    return F("Unknown error");
}

bool io_extender::get_taster_state(taster_name name)
{
    if (name == front_left)
        return _fl_state;
    else if (name == front_right)
        return _fr_state;
    else if (name == back_left)
        return _bl_state;
    else if (name == back_right)
        return _br_state;

    return false;
}

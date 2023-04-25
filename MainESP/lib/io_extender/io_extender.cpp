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
    else if (_claw_tof_error == TOF_ERROR_MIN_DISTANCE)
        return 0;
    
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

void io_extender::tick()
{
    if (_enabled)
    {
        #ifdef USE_WIRE_AS_COMM
        Wire.requestFrom((uint8_t)I2C_ADDRESS_IO_EXTENDER, (uint8_t)sizeof(receive_struct), (uint8_t)1);
        Wire.readBytes( (byte*) &rxData, sizeof(receive_struct));
        #endif

        accel_roll_degrees = rxData.accel_value;

        _claw_tof_distance = rxData.claw_tof_value;
        _claw_tof_error = (tof_error_types)rxData.claw_tof_error;

        interpret_taster_states(rxData.taster_values);
    }
}

void io_extender::interpret_taster_states(uint8_t value)
{
    _fl_state = (value >> 3) & 0x01;
    _fr_state = (value >> 2) & 0x01;
    _bl_state = (value >> 1) & 0x01;
    _br_state = value & 0x01;
}


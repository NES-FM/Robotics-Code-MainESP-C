#pragma once

#include <Arduino.h>
#include "../../include/i2c_addresses.h"
#include <Wire.h>
#include "logger.h"

// Values to get of io_extender:
// Accel Roll Degrees
// tof_claw (Distance + Error)

class io_extender
{
    public:
        io_extender();

        // Accel
        enum ramp_types {
            up,
            down,
            level
        };
        ramp_types getCurrentRampState();
        float get_roll_degrees() { return accel_roll_degrees; };

        // Tof claw
        enum tof_error_types {
            TOF_ERROR_NONE,
            TOF_ERROR_TIMEOUT,
            TOF_ERROR_MAX_DISTANCE,
            TOF_ERROR_NOT_ENABLED,
            TOF_ERROR_FAILED_TO_INITIALIZE,
            TOF_ERROR_MIN_DISTANCE,
            TOF_ERROR_SYSERROR,
            TOF_ERROR_ECE_ERROR,
            TOF_ERROR_NOCONVERGENCE,
            TOF_ERROR_SNR_ERROR,
        };
        tof_error_types claw_getMeasurementError() { return _claw_tof_error; };
        String claw_getMeasurementErrorString();
        uint16_t claw_getMeasurement();

    private:
        // Accel
        float accel_roll_degrees = 0.0;

        // Tof claw
        tof_error_types _claw_tof_error = TOF_ERROR_NONE;
        uint16_t _claw_tof_distance = 0;
};

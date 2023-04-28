#pragma once

#include <Arduino.h>
#include "../../include/i2c_addresses.h"
#include <Wire.h>
#include "logger.h"

#define USE_WIRE_AS_COMM
//#define USE_UART_AS_COMM

#define IO_EXT_REFRESH_TIME 10
#define IO_EXT_NOT_ENABLED_RECONNECT_TIME 1000

// Values to get of io_extender:
// Accel Roll Degrees
// tof_claw (Distance + Error)
// taster

class io_extender
{
    public:
        io_extender();

        void enable(bool enabled) { _enabled = enabled; };

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

        // Taster
        enum taster_name {
            front_left,
            front_right,
            back_left,
            back_right
        };
        bool get_taster_state(taster_name name);

        #pragma pack(1)
        struct receive_struct
        {
            float accel_value;
            
            uint16_t claw_tof_value;
            uint8_t claw_tof_error;

            uint8_t taster_values;
        };
        #pragma pack()
        void tick();

    private:
        // Accel
        float accel_roll_degrees = 0.0;

        // Tof claw
        tof_error_types _claw_tof_error = TOF_ERROR_NONE;
        uint16_t _claw_tof_distance = 0;

        // Taster States
        bool _fl_state = false, _fr_state = false, _bl_state = false, _br_state = false;
        void interpret_taster_states(uint8_t value);

        receive_struct rxData;

        bool _enabled = false;

        unsigned long _tick_last_millis = 0;
};

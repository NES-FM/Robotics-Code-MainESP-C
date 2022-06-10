#pragma once

#include "VL53L0X.h"
#include "logger.h"

#define TOF_MAX_VALID_DISTANCE 

class tof {
    public:
        enum tof_error_types {
            TOF_ERROR_NONE,
            TOF_ERROR_TIMEOUT,
            TOF_ERROR_MAX_DISTANCE,
            TOF_ERROR_NOT_ENABLED,
            TOF_ERROR_FAILED_TO_INITIALIZE
        };

        tof(uint8_t xshut, int offset_x, int offset_y, int offset_a) { _xshut = xshut; _offset_x = offset_x; _offset_y = offset_y; _offset_a = offset_a; };
        void init();
        void begin(uint8_t address = 0b0101001);
        void enable(bool enabled);
        void setLongRangeMode(bool mode);
        void setHighAccuracy(bool mode);
        void setHighSpeed(bool mode);
        void setContinuous(bool mode, uint32_t period_ms = 0);
        inline bool timeoutOccurred() { return sensor->timeoutOccurred(); }

        tof_error_types getMeasurementError() { return _error; };
        String getMeasurementErrorString();

        void holdReset();
        void releaseReset();
        void reset();

        void changeAddress(uint8_t address);

        uint16_t getMeasurement();
    private:
        VL53L0X* sensor = new VL53L0X();

        bool _long_range = false;
        bool _high_accuracy = false;
        bool _high_speed = false;

        bool continuous_mode = false;

        uint16_t last_measurement = 0;

        uint8_t _xshut = -1;
        int _offset_x = 0;
        int _offset_y = 0;
        int _offset_a = 0;

        bool _enabled = false;

        tof_error_types _error = TOF_ERROR_NOT_ENABLED;

    friend class Robot;
};

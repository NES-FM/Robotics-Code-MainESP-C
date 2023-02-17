#pragma once

#include "VL53L0X.h"
#include "VL53L1X.h"
#include "Adafruit_VL6180X.h"
#include "logger.h"
#include "io_extender.h"

#define TOF_MAX_VALID_DISTANCE 

#define TOF_SENSOR_VL53L0X 0
#define TOF_SENSOR_VL53l1X 1
#define TOF_SENSOR_VL6180X 2

class tof {
    public:
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

        tof(int sensor_type, int offset_x, int offset_y, int offset_a, int xshut = -1);
        tof(int sensor_type, int offset_x, int offset_y, int offset_a, io_ext_pins xshut);
        void init();
        void begin(uint8_t address = 0b0101001);
        void enable(bool enabled);
        void setLongRangeMode(bool mode);
        void setHighAccuracy(bool mode);
        void setHighSpeed(bool mode);
        void setDistanceMode(VL53L1X::DistanceMode distanceMode);
        void setRoiSize(int x, int y);
        void setRoiCenter(int spad_num);
        bool getLongRangeMode() { return _long_range; }
        bool getHighAccuracy() { return _high_accuracy; }
        bool getHighSpeed() { return _high_speed; }
        void setContinuous(bool mode, uint32_t period_ms = 0);
        bool timeoutOccurred();

        tof_error_types getMeasurementError() { return _error; };
        String getMeasurementErrorString();

        void holdReset();
        void releaseReset();
        void reset();

        void changeAddress(uint8_t address);

        uint16_t getMeasurement();
        
        int _offset_x = 0;
        int _offset_y = 0;
        int _offset_a = 0;
    private:
        VL53L0X* vl53l0x_sensor;
        VL53L1X* vl53l1x_sensor;
        Adafruit_VL6180X* vl6180x_sensor;

        int _sensor_type;

        bool _long_range = false;
        bool _high_accuracy = false;
        bool _high_speed = false;

        bool continuous_mode = false;

        uint16_t last_measurement = 0;

        bool _io_ext_mode = false;
        int _xshut = -1;
        io_ext_pins _xshut_io_ext_mode;

        bool _enabled = false;

        tof_error_types _error = TOF_ERROR_NOT_ENABLED;

        void _vl53l0x_begin(uint8_t address);
        void _vl53l0x_setLongRangeMode(bool mode);
        void _vl53l0x_setHighAccuracy(bool mode);
        void _vl53l0x_setHighSpeed(bool mode);
        void _vl53l0x_changeAddress(uint8_t address);
        void _vl53l0x_setContinuous(bool mode, uint32_t period_ms);
        uint16_t _vl53l0x_getMeasurement();
        bool _vl53l0x_timeoutOccured();

        void _vl53l1x_begin(uint8_t address);
        void _vl53l1x_changeAddress(uint8_t address);
        void _vl53l1x_setDistanceMode(VL53L1X::DistanceMode mode);
        void _vl53l1x_setRoiSize(int x, int y);
        void _vl53l1x_setRoiCenter(int spad_num);
        void _vl53l1x_setContinuous(bool mode, uint32_t period_ms);
        uint16_t _vl53l1x_getMeasurement();

        void _vl6180x_begin(uint8_t address);
        void _vl6180x_changeAddress(uint8_t address);
        uint16_t _vl6180x_getMeasurement();
        void _vl6180x_setContinuous(bool mode, uint32_t period_ms);


    friend class Robot;
};

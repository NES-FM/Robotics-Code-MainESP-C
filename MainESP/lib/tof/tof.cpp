#include "tof.h"

void tof::init()
{
    if (_xshut != -1) 
    {
        pinMode(_xshut, OUTPUT);
        digitalWrite(_xshut, LOW);
    }
}

void tof::begin(uint8_t address)
{
    if (_enabled)
    {
        sensor->setTimeout(500);
        if (!sensor->init())
        {
            Serial.println(F("Failed to detect and initialize VL53L0X!"));
            this->enable(false);
            _error = TOF_ERROR_FAILED_TO_INITIALIZE;
            return;
        }
        delay(10);
        changeAddress(address);
        // setHighAccuracy(false);
        // setLongRangeMode(false);
        _error = TOF_ERROR_NONE;
    }
}

void tof::enable(bool enabled) {
    _enabled = enabled; 
    if (!enabled)
        _error = TOF_ERROR_NOT_ENABLED; 
}

void tof::setLongRangeMode(bool mode)
{
    if (_enabled)
    {
        _long_range = mode;
        if (mode)
        {
            // lower the return signal rate limit (default is 0.25 MCPS)
            sensor->setSignalRateLimit(0.1);
            // increase laser pulse periods (defaults are 14 and 10 PCLKs)
            sensor->setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
            sensor->setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
        }
        else
        {
            // lower the return signal rate limit (default is 0.25 MCPS)
            sensor->setSignalRateLimit(0.25);
            // increase laser pulse periods (defaults are 14 and 10 PCLKs)
            sensor->setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 14);
            sensor->setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 10);
        }
    }
}

void tof::setHighSpeed(bool mode)
{
    if (_enabled)
    {
        if (mode)
        {
            _high_accuracy = false;
            _high_speed = true;
            sensor->setMeasurementTimingBudget(20000);
        }
        else
        {
            _high_accuracy = false;
            _high_speed = false;
            sensor->setMeasurementTimingBudget(33000);
        }
    }
}

void tof::setHighAccuracy(bool mode)
{
    if (_enabled)
    {
        if (mode)
        {
            _high_accuracy = true;
            _high_speed = false;
            sensor->setMeasurementTimingBudget(200000);
        }
        else
        {
            _high_accuracy = false;
            _high_speed = false;
            sensor->setMeasurementTimingBudget(33000);
        }
    }
}

void tof::setContinuous(bool mode, uint32_t period_ms)
{
    if (_enabled)
    {
        continuous_mode = mode;
        if (mode)
        {
            sensor->startContinuous(period_ms);
        }
        else
        {
            sensor->stopContinuous();
        }
    }
}

uint16_t tof::getMeasurement()
{
    if (_enabled)
    {
        uint16_t measurment;
        if (continuous_mode)
        {
            measurment = sensor->readRangeContinuousMillimeters();
        }
        else
        {
            measurment = sensor->readRangeSingleMillimeters();
        }
        last_measurement = measurment;

        if (sensor->timeoutOccurred())
        {
            _error = TOF_ERROR_TIMEOUT;
        }
        else if (measurment > 8000)
        {
            _error = TOF_ERROR_MAX_DISTANCE;
        }
        else
        {
            _error = TOF_ERROR_NONE;
        }

        return measurment;// + abs(_offset_x);
    }
    else
    {
        return 0;
    }
}

void tof::holdReset()
{
    if (_xshut != -1)
    {
        digitalWrite(_xshut, LOW);
    }
}

void tof::releaseReset()
{
    if (_xshut != -1)
    {
        digitalWrite(_xshut, HIGH);
    }
}

void tof::reset()
{
    if (_xshut != -1)
    {
        this->holdReset();
        delay(10);
        this->releaseReset();
    }
}

void tof::changeAddress(uint8_t address)
{
    if (_enabled)
        sensor->setAddress(address);
}

String tof::getMeasurementErrorString()
{
    switch (_error)
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
    }
    return F("Unknown error");
}


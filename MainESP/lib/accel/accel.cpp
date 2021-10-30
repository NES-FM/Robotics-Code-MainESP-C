#include "accel.h"

accel::accel() {};

void accel::init()
{
    if (_accel_enabled)
    {
        if (!mpu->begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
        {
            Serial.println("Accel initialisation failed!");
            this->enable(false);
            return;
        }
        mpu->setI2CMasterModeEnabled(false);
        mpu->setI2CBypassEnabled(true);
        mpu->setSleepEnabled(false);
    }
}

void accel::enable(bool enabled)
{
    _accel_enabled = enabled;
}

// Copy-pasted from MPU6050_accel_pitch_roll sample script
float accel::get_pitch_degrees()
{
    if (_accel_enabled)
    {
        Vector normAccel = mpu->readNormalizeAccel();
        return -(atan2(normAccel.XAxis, sqrt(normAccel.YAxis*normAccel.YAxis + normAccel.ZAxis*normAccel.ZAxis))*180.0)/PI;
    }
    return 0.0f;
}

float accel::get_roll_degrees()
{
    if (_accel_enabled)
    {
        Vector normAccel = mpu->readNormalizeAccel();
        float ret = (atan2(normAccel.YAxis, normAccel.ZAxis)*180.0)/PI;
        Serial.println(ret);
        return ret;
    }
    return 0.0f;
}

// Copy-pasted from HMC5883l_compensation_MPU6050 sample script
float accel::get_pitch_for_compensation()
{
    if (_accel_enabled)
    {
        Vector scaledAccel = mpu->readScaledAccel();
        return asin(-scaledAccel.XAxis);
    }
    return 0.0f;
}

float accel::get_roll_for_compensation()
{
    if (_accel_enabled)
    {
        Vector scaledAccel = mpu->readScaledAccel();
        return asin(scaledAccel.YAxis);
    }
    return 0.0f;
}

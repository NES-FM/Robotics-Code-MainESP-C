#include "accel.h"

accel::accel() {};

#if ACCEL_LIBRARY == 0
void accel::init()
{
    if (_accel_enabled)
    {
        if (!mpu->begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
        {
            logln("Accel initialisation failed!");
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
        // logln("[Accel]: XAxis: %f, YAxis: %f, ZAxis: %f, Ret: %f",normAccel.XAxis,normAccel.YAxis,normAccel.ZAxis,ret);
        // logln(ret);
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
        // logln("-X: %f, Y: %f\r\n", -scaledAccel.XAxis, scaledAccel.YAxis);
        return asin(-scaledAccel.XAxis);
    }
    return 0.0f;
}

float accel::get_roll_for_compensation()
{
    if (_accel_enabled)
    {
        Vector scaledAccel = mpu->readScaledAccel();
        // logln("-X: %f, Y: %f\r\n", -scaledAccel.XAxis, scaledAccel.YAxis);
        return asin(scaledAccel.YAxis);
    }
    return 0.0f;
}
#elif ACCEL_LIBRARY == 1
void accel::init()
{
    if (_accel_enabled)
    {
        if (!mpu->begin()) {
            logln("Sensor init failed");
            while (1)
                yield();
        }
    }
}

void accel::enable(bool enabled)
{
    _accel_enabled = enabled;
}

// Copy-pasted from MPU6050_accel_pitch_roll sample script
float accel::get_pitch_degrees()
{
    // if (_accel_enabled)
    // {
    //     Vector normAccel = mpu->readNormalizeAccel();
    //     return -(atan2(normAccel.XAxis, sqrt(normAccel.YAxis*normAccel.YAxis + normAccel.ZAxis*normAccel.ZAxis))*180.0)/PI;
    // }
    return 0.0f;
}

float accel::get_roll_degrees()
{
    if (_accel_enabled)
    {
        sensors_event_t a, g, temp;
        mpu->getEvent(&a, &g, &temp);

        // Serial.print("Accelerometer ");
        // Serial.print("X: ");
        // Serial.print(a.acceleration.x, 1);
        // Serial.print(" m/s^2, ");
        // Serial.print("Y: ");
        // Serial.print(a.acceleration.y, 1);
        // Serial.print(" m/s^2, ");
        // Serial.print("Z: ");
        // Serial.print(a.acceleration.z, 1);
        // Serial.print(" m/s^2");
        // Serial.print("Roll: ");
        // Serial.print(a.acceleration.roll);
        // Serial.println("°");
        // Vector normAccel = mpu->readNormalizeAccel();
        // float ret = (atan2(normAccel.YAxis, normAccel.ZAxis)*180.0)/PI;
        // Serial.printf("[Accel]: XAxis: %f, YAxis: %f, ZAxis: %f, Ret: %f\r\n",normAccel.XAxis,normAccel.YAxis,normAccel.ZAxis,ret);
        // Serial.println(ret);
        return 0.0f;//ret;
    }
    return 0.0f;
}

// Copy-pasted from HMC5883l_compensation_MPU6050 sample script
float accel::get_pitch_for_compensation()
{
    // if (_accel_enabled)
    // {
    //     Vector scaledAccel = mpu->readScaledAccel();
    //     return asin(-scaledAccel.XAxis);
    // }
    return 0.0f;
}

float accel::get_roll_for_compensation()
{
    // if (_accel_enabled)
    // {
    //     Vector scaledAccel = mpu->readScaledAccel();
    //     return asin(scaledAccel.YAxis);
    // }
    return 0.0f;
}
#elif ACCEL_LIBRARY == 2
void accel::init()
{
    if (_accel_enabled)
    {
        if (mpu->begin() != 0) {
            logln("Accel initialisation failed!");
            this->enable(false);
            return;
        }
        mpu->setAccOffsets(-0.02, 0.06, 0.09);
        mpu->setGyroOffsets(5.69, -0.36, -0.77);
        // mpu->calcOffsets();
        // this->print_values();
    }
}

void accel::enable(bool enabled)
{
    _accel_enabled = enabled;
}

float accel::get_pitch_degrees()
{
    if (_accel_enabled)
    {
        mpu->update();
        return mpu->getAngleX();
    }
    return 0.0f;
}

float accel::get_roll_degrees()
{
    if (_accel_enabled)
    {
        mpu->update();
        // print_values();
        
        return mpu->getAngleY();
    }
    return 0.0f;
}

float accel::get_pitch_for_compensation()
{
    return 0.0f;
}

float accel::get_roll_for_compensation()
{
    return 0.0f;
}

accel::ramp_types accel::getCurrentRampState()
{
    float angle = this->get_roll_degrees();
    if (angle < -10)
        return up;
    else if (angle > 10)
        return down;
    return level;
}

void accel::print_values()
{
    // mpu->update();
    logln("ACCELERO  X: %f\tY: %f\tZ: %f", mpu->getAccX(), mpu->getAccY(), mpu->getAccZ());

    logln("GYRO      X: %f\tY: %f\tZ: %f", mpu->getGyroX(), mpu->getGyroY(), mpu->getGyroZ());

    logln("ACC ANGLE X: %f\tY: %f", mpu->getAccAngleX(), mpu->getAccAngleY());

    logln("ANGLE     X: %f\tY: %f", mpu->getAngleX(), mpu->getAngleY());

    logln("RAW       X: %f\tY: %f", acc_lsb_to_g * (mpu->getAccX() + mpu->getAccXoffset()), acc_lsb_to_g * (mpu->getAccY() + mpu->getAccYoffset()));

    logln("OFFSET-A  X: %f\tY: %f\tZ: %f", mpu->getAccXoffset(), mpu->getAccYoffset(), mpu->getAccZoffset());

    logln("OFFSET-G  X: %f\tY: %f\tZ: %f", mpu->getGyroXoffset(), mpu->getGyroYoffset(), mpu->getGyroZoffset());

    logln("\r\n=====================================================\r\n");
}
#endif

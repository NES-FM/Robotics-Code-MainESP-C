#include "accel.h"

accel::accel() {};

#if ACCEL_LIBRARY == 0
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
        // Serial.printf("[Accel]: XAxis: %f, YAxis: %f, ZAxis: %f, Ret: %f\r\n",normAccel.XAxis,normAccel.YAxis,normAccel.ZAxis,ret);
        // Serial.println(ret);
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
        // Serial.printf("-X: %f, Y: %f\r\n", -scaledAccel.XAxis, scaledAccel.YAxis);
        return asin(-scaledAccel.XAxis);
    }
    return 0.0f;
}

float accel::get_roll_for_compensation()
{
    if (_accel_enabled)
    {
        Vector scaledAccel = mpu->readScaledAccel();
        // Serial.printf("-X: %f, Y: %f\r\n", -scaledAccel.XAxis, scaledAccel.YAxis);
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
            Serial.println("Sensor init failed");
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

        Serial.print("Accelerometer ");
        Serial.print("X: ");
        Serial.print(a.acceleration.x, 1);
        Serial.print(" m/s^2, ");
        Serial.print("Y: ");
        Serial.print(a.acceleration.y, 1);
        Serial.print(" m/s^2, ");
        Serial.print("Z: ");
        Serial.print(a.acceleration.z, 1);
        Serial.print(" m/s^2");
        Serial.print("Roll: ");
        Serial.print(a.acceleration.roll);
        Serial.println("°");
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
            Serial.println("Accel initialisation failed!");
            this->enable(false);
            return;
        }
        mpu->setAccOffsets(0.02,0.04,0.01);
        mpu->setGyroOffsets(0.54,-0.66,9.34);
        // mpu->calcOffsets();
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
        return mpu->getAngleY();
    }
    return 0.0f;
}

float accel::get_roll_degrees()
{
    if (_accel_enabled)
    {
        mpu->update();
        // print_values();
        return mpu->getAngleX();
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

void accel::print_values()
{
    // mpu->update();
    Serial.print(F("ACCELERO  X: "));Serial.print(mpu->getAccX());
    Serial.print("\tY: ");Serial.print(mpu->getAccY());
    Serial.print("\tZ: ");Serial.println(mpu->getAccZ());

    Serial.print(F("GYRO      X: "));Serial.print(mpu->getGyroX());
    Serial.print("\tY: ");Serial.print(mpu->getGyroY());
    Serial.print("\tZ: ");Serial.println(mpu->getGyroZ());

    Serial.print(F("ACC ANGLE X: "));Serial.print(mpu->getAccAngleX());
    Serial.print("\tY: ");Serial.println(mpu->getAccAngleY());
    
    Serial.print(F("ANGLE     X: "));Serial.print(mpu->getAngleX());
    Serial.print("\tY: ");Serial.println(mpu->getAngleY());

    Serial.print(F("RAW       X: "));Serial.print(acc_lsb_to_g * (mpu->getAccX() + mpu->getAccXoffset()));
    Serial.print("\tY: ");Serial.println(acc_lsb_to_g * (mpu->getAccY() + mpu->getAccYoffset()));

    Serial.print(F("OFFSET-A  X: "));Serial.print(mpu->getAccXoffset());
    Serial.print("\tY: ");Serial.print(mpu->getAccYoffset());
    Serial.print("\tZ: ");Serial.println(mpu->getAccZoffset());

    Serial.print(F("OFFSET-G  X: "));Serial.print(mpu->getGyroXoffset());
    Serial.print("\tY: ");Serial.print(mpu->getGyroYoffset());
    Serial.print("\tZ: ");Serial.println(mpu->getGyroZoffset());

    Serial.println(F("\n=====================================================\n"));
}
#endif

#include "compass.h"

compass_hmc::compass_hmc() {};

void compass_hmc::init(accel* ac_pointer) 
{
    ac = ac_pointer;
    if (_compass_enabled)
    {
        if (!hmc->begin())
        {
            Serial.println("Compass initialisation failed!");
            this->enable(false);
            return;
        }
        hmc->setRange(HMC5883L_RANGE_1_3GA);
        hmc->setMeasurementMode(HMC5883L_CONTINOUS);
        hmc->setDataRate(HMC5883L_DATARATE_30HZ);
        hmc->setSamples(HMC5883L_SAMPLES_8);

        hmc->setOffset(-186, -225); // Calibrated for home, maybe needing recalibration on every location, though im not to sure
    }
}

void compass_hmc::enable(bool enabled)
{
    _compass_enabled = enabled;
}

float compass_hmc::get_angle()
{
    if (_compass_enabled)
    {
        float heading = this->tiltCompensation();

        // Set declination angle on your location and fix heading
        // You can find your declination on: http://magnetic-declination.com/
        // (+) Positive or (-) for negative
        // For Bytom / Poland declination angle is 4'26E (positive)
        // Formula: (deg + (min / 60.0)) / (180 / M_PI);
        float declinationAngle = (3 + (19 / 60.0)) / (180 / PI);
        heading += declinationAngle;

        heading = correctAngle(heading) * 180/PI;

        return heading; // correcting for <0 and >360 and converting to degrees
    }
    return 0.0f;
}

float compass_hmc::noTiltCompensation()
{
    Vector mag = hmc->readNormalize();
    return atan2(mag.YAxis, mag.XAxis);
}
float compass_hmc::tiltCompensation()
{
    Vector mag = hmc->readNormalize();
    float roll;
    float pitch;
    roll = ac->get_roll_for_compensation();
    pitch = ac->get_pitch_for_compensation();

    if (roll > 0.78 || roll < -0.78 || pitch > 0.78 || pitch < -0.78)
    {
        return this->noTiltCompensation();
    }
    
    // Some of these are used twice, so rather than computing them twice in the algorithm we precompute them beforehand.
    float cosRoll = cos(roll);
    float sinRoll = sin(roll);  
    float cosPitch = cos(pitch);
    float sinPitch = sin(pitch);
    
    // Tilt compensation
    float Xh = mag.XAxis * cosPitch + mag.ZAxis * sinPitch;
    float Yh = mag.XAxis * sinRoll * sinPitch + mag.YAxis * cosRoll - mag.ZAxis * sinRoll * cosPitch;
        
    return atan2(Yh, Xh);;
}
float compass_hmc::correctAngle(float heading)
{
    if (heading < 0) { heading += 2 * PI; }
    if (heading > 2 * PI) { heading -= 2 * PI; }

    return heading;
}

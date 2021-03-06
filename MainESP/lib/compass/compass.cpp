#include "compass.h"

compass_hmc::compass_hmc() {};

#if COMPASS_LIBRARY == 0
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

    if (roll > 0.78 || roll < -0.78 || pitch > 0.78 || pitch < -0.78 || (roll == 0 && pitch == 0))
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
#elif COMPASS_LIBRARY == 1
void compass_hmc::init(accel* ac_pointer) 
{
    this->init();
}
void compass_hmc::init()
{
    if (_compass_enabled)
    {
        compass_prefs->begin("compass", false);
        compass->initCompass();
        // error = compass->setScale(1.3); // Set the scale of the compass.
        // if(error != 0) // If there is an error, print it out.
        //     logln("%s", compass->getErrorText(error));

        // error = compass->setMeasurementMode(MEASUREMENT_CONTINUOUS); // Set the measurement mode to Continuous
        // if(error != 0) // If there is an error, print it out.
        //     logln("%s", compass->getErrorText(error));

        valueOffset->XAxis = compass_prefs->getFloat("offset.x", 0);
        valueOffset->YAxis = compass_prefs->getFloat("offset.y", 0);
        valueOffset->ZAxis = compass_prefs->getFloat("offset.z", 0);

        logln("Compass Initialized with: Offset[x,y,z]: %f, %f, %f", valueOffset->XAxis, valueOffset->YAxis, valueOffset->ZAxis);

        // this->calibrate();
    }
}

void compass_hmc::enable(bool enabled)
{
    _compass_enabled = enabled;
}

float compass_hmc::get_angle()
{
    float headingDegrees = 0.0f;
    if (_compass_enabled)
    {
        MagnetometerScaled scaled = compass->readScaledAxis();

        scaled.XAxis -= valueOffset->XAxis;
        scaled.YAxis -= valueOffset->YAxis;

        // Calculate heading when the magnetometer is level, then correct for signs of axis.
        float heading = atan2(scaled.YAxis, scaled.XAxis);
        
        // float heading = yxHeading;
        
        // Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
        // Find yours here: http://www.magnetic-declination.com/
        // Mine is: -2?? 37' which is -2.617 Degrees, or (which we need) -0.0456752665 radians, I will use -0.0457
        // If you cannot find your Declination, comment out these two lines, your compass will be slightly off.

        // In Weingarten: 3?? 20'  ->  3.33 Degrees  ->  0.05811946 Radians
        float declinationAngle = 0.05811946;
        heading += declinationAngle;
        
        // Correct for when signs are reversed.
        if(heading < 0)
            heading += 2*PI;
            
        // Check for wrap due to addition of declination.
        if(heading > 2*PI)
            heading -= 2*PI;
        
        // Convert radians to degrees for readability.
        headingDegrees = heading * 180/PI;

        // this->output(compass->readRawAxis(), scaled, heading, headingDegrees);
    }

    return headingDegrees; 
}

void compass_hmc::calibrate()
{
    logln("calibrate the compass");
    if (_compass_enabled)
    {
        MagnetometerScaled valueMax = {0, 0, 0};
        MagnetometerScaled valueMin = {0, 0, 0};

        MagnetometerScaled cur = {0,0,0};

        for (int i = 0; i < (10000); i+=100)
        {
            cur = compass->readScaledAxis();
            valueMax.XAxis = max(valueMax.XAxis, cur.XAxis);
            valueMax.YAxis = max(valueMax.YAxis, cur.YAxis);
            valueMax.ZAxis = max(valueMax.ZAxis, cur.ZAxis);

            valueMin.XAxis = min(valueMin.XAxis, cur.XAxis);
            valueMin.YAxis = min(valueMin.YAxis, cur.YAxis);
            valueMin.ZAxis = min(valueMin.ZAxis, cur.ZAxis);
            delay(100);
        }

        valueOffset->XAxis = (valueMax.XAxis + valueMin.XAxis) / 2;
        valueOffset->YAxis = (valueMax.YAxis + valueMin.YAxis) / 2;
        valueOffset->ZAxis = (valueMax.ZAxis + valueMin.ZAxis) / 2;

        logln("Compass: \r\nMax[x,y,z]: %f, %f, %f\r\nMin[x,y,z]: %f, %f, %f\r\nOffset[x,y,z]: %f, %f, %f", valueMax.XAxis, valueMax.YAxis, valueMax.ZAxis, valueMin.XAxis, valueMin.YAxis, valueMin.ZAxis, valueOffset->XAxis, valueOffset->YAxis, valueOffset->ZAxis);

        compass_prefs->putFloat("offset.x", valueOffset->XAxis);
        compass_prefs->putFloat("offset.y", valueOffset->YAxis);
        compass_prefs->putFloat("offset.z", valueOffset->ZAxis);

    /*
        Serial.print("max: ");
        Serial.print(valueMax.XAxis);
        Serial.print(valueMax.YAxis);
        Serial.println(valueMax.ZAxis);
        Serial.print("min: ");
        Serial.print(valueMin.XAxis);
        Serial.print(valueMin.YAxis);
        Serial.println(valueMin.ZAxis);
        Serial.print("offset: ");
        Serial.print(valueOffset->XAxis);
        Serial.print(valueOffset->YAxis);
        Serial.println(valueOffset->ZAxis);
        */
    }
    else
    {
        logln("nvm, compass not enabled");
    }
}

void compass_hmc::output(MagnetometerRaw raw, MagnetometerScaled scaled, float heading, float headingDegrees)
{
    logln("Raw: %h   %h   %h   \tScaled: %f   %f   %f   \tHeading: %f Radians   \t%f Degrees", raw.XAxis, raw.YAxis, raw.ZAxis, scaled.XAxis, scaled.YAxis, scaled.ZAxis, heading, headingDegrees);
}

void compass_hmc::tick() 
{

}

float compass_hmc::keep_in_360_range(float x)
{
    if (x > 360)
    {
        return x - 360;
    }
    if (x < 0)
    {
        return 360 + x;
    }
    return x;
}

#endif

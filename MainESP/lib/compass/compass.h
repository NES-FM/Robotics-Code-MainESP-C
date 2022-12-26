#ifndef COMPASS_H
#define COMPASS_H

#include "DFRobot_BMM150.h"
#include "../../include/i2c_addresses.h"
#include "logger.h"

class compass_bmm {
    public:
        compass_bmm();
        float get_angle();
        void init();
        static float keep_in_360_range(float alpha);
        void enable(bool enabled);
    private:
        bool _compass_enabled = false;
        DFRobot_BMM150_I2C* bmm150 = new DFRobot_BMM150_I2C(&Wire, I2C_ADDRESS_COMPASS);
};

// #define COMPASS_LIBRARY 1

// #if COMPASS_LIBRARY == 0
// #include <HMC5883L.h>
// #elif COMPASS_LIBRARY == 1
// #include <../HMC5883L_SeedStudio/HMC5883L_SeedStudio.h>
// #endif
// #include "accel.h"

// #include <Preferences.h>

// class compass_hmc {
//     public:
//         compass_hmc();
//         void init(accel* ac_pointer);
//         void init();
//         void tick();
//         void enable(bool enabled);
//         bool is_enabled() { return _compass_enabled; }
//         float get_angle();
//         #if COMPASS_LIBRARY == 1
//         void calibrate();
//         void calibrate_background_task();
//         void start_calibrate_background_task();
//         void stop_calibrate_background_task();        
//         #endif

//         static float keep_in_360_range(float alpha);
//     private:
//         bool _compass_enabled = false;

//         #if COMPASS_LIBRARY == 0
//         float noTiltCompensation();
//         float tiltCompensation();
//         float correctAngle(float heading);
//         HMC5883L* hmc = new HMC5883L();
//         accel* ac;
//         #elif COMPASS_LIBRARY == 1
//         HMC5883L *compass = new HMC5883L();
//         int error = 0;
//         MagnetometerScaled *valueOffset = new MagnetometerScaled();
//         void output(MagnetometerRaw raw, MagnetometerScaled scaled, float heading, float headingDegrees);

//         Preferences* compass_prefs = new Preferences();

//         MagnetometerScaled calibrate_background_task_valueMax = {0, 0, 0};
//         MagnetometerScaled calibrate_background_task_valueMin = {0, 0, 0};

//         #endif
// };

#endif /* COMPASS_H */

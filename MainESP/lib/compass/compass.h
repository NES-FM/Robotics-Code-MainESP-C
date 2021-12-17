#ifndef COMPASS_H
#define COMPASS_H

#define COMPASS_LIBRARY 1

#if COMPASS_LIBRARY == 0
#include <HMC5883L.h>
#elif COMPASS_LIBRARY == 1
#include <../HMC5883L_SeedStudio/HMC5883L_SeedStudio.h>
#endif
#include "accel.h"

class compass_hmc {
    public:
        compass_hmc();
        void init(accel* ac_pointer);
        void init();
        void enable(bool enabled);
        bool is_enabled() { return _compass_enabled; }
        float get_angle();
        #if COMPASS_LIBRARY == 1
        void calibrate();
        #endif
    private:
        bool _compass_enabled = false;

        #if COMPASS_LIBRARY == 0
        float noTiltCompensation();
        float tiltCompensation();
        float correctAngle(float heading);
        HMC5883L* hmc = new HMC5883L();
        accel* ac;
        #elif COMPASS_LIBRARY == 1
        HMC5883L *compass = new HMC5883L();
        int error = 0;
        MagnetometerScaled *valueOffset = new MagnetometerScaled();
        void output(MagnetometerRaw raw, MagnetometerScaled scaled, float heading, float headingDegrees);
        #endif
};

#endif /* COMPASS_H */

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
        void tick();
        void enable(bool enabled);
        bool is_enabled() { return _compass_enabled; }
        float get_angle();
        #if COMPASS_LIBRARY == 1
        void calibrate();
        #endif
        
        float getContinuousAngle() { return continuous_angle; }
        void setRelativeZero() { relativeZero = get_angle(); num_total_rotations = 0; }
        void setRelativeGoal(float angle) { relativeGoal = angle; }
        void setRelativeZeroAndGoal(float angle) { setRelativeZero(); setRelativeGoal(angle); }
        float getRelativeAngle() { return getContinuousAngle() - relativeZero; }
        bool reachedRelativeGoal();
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

        int calculate_section();

        float continuous_angle = 0.0f;
        int num_total_rotations = 0;
        float relativeGoal = 0.0f;
        float relativeZero = 0.0f;
        int cur_section = 0; // 0: 0-90  1: 90-180  2: 180-270  3: 270-360

        #endif
};

#endif /* COMPASS_H */

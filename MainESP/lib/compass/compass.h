#ifndef COMPASS_H
#define COMPASS_H

#include <HMC5883L.h>
#include "accel.h"

class compass_hmc {
    public:
        compass_hmc();
        void init(accel* ac_pointer);
        void enable(bool enabled);
        bool is_enabled() { return _compass_enabled; }
        float get_angle();
    private:
        bool _compass_enabled = false;
        accel* ac;
        float noTiltCompensation();
        float tiltCompensation();
        float correctAngle(float heading);

        HMC5883L* hmc = new HMC5883L();
};

#endif /* COMPASS_H */

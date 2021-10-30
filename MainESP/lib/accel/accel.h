#ifndef ACCEL_H
#define ACCEL_H

#include <MPU6050.h>

class accel {
    public:
        accel();
        void init();
        void enable(bool enabled);
        bool is_enabled() { return _accel_enabled; }
        float get_pitch_degrees();
        float get_roll_degrees();
        float get_pitch_for_compensation();
        float get_roll_for_compensation();
    private:
        // Adafruit_SSD1306* oled = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
        MPU6050* mpu = new MPU6050();
        bool _accel_enabled = false;
};

#endif /* ACCEL_H */

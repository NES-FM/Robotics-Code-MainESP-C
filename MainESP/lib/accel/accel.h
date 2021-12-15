#ifndef ACCEL_H
#define ACCEL_H

#define ACCEL_LIBRARY 2

#if ACCEL_LIBRARY == 0
#include <MPU6050.h>
#elif ACCEL_LIBRARY == 1
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#elif ACCEL_LIBRARY == 2
#include <MPU6050_light.h>
#endif

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
        #if ACCEL_LIBRARY == 2
        void print_values();
        #endif
    private:
        // Adafruit_SSD1306* oled = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
        #if ACCEL_LIBRARY == 0
        MPU6050* mpu = new MPU6050();
        #elif ACCEL_LIBRARY == 1
        Adafruit_MPU6050* mpu = new Adafruit_MPU6050();
        #elif ACCEL_LIBRARY == 2
        MPU6050* mpu = new MPU6050(Wire);
        float acc_lsb_to_g = 16384.0f; // Needs to be changed if mpu->setAccConfig() is called [https://github.com/rfetick/MPU6050_light/blob/master/src/MPU6050_light.cpp#L89]
        float range_per_digit = .000061f; // "  [https://github.com/jarzebski/Arduino-MPU6050/blob/master/MPU6050.cpp#L107]
        #endif
        bool _accel_enabled = false;
};

#endif /* ACCEL_H */

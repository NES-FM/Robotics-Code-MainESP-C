#ifndef DEBUG_DISP_H
#define DEBUG_DISP_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "../../include/i2c_addresses.h"
#include "../../include/cuart_line_types.h"
#include "compass.h"
#include "accel.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define DISPLAY_REFRESH_TIME 100 // Display refresh time in milliseconds

static const unsigned char star_height = 16, star_width = 16;
static const unsigned char PROGMEM star_bmp[] =
            { B00000000,B11000000,
            B00000001,B11000000,
            B00000001,B11000000,
            B00000011,B11100000,
            B11110011,B11100000,
            B11111110,B11111000,
            B01111110,B11111111,
            B00110011,B10011111,
            B00011111,B11111100,
            B00001101,B01110000,
            B00011011,B10100000,
            B00111111,B11100000,
            B00111111,B11110000,
            B01111100,B11110000,
            B01110000,B01110000,
            B00000000,B00110000 };

class debug_disp {
    public:
        debug_disp();
        void init(bool* sensor_array, bool* green_dots, unsigned char* type, signed char* angle, signed char* midfactor, int* l_sens, int* m_sens, int* r_sens, int* lm_val, int* rm_val, bool* int_sit, bool* int_bi_left, bool* int_bi_right, bool* int_bi_both, compass_hmc* comp, accel* acc) ;
        void tick();
        void enable(bool enabled);
        bool is_enabled() { return _display_i2c_enabled; }
        void draw_star();
    private:
        uint8_t _i2c_address = I2C_ADDRESS_DISPLAY;
        bool _display_i2c_enabled = false;
        Adafruit_SSD1306* oled = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

        int _tick_last_millis = 0;
        bool* _local_cuart_sensor_array;

        bool* _local_cuart_green_dots;

        unsigned char* _local_cuart_line_type;
        signed char* _local_cuart_line_angle;
        signed char* _local_cuart_line_midfactor;

        int* _l_motor_value, * _r_motor_value;
        int* _l_sensor, * _m_sensor, * _r_sensor;
        
        float _battery_voltage = 3.7;

        void draw_sensor_array(int x, int y, int element_width, int element_height);
        void draw_green_dots(int x, int y, int width, int height);
        void draw_ltype(int x, int y);
        void draw_motor_values(int x, int y);
        void draw_comp_accel(int x, int y);


        bool heartbeat_state = false;

        bool* _interesting_situation, *_int_bias_left, *_int_bias_right, *_int_bias_both;

        compass_hmc* _compass;
        accel* _accelerometer;
};

#endif /* DEBUG_DISP_H */

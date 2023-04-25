#ifndef DEBUG_DISP_H
#define DEBUG_DISP_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "../../include/i2c_addresses.h"
#include "../../include/cuart_line_types.h"
#include "../../include/room_stuff.h"
#include "analog_sensor.h"
#include "dip.h"
#include "cuart.h"
#include "moving_in_room.h"
#include "robot.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define SCREEN_MID_X SCREEN_WIDTH/2
#define SCREEN_MID_Y SCREEN_HEIGHT/2

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
        // bool* sensor_array, bool* green_dots, unsigned char* type, signed char* angle, signed char* midfactor, int* l_sens, int* m_sens, int* r_sens, int* lm_val, int* rm_val, bool* int_sit, bool* int_bi_left, bool* int_bi_right, bool* int_bi_both, compass_hmc* comp, accel* acc, analog_sensor* volt, DIP* d, taster_class* t, IR* irl, IR* irr, int* ecke, int* hole, digital_sensor* kugel_in_greifer, float* lir_value
        void init(CUART_class* c, Robot* r, bool* int_sit, bool* int_bi_left, bool* int_bi_right, bool* int_bi_both);
        void tick();
        void force_tick() { _tick_last_millis = 0; tick(); }
        void enable(bool enabled);
        bool is_enabled() { return _display_i2c_enabled; }
        void draw_star();
        void disable_i2c_device(String dev);

    private:
        uint8_t _i2c_address = I2C_ADDRESS_DISPLAY;
        bool _display_i2c_enabled = false;
        Adafruit_SSD1306* oled = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

        int _tick_last_millis = 0;
        
        CUART_class* _cuart;
        Robot* _robot;

        struct screen_point
        {
            float x;
            float y;
        };

        void draw_sensor_array(int x, int y, int element_width, int element_height);
        void draw_green_dots(int x, int y, int width, int height);
        void draw_ltype(int x, int y);
        void draw_motor_values(int x, int y);
        void draw_accel(int x, int y);
        // void draw_comp_accel(int x, int y);
        // void draw_compass(int x, int y);
        void draw_disabled_i2c_devices(int x, int y);
        void draw_voltage(int x, int y);
        void draw_voltage_smol(int x, int y);
        void draw_dip(int x, int y);
        void draw_cuart(int x, int y);
        void draw_taster(int x, int y, int w, int h);

        void draw_room_tof(int x, int y);
        // void draw_robot_in_room_coordinates();
        // void draw_balls_in_room_coordinates();
        // void draw_corners_in_room_coordinates();
        // void draw_move_in_room_steps();
        // void draw_room_space_line(Robot::point point_1, Robot::point point_2);

        float room_conversion_factor;

        bool heartbeat_state = false;

        bool* _interesting_situation, *_int_bias_left, *_int_bias_right, *_int_bias_both;

        String i2c_disabled_devices = "";
};

#endif /* DEBUG_DISP_H */

#ifndef CUART_H
#define CUART_H

#include "HardwareSerial.h"
#include "../../include/pin_definitions.h"
#include "logger.h"

// Calculating the Sensors used for driving
// (0 1 2) 3 4 5 6 7 8 9 | 10 11 12 13 14 15 | 16 17 18 19 20 21 22 (23)
// (0 0 0) 0 0 0 0 0 0 0 |  0 1  1  1  1  0  | 0  0  0  0  0  0  0  (0)

class CUART_class
{
    public:
        CUART_class();
        void init();
        void tick();
        void debugPrintLine();
        void debugPrintArray();
        void debugPrint();

        bool green_dots[4] = {false, false, false, false};
        unsigned char line_type = 0;
        signed char line_angle = 0;
        signed char line_midfactor = 0;
        bool sensor_array[24] = {};

        bool red_line = false;
        bool green_line = false;
        bool silver_line = false;

        int array_left_sensor = 0, array_mid_sensor = 0, array_right_sensor = 0, array_total = 0;
    private:
        signed char unsigned_to_signed(unsigned char s);
        void line_handler();
        void green_handler();
        void sensor_array_handler();

        void red_line_handler();
        void green_line_handler();
        void silver_line_handler();

        HardwareSerial* _cuart_hwserial = new HardwareSerial(1);
        char _received_bytes[16] = {0x01};
        int _received_bytes_next_index = 0;
        char _currently_receiving = ' ';
};

#endif

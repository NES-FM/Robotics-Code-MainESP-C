#ifndef CUART_H
#define CUART_H

#include "HardwareSerial.h"
#include "../../include/pin_definitions.h"


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

        int array_left_sensor = 0, array_mid_sensor = 0, array_right_sensor = 0;
    private:
        signed char unsigned_to_signed(unsigned char s);
        void line_handler();
        void green_handler();
        void sensor_array_handler();

        HardwareSerial* _cuart_hwserial = new HardwareSerial(1);
        char _received_bytes[16] = {0x01};
        int _received_bytes_next_index = 0;
        char _currently_receiving = ' ';
};

#endif

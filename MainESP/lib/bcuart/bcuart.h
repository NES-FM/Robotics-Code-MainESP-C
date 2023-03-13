#ifndef BCUART_H
#define BCUART_H

#include "HardwareSerial.h"
#include "../../include/pin_definitions.h"
#include "logger.h"

#define BCUART_END_CHAR 0xff
#define BCUART_HEARTBEAT_ID 0x01
#define BCUART_BALLS_ID 0x10
#define BCUART_CORNER_ID 0x11
#define BCUART_EXIT_LINE_ID 0x12

class BCUART_class
{
    public:
        BCUART_class();
        void init();
        void tick();

        #pragma pack(1)
        struct ball {
            float x_offset;
            float distance;
            float conf;
            uint8_t black;
        };
        struct corner {
            float x_offset;
            float distance;
            float conf;
            uint8_t screen_w;
        };
        struct exit_line {
            float x_offset;
            float distance;
            float conf;
        };
        
        #pragma pack()
        ball received_balls[10];
        corner received_corner;
        exit_line received_exit_line;

        uint8_t num_balls_in_array = 0;
        bool corner_valid = false;
        bool exit_line_valid = false;

        void reset_balls();
        void reset_corner();
        void reset_exit_line();
    private:
        HardwareSerial* _bcuart_hwserial = new HardwareSerial(2);
        char _received_bytes[100];
        int _bytes_read = 0;

        void balls_handler();
        void corner_handler();
        void exit_line_handler();
};

#endif

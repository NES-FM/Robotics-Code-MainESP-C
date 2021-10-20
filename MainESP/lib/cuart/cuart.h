#ifndef CUART_H
#define CUART_H

#include "HardwareSerial.h"
#include "../../include/pin_definitions.h"

HardwareSerial _cuart_hwserial(1);
char _received_bytes[16] = {0x01};
int _received_bytes_next_index = 0;
char _currently_receiving = ' ';

bool CUART_green_dots[4] = {false};
unsigned char CUART_line_type = 0;
signed char CUART_line_angle = 0;
signed char CUART_line_midfactor = 0;
bool CUART_sensor_array[24] = {false};


void CUART_init()
{
    _cuart_hwserial.begin(115200, SERIAL_8N1, PIN_RX_CAM, PIN_TX_CAM);
}

signed char CUART_unsigned_to_signed(unsigned char s)
{
    if (s >= 128)
        return s - 256;
    return s;
}

void CUART_line_handler()
{
    CUART_line_type = _received_bytes[0];
    CUART_line_angle = CUART_unsigned_to_signed(_received_bytes[1]);
    CUART_line_midfactor = CUART_unsigned_to_signed(_received_bytes[2]);
}

void CUART_green_handler()
{
    Serial.println("Cuart Green --- To Be Done!");
}

void CUART_sensor_array_handler()
{
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            CUART_sensor_array[(y * 8) + x] = (_received_bytes[y] << x) & 0b10000000;
        }
    }
}

void CUART_tick()
{
    while (_cuart_hwserial.available() > 0)
    {
        char rec_char = _cuart_hwserial.read();
        // Serial.print(int(rec_char));
        // Serial.print(" ");

        if (rec_char == 0xff)
        {
            if (_received_bytes[_received_bytes_next_index-1] == 0x00)
            {
                switch(_currently_receiving)
                {
                    case 'L':
                        CUART_line_handler();
                    break;
                    case 'G':
                        CUART_green_handler();
                    break;
                    case 'S':
                        CUART_sensor_array_handler();
                    break;
                }
                _currently_receiving = ' ';
                _received_bytes_next_index = 0;
                for (int i = 0; i < 16; i++) {
                    _received_bytes[i] = 0x01;
                }

                // Serial.println("");
                return;
            }
        }

        if (_currently_receiving == ' ')
        {
            if (rec_char == 'L' || rec_char == 'G' || rec_char == 'S')
                _currently_receiving = rec_char;
        }
        else
        {
            _received_bytes[_received_bytes_next_index] = rec_char;
            _received_bytes_next_index++;
        }   	
    }
}

#endif /* CUART_H */

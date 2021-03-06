#include "cuart.h"

CUART_class::CUART_class() {}

void CUART_class::init()
{
    _cuart_hwserial->begin(115200, SERIAL_8N1, PIN_RX_CAM, PIN_TX_CAM);
}

void CUART_class::tick()
{
    while (_cuart_hwserial->available() > 0)
    {
        char rec_char = _cuart_hwserial->read();
        // Serial.print(int(rec_char));
        // Serial.print(" ");

        if (rec_char == 0xff)
        {
            if (_received_bytes[_received_bytes_next_index-1] == 0x00)
            {
                switch(_currently_receiving)
                {
                    case 'L':
                        line_handler();
                    break;
                    case 'G':
                        green_handler();
                    break;
                    case 'S':
                        sensor_array_handler();
                    break;
                    case 'R':
                        red_line_handler();
                    break;
                    case 'E':
                        green_line_handler();
                    break;
                    case 'K':
                        silver_line_handler();
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
            if (rec_char == 'L' || rec_char == 'G' || rec_char == 'S' || rec_char == 'R' || rec_char == 'E' || rec_char == 'K')
                _currently_receiving = rec_char;
        }
        else
        {
            _received_bytes[_received_bytes_next_index] = rec_char;
            _received_bytes_next_index++;
        }   	
    }
}

void CUART_class::line_handler()
{
    if (_received_bytes_next_index >= 3)
    {
        line_type = _received_bytes[0];
        line_angle = unsigned_to_signed(_received_bytes[1]);
        line_midfactor = unsigned_to_signed(_received_bytes[2]);
    }
    else
    {
        logln("CUART_line_handler: Too few bytes received (%d/%d) ~ %d %d %d\r\n", _received_bytes_next_index, 3, sensor_array[0], sensor_array[1], sensor_array[2]);
    }
}

void CUART_class::green_handler()
{
    green_dots[0] = (_received_bytes[0] >> 3) & 0b00000001;
    green_dots[1] = (_received_bytes[0] >> 2) & 0b00000001;
    green_dots[2] = (_received_bytes[0] >> 1) & 0b00000001;
    green_dots[3] = (_received_bytes[0] >> 0) & 0b00000001;
    green_line = (green_dots[0] || green_dots[1] || green_dots[2] || green_dots[3]) && !silver_line;
}

void CUART_class::sensor_array_handler()
{
    if (_received_bytes_next_index >= 3)
    {
        for (int y = 0; y < 3; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                // Saving one byte after the other into the array, while flipping 1s to 0s
                if (((_received_bytes[y] << x) & 0b10000000) == 0)
                    sensor_array[(y * 8) + x] = true;
                else
                    sensor_array[(y * 8) + x] = false;
            }
        }
    }
    else
    {
        logln("CUART_sensor_array_handler: Too few bytes received (%d/%d) ~ %d %d %d\r\n", _received_bytes_next_index, 3, sensor_array[0], sensor_array[1], sensor_array[2]);
    }

    // Calculating the Sensors used for driving
    // (0 1 2) 3 4 5 6 7 8 9 | 10 11 12 13 14 15 | 16 17 18 19 20 21 22 (23)
    // (0 0 0) 0 0 0 0 0 0 0 |  0 1  1  1  1  0  | 0 0 0 0 0 0 0 (0)
    array_left_sensor = 0;
    array_mid_sensor = 0;
    array_right_sensor = 0;
    for (int i = 0; i < 24; i++)
    {
        if (3 <= i && i <= 9)
            array_left_sensor += sensor_array[i];
        else if (10 <= i && i <= 15)
        	array_mid_sensor += sensor_array[i];
        else if (16 <= i && i <= 22)
            array_right_sensor += sensor_array[i];
        
    }
    array_total = array_left_sensor + array_right_sensor + array_mid_sensor;
}

void CUART_class::red_line_handler()
{
    red_line = true;
}

void CUART_class::green_line_handler()
{
    //green_line = true;
}

void CUART_class::silver_line_handler()
{
    silver_line = true;
}


void CUART_class::debugPrintLine()
{
    logln("~");
    logln("%u %d %d", line_type, line_angle, line_midfactor);
}

void CUART_class::debugPrintArray()
{
    log_inline_begin();
    for (int i = 0; i < 24; i++)
    {
        if (sensor_array[i])
            log_inline("1 ");
        else
            log_inline("0 ");
    }
    log_inline_end();
}

void CUART_class::debugPrint()
{
    debugPrintLine();
    debugPrintArray();
}

signed char CUART_class::unsigned_to_signed(unsigned char s)
{
    if (s >= 128)
        return s - 256;
    return s;
}

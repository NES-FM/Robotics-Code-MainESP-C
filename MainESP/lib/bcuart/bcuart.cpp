#include "bcuart.h"

BCUART_class::BCUART_class() {}

void BCUART_class::init()
{
    _bcuart_hwserial->begin(115200, SERIAL_8N1, PIN_RX_ROOM_CAM, PIN_TX_ROOM_CAM);

    reset_balls();
    reset_corner();
    reset_exit_line();
}

void BCUART_class::tick()
{
    if (_bcuart_hwserial->available() > 0)
    {
        _bytes_read = _bcuart_hwserial->readBytesUntil(BCUART_END_CHAR, _received_bytes, 100);
        if (_bytes_read > 0)
        {
            switch (_received_bytes[0])
            {
                case BCUART_BALLS_ID:
                    balls_handler();
                    break;
                case BCUART_CORNER_ID:
                    corner_handler();
                    break;
                case BCUART_EXIT_LINE_ID:
                    exit_line_handler();
                    break;
            };
        }
    }
}

void BCUART_class::balls_handler() // char rec_bytes[40] = {0x10, 0x2, 0x0, 0x0, 0xc, 0xc, 0x5d, 0x1, 0x2d, 0x17, 0xc, 0xc, 0x4e, 0x0}
{
    uint8_t num_balls = _received_bytes[1];
    if (_bytes_read == ( ( num_balls*sizeof(ball) ) + 2 ) ) // Check if number of received bytes is right
    {
        for (int i = 0; i < num_balls; i++)
        {
            memcpy(&received_balls[i], &_received_bytes[ ( i*sizeof(ball) ) + 2 ], sizeof(ball)); // Copy every received ball into the array
        }
        num_balls_in_array = num_balls;
    }
    else
    {
        logln("Amount of received bytes is wrong!! Received %d, expected %d!", _bytes_read, ( ( num_balls*sizeof(ball) ) + 2 ));
    }
}

void BCUART_class::corner_handler()
{
    if (_bytes_read == ( sizeof(corner) + 1 ) )
    {
        memcpy(&received_corner, &_received_bytes[1], sizeof(corner)); 
        corner_valid = true;
    }
    else
    {
        logln("Amount of received bytes is wrong!! Received %d, expected %d!", _bytes_read, ( sizeof(corner) + 2 ));
    }
}

void BCUART_class::exit_line_handler()
{
    if (_bytes_read == ( sizeof(exit_line) + 1 ) )
    {
        memcpy(&received_exit_line, &_received_bytes[1], sizeof(exit_line)); 
        exit_line_valid = true;
    }
    else
    {
        logln("Amount of received bytes is wrong!! Received %d, expected %d!", _bytes_read, ( sizeof(exit_line) + 2 ));
    }
}

void BCUART_class::reset_balls()
{
    for (ball b : received_balls)
    {
        b.x_offset = 0;
        b.distance = 0;
        b.conf = 0;
        b.black = false;
    }
    num_balls_in_array = 0;
}
void BCUART_class::reset_corner()
{
    received_corner.x_offset = 0;
    received_corner.distance = 0;
    received_corner.conf = 0;
    corner_valid = false;
}
void BCUART_class::reset_exit_line()
{
    received_exit_line.x_offset = 0;
    received_exit_line.distance = 0;
    received_exit_line.conf = 0;
    exit_line_valid = false;
}

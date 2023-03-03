#include "logger.h"

// char _bt_buf[LOGGER_BUFFLEN];
// uint _bt_buf_idx = 0;
char _logger_ser_buf[LOGGER_BUFFLEN];
uint _logger_ser_buf_idx = 0;

void logger_init()
{
    Serial.println("\r\n");
    logln("Starting up MainESP...");
}

String logger_tick()
{
    if (Serial.available() > 0)
    {
        while (Serial.available() > 0)
        {
            char inByte = Serial.read();
            //Message coming in (check not terminating character) and guard for over message size
            if ( inByte != '\n' && (_logger_ser_buf_idx < LOGGER_BUFFLEN - 1) )
            {
                //Add the incoming byte to our message
                _logger_ser_buf[_logger_ser_buf_idx] = inByte;
                _logger_ser_buf_idx++;

                Serial.print(inByte); // INTENDED
            }
            //Full message received...
            else
            {
                _logger_ser_buf[_logger_ser_buf_idx] = '\0';

                //Reset for the next message
                _logger_ser_buf_idx = 0;
                return (_logger_ser_buf);
            }
        }
    }
    return "";
}

void logger_log_formatted_string(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    char str[256];
    vsprintf(str, format, args);

    Serial.print(str); // INTENDED
    va_end(args);
}

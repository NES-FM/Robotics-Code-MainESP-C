#include "logger.h"

BluetoothSerial logger_btser;

bool _logger_bluetooth_connected = false;

// char _bt_buf[LOGGER_BUFFLEN];
// uint _bt_buf_idx = 0;
char _logger_ser_buf[LOGGER_BUFFLEN];
uint _logger_ser_buf_idx = 0;

void logger_init(bool bluetoothEnabled)
{
    _logger_bluetooth_connected = false;
    Serial.println("\r\n");
    logln("Starting up MainESP...");

    if (bluetoothEnabled)
    {
        logln("Starting up bluetooth...");
        logger_btser.begin("Urgestein ESP32");
        logger_btser.register_callback(logger_bluetooth_callback);
    }
    else
    {
        logln("Bluetooth disabled!");
    }
}

String logger_tick()
{
    if (_logger_bluetooth_connected && logger_btser.available() > 0)
    {
        return (logger_btser.readStringUntil('\n'));
    }
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

    if (_logger_bluetooth_connected)
    {
        logger_btser.print(str);
    }

    va_end(args);
}

void logger_bluetooth_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    if (event == ESP_SPP_SRV_OPEN_EVT)
    {
        _logger_bluetooth_connected = true;
        logln("Bluetooth connected!");
    }
    else if (event == ESP_SPP_CLOSE_EVT)
    {
        _logger_bluetooth_connected = false;
        logln("Bluetooth disconnected!");
    }
}


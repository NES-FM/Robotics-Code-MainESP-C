#include "io_extender.h"

struct digital_read_command {
    uint8_t pin;              // + 1 byte
                                // = 1 byte
};

struct digital_write_command {
    uint8_t pin;              // + 1 byte
    uint8_t value;            // + 1 byte
                                // = 2 bytes
};

struct pin_mode_command {
    uint8_t pin;              // + 1 byte
    uint8_t mode;             // + 1 byte
    byte padding[1];          // + 1 byte
                                // = 3 bytes
};

struct analog_read_command {
    uint8_t pin;              // + 1 byte
    byte padding[3];          // + 3 bytes
                                // = 4 bytes
};

struct analog_write_command {
    uint8_t pin;              // + 1 byte
    uint8_t value;            // + 1 byte
    byte padding[3];          // + 3 bytes
                                // = 5 bytes
};

void pinMode(io_ext_pins pin, uint8_t mode)
{
    pin_mode_command pin_mode_send;
    pin_mode_send.pin = (uint8_t)pin;
    switch (mode)
    {
        case INPUT:
        pin_mode_send.mode = 0x00;
        break;

        case INPUT_PULLUP:
        pin_mode_send.mode = 0x02;
        break;

        case OUTPUT:
        pin_mode_send.mode = 0x01;
        break;

        default:
        pin_mode_send.mode = 0x00;
        break;
    };

    Wire.beginTransmission(I2C_ADDRESS_IO_EXTENDER);
    Wire.write((unsigned char*) &pin_mode_send, sizeof(pin_mode_send));
    Wire.endTransmission();
}

void digitalWrite(io_ext_pins pin, uint8_t value)
{
    digital_write_command digital_write_send;
    digital_write_send.pin = (uint8_t)pin;
    digital_write_send.value = value;

    Wire.beginTransmission(I2C_ADDRESS_IO_EXTENDER);
    Wire.write((unsigned char*) &digital_write_send, sizeof(digital_write_send));
    Wire.endTransmission();
}

int digitalRead(io_ext_pins pin)
{
    return -1;
}

int analogRead(io_ext_pins pin)
{
    return -1;
}

void analogWrite(io_ext_pins pin_io, uint8_t value)
{
    int pin = (int)pin_io;
    if (pin == 3 || pin == 5 || pin == 6 || pin == 9 || pin == 10 || pin == 11) 
    {
        analog_write_command analog_write_send;
        analog_write_send.pin = (uint8_t)pin;
        analog_write_send.value = value;

        Wire.beginTransmission(I2C_ADDRESS_IO_EXTENDER);
        Wire.write((unsigned char*) &analog_write_send, sizeof(analog_write_send));
        Wire.endTransmission();
    }
    else
    {
        logln("Warning! Didnt do analogWrite, as the pin %d is invalid!", pin);
    }
}

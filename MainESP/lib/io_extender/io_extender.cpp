#include "io_extender.h"

PCF8574* pcf8574 = new PCF8574(&Wire, I2C_ADDRESS_IO_EXT_PCF);

struct digital_write_command {
    uint8_t pin;              // + 1 byte
    uint8_t value;            // + 1 byte
                                // = 2 bytes
};

struct read_command {
    uint8_t what_to_read;     // + 1 byte
    uint8_t pin;              // + 1 byte
    byte padding[1];          // + 1 byte
                                // = 3 bytes
};

struct pin_mode_command {
    uint8_t pin;              // + 1 byte
    uint8_t mode;             // + 1 byte
    byte padding[2];          // + 2 byte
                                // = 4 bytes
};

struct analog_write_command {
    uint8_t pin;              // + 1 byte
    uint8_t value;            // + 1 byte
    byte padding[3];          // + 3 bytes
                                // = 5 bytes
};

enum read_types
{
    READ_TYPE_ANALOG = 0,
    READ_TYPE_DIGITAL = 1,
    READ_TYPE_LC02 = 2
};

struct analog_read_answer
{
    uint16_t value;
};

struct digital_read_answer
{
    uint8_t value;
};

void pinMode(io_ext_pins pin, uint8_t mode)
{
    if ((uint8_t)pin < 40) // Arduino
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
    else // PCF
    {
        pcf8574->pinMode((uint8_t)pin - 40, mode);
    }
}

void digitalWrite(io_ext_pins pin, uint8_t value)
{
    if ((uint8_t)pin >= 40) // PCF
    {
        pcf8574->digitalWrite((uint8_t)pin - 40, value);
    }
    else // Arduino
    {
        digital_write_command digital_write_send;
        digital_write_send.pin = (uint8_t)pin;
        digital_write_send.value = value;

        Wire.beginTransmission(I2C_ADDRESS_IO_EXTENDER);
        Wire.write((unsigned char*) &digital_write_send, sizeof(digital_write_send));
        Wire.endTransmission();
    }
}

int digitalRead(io_ext_pins pin)
{
    if ((uint8_t)pin >= 40) // PCF
    {
        return pcf8574->digitalRead((uint8_t)pin - 40);
    }
    else // Arduino
    {
        // First sending the arduino what to read
        read_command read_command_send;
        read_command_send.what_to_read = read_types::READ_TYPE_DIGITAL;
        read_command_send.pin = (uint8_t)pin;

        Wire.beginTransmission(I2C_ADDRESS_IO_EXTENDER);
        Wire.write((unsigned char*) &read_command_send, sizeof(read_command_send));
        Wire.endTransmission();

        // Then requesting the Data
        digital_read_answer answer_received;
        Wire.requestFrom(I2C_ADDRESS_IO_EXTENDER, sizeof(digital_read_answer));
        Wire.readBytes((byte*) &answer_received, sizeof(digital_read_answer));

        return answer_received.value;
    }
    return 0;
}

int analogRead(io_ext_pins pin)
{
    if ((uint8_t)pin < 40)  // Arduino
    {
        // First sending the arduino what to read
        read_command read_command_send;
        read_command_send.what_to_read = read_types::READ_TYPE_ANALOG;
        read_command_send.pin = (uint8_t)pin;

        Wire.beginTransmission(I2C_ADDRESS_IO_EXTENDER);
        Wire.write((unsigned char*) &read_command_send, sizeof(read_command_send));
        Wire.endTransmission();

        // Then requesting the Data
        analog_read_answer answer_received;
        Wire.requestFrom(I2C_ADDRESS_IO_EXTENDER, sizeof(analog_read_answer));
        Wire.readBytes((byte*) &answer_received, sizeof(analog_read_answer));

        return answer_received.value;
    }
    return 0;
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
        logln("Warning! Didn't do analogWrite, as the pin %d is invalid!", pin);
    }
}

PCF8574::DigitalInput pcfReadAll()
{
    return pcf8574->digitalReadAll();
}

void pcfWriteAll(PCF8574::DigitalInput pins)
{
    pcf8574->digitalWriteAll(pins);
}

void pcfBegin()
{
    pcf8574->begin();
}

lc02_answer getArduinoLc02Distance()
{
    // First sending the arduino what to read
    read_command read_command_send;
    read_command_send.what_to_read = read_types::READ_TYPE_LC02;
    read_command_send.pin = 0;

    Wire.beginTransmission(I2C_ADDRESS_IO_EXTENDER);
    Wire.write((unsigned char*) &read_command_send, sizeof(read_command_send));
    Wire.endTransmission();

    // Then requesting the Data
    lc02_answer answer_received;
    Wire.requestFrom(I2C_ADDRESS_IO_EXTENDER, sizeof(lc02_answer));
    Wire.readBytes((byte*) &answer_received, sizeof(lc02_answer));

    return answer_received;
}

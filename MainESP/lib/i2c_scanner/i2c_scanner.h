#ifndef I2C_SCANNER_H
#define I2C_SCANNER_H

#include <Wire.h>
#include <HardwareSerial.h>
#include "../../include/i2c_addresses.h"

uint8_t I2C_Addresses[32];
uint8_t I2C_Addresses_len = 0;


void scan_i2c_addresses()
{
    std::fill_n(I2C_Addresses, 32, 0);
    I2C_Addresses_len = 0;
    uint8_t error, address;
    for (address = 0x01; address < 0x7f; address++)
    {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0 || error == 4)
        {
            I2C_Addresses[I2C_Addresses_len] = address;
            I2C_Addresses_len++;
        }
    }
}

void print_i2c_addresses()
{
    if (I2C_Addresses_len > 0)
        Serial.print("I2C Addresses: ");
    else
        Serial.print("No I2C Devices found!");
    for (int i = 0; i < I2C_Addresses_len; i++)
    {
        Serial.print("0x");
        if (I2C_Addresses[i] < 16)
            Serial.print("0");
        Serial.print(I2C_Addresses[i], HEX);
        Serial.print(" ");
    }
    Serial.println("");
}

bool check_device_enabled(uint8_t address, String name, String short_name)
{
    bool ret = false;
    for (uint8_t i = 0; i < I2C_Addresses_len; i++) 
    {
        if (I2C_Addresses[i] == address)
        {
            Serial.println(name + " enabled");
            ret = true;
            break;
        }
    }

    if (!ret)
    {
        display.disable_i2c_device(short_name);
        Serial.println("ERROR: " + name + "! DEVICE NOT FOUND!");
    }
    return ret;
}

#endif /* I2C_SCANNER_H */

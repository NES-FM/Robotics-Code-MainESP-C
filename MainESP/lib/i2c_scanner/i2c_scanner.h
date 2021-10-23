#ifndef I2C_SCANNER_H
#define I2C_SCANNER_H

#include <Wire.h>

unsigned char I2C_Addresses[32];
unsigned char I2C_Addresses_len = 0;

void scan_i2c_addresses()
{
    unsigned char error, address;
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

#endif /* I2C_SCANNER_H */

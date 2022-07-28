#include <Arduino.h>
#include "../../include/i2c_addresses.h"
#include <Wire.h>
#include "logger.h"
#include "../../include/io_ext_pins.h"

void pinMode(io_ext_pins pin, uint8_t mode);
void digitalWrite(io_ext_pins pin, uint8_t value);
int digitalRead(io_ext_pins pin);
int analogRead(io_ext_pins pin);
void analogWrite(io_ext_pins pin, uint8_t value);

#include <Arduino.h>
#include "../../include/i2c_addresses.h"
#include <Wire.h>
#include "logger.h"
#include "../../include/io_ext_pins.h"
#include "PCF8574.h"

void pinMode(io_ext_pins pin, uint8_t mode);
void digitalWrite(io_ext_pins pin, uint8_t value);
int digitalRead(io_ext_pins pin);
int analogRead(io_ext_pins pin);
void analogWrite(io_ext_pins pin, uint8_t value);
PCF8574::DigitalInput pcfReadAll();
void pcfWriteAll(PCF8574::DigitalInput pins);

void pcfBegin();

lc02_answer getArduinoLc02Distance();

extern PCF8574* pcf8574;

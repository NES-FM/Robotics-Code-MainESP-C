#include <Arduino.h>
#include <pin_definitions.h>
#include <Wire.h>
#include <libraries.h>

motor motor_left;
motor motor_right;

#include "driving.h"

void setup() {
    Serial.begin(115200); 
    Wire.begin(PIN_SDA, PIN_SCL, 400000);

    motor_left.init(1);
    motor_right.init(2);

    scan_i2c_addresses();
    print_i2c_addresses();

    motor_left.enable(check_device_enabled(I2C_ADDRESSS_MOTOR_CONTROLLER, "motor_left"));
    motor_right.enable(check_device_enabled(I2C_ADDRESSS_MOTOR_CONTROLLER, "motor_right"));

    check_device_enabled(I2C_ADDRESS_DISPLAY, "display");
    check_device_enabled(I2C_ADDRESS_IO_EXTENDER, "io-extender");

    check_device_enabled(I2C_ADDRESS_COMPASS, "compass");
    check_device_enabled(I2C_ADDRESS_ACCELEROMETER, "accelerometer");

    CUART_init();
}

void loop() {
    CUART_tick();

    // CUART_debugPrintArray();

    // drive_sensor_array();

    // CUART_debugPrint();
}
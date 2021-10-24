#include <Arduino.h>
#include <Wire.h>

#include "pin_definitions.h"
#include "cuart_line_types.h"

#include "cuart.h"

#include "motor.h"
#include "debug_disp.h"
#include "i2c_scanner.h"

motor motor_left;
motor motor_right;
#include "driving.h"

debug_disp display;

#include "multithreaded_loop.h"

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);

    Serial.begin(115200); 
    Wire.begin(PIN_SDA, PIN_SCL, 400000);

    // I2C Enable
    scan_i2c_addresses();
    print_i2c_addresses();

    motor_left.enable(check_device_enabled(I2C_ADDRESSS_MOTOR_CONTROLLER, "motor_left"));
    motor_right.enable(check_device_enabled(I2C_ADDRESSS_MOTOR_CONTROLLER, "motor_right"));

    display.enable(check_device_enabled(I2C_ADDRESS_DISPLAY, "display"));
    check_device_enabled(I2C_ADDRESS_IO_EXTENDER, "io-extender");

    check_device_enabled(I2C_ADDRESS_COMPASS, "compass");
    check_device_enabled(I2C_ADDRESS_ACCELEROMETER, "accelerometer");

    // Initialization of lib
    motor_left.init(1);
    motor_right.init(2);

    CUART_init();

    display.init(CUART_sensor_array, CUART_green_dots, &CUART_line_type, &CUART_line_angle, &CUART_line_midfactor, &CUART_array_left_sensor, &CUART_array_mid_sensor, &CUART_array_right_sensor, &motor_left.motor_speed, &motor_right.motor_speed);

    init_multithreaded_loop();
}

void loop() {
    display.tick();

    // CUART_debugPrintArray();

    // drive_sensor_array();

    // CUART_debugPrint();
}

#include <Arduino.h>
#include <Wire.h>

#include "pin_definitions.h"
#include "cuart_line_types.h"

#include "cuart.h"
#include "motor.h"
#include "debug_disp.h"
#include "accel.h"
#include "compass.h"


debug_disp display;
#include "i2c_scanner.h"

accel accel_sensor;
compass_hmc compass;

motor motor_left;
motor motor_right;
#include "driving.h"

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

    motor_left.enable(check_device_enabled(I2C_ADDRESSS_MOTOR_CONTROLLER, "motor_left", "ML"));
    motor_right.enable(check_device_enabled(I2C_ADDRESSS_MOTOR_CONTROLLER, "motor_right", "MR"));

    display.enable(check_device_enabled(I2C_ADDRESS_DISPLAY, "display", "DI"));
    check_device_enabled(I2C_ADDRESS_IO_EXTENDER, "io-extender", "IO");

    compass.enable(check_device_enabled(I2C_ADDRESS_COMPASS, "compass", "CO"));
    accel_sensor.enable(check_device_enabled(I2C_ADDRESS_ACCELEROMETER, "accelerometer", "AC"));

    // Initialization of lib
    motor_left.init(1);
    motor_right.init(2);

    CUART_init();

    display.init(CUART_sensor_array, CUART_green_dots, &CUART_line_type, &CUART_line_angle, &CUART_line_midfactor, &CUART_array_left_sensor, &CUART_array_mid_sensor, &CUART_array_right_sensor, &motor_left.motor_speed, &motor_right.motor_speed, &driving_interesting_situation, &driving_interesting_bias_left, &driving_interesting_bias_right, &driving_interesting_bias_both, &compass, &accel_sensor);

    accel_sensor.init();
    compass.init(&accel_sensor);

    init_multithreaded_loop();
}

void loop() {
    display.tick();

    // CUART_debugPrintArray();

    if (motor_left.is_enabled() && motor_right.is_enabled())
        drive_sensor_array();

    // CUART_debugPrint();
}

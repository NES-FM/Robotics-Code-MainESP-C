#include <Arduino.h>
#include <Wire.h>

#include "pin_definitions.h"
#include "cuart_line_types.h"

#include "cuart.h"
#include "motor.h"
#include "accel.h"
#include "compass.h"
#include "analog_sensor.h"
#include "debug_disp.h"

CUART_class cuart;

debug_disp display;
#include "i2c_scanner.h"

accel accel_sensor;
compass_hmc compass;

motor motor_left;
motor motor_right;
#include "driving.h"

analog_sensor bat_voltage(PIN_BATPROBE, true);

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

    cuart.init();

    display.init(cuart.sensor_array, cuart.green_dots, &cuart.line_type, &cuart.line_angle, &cuart.line_midfactor, &cuart.array_left_sensor, &cuart.array_mid_sensor, &cuart.array_right_sensor, &motor_left.motor_speed, &motor_right.motor_speed, &driving_interesting_situation, &driving_interesting_bias_left, &driving_interesting_bias_right, &driving_interesting_bias_both, &compass, &accel_sensor, &bat_voltage);

    accel_sensor.init();
    compass.init(&accel_sensor);

    init_multithreaded_loop();

/*
    ledcAttachPin(26, 1);

    ledcWriteNote(1, NOTE_C, 4);
    delay(500);
    ledcWriteNote(1, NOTE_D, 4);
    delay(500);
    ledcWriteNote(1, NOTE_E, 4);
    delay(500);
    ledcWriteNote(1, NOTE_F, 4);
    delay(500);
    ledcWriteNote(1, NOTE_G, 4);
    delay(500);
    ledcWriteNote(1, NOTE_A, 4);
    delay(500);
    ledcWriteNote(1, NOTE_B, 4);
    delay(500);
    ledcDetachPin(26);
    */
}

void loop() {
    display.tick();

    // cuart.debugPrintArray();

    if (motor_left.is_enabled() && motor_right.is_enabled())
        drive_sensor_array();

    // cuart.debugPrint();
}

// #define EXTENSIVE_DEBUG

#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_GFX.h>

#include "pin_definitions.h"
#include "cuart_line_types.h"
#include "drive_speeds.h"

#include "debug_disp.h"

#include "cuart.h"
#include "timer.h"
#include <Preferences.h>

Preferences preferences;

CUART_class cuart;

debug_disp display;

#include "robot.h"
Robot robot;

#include "i2c_scanner.h"
#include "driving.h"
#include "multithreaded_loop.h"

void setup() 
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);

    Serial.begin(115200); 
    Wire.begin(PIN_SDA, PIN_SCL, 400000);

    preferences.begin("main_esp", false);

    // I2C Enable
    scan_i2c_addresses();
    print_i2c_addresses();

    robot.motor_left->enable(check_device_enabled(I2C_ADDRESSS_MOTOR_CONTROLLER, "motor_left", "ML"));
    robot.motor_right->enable(check_device_enabled(I2C_ADDRESSS_MOTOR_CONTROLLER, "motor_right", "MR"));

    display.enable(check_device_enabled(I2C_ADDRESS_DISPLAY, "display", "DI"));
    // check_device_enabled(I2C_ADDRESS_IO_EXTENDER, "io-extender", "IO");

    robot.compass->enable(check_device_enabled(I2C_ADDRESS_COMPASS, "compass", "CO"));
    robot.accel_sensor->enable(check_device_enabled(I2C_ADDRESS_ACCELEROMETER, "accelerometer", "AC"));

    // Initialization of libs
    robot.init();

    cuart.init();

    display.init(&cuart, &robot, &driving_interesting_situation, &driving_interesting_bias_left, &driving_interesting_bias_right, &driving_interesting_bias_both);

    init_multithreaded_loop();

    // Done Initializing
    robot.PlayBeginSound();
}

void loop() 
{
    main_loop();
}

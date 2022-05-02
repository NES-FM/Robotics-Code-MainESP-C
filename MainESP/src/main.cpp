// #define EXTENSIVE_DEBUG

#include <Arduino.h>
#include <Wire.h>

#include "pin_definitions.h"
#include "cuart_line_types.h"
#include "drive_speeds.h"

#include "debug_disp.h"

#include "cuart.h"
#include "timer.h"
#include <Preferences.h>
#include "i2c_scanner.h"
#include "driving.h"

Preferences preferences;

CUART_class cuart;

debug_disp display;

#include "robot.h"
Robot robot;

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

    // Enable I2C Devices
    robot.enable();

    // Initialization of libs
    robot.init();

    cuart.init();

    // cuart.sensor_array, cuart.green_dots, &cuart.line_type, &cuart.line_angle, &cuart.line_midfactor, &cuart.array_left_sensor, &cuart.array_mid_sensor, &cuart.array_right_sensor, &motor_left.motor_speed, &motor_right.motor_speed, &driving_interesting_situation, &driving_interesting_bias_left, &driving_interesting_bias_right, &driving_interesting_bias_both, &compass, &accel_sensor, &bat_voltage, &dip, &taster, &IR_L, &IR_R, &ecke, &hole, &kugel_in_greifer, &lir_value
    display.init(&cuart, &robot, &driving_interesting_situation, &driving_interesting_bias_left, &driving_interesting_bias_right, &driving_interesting_bias_both);

    init_multithreaded_loop();

    // Done Initializing
    robot.PlayBeginSound();
}

void loop() 
{
    main_loop();
}

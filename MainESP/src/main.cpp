#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_GFX.h>

// deg = rad*180/pi  rad = deg*pi/180
#define rad_to_deg(rad) (rad*(180/PI))
#define deg_to_rad(deg) (deg*(PI/180))

#include "pin_definitions.h"
#include "cuart_line_types.h"
#include "drive_speeds.h"

#include "io_extender.h"
#include "logger.h"

#include "moving_in_room.h"
#include "debug_disp.h"

#include "cuart.h"
#include "bcuart.h"
#include "target_timer.h"
#include <Preferences.h>

Preferences preferences;

CUART_class cuart;
BCUART_class bcuart;

debug_disp display;

#include "robot.h"
Robot robot(&cuart, &bcuart);

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
    Wire.begin(PIN_SDA, PIN_SCL, 400000U);

    logger_init();

    preferences.begin("main_esp", false);

    // I2C Enable
    scan_i2c_addresses();
    print_i2c_addresses();

    robot.motor_left->enable(check_device_enabled(I2C_ADDRESSS_MOTOR_CONTROLLER, "motor_left", "ML"));
    robot.motor_right->enable(check_device_enabled(I2C_ADDRESSS_MOTOR_CONTROLLER, "motor_right", "MR"));

    display.enable(check_device_enabled(I2C_ADDRESS_DISPLAY, "display", "DI"));

    delay(500); // Disgusting fix because io-ext is sometimes not initialized quick enough after reboot
    robot.io_ext->enable(check_device_enabled(I2C_ADDRESS_IO_EXTENDER, "io-extender", "IO"));

    // robot.compass->enable(check_device_enabled(I2C_ADDRESS_COMPASS_BMM, "compass", "CO"));
    // robot.accel_sensor->enable(check_device_enabled(I2C_ADDRESS_ACCELEROMETER, "accelerometer", "AC"));

    // Initialization of libs
    robot.init();

    // logln("I2C after robot.init:");
    // scan_i2c_addresses();
    // print_i2c_addresses();

    cuart.init();
    bcuart.init();

    display.init(&cuart, &robot, &driving_interesting_situation, &driving_interesting_bias_left, &driving_interesting_bias_right, &driving_interesting_bias_both);

    // logln("Test compass angle before init: %f", robot.compass->get_angle());

    init_multithreaded_loop();

    // Done Initializing
    robot.PlayBeginSound();

    // robot.setRoomBeginningAngle();
}

void loop() 
{
    main_loop();
}

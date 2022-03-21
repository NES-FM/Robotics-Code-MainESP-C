// #define OTA_BUILD

#include <Arduino.h>
#include <Wire.h>

#include "pin_definitions.h"
#include "cuart_line_types.h"
#include "drive_speeds.h"

#include "cuart.h"
#include "motor.h"
#include "accel.h"
#include "compass.h"
#include "analog_sensor.h"
#include "debug_disp.h"
#include "dip.h"
#include "timer.h"

CUART_class cuart;

debug_disp display;
#include "i2c_scanner.h"

#ifdef OTA_BUILD
#include "secrets.h"
#include "ota.h"
OTA ota(&display, ssid_define, password_define);
#include "web.h"
web web_app;
#endif

accel accel_sensor;
compass_hmc compass;

motor motor_left;
motor motor_right;
#include "driving.h"

analog_sensor bat_voltage(PIN_BATPROBE, true);

analog_sensor poti_l(PIN_SENS1);
analog_sensor poti_r(PIN_SENS2);

DIP dip;

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

    #ifdef OTA_BUILD
    ota.enable(!dip.get_wettkampfmodus());
    ota.init();
    web_app.init();
    #endif

    // Initialization of lib
    motor_left.init(1);
    motor_right.init(2);

    cuart.init();

    display.init(cuart.sensor_array, cuart.green_dots, &cuart.line_type, &cuart.line_angle, &cuart.line_midfactor, &cuart.array_left_sensor, &cuart.array_mid_sensor, &cuart.array_right_sensor, &motor_left.motor_speed, &motor_right.motor_speed, &driving_interesting_situation, &driving_interesting_bias_left, &driving_interesting_bias_right, &driving_interesting_bias_both, &compass, &accel_sensor, &bat_voltage, &dip);

    accel_sensor.init();
    compass.init(&accel_sensor);

    if (dip.get_state(dip.dip2))
    {
        motor_left.move(DRIVE_SPEED_NORMAL_DEFAULT);
        motor_right.move(-DRIVE_SPEED_NORMAL_DEFAULT);
        compass.calibrate();
    }

    // move(-DRIVE_SPEED_NORMAL_DEFAULT-3, DRIVE_SPEED_NORMAL_DEFAULT+5);

    init_multithreaded_loop();
}

void adjusted_drive(int ml, int mr)
{
    motor_left.move(ml + map(poti_l.get_state(), 0, 4095, -15, 15));
    motor_right.move(mr + map(poti_r.get_state(), 0, 4095, -15, 15));
}

void loop() {
    main_loop();
    // adjusted_drive(30, -30);
    // Serial.printf("Potis: L: %d, R: %d\r\n", map(poti_l.get_state(), 0, 4095, -15, 15), map(poti_r.get_state(), 0, 4095, -15, 15));
}

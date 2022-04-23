// #define OTA_BUILD
// #define EXTENSIVE_DEBUG

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
#include "taster.h"
#include "ir.h"
#include <Preferences.h>

Preferences preferences;

#include <Servo.h>
#include "servo_angles.h"
Servo greifer_up;
Servo greifer_zu;

DIP dip;

#include "buzz.h"
buzz main_buzzer(PIN_BUZZ1, 128, &dip);

CUART_class cuart;

taster_class taster;

IR IR_L(PIN_SENS1);
IR IR_R(PIN_SENS2);

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

#include "multithreaded_loop.h"

void setup() {
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

    motor_left.enable(check_device_enabled(I2C_ADDRESSS_MOTOR_CONTROLLER, "motor_left", "ML"));
    motor_right.enable(check_device_enabled(I2C_ADDRESSS_MOTOR_CONTROLLER, "motor_right", "MR"));

    display.enable(check_device_enabled(I2C_ADDRESS_DISPLAY, "display", "DI"));
    // check_device_enabled(I2C_ADDRESS_IO_EXTENDER, "io-extender", "IO");

    compass.enable(check_device_enabled(I2C_ADDRESS_COMPASS, "compass", "CO"));
    accel_sensor.enable(check_device_enabled(I2C_ADDRESS_ACCELEROMETER, "accelerometer", "AC"));

    greifer_zu.attach(PIN_SERVO1);
    greifer_zu.write(ANGLE_GREIFER_CLOSE_CUBE);
    delay(300);
    greifer_up.attach(PIN_SERVO2);
    greifer_up.write(ANGLE_GREIFER_UP); //NEEDS TO BE CHANGED

    #ifdef OTA_BUILD
    ota.enable(!dip.get_wettkampfmodus());
    ota.init();
    web_app.init();
    #endif

    // Initialization of lib
    motor_left.init(1);
    motor_right.init(2);

    cuart.init();

    display.init(cuart.sensor_array, cuart.green_dots, &cuart.line_type, &cuart.line_angle, &cuart.line_midfactor, &cuart.array_left_sensor, &cuart.array_mid_sensor, &cuart.array_right_sensor, &motor_left.motor_speed, &motor_right.motor_speed, &driving_interesting_situation, &driving_interesting_bias_left, &driving_interesting_bias_right, &driving_interesting_bias_both, &compass, &accel_sensor, &bat_voltage, &dip, &taster, &IR_L, &IR_R, &ecke, &hole, &cuart.silver_line);

    accel_sensor.init();
    compass.init(&accel_sensor);

    // if (dip.get_state(dip.dip2))
    // {
    //     motor_left.move(DRIVE_SPEED_NORMAL_DEFAULT);
    //     motor_right.move(-DRIVE_SPEED_NORMAL_DEFAULT);
    //     compass.calibrate();
    // }

    // move(-DRIVE_SPEED_NORMAL_DEFAULT-3, DRIVE_SPEED_NORMAL_DEFAULT+5);

    if (dip.get_state(dip.dip1))
    {
        preferences.putInt("balls", 0);
        preferences.putInt("ecke", 0);
    }

    main_buzzer.tone(NOTE_C, 4, 100);
    main_buzzer.tone(NOTE_D, 4, 100);
    main_buzzer.tone(NOTE_E, 4, 100);
    main_buzzer.tone(NOTE_F, 4, 100);
    main_buzzer.tone(NOTE_G, 4, 100);
    main_buzzer.tone(NOTE_A, 4, 100);
    main_buzzer.tone(NOTE_B, 4, 100);

    init_multithreaded_loop();
}

void adjusted_drive(int ml, int mr)
{
    motor_left.move(ml + map(poti_l.get_state(), 0, 4095, -15, 15));
    motor_right.move(mr + map(poti_r.get_state(), 0, 4095, -15, 15));
}

void loop() {
    // int pos = 0;
    
    // for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // // in steps of 1 degree
    // greifer_up.write(pos);              // tell servo to go to position in variable 'pos'
    // Serial.println(pos);
    // delay(15);                       // waits 15 ms for the servo to reach the position
    // }
    // for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    //     greifer_up.write(pos);              // tell servo to go to position in variable 'pos'
    //     Serial.println(pos);
    //     delay(15);                       // waits 15 ms for the servo to reach the position
    // }
    
    // Serial.printf("L: Raw: %d, Cm: %f  R: Raw: %d, Cm: %f\r\n", IR_L.get_raw(), IR_L.get_cm(), IR_R.get_raw(), IR_R.get_cm());
    // delay(200);
    
    main_loop();

    // move(DRIVE_SPEED_CORNER, -DRIVE_SPEED_CORNER);
}

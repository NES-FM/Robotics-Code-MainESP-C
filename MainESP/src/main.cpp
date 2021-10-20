#include <Arduino.h>
#include <pin_definitions.h>
#include <libraries.h>
#include <Wire.h>

motor motor_left;
motor motor_right;

void setup() {
    Serial.begin(115200); 
    Wire.begin(PIN_SDA, PIN_SCL, 400000);

    motor_left.init(1);
    motor_right.init(2);

    // CUART_init();
}

void loop() {
    // CUART_tick();

    // Serial.println(1);
    // for (int i = 0; i < 30; i++)
    // {
    //     motor_left.move(i);
    //     delay(100);
    // }
    // Serial.println(2);
    // for (int i = 0; i < 30; i++)
    // {
    //     motor_right.move(i);
    //     delay(100);
    // }
    // delay(2000);
    Serial.println(3);
    for (int i = 0; i > -30; i--)
    {
        // -----> Left is only moving forward, right is functioning properly
        motor_left.move(i);
        motor_right.move(i);
        delay(100);
    }

    delay(1000);


    // Serial.println("~");
    // // Serial.print(CUART_line_type); Serial.print(CUART_line_angle); Serial.println(CUART_line_midfactor);
    // for (int i = 0; i < 24; i++)
    // {
    //     if (CUART_sensor_array[i])
    //         Serial.print("1 ");
    //     else
    //         Serial.print("0 ");
    // }
    // Serial.println("");
}
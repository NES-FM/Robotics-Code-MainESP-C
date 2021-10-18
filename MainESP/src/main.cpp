#include <Arduino.h>
#include <pin_definitions.h>
#include "../lib/cuart/cuart.h"

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200); 
    CUART_init();
}

void loop() {
    // put your main code here, to run repeatedly:
    CUART_tick();
    Serial.println("~");
    Serial.print(CUART_line_type); Serial.print(CUART_line_angle); Serial.println(CUART_line_midfactor);
    for (int i = 0; i < 24; i++)
    {
        if (CUART_sensor_array[i])
            Serial.print("1 ");
        else
            Serial.print("0 ");
    }
    Serial.println("");
}
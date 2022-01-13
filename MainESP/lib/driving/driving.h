#ifndef DRIVING_H
#define DRIVING_H

#include "../../include/drive_speeds.h"

#define DEBUG_MOTOR_VALUES 0

void drive(int speed_left, int speed_right)
{
    //display.draw_star();
    motor_left.move(speed_left);
    motor_right.move(speed_right);
}

bool driving_interesting_situation = false;
bool driving_interesting_bias_left = false;  // If it is suspected to be either Tl or 90l, discard all right situations
bool driving_interesting_bias_right = false; // ... and vice versa
bool driving_interesting_bias_both = false;  // ... or if the line is so wide, that only X or T is possible, discard 90lr and Tlr
int driving_interesting_actual_ltype = 0;
bool driving_interesting_actual_ltype_override = false;

void drive_sensor_array()
{
    // Line is left...
    if (!driving_interesting_situation && cuart.array_left_sensor >= 1 && cuart.array_right_sensor == 0)
    {
        if ((cuart.sensor_array[3] == true || cuart.sensor_array[4] == true) && cuart.array_left_sensor > 3)
            drive(-DRIVE_SPEED_HIGH, DRIVE_SPEED_NORMAL);
        else if (cuart.sensor_array[5] == true && cuart.array_left_sensor > 3)
            drive(-DRIVE_SPEED_LOWER, DRIVE_SPEED_HIGH);
        else if (cuart.sensor_array[6] == true && cuart.array_left_sensor > 3)
            drive(-DRIVE_SPEED_LOW, DRIVE_SPEED_HIGH);
        else if (cuart.sensor_array[7] == true)
            drive(0, DRIVE_SPEED_HIGHER);
        else if (cuart.sensor_array[8] == true)
            drive(DRIVE_SPEED_LOWER, DRIVE_SPEED_HIGH); 
        else if (cuart.sensor_array[9] == true)
            drive(DRIVE_SPEED_NORMAL, DRIVE_SPEED_HIGH);
    }

    // Line is right...
    if (!driving_interesting_situation && cuart.array_left_sensor == 0 && cuart.array_right_sensor >= 1)
    {
        if ((cuart.sensor_array[22] == true || cuart.sensor_array[21] == true) && cuart.array_right_sensor > 3)
            drive(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_HIGH);
        else if (cuart.sensor_array[20] == true && cuart.array_right_sensor > 3)
            drive(DRIVE_SPEED_HIGH, -DRIVE_SPEED_LOWER);
        else if (cuart.sensor_array[19] == true && cuart.array_right_sensor > 3)
            drive(DRIVE_SPEED_HIGH, -DRIVE_SPEED_LOW);
        else if (cuart.sensor_array[18] == true)
            drive(DRIVE_SPEED_HIGHER, 0);
        else if (cuart.sensor_array[17] == true)
            drive(DRIVE_SPEED_HIGH, DRIVE_SPEED_LOWER); 
        else if (cuart.sensor_array[16] == true)
            drive(DRIVE_SPEED_HIGH, DRIVE_SPEED_NORMAL);
    }

    // Line is interesting (could be 90 degree, could be T, could be X, etc...)
    if ((cuart.array_left_sensor + cuart.array_mid_sensor + cuart.array_right_sensor) > 9 && !driving_interesting_bias_both)
    {
        driving_interesting_situation = true;
        drive(DRIVE_SPEED_HALF, DRIVE_SPEED_HALF);
        driving_interesting_bias_both = ((cuart.array_left_sensor + cuart.array_mid_sensor + cuart.array_right_sensor) > 16);
        if (!driving_interesting_bias_both)
        {
            driving_interesting_bias_left = ((cuart.array_left_sensor + cuart.array_mid_sensor) > 6 && cuart.array_right_sensor < 2);
            driving_interesting_bias_right = (cuart.array_left_sensor < 2 && (cuart.array_mid_sensor + cuart.array_right_sensor) > 6);
        }
        else
        {
            driving_interesting_bias_left = false;
            driving_interesting_bias_right = false;
        }
        digitalWrite(LED_BUILTIN, HIGH);
    }

    // Line is only in the middle
    if (cuart.array_left_sensor < 2 && cuart.array_mid_sensor > 2 && cuart.array_right_sensor < 2)
    {
        if (driving_interesting_situation)
            drive(DRIVE_SPEED_HALF, DRIVE_SPEED_HALF);
        else
            drive(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
    }

    // Interesting situation passed
    if (cuart.array_left_sensor < 2 && cuart.array_right_sensor < 2 && cuart.sensor_array[0] && driving_interesting_situation)
    {
        drive(0, 0);
        // delay(2000);
        for (int del = 0; del <= 2000; del += 100)
        {
            display.tick();
            delay(100);
        }

        driving_interesting_actual_ltype = cuart.line_type;

        if (cuart.array_mid_sensor < 2)
        {
            if (driving_interesting_bias_left)
            {
                driving_interesting_actual_ltype = CUART_LTYPE_90l;
                driving_interesting_actual_ltype_override = true;
            }
            else
            {
                driving_interesting_actual_ltype = CUART_LTYPE_90r;
                driving_interesting_actual_ltype_override = true;
            }
        }
        if (!driving_interesting_actual_ltype_override)
        {
            if (driving_interesting_bias_left)
            {
                if (cuart.line_type == CUART_LTYPE_90r)
                    driving_interesting_actual_ltype = CUART_LTYPE_t;
                else if (cuart.line_type == CUART_LTYPE_tr)
                    driving_interesting_actual_ltype = CUART_LTYPE_X;
                else if (cuart.line_type == CUART_LTYPE_STRAIGHT || cuart.line_type == CUART_LTYPE_UNKNOWN || cuart.line_type == CUART_LTYPE_SPACE)
                    driving_interesting_actual_ltype = CUART_LTYPE_90l;
            }
            else if (driving_interesting_bias_right)
            {
                if (cuart.line_type == CUART_LTYPE_90l)
                    driving_interesting_actual_ltype = CUART_LTYPE_t;
                else if (cuart.line_type == CUART_LTYPE_tl)
                    driving_interesting_actual_ltype = CUART_LTYPE_X;
                else if (cuart.line_type == CUART_LTYPE_STRAIGHT || cuart.line_type == CUART_LTYPE_UNKNOWN || cuart.line_type == CUART_LTYPE_SPACE)
                    driving_interesting_actual_ltype = CUART_LTYPE_90r;
            }
            else if (driving_interesting_bias_both)
            {
                if (cuart.line_type == CUART_LTYPE_90r || cuart.line_type == CUART_LTYPE_90l)
                    driving_interesting_actual_ltype = CUART_LTYPE_t;
                else if (cuart.line_type == CUART_LTYPE_tr || cuart.line_type == CUART_LTYPE_tl)
                    driving_interesting_actual_ltype = CUART_LTYPE_X;
                else if (cuart.line_type != CUART_LTYPE_t && cuart.line_type != CUART_LTYPE_X)
                    driving_interesting_actual_ltype = CUART_LTYPE_X;
            }
        }

        // Serial.printf("Driving interesting situation with ltype: %d, Bias: L: %s, R: %s, B: %s\r\n", driving_interesting_actual_ltype, driving_interesting_bias_left ? "T" : "F", driving_interesting_bias_right ? "T" : "F", driving_interesting_bias_both ? "T" : "F");

        // Actual driving

        // Dead End: Turn Around
        if ((driving_interesting_actual_ltype == CUART_LTYPE_t || 
                 driving_interesting_actual_ltype == CUART_LTYPE_X) && 
                 (cuart.green_dots[2] && cuart.green_dots[3]))
        {
            Serial.printf("Interesting: Turning Around (Dead End) with ltype %d and dl green dot %s and dr green dot %s\r\n", driving_interesting_actual_ltype, cuart.green_dots[2] ? "True" : "False", cuart.green_dots[3] ? "True" : "False");
            drive(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
            while(cuart.array_right_sensor < 2) {
                display.tick();
                vTaskDelay( pdMS_TO_TICKS( 10 ) );
            }
            while(cuart.array_mid_sensor < 2) {
                display.tick();
                vTaskDelay( pdMS_TO_TICKS( 10 ) );
            }
            while(cuart.array_left_sensor < 2) {
                display.tick();
                vTaskDelay( pdMS_TO_TICKS( 10 ) );
            }
            while(cuart.array_right_sensor < 2) {
                display.tick();
                vTaskDelay( pdMS_TO_TICKS( 10 ) );
            }
            while(cuart.array_mid_sensor < 2) {
                display.tick();
                vTaskDelay( pdMS_TO_TICKS( 10 ) );
            }
            while(!cuart.sensor_array[10]) {
                display.tick();
                vTaskDelay( pdMS_TO_TICKS( 10 ) );
            }
            drive(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
        }
        // Turn left
        else if ((driving_interesting_actual_ltype == CUART_LTYPE_90l) || 
              ((driving_interesting_actual_ltype == CUART_LTYPE_tl || 
                driving_interesting_actual_ltype == CUART_LTYPE_t || 
                driving_interesting_actual_ltype == CUART_LTYPE_X) && cuart.green_dots[2]))
        {
            Serial.printf("Interesting: Turning left with ltype %d and dl green dot %s\r\n", driving_interesting_actual_ltype, cuart.green_dots[2] ? "True" : "False");
            drive(-DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
            while(cuart.array_mid_sensor > 2) {
                display.tick();
                vTaskDelay( pdMS_TO_TICKS( 10 ) );
            }
            while(cuart.sensor_array[15]) {
                display.tick();
                vTaskDelay( pdMS_TO_TICKS( 10 ) );
            }
            delay(10);
            while(!cuart.sensor_array[15]) {
                display.tick();
                vTaskDelay( pdMS_TO_TICKS( 10 ) );
            }
            drive(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
        }
        // Turn right
        else if ((driving_interesting_actual_ltype == CUART_LTYPE_90r) || 
                  ((driving_interesting_actual_ltype == CUART_LTYPE_tr || 
                    driving_interesting_actual_ltype == CUART_LTYPE_t || 
                    driving_interesting_actual_ltype == CUART_LTYPE_X) && cuart.green_dots[3]))
        {
            Serial.printf("Interesting: Turning right with ltype %d and dr green dot %s\r\n", driving_interesting_actual_ltype, cuart.green_dots[3] ? "True" : "False");
            drive(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
            while(cuart.array_mid_sensor > 2) {
                display.tick();
                vTaskDelay( pdMS_TO_TICKS( 10 ) );
            }
            while(cuart.sensor_array[10]) {
                display.tick();
                vTaskDelay( pdMS_TO_TICKS( 10 ) );
            }
            delay(10);
            while(!cuart.sensor_array[10]) {
                display.tick();
                vTaskDelay( pdMS_TO_TICKS( 10 ) );
            }
            drive(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
        }
        // Keep Straight
        else
        {
            Serial.printf("Interesting: Keeping Straight with ltype %d and dl green dot %s and dr green dot %s\r\n", driving_interesting_actual_ltype, cuart.green_dots[2] ? "True" : "False", cuart.green_dots[3] ? "True" : "False");
            drive(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
        }

        driving_interesting_situation = false;
        driving_interesting_actual_ltype_override = false;
        driving_interesting_bias_both = false;
        driving_interesting_bias_left = false;
        driving_interesting_bias_right = false;
        digitalWrite(LED_BUILTIN, LOW);
    }
    
    if (DEBUG_MOTOR_VALUES == 1)
    {
        Serial.printf("L: %d, M: %d, R: %d, I: %s, Bias: L: %s, R: %s, B: %s\r\n", cuart.array_left_sensor, cuart.array_mid_sensor, cuart.array_right_sensor, driving_interesting_situation ? "T" : "F", driving_interesting_bias_left ? "T" : "F", driving_interesting_bias_right ? "T" : "F", driving_interesting_bias_both ? "T" : "F");
    }
}

#endif /* DRIVING_H */

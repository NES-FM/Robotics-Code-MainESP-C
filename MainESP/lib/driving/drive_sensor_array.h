#include "timer.h"

int DRIVE_SPEED_NORMAL = DRIVE_SPEED_NORMAL_DEFAULT;
uint_fast64_t lowering_drive_speed_millis = 0;
bool driving_interesting_situation = false;
bool driving_interesting_bias_left = false;  // If it is suspected to be either Tl or 90l, discard all right situations
bool driving_interesting_bias_right = false; // ... and vice versa
bool driving_interesting_bias_both = false;  // ... or if the line is so wide, that only X or T is possible, discard 90lr and Tlr
int driving_interesting_actual_ltype = 0;
bool driving_interesting_actual_ltype_override = false;

timer white_timer(1500);
#include "find_line.h"

/*
void crossing_90_right()
{
    move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    delay(100);
    move(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    while(cuart.array_mid_sensor > 2) {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    // delay(25);
    while(cuart.sensor_array[10]) {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    delay(10);
    while(!cuart.sensor_array[10]) {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    // delay(50);
    // move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    // delay(100);
    move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
}

void crossing_90_left()
{
    move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    delay(100);
    move(-DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
    while(cuart.array_mid_sensor > 2) {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    // delay(25);
    while(cuart.sensor_array[15]) {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    delay(10);
    while(!cuart.sensor_array[15]) {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    // delay(50);
    // move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    // delay(100);
    move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
}
//*/

void crossing_90_right()
{
    // move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    // delay(100);
    move(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    while(cuart.array_right_sensor > 2) {
        display.tick();
        vTaskDelay(watchdog_delay);
    }

    while(cuart.array_mid_sensor >= 2) {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    delay(10);
    while(cuart.array_mid_sensor < 3) {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    while(cuart.sensor_array[9] || cuart.sensor_array[16]) {
        display.tick();
        vTaskDelay(watchdog_delay);
    }

    move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
}

void crossing_90_left()
{
    // move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    // delay(100);
    move(-DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
    while(cuart.array_left_sensor > 2) {
        display.tick();
        vTaskDelay(watchdog_delay);
    }

    while(cuart.array_mid_sensor >= 2) {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    delay(10);
    while(cuart.array_mid_sensor < 3) {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    while(cuart.sensor_array[9] || cuart.sensor_array[16]) {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    
    move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
}

void drive_sensor_array()
{
    // Line is left...
    if (!driving_interesting_situation && cuart.array_left_sensor >= 1 && cuart.array_right_sensor == 0/* && cuart.array_mid_sensor <= 2*/)
    {
        if ((cuart.sensor_array[3] == true || cuart.sensor_array[4] == true) && cuart.array_left_sensor > 3)
            move(-DRIVE_SPEED_HIGH, DRIVE_SPEED_NORMAL);
        else if (cuart.sensor_array[5] == true && cuart.array_left_sensor > 3)
            move(-DRIVE_SPEED_LOWER, DRIVE_SPEED_HIGH);
        else if (cuart.sensor_array[6] == true && cuart.array_left_sensor > 3)
            move(-DRIVE_SPEED_LOW, DRIVE_SPEED_HIGH);
        else if (cuart.sensor_array[7] == true)
            move(-DRIVE_SPEED_LOW, DRIVE_SPEED_HIGH);
        else if (cuart.sensor_array[8] == true)
            move(DRIVE_SPEED_LOWER, DRIVE_SPEED_HIGH); 
        else if (cuart.sensor_array[9] == true)
            move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_HIGH);
    }

    // Line is right...
    if (!driving_interesting_situation && cuart.array_left_sensor == 0 && cuart.array_right_sensor >= 1 /*&& cuart.array_mid_sensor <= 2*/)
    {
        if ((cuart.sensor_array[22] == true || cuart.sensor_array[21] == true) && cuart.array_right_sensor > 3)
            move(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_HIGH);
        else if (cuart.sensor_array[20] == true && cuart.array_right_sensor > 3)
            move(DRIVE_SPEED_HIGH, -DRIVE_SPEED_LOWER);
        else if (cuart.sensor_array[19] == true && cuart.array_right_sensor > 3)
            move(DRIVE_SPEED_HIGH, -DRIVE_SPEED_LOW);
        else if (cuart.sensor_array[18] == true)
            move(DRIVE_SPEED_HIGH, -DRIVE_SPEED_LOW);
        else if (cuart.sensor_array[17] == true)
            move(DRIVE_SPEED_HIGH, DRIVE_SPEED_LOWER); 
        else if (cuart.sensor_array[16] == true)
            move(DRIVE_SPEED_HIGH, DRIVE_SPEED_NORMAL);
    }

    // Line is interesting (could be 90 degree, could be T, could be X, etc...)
    if (cuart.array_total > 9 && !driving_interesting_bias_both)
    {
        driving_interesting_situation = true;
        // DRIVE_SPEED_NORMAL = DRIVE_SPEED_LOW_DEFAULT;
        lowering_drive_speed_millis = millis();
        move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
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
        move(DRIVE_SPEED_HALF, DRIVE_SPEED_HALF);
    }

    // Line is only in the middle
    if (cuart.array_left_sensor < 2 && cuart.array_mid_sensor > 2 && cuart.array_right_sensor < 2)
    {
        if (driving_interesting_situation)
            move(DRIVE_SPEED_HALF, DRIVE_SPEED_HALF);
        else
            move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
    }

    // // Circle
    // if (driving_interesting_bias_both && 
    //     (cuart.array_left_sensor > 2 && cuart.array_mid_sensor <= 2 && cuart.array_right_sensor > 2))
    // {
    //     // Here something would happen, probably set a variable to change the behaviour everywhere else
    //     move(0, 0);
    //     delay(2000);
    //     move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
    // }

    // Driven past line, no center line
    if (driving_interesting_situation && !cuart.sensor_array[0] && cuart.array_total < 4)
    {
        if (driving_interesting_bias_left)
        {
            delay(100);
            crossing_90_left();
        }
        else if (driving_interesting_bias_right)
        {
            delay(100);
            crossing_90_right();
        }
    }

    // Interesting situation passed
    // if ((
    //     (cuart.array_left_sensor < 4 && cuart.array_mid_sensor > 2 && cuart.array_right_sensor < 4) || 
    //     (cuart.array_left_sensor < 4 && cuart.array_mid_sensor < 4 && cuart.array_right_sensor > 2) ||
    //     (cuart.array_left_sensor > 2 && cuart.array_mid_sensor < 4 && cuart.array_right_sensor < 4) ||
    //     (cuart.array_left_sensor < 4 && cuart.array_mid_sensor < 4 && cuart.array_right_sensor < 4)
    // ) && cuart.sensor_array[0] && driving_interesting_situation)
    if (driving_interesting_situation && cuart.array_total <= 7 && cuart.sensor_array[0])
    {
        driving_interesting_actual_ltype = cuart.line_type;
        driving_interesting_actual_ltype_override = false;

        // move(0,0);
        // delay(1000);
        // move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);

        if (cuart.array_total < 4 && !cuart.sensor_array[3] && !cuart.sensor_array[22])
        {
            if (driving_interesting_bias_left)
            {
                driving_interesting_actual_ltype = CUART_LTYPE_90l;
                driving_interesting_actual_ltype_override = true;
            }
            else if (driving_interesting_bias_right)
            {
                driving_interesting_actual_ltype = CUART_LTYPE_90r;
                driving_interesting_actual_ltype_override = true;
            }
        }
        if (!driving_interesting_actual_ltype_override && !(cuart.green_dots[2] || cuart.green_dots[3]))
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

        // If there is a green dot somewhere
        if (cuart.green_dots[2] || cuart.green_dots[3])
        {
            // Dead End: Turn Around
            if (/*(driving_interesting_actual_ltype == CUART_LTYPE_t || 
                    driving_interesting_actual_ltype == CUART_LTYPE_X) && */
                    (cuart.green_dots[2] && cuart.green_dots[3]))
            {
                Serial.printf("Interesting: Turning Around (Dead End) with ltype %d and dl green dot %s and dr green dot %s\r\n", driving_interesting_actual_ltype, cuart.green_dots[2] ? "True" : "False", cuart.green_dots[3] ? "True" : "False");
                move(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
                while(cuart.array_right_sensor < 2) {
                    display.tick();
                    vTaskDelay(watchdog_delay);
                }
                while(cuart.array_mid_sensor < 2) {
                    display.tick();
                    vTaskDelay(watchdog_delay);
                }
                while(cuart.array_left_sensor < 2) {
                    display.tick();
                    vTaskDelay(watchdog_delay);
                }
                while(cuart.array_right_sensor < 2) {
                    display.tick();
                    vTaskDelay(watchdog_delay);
                }
                while(cuart.array_mid_sensor < 2) {
                    display.tick();
                    vTaskDelay(watchdog_delay);
                }
                while(!cuart.sensor_array[10]) {
                    display.tick();
                    vTaskDelay(watchdog_delay);
                }
                move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
            }
            // Turn left - green dot
            else if (/*(driving_interesting_actual_ltype == CUART_LTYPE_tl || 
                    driving_interesting_actual_ltype == CUART_LTYPE_t || 
                    driving_interesting_actual_ltype == CUART_LTYPE_X) && */cuart.green_dots[2])
            {
                crossing_90_left();
                // delay(150);
            }
            // Turn right - green dot
            else if (/*(driving_interesting_actual_ltype == CUART_LTYPE_tr || 
                        driving_interesting_actual_ltype == CUART_LTYPE_t || 
                        driving_interesting_actual_ltype == CUART_LTYPE_X) && */cuart.green_dots[3])
            {
                crossing_90_right();
                // delay(150);
            }
        }
        else 
        {
            // Turn left - 90 deg
            if (driving_interesting_actual_ltype == CUART_LTYPE_90l)
            {
                Serial.printf("Interesting: Turning left with ltype %d and dl green dot %s\r\n", driving_interesting_actual_ltype, cuart.green_dots[2] ? "True" : "False");
                crossing_90_left();
            }
            // Turn right - 90 deg
            else if (driving_interesting_actual_ltype == CUART_LTYPE_90r)
            {
                Serial.printf("Interesting: Turning right with ltype %d and dr green dot %s\r\n", driving_interesting_actual_ltype, cuart.green_dots[3] ? "True" : "False");
                crossing_90_right();
            }
            // Keep Straight
            else
            {
                Serial.printf("Interesting: Keeping Straight with ltype %d and dl green dot %s and dr green dot %s\r\n", driving_interesting_actual_ltype, cuart.green_dots[2] ? "True" : "False", cuart.green_dots[3] ? "True" : "False");
                move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
            }
        }

        driving_interesting_situation = false;
        driving_interesting_actual_ltype_override = false;
        driving_interesting_bias_both = false;
        driving_interesting_bias_left = false;
        driving_interesting_bias_right = false;
        move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
        digitalWrite(LED_BUILTIN, LOW);
    }

    if ((millis() - lowering_drive_speed_millis) >= 1000)
    {
        DRIVE_SPEED_NORMAL = DRIVE_SPEED_NORMAL_DEFAULT;
    }

    if (cuart.array_total > 2)
    {
        white_timer.reset();
    }

    if (white_timer.has_reached_target())
    {
        find_line();
    }
    
    if (DEBUG_MOTOR_VALUES == 1)
    {
        Serial.printf("L: %d, M: %d, R: %d, I: %s, Bias: L: %s, R: %s, B: %s\r\n", cuart.array_left_sensor, cuart.array_mid_sensor, cuart.array_right_sensor, driving_interesting_situation ? "T" : "F", driving_interesting_bias_left ? "T" : "F", driving_interesting_bias_right ? "T" : "F", driving_interesting_bias_both ? "T" : "F");
    }
}

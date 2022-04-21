#include "timer.h"

bool driving_interesting_situation = false;
bool driving_interesting_bias_left = false;  // If it is suspected to be either Tl or 90l, discard all right situations
bool driving_interesting_bias_right = false; // ... and vice versa
bool driving_interesting_bias_both = false;  // ... or if the line is so wide, that only X or T is possible, discard 90lr and Tlr
int driving_interesting_actual_ltype = 0;
bool driving_interesting_actual_ltype_override = false;

timer white_timer(1500);
#include "find_line.h"
#include "ausweichen.h"

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
    main_buzzer.turnLeftSound();
    move(DRIVE_SPEED_CORNER, -DRIVE_SPEED_CORNER);
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
    while(/*cuart.sensor_array[9] ||*/ cuart.sensor_array[16] && cuart.array_total < 7) {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
}

void crossing_90_left()
{
    // move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    // delay(100);
    main_buzzer.turnRightSound();
    move(-DRIVE_SPEED_CORNER, DRIVE_SPEED_CORNER);
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
    while(cuart.sensor_array[9]/* || cuart.sensor_array[16]*/ && cuart.array_total < 7) {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
}

void drive_sensor_array()
{
    #ifdef EXTENSIVE_DEBUG
    Serial.println("[DRIVE_SENSOR_ARRAY] Start!");
    #endif

    // Line is left...
    if (!driving_interesting_situation && cuart.array_left_sensor >= 1 && cuart.array_right_sensor == 0/* && cuart.array_mid_sensor <= 2*/)
    {
        #ifdef EXTENSIVE_DEBUG
        Serial.printf("[DRIVE_SENSOR_ARRAY] Line is left with: driving_interesting_situation: %s, cuart.array_left_sensor: %d, cuart.array_right_sensor: %d\r\n", driving_interesting_situation ? "T" : "F", cuart.array_left_sensor, cuart.array_right_sensor);
        #endif
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
        #ifdef EXTENSIVE_DEBUG
        Serial.printf("[DRIVE_SENSOR_ARRAY] Line is right with: driving_interesting_situation: %s, cuart.array_left_sensor: %d, cuart.array_right_sensor: %d\r\n", driving_interesting_situation ? "T" : "F", cuart.array_left_sensor, cuart.array_right_sensor);
        #endif
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
    if (cuart.array_total > 8 && !driving_interesting_bias_both && accel_sensor.getCurrentRampState() != accel_sensor.down && accel_sensor.getCurrentRampState() != accel_sensor.up)
    {
        #ifdef EXTENSIVE_DEBUG
        Serial.printf("[DRIVE_SENSOR_ARRAY] Line is interesting with: array_total: %d, left_sensor: %d, mid_sensor: %d, right_sensor: %d\r\n", cuart.array_total, cuart.array_left_sensor, cuart.array_mid_sensor, cuart.array_right_sensor);
        #endif
        driving_interesting_situation = true;
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
        main_buzzer.startCrossingSound();
        move(DRIVE_SPEED_HALF, DRIVE_SPEED_HALF);
        #ifdef EXTENSIVE_DEBUG
        Serial.printf("[DRIVE_SENSOR_ARRAY] ==> bias_left: %s, bias_right: %s, bias_both: %s\r\n", driving_interesting_bias_left ? "T" : "F", driving_interesting_bias_right ? "T" : "F", driving_interesting_bias_both ? "T" : "F");
        #endif
    }

    // Line is only in the middle
    if (cuart.array_left_sensor < 2 && cuart.array_mid_sensor > 2 && cuart.array_right_sensor < 2)
    {
        #ifdef EXTENSIVE_DEBUG
        Serial.printf("[DRIVE_SENSOR_ARRAY] Line is only in the middle with: left_sensor: %d, mid_sensor: %d, right_sensor: %d\r\n", cuart.array_left_sensor, cuart.array_mid_sensor, cuart.array_right_sensor);
        #endif
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

    // // Driven past line, no center line
    // if (driving_interesting_situation && !cuart.sensor_array[0] && cuart.array_total < 4)
    // {
    //     #ifdef EXTENSIVE_DEBUG
    //     Serial.printf("[DRIVE_SENSOR_ARRAY] Driven past line, no center line with: left_sensor: %d, mid_sensor: %d, right_sensor: %d, array[0]: %s\r\n", cuart.array_left_sensor, cuart.array_mid_sensor, cuart.array_right_sensor, cuart.sensor_array[0] ? "T": "F");
    //     #endif

    //     if (driving_interesting_bias_left || driving_interesting_bias_right)
    //     {
    //         if (driving_interesting_bias_left)
    //         {
    //             delay(100);
    //             crossing_90_left();
    //         }
    //         else if (driving_interesting_bias_right)
    //         {
    //             delay(100);
    //             crossing_90_right();
    //         }

    //         driving_interesting_situation = false;
    //         driving_interesting_actual_ltype_override = false;
    //         driving_interesting_bias_both = false;
    //         driving_interesting_bias_left = false;
    //         driving_interesting_bias_right = false;
    //         move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
    //         main_buzzer.noTone();
    //         digitalWrite(LED_BUILTIN, LOW);
    //     }
    // }

    // Interesting situation passed
    // if ((
    //     (cuart.array_left_sensor < 4 && cuart.array_mid_sensor > 2 && cuart.array_right_sensor < 4) || 
    //     (cuart.array_left_sensor < 4 && cuart.array_mid_sensor < 4 && cuart.array_right_sensor > 2) ||
    //     (cuart.array_left_sensor > 2 && cuart.array_mid_sensor < 4 && cuart.array_right_sensor < 4) ||
    //     (cuart.array_left_sensor < 4 && cuart.array_mid_sensor < 4 && cuart.array_right_sensor < 4)
    // ) && cuart.sensor_array[0] && driving_interesting_situation)
    if (driving_interesting_situation && cuart.array_total <= 7 && cuart.sensor_array[0])
    {
        #ifdef EXTENSIVE_DEBUG
        Serial.printf("[DRIVE_SENSOR_ARRAY] Interesting Situation started with: left_sensor: %d, mid_sensor: %d, right_sensor: %d, array[0]: %s\r\n", cuart.array_left_sensor, cuart.array_mid_sensor, cuart.array_right_sensor, cuart.sensor_array[0] ? "T": "F");
        #endif

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
                #ifdef EXTENSIVE_DEBUG
                Serial.printf("[DRIVE_SENSOR_ARRAY] Overwritten with 90l\r\n");
                #endif
            }
            else if (driving_interesting_bias_right)
            {
                driving_interesting_actual_ltype = CUART_LTYPE_90r;
                driving_interesting_actual_ltype_override = true;
                #ifdef EXTENSIVE_DEBUG
                Serial.printf("[DRIVE_SENSOR_ARRAY] Overwritten with 90r\r\n");
                #endif
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
            #ifdef EXTENSIVE_DEBUG
            Serial.printf("[DRIVE_SENSOR_ARRAY] Green dot somewhere\r\n");
            #endif
            // Dead End: Turn Around
            if (/*(driving_interesting_actual_ltype == CUART_LTYPE_t || 
                    driving_interesting_actual_ltype == CUART_LTYPE_X) && */
                    (cuart.green_dots[2] && cuart.green_dots[3]))
            {
                Serial.printf("Interesting: Turning Around (Dead End) with ltype %d and dl green dot %s and dr green dot %s\r\n", driving_interesting_actual_ltype, cuart.green_dots[2] ? "True" : "False", cuart.green_dots[3] ? "True" : "False");
                move(DRIVE_SPEED_CORNER, -DRIVE_SPEED_CORNER);
                for (int x = 0; x < 1800; x += 50)
                {
                    display.tick();
                    delay(50);
                }
                // while(cuart.array_right_sensor < 2) {
                //     display.tick();
                //     vTaskDelay(watchdog_delay);
                // }
                // while(cuart.array_mid_sensor < 2) {
                //     display.tick();
                //     vTaskDelay(watchdog_delay);
                // }
                // while(cuart.array_left_sensor < 2) {
                //     display.tick();
                //     vTaskDelay(watchdog_delay);
                // }
                // while(cuart.array_right_sensor < 2) {
                //     display.tick();
                //     vTaskDelay(watchdog_delay);
                // }
                // while(cuart.array_mid_sensor < 2) {
                //     display.tick();
                //     vTaskDelay(watchdog_delay);
                // }
                // while(!cuart.sensor_array[10]) {
                //     display.tick();
                //     vTaskDelay(watchdog_delay);
                // }
                move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
            }
            // Turn left - green dot
            else if (/*(driving_interesting_actual_ltype == CUART_LTYPE_tl || 
                    driving_interesting_actual_ltype == CUART_LTYPE_t || 
                    driving_interesting_actual_ltype == CUART_LTYPE_X) && */cuart.green_dots[2])
            {
                Serial.printf("[DRIVE_SENSOR_ARRAY] Turning left with ltype %d and dl green dot %s\r\n", driving_interesting_actual_ltype, cuart.green_dots[2] ? "True" : "False");
                crossing_90_left();
                // delay(150);
            }
            // Turn right - green dot
            else if (/*(driving_interesting_actual_ltype == CUART_LTYPE_tr || 
                        driving_interesting_actual_ltype == CUART_LTYPE_t || 
                        driving_interesting_actual_ltype == CUART_LTYPE_X) && */cuart.green_dots[3])
            {
                Serial.printf("[DRIVE_SENSOR_ARRAY] Turning right with ltype %d and dr green dot %s\r\n", driving_interesting_actual_ltype, cuart.green_dots[3] ? "True" : "False");
                crossing_90_right();
                // delay(150);
            }
        }
        else 
        {
            #ifdef EXTENSIVE_DEBUG
            Serial.printf("[DRIVE_SENSOR_ARRAY] No green dot!\r\n");
            #endif
            // Turn left - 90 deg
            if (driving_interesting_actual_ltype == CUART_LTYPE_90l)
            {
                Serial.printf("[DRIVE_SENSOR_ARRAY] Turning left with ltype %d and dl green dot %s\r\n", driving_interesting_actual_ltype, cuart.green_dots[2] ? "True" : "False");
                crossing_90_left();
            }
            // Turn right - 90 deg
            else if (driving_interesting_actual_ltype == CUART_LTYPE_90r)
            {
                Serial.printf("[DRIVE_SENSOR_ARRAY] Turning right with ltype %d and dr green dot %s\r\n", driving_interesting_actual_ltype, cuart.green_dots[3] ? "True" : "False");
                crossing_90_right();
            }
            // Keep Straight
            else
            {
                Serial.printf("[DRIVE_SENSOR_ARRAY] Keeping Straight with ltype %d and dl green dot %s and dr green dot %s\r\n", driving_interesting_actual_ltype, cuart.green_dots[2] ? "True" : "False", cuart.green_dots[3] ? "True" : "False");
                move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
            }
        }

        driving_interesting_situation = false;
        driving_interesting_actual_ltype_override = false;
        driving_interesting_bias_both = false;
        driving_interesting_bias_left = false;
        driving_interesting_bias_right = false;
        move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
        main_buzzer.noTone();
        digitalWrite(LED_BUILTIN, LOW);

        #ifdef EXTENSIVE_DEBUG
        Serial.printf("[DRIVE_SENSOR_ARRAY] Interesting Situation Done! Resetted all vars to false: %s %s %s %s %s\r\n", driving_interesting_situation ? "T": "F", driving_interesting_actual_ltype_override ? "T": "F", driving_interesting_bias_both ? "T": "F", driving_interesting_bias_left ? "T": "F", driving_interesting_bias_right ? "T" : "F");
        #endif
    }

    if (cuart.array_total > 2)
    {
        #ifdef EXTENSIVE_DEBUG
        Serial.printf("[DRIVE_SENSOR_ARRAY] White Timer Reset!\r\n");
        #endif
        white_timer.reset();
    }

    if (white_timer.has_reached_target())
    {
        #ifdef EXTENSIVE_DEBUG
        Serial.printf("[DRIVE_SENSOR_ARRAY] Starting Findline!\r\n");
        #endif
        find_line();
    }

    if (taster.get_state(taster.front))
    {
        #ifdef EXTENSIVE_DEBUG
        Serial.printf("[DRIVE_SENSOR_ARRAY] Ausweichen!\r\n");
        #endif
        ausweichen();
    }
    
    if (DEBUG_MOTOR_VALUES == 1)
    {
        Serial.printf("L: %d, M: %d, R: %d, I: %s, Bias: L: %s, R: %s, B: %s\r\n", cuart.array_left_sensor, cuart.array_mid_sensor, cuart.array_right_sensor, driving_interesting_situation ? "T" : "F", driving_interesting_bias_left ? "T" : "F", driving_interesting_bias_right ? "T" : "F", driving_interesting_bias_both ? "T" : "F");
    }

    if (accel_sensor.getCurrentRampState() == accel_sensor.down || accel_sensor.getCurrentRampState() == accel_sensor.up)
    {
        driving_interesting_situation = false;
        driving_interesting_bias_both = false;
        driving_interesting_bias_left = false;
        driving_interesting_bias_right = false;
        move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
    }

    /*if (cuart.silver_line)
    {
        move(0, 0);
        for (int i = 0; i < 5000; i+=10)
        {
            display.tick();
            delay(10);
        }
        cuart.silver_line = false;
    }*/
    if (cuart.red_line)
    {
        move(0, 0);
        while(true)
        {
            display.tick();
            vTaskDelay(watchdog_delay);
        }
        cuart.red_line = false;
    }

    #ifdef EXTENSIVE_DEBUG
    Serial.printf("[DRIVE_SENSOR_ARRAY] END!\r\n");
    #endif
}

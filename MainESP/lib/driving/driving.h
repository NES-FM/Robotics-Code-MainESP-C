#ifndef DRIVING_H
#define DRIVING_H

#define DEBUG_MOTOR_VALUES 0

void drive(int speed_left, int speed_right)
{
    //display.draw_star();
    motor_left.move(speed_left);
    motor_right.move(speed_right);
}

bool driving_interesting_situation_locked = false; // If an interesting situation is found, lock it so it doesn't get deleted
bool driving_interesting_situation = false;
bool driving_interesting_bias_left = false;  // If it is suspected to be either Tl or 90l, discard all right situations
bool driving_interesting_bias_right = false; // ... and vice versa
bool driving_interesting_bias_both = false;  // ... or if the line is so wide, that only X or T is possible, discard 90lr and Tlr
int driving_interesting_actual_ltype = 0;

void drive_sensor_array()
{
    // Line is left...
    if (2 < CUART_array_left_sensor && CUART_array_left_sensor < 6 && CUART_array_right_sensor < 2)
    {
        // ... and not in the middle...
        if (CUART_array_mid_sensor <= 2)
        {
            // ... but not too far left
            if (CUART_sensor_array[5] == 0)
                drive(20,30);
            // ... and far left
            else
                drive(-10,20);
        }
    }

    // Line is right...
    if (2 < CUART_array_right_sensor && CUART_array_right_sensor < 6 && CUART_array_left_sensor < 2)
    {
        // ... and not in the middle...
        if (CUART_array_mid_sensor <= 2)
        {
            // ... but not too far right
            if (CUART_sensor_array[20] == 0)
                drive(30,20);
            // ... and far right
            else
                drive(20,-10);
        }
    }

    // Line is interesting (could be 90 degree, could be T, could be X, etc...)
    if ((CUART_array_left_sensor + CUART_array_mid_sensor + CUART_array_right_sensor) > 9 && !driving_interesting_situation_locked)
    {
        driving_interesting_situation = true;
        driving_interesting_bias_left = ((CUART_array_left_sensor + CUART_array_mid_sensor) > 6 && CUART_array_right_sensor < 2);
        driving_interesting_bias_right = (CUART_array_left_sensor < 2 && (CUART_array_mid_sensor + CUART_array_right_sensor) > 6);
        driving_interesting_bias_both = ((CUART_array_left_sensor + CUART_array_mid_sensor + CUART_array_right_sensor) > 16);
        driving_interesting_situation_locked = true;
    }

    // Line is only in the middle
    if (CUART_array_left_sensor < 2 && CUART_array_mid_sensor > 2 && CUART_array_right_sensor < 2)
    {
        drive(20, 20);
    }

    // Interesting situation passed
    if (CUART_array_left_sensor < 2 && CUART_array_right_sensor < 2 && CUART_sensor_array[0] && driving_interesting_situation_locked)
    {
        driving_interesting_actual_ltype = CUART_line_type;
        if (driving_interesting_bias_left)
        {
            if (CUART_line_type == CUART_LTYPE_90r)
                driving_interesting_actual_ltype = CUART_LTYPE_t;
            else if (CUART_line_type == CUART_LTYPE_tr)
                driving_interesting_actual_ltype = CUART_LTYPE_X;
            else if (CUART_line_type == CUART_LTYPE_STRAIGHT || CUART_line_type == CUART_LTYPE_UNKNOWN || CUART_line_type == CUART_LTYPE_SPACE)
                driving_interesting_actual_ltype = CUART_LTYPE_90l;
        }
        else if (driving_interesting_bias_right)
        {
            if (CUART_line_type == CUART_LTYPE_90l)
                driving_interesting_actual_ltype = CUART_LTYPE_t;
            else if (CUART_line_type == CUART_LTYPE_tl)
                driving_interesting_actual_ltype = CUART_LTYPE_X;
            else if (CUART_line_type == CUART_LTYPE_STRAIGHT || CUART_line_type == CUART_LTYPE_UNKNOWN || CUART_line_type == CUART_LTYPE_SPACE)
                driving_interesting_actual_ltype = CUART_LTYPE_90r;
        }
        else if (driving_interesting_bias_both)
        {
            if (CUART_line_type == CUART_LTYPE_90r || CUART_line_type == CUART_LTYPE_90l)
                driving_interesting_actual_ltype = CUART_LTYPE_t;
            else if (CUART_line_type == CUART_LTYPE_tr || CUART_line_type == CUART_LTYPE_tl)
                driving_interesting_actual_ltype = CUART_LTYPE_X;
            else if (CUART_line_type != CUART_LTYPE_t && CUART_line_type != CUART_LTYPE_X)
                driving_interesting_actual_ltype = CUART_LTYPE_X;
        }

        Serial.printf("Driving interesting situation with ltype: %d, Bias: L: %s, R: %s, B: %s\r\n", driving_interesting_actual_ltype, driving_interesting_bias_left ? "T" : "F", driving_interesting_bias_right ? "T" : "F", driving_interesting_bias_both ? "T" : "F");
        driving_interesting_situation_locked = false;
        driving_interesting_situation = false;
        driving_interesting_bias_left = false;
        driving_interesting_bias_right = false;
        driving_interesting_bias_both = false;

        // Actual driving

        // Turn left
        if ((driving_interesting_actual_ltype == CUART_LTYPE_90l) || 
              ((driving_interesting_actual_ltype == CUART_LTYPE_tl || 
                driving_interesting_actual_ltype == CUART_LTYPE_t || 
                driving_interesting_actual_ltype == CUART_LTYPE_X) && CUART_green_dots[2]))
        {
            drive(-20, 20);
            while(CUART_array_mid_sensor > 2) {}
            while(CUART_sensor_array[15]) {}
            delay(100);
            while(!CUART_sensor_array[15]) {}
            drive(20, 20);
        }
        // Turn right
        else if ((driving_interesting_actual_ltype == CUART_LTYPE_90r) || 
                  ((driving_interesting_actual_ltype == CUART_LTYPE_tr || 
                    driving_interesting_actual_ltype == CUART_LTYPE_t || 
                    driving_interesting_actual_ltype == CUART_LTYPE_X) && CUART_green_dots[3]))
        {
            drive(20, -20);
            while(CUART_array_mid_sensor > 2) {}
            while(CUART_sensor_array[10]) {}
            delay(100);
            while(!CUART_sensor_array[10]) {}
            drive(20, 20);
        }
        // Keep Straight
        else
        {
            drive(20, 20);
        }
    }
    
    if (DEBUG_MOTOR_VALUES == 1)
    {
        Serial.printf("L: %d, M: %d, R: %d, I: %s, Bias: L: %s, R: %s, B: %s\r\n", CUART_array_left_sensor, CUART_array_mid_sensor, CUART_array_right_sensor, driving_interesting_situation ? "T" : "F", driving_interesting_bias_left ? "T" : "F", driving_interesting_bias_right ? "T" : "F", driving_interesting_bias_both ? "T" : "F");
    }
}

#endif /* DRIVING_H */

#ifndef DRIVING_H
#define DRIVING_H

#define DEBUG_MOTOR_VALUES 1

void drive(int speed_left, int speed_right)
{
    //display.draw_star();
    motor_left.move(speed_left);
    motor_right.move(speed_right);
}

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
        // ... and wide (crossing)
        else
        {
            // Serial.printf("Crossing (initiated from left): L: %d M: %d R: %d\r\n", CUART_array_left_sensor, CUART_array_mid_sensor, CUART_array_right_sensor);
            // drive(20, 20);
            // while(CUART_array_mid_sensor > 2) {}
            // drive(-30, 30);
            // while(CUART_array_mid_sensor <= 2) {}
            // drive(20, 20);
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
        // ... and wide (crossing)
        else
        {
            Serial.printf("Crossing (initiated from right): L: %d M: %d R: %d\r\n", CUART_array_left_sensor, CUART_array_mid_sensor, CUART_array_right_sensor);
            drive(20, 20);
            while(CUART_array_mid_sensor > 2) {}
            drive(30, -30);
            while(CUART_array_mid_sensor <= 2) {}
            drive(20, 20);
        }
    }

    // Line is only in the middle
    if (CUART_array_left_sensor < 2 && CUART_array_mid_sensor > 2 && CUART_array_right_sensor < 2)
    {
        drive(20, 20);
    }
    
    if (DEBUG_MOTOR_VALUES == 1)
    {
        Serial.printf("L: %d, M: %d, R: %d\r\n", CUART_array_left_sensor, CUART_array_mid_sensor, CUART_array_right_sensor);
    }
}

#endif /* DRIVING_H */

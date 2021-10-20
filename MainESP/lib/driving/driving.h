#ifndef DRIVING_H
#define DRIVING_H

void drive(int speed_left, int speed_right)
{
    motor_left.move(speed_left);
    motor_right.move(speed_right);
}

void drive_sensor_array()
{
    // (0 1 2) 3 4 5 6 7 8 9 | 10 11 12 13 14 15 | 16 17 18 19 20 21 22 (23)
    // (0 0 0) 0 0 0 0 0 0 0 |  0 1  1  1  1  0  | 0 0 0 0 0 0 0 (0)
    int left_sensor = 0, mid_sensor = 0, right_sensor = 0;
    for (int i = 0; i < 24; i++)
    {
        if (3 <= i && i <= 9)
            left_sensor += CUART_sensor_array[i];
        else if (10 <= i && i <= 15)
        	mid_sensor += CUART_sensor_array[i];
        else if (16 <= i && i <= 22)
            right_sensor += CUART_sensor_array[i];
        
    }

    // Line is left...
    if (2 < left_sensor && left_sensor < 6 && right_sensor < 2)
    {
        // ... and not in the middle...
        if (mid_sensor < 2)
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
            Serial.printf("Crossing (initiated from left): L: %d M: %d R: %d\r\n", left_sensor, mid_sensor, right_sensor);
        }
    }

    // Line is right...
    if (2 < right_sensor && right_sensor < 6 && left_sensor < 2)
    {
        // ... and not in the middle...
        if (mid_sensor < 2)
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
            Serial.printf("Crossing (initiated from right): L: %d M: %d R: %d\r\n", left_sensor, mid_sensor, right_sensor);
        }
    }

    // Line is only in the middle
    if (left_sensor < 2 && mid_sensor > 2 && right_sensor < 2)
    {
        drive(20, 20);
    }
}

#endif /* DRIVING_H */

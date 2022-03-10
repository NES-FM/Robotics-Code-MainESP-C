bool trust_cuart = false;
bool array_has_pixels = false;
int using_ltype = 0;

void wait_until(bool condition)
{
    while (!condition)
    {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
}

void turn_90_left()
{
    move(-DRIVE_SPEED_NORMAL/*-2*/, DRIVE_SPEED_NORMAL);

    // while(!cuart.sensor_array[6]) {
    //     vTaskDelay(watchdog_delay);
    // }
    wait_until(cuart.sensor_array[6]);

    // while(cuart.sensor_array[7]) {
    //     vTaskDelay(watchdog_delay);
    // }
    wait_until(!cuart.sensor_array[7]);

    // while(cuart.sensor_array[15]) {
    //     vTaskDelay(watchdog_delay);
    // }
    wait_until(!cuart.sensor_array[15]);

    // while(!cuart.sensor_array[14]) {
    //     vTaskDelay(watchdog_delay);
    // }
    wait_until(cuart.sensor_array[14]);

    move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
}

void turn_90_right()
{
    move(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL/*-2*/);

    // while(!cuart.sensor_array[19]) {
    //     vTaskDelay(watchdog_delay);
    // }
    wait_until(cuart.sensor_array[19]);

    // while(cuart.sensor_array[18]) {
    //     vTaskDelay(watchdog_delay);
    // }
    wait_until(!cuart.sensor_array[18]);

    // while(cuart.sensor_array[10]) {
    //     vTaskDelay(watchdog_delay);
    // }
    wait_until(!cuart.sensor_array[10]);

    // while(!cuart.sensor_array[11]) {
    //     vTaskDelay(watchdog_delay);
    // }
    wait_until(cuart.sensor_array[11]);

    move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
}

void turn_deadend()
{
    move(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    // while(cuart.array_right_sensor < 2) {
    //     vTaskDelay(watchdog_delay);
    // }
    wait_until(cuart.array_right_sensor >= 2);

    // while(cuart.array_mid_sensor < 2) {
    //     vTaskDelay(watchdog_delay);
    // }
    wait_until(cuart.array_mid_sensor >= 2);

    // while(cuart.array_left_sensor < 2) {
    //     vTaskDelay(watchdog_delay);
    // }
    wait_until(cuart.array_left_sensor >= 2);

    // while(cuart.array_right_sensor < 2) {
    //     vTaskDelay(watchdog_delay);
    // }
    wait_until(cuart.array_right_sensor >= 2);

    // while(cuart.array_mid_sensor < 2) {
    //     vTaskDelay(watchdog_delay);
    // }
    wait_until(cuart.array_mid_sensor >= 2);

    // while(!cuart.sensor_array[10]) {
    //     vTaskDelay(watchdog_delay);
    // }
    wait_until(cuart.sensor_array[10]);
    
    move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
}

void drive_new()
{
    trust_cuart = false;

    // Some sort of crossing
    if (cuart.array_total > 7)
    {
        // 90l or tr
        if (cuart.array_left_sensor > 2 && cuart.array_mid_sensor > 2 && cuart.array_right_sensor <= 2)
        {
            
            move(DRIVE_SPEED_LOW, DRIVE_SPEED_LOW);
            
            // while (cuart.array_left_sensor > 2 || cuart.array_right_sensor > 2) 
            // { 
            //     vTaskDelay(watchdog_delay); 
            //     if (cuart.array_left_sensor > 2 && cuart.array_mid_sensor > 2 && cuart.array_right_sensor > 2)
            //     {
            //         goto crossing;
            //     }
            // }
            wait_until(cuart.array_left_sensor <= 2 && cuart.array_right_sensor <= 2);
            array_has_pixels = (cuart.array_total > 2);
            
            if (!array_has_pixels)
            {
                turn_90_left(); 
                return;
            }
            else // if (array_has_pixels)
            {
                // while (!cuart.sensor_array[0]) { vTaskDelay(watchdog_delay); }
                wait_until(cuart.sensor_array[0]);
                if (cuart.green_dots[2])
                {
                    turn_90_left();
                    return;
                }
            }

            move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
        }

        // 90r or tr
        if (cuart.array_left_sensor <= 2 && cuart.array_mid_sensor > 2 && cuart.array_right_sensor > 2)
        {
            
            move(DRIVE_SPEED_LOW, DRIVE_SPEED_LOW);
            
            // while (cuart.array_left_sensor > 2 || cuart.array_right_sensor > 2) 
            // { 
            //     vTaskDelay(watchdog_delay); 
            //     if (cuart.array_left_sensor > 2 && cuart.array_mid_sensor > 2 && cuart.array_right_sensor > 2)
            //     {
            //         goto crossing;
            //     }
            // }
            wait_until(cuart.array_left_sensor <= 2 && cuart.array_right_sensor <= 2);
            array_has_pixels = (cuart.array_total > 2);
            
            if (!array_has_pixels)
            {
                turn_90_right(); 
                return;
            }
            else // if (array_has_pixels)
            {
                // while (!cuart.sensor_array[0]) { vTaskDelay(watchdog_delay); }
                wait_until(cuart.sensor_array[0]);
                if (cuart.green_dots[3])
                {
                    turn_90_right();
                    return;
                }
            }

            move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
        }

        // T or X
        if (cuart.array_left_sensor > 2 && cuart.array_mid_sensor > 2 && cuart.array_right_sensor > 2)
        {
            crossing: ; // Label crossing

            move(DRIVE_SPEED_LOW, DRIVE_SPEED_LOW);
            
            // while (cuart.array_left_sensor > 2 || cuart.array_right_sensor > 2) 
            // { 
            //     vTaskDelay(watchdog_delay); 
            // }
            wait_until(cuart.array_left_sensor <= 2 && cuart.array_right_sensor <= 2);

            array_has_pixels = (cuart.array_total > 2);

            // while (!cuart.sensor_array[0]) { vTaskDelay(watchdog_delay); }
            wait_until(cuart.sensor_array[0]);
            
            if (cuart.green_dots[2] && !cuart.green_dots[3])
            {
                turn_90_left();
                return;
            }
            else if (!cuart.green_dots[2] && cuart.green_dots[3])
            {
                turn_90_right();
                return;
            }
            else if (cuart.green_dots[2] && cuart.green_dots[3])
            {
                turn_deadend();
                return;
            }

            move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
        }
    }
    else // if (total_number_pixels <= 7)
    {
        // Line is left... (copy pasted from previous)
        if (cuart.array_left_sensor > 2)
        {
            if ((cuart.sensor_array[3] == true || cuart.sensor_array[4] == true) && cuart.array_left_sensor > 3)
                move(-DRIVE_SPEED_HIGH, DRIVE_SPEED_NORMAL);
            else if (cuart.sensor_array[5] == true && cuart.array_left_sensor > 3)
                move(-DRIVE_SPEED_LOWER, DRIVE_SPEED_HIGH);
            else if (cuart.sensor_array[6] == true && cuart.array_left_sensor > 3)
                move(-DRIVE_SPEED_LOW, DRIVE_SPEED_HIGH);
            else if (cuart.sensor_array[7] == true)
                move(0, DRIVE_SPEED_HIGHER);
            else if (cuart.sensor_array[8] == true)
                move(DRIVE_SPEED_LOWER, DRIVE_SPEED_HIGH); 
            else if (cuart.sensor_array[9] == true)
                move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_HIGH);
        }

        // Line is right... (copy pasted from previous)
        if (cuart.array_right_sensor > 2)
        {
            if ((cuart.sensor_array[22] == true || cuart.sensor_array[21] == true) && cuart.array_right_sensor > 3)
                move(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_HIGH);
            else if (cuart.sensor_array[20] == true && cuart.array_right_sensor > 3)
                move(DRIVE_SPEED_HIGH, -DRIVE_SPEED_LOWER);
            else if (cuart.sensor_array[19] == true && cuart.array_right_sensor > 3)
                move(DRIVE_SPEED_HIGH, -DRIVE_SPEED_LOW);
            else if (cuart.sensor_array[18] == true)
                move(DRIVE_SPEED_HIGHER, 0);
            else if (cuart.sensor_array[17] == true)
                move(DRIVE_SPEED_HIGH, DRIVE_SPEED_LOWER); 
            else if (cuart.sensor_array[16] == true)
                move(DRIVE_SPEED_HIGH, DRIVE_SPEED_NORMAL);
        }

        // Line is only in the middle (copy pasted from previous)
        if (cuart.array_left_sensor < 2 && cuart.array_mid_sensor > 2 && cuart.array_right_sensor < 2)
        {
            move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
        }
    }
}

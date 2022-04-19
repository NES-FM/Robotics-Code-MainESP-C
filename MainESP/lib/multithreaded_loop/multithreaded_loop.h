#ifndef MULTITHREADED_LOOP_H
#define MULTITHREADED_LOOP_H

TaskHandle_t multithreaded_loop_handle;

#define DRIVE_IN_MAIN
// #define DRIVE_IN_THREAD

// Secondary loop running on core 0
void multithreaded_loop(void* parameters) { 
    Serial.print("multithreaded_loop running on core ");
    Serial.println(xPortGetCoreID());
    while(true)
    {
        // #ifdef DRIVE_IN_THREAD
        // display.tick();

        // // cuart.debugPrintArray();

        // if (motor_left.is_enabled() && motor_right.is_enabled())
        // {
        //     drive();
        // }

        // // delay(10);

        // // cuart.debugPrint();
        // #endif

        #ifdef DRIVE_IN_MAIN
        cuart.tick();
        // accel_sensor.print_values();
        // compass.tick();
        // display.tick();
        #endif
    }
}

// Main loop running on core 1
void main_loop()
{
    // #ifdef DRIVE_IN_THREAD
    // cuart.tick();
    // compass.tick();
    // #endif

    #ifdef DRIVE_IN_MAIN
    display.tick();

    if (motor_left.is_enabled() && motor_right.is_enabled())
    {
        drive();
    }
    #endif

    // #ifdef OTA_BUILD
    // ota.tick();
    // #endif

    // compass.tick();
    
    // delay(10);
}

void init_multithreaded_loop()
{
    // Secondary loop on low power core
    xTaskCreatePinnedToCore(multithreaded_loop, /* Function to implement the task */
                            "Multithreaded Loop", /* Name of the task */
                            10000, /* Stack size in words */
                            NULL, /* Task input parameter */
                            0, /* Priority of the task */
                            &multithreaded_loop_handle, /* Task handle. */
                            0); /* Core where the task should run */
    Serial.print("main loop running on core ");
    Serial.println(xPortGetCoreID());
}

#endif /* MULTITHREADED_LOOP_H */

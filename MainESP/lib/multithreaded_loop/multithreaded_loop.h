#ifndef MULTITHREADED_LOOP_H
#define MULTITHREADED_LOOP_H

TaskHandle_t multithreaded_loop_handle;

// Secondary loop running on core 0
void multithreaded_loop(void* parameters) { 
    Serial.print("multithreaded_loop running on core ");
    Serial.println(xPortGetCoreID());
    while(true)
    {
        display.tick();

        // cuart.debugPrintArray();

        if (motor_left.is_enabled() && motor_right.is_enabled())
            drive_new();

        // delay(10);

        // cuart.debugPrint();
    }
}

// Main loop running on core 1
void main_loop()
{
    cuart.tick();
    // display.tick();
    // ota.tick();
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

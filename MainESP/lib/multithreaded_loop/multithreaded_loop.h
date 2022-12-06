#ifndef MULTITHREADED_LOOP_H
#define MULTITHREADED_LOOP_H

TaskHandle_t multithreaded_loop_handle;

// Secondary loop running on core 0
void multithreaded_loop(void* parameters) { 
    logln("multithreaded_loop running on core %u", xPortGetCoreID());
    while(true)
    {
        cuart.tick();
        bcuart.tick();
    }
}

// Main loop running on core 1
void main_loop()
{
    display.tick();

    if (robot.motor_left->is_enabled() && robot.motor_right->is_enabled())
    {
        drive();
    }
    
    robot.tick();
    
    // delay(10);
}

void init_multithreaded_loop()
{
    logln("init_multithreaded_loop");
    // Secondary loop on low power core
    xTaskCreatePinnedToCore(multithreaded_loop, /* Function to implement the task */
                            "Multithreaded Loop", /* Name of the task */
                            10000, /* Stack size in words */
                            NULL, /* Task input parameter */
                            0, /* Priority of the task */
                            &multithreaded_loop_handle, /* Task handle. */
                            0); /* Core where the task should run */
    logln("main loop running on core %u", xPortGetCoreID());
}

#endif /* MULTITHREADED_LOOP_H */

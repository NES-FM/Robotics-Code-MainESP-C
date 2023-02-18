#ifndef MULTITHREADED_LOOP_H
#define MULTITHREADED_LOOP_H

TaskHandle_t multithreaded_loop_handle;

// Secondary loop running on core 0
void multithreaded_loop(void* parameters) { 
    logln("multithreaded_loop running on core %u", xPortGetCoreID());
    while(true)
    {
        #ifdef TIMEIT
        uint32_t start_time = millis();
        #endif

        cuart.tick();

        #ifdef TIMEIT
        logln("Secondary Core: cuart.tick() took %dms", millis()-start_time);
        start_time = millis();
        #endif

        bcuart.tick();

        #ifdef TIMEIT
        logln("Secondary Core: bcuart.tick() took %dms", millis()-start_time);
        start_time = millis();
        #endif
    }
}

// Main loop running on core 1
void main_loop()
{
    #ifdef TIMEIT
    uint32_t start_time = millis();
    #endif

    display.tick();

    #ifdef TIMEIT
    logln("Primary Core: display.tick() took %dms", millis()-start_time);
    start_time = millis();
    #endif

    if (robot.motor_left->is_enabled() && robot.motor_right->is_enabled())
    {
        drive();
    }

    #ifdef TIMEIT
    logln("Primary Core: drive() took %dms", millis()-start_time);
    start_time = millis();
    #endif
    
    robot.tick();

    #ifdef TIMEIT
    logln("Primary Core: robot.tick() took %dms", millis()-start_time);
    start_time = millis();
    #endif
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

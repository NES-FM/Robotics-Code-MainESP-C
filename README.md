# Main ESP (C-Version)

This is the C-Version of [this](https://github.com/NES-FM/Robotics-Code-MainESP), which is written in MicroPython, but is really unreliably so the switch to C / PlatformIO.

Project-Structure:
```
blink  # Default Arduino Blink programm to test

MainESP  # Main Program for the Main ESP
    |-include  
    |   -> ...  # Files with defines, that need to be accessed from more than one location
    |-lib
    |   -> ...  # Every Sensor gets its own class
    |   -> multithreaded_loop  # 2nd thread for all the sensor stuff (running on core 0)
    |   -> cuart  # Communication with our [Camera](https://github.com/NES-FM/Robotics-Code-ESPCam)
    |   -> driving  # Our driving script. The function gets called every tick
    |-src
        -> main.cpp  # Main Script
```


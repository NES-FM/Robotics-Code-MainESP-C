@echo off

java -jar EspStackTraceDecoder.jar "C:\Users\flori\AppData\Local\Arduino15\packages\esp32\tools\xtensa-esp32-elf-gcc\1.22.0-97-gc752ad5-5.2.0\bin\xtensa-esp32-elf-addr2line.exe" "D:\Develop\Robotics\Robotics-Code-MainESP-C\MainESP\.pio\build\wemos_d1_mini32\firmware.elf" "stackdump.txt"

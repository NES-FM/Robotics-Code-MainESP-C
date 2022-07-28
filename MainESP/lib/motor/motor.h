#ifndef MOTOR_H
#define MOTOR_H

#define comm_v2

#include <Wire.h>
#include "../../include/pin_definitions.h"

#ifndef comm_v2
#include "../../include/motor_directions.h"
#endif

#include "../../include/i2c_addresses.h"
#include <HardwareSerial.h>

#include "logger.h"

class motor 
{
    public:
        motor();
        void init(char mnum);
        void move(int speed);
        void move_steps(int speed, int steps);

        #ifndef comm_v2
        void move_direction(int speed, int direction);
        #endif

        void stop() { this->move(0); }
        void off();
        void enable(bool enabled);
        bool is_enabled() { return _motor_i2c_enabled; }
        void force_resend();
        int motor_speed = 0;

        #define stop_type_stop 0
        #define stop_type_off 1

        #define move_direction_forward 0
        #define move_direction_backward 1

    private:
        #ifndef comm_v2
        const char _i2c_set_speed = 0x00;
        const char _i2c_set_direc = 0x01;
        const char _i2c_offset_motor_1 = 0x30;
        const char _i2c_offset_motor_2 = 0x35;

        char current_i2c_offset = 0;

        char current_direc = MOTOR_DIREC_STOP;

        void writeto_mem(char addr, char reg, char data);
        #endif

        int current_speed = -1;

        unsigned char _i2c_address = I2C_ADDRESSS_MOTOR_CONTROLLER;

        bool _motor_i2c_enabled = false;

        signed char motor_num = -1;

        #ifdef comm_v2

        struct stop_command {
            uint8_t motor_num;        // + 1 byte
            uint8_t type;             // + 1 byte
                                      // = 2 bytes
        };

        struct drive_command {
            uint8_t motor_num;        // + 1 byte
            uint8_t speed;            // + 1 byte
            uint8_t direction;        // + 1 byte
                                      // = 3 bytes
        };

        struct move_steps_command {
            uint8_t motor_num;        // + 1 byte
            uint8_t speed;            // + 1 byte
            uint8_t direction;        // + 1 byte
            uint16_t steps;           // + 3 byte
                                      // = 6 bytes
        };

        
        #endif
};

#endif /* MOTOR_H */

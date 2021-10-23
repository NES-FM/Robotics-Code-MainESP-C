#include "motor.h"

motor::motor() {};

void motor::writeto_mem(char addr, char reg, char data)
{
    if (_motor_i2c_enabled == true) 
    {
        Wire.beginTransmission(addr);
        Wire.write(reg);
        Wire.write(data);
        Wire.endTransmission();
    }
}

void motor::init(char mnum) 
{
    

    motor_num = mnum;
    if (motor_num == 1)
        current_i2c_offset = _i2c_offset_motor_1;
    else if (motor_num == 2)
        current_i2c_offset = _i2c_offset_motor_2;

    writeto_mem(_i2c_address, current_i2c_offset + _i2c_set_speed, 0x00);
    writeto_mem(_i2c_address, current_i2c_offset + _i2c_set_direc, MOTOR_DIREC_STOP);

}

void motor::move(int speed)
{
    int direction = 0;
    if (speed < 0)
        direction = MOTOR_DIREC_BACK;
    else if (speed == 0)
        direction = MOTOR_DIREC_OFF;
    else if (speed > 0)
        direction = MOTOR_DIREC_FORWARD;
    
    speed = abs(speed);

    if (current_direc != direction)
        writeto_mem(_i2c_address, current_i2c_offset + _i2c_set_direc, direction);
    if (current_speed != speed)
        writeto_mem(_i2c_address, current_i2c_offset + _i2c_set_speed, speed);

    current_direc = direction;
    current_speed = speed;
}

void motor::move_direction(int speed, int direction)
{
    if (direction == MOTOR_DIREC_STOP)
        speed = 255;
    else if (direction == MOTOR_DIREC_OFF)
        speed = 0;
    
    speed = abs(speed);

    if (current_direc != direction)
        writeto_mem(_i2c_address, current_i2c_offset + _i2c_set_direc, direction);
    if (current_speed != speed)
        writeto_mem(_i2c_address, current_i2c_offset + _i2c_set_speed, speed);

    current_direc = direction;
    current_speed = speed;
}

void motor::stop() { this->move_direction(0, MOTOR_DIREC_STOP); }

void motor::off() { this->move_direction(0, MOTOR_DIREC_OFF); }

void motor::enable(bool enabled) 
{
    _motor_i2c_enabled = enabled;
}

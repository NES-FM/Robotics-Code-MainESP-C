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
    
    this->move_direction(speed, direction);
}

void motor::move_direction(int speed, int direction)
{
    if (direction == MOTOR_DIREC_STOP)
        speed = 0;
    else if (direction == MOTOR_DIREC_OFF)
        speed = 0;
    
    speed = abs(speed);

    if (current_direc != direction)
        writeto_mem(_i2c_address, current_i2c_offset + _i2c_set_direc, direction);
    if (current_speed != speed)
        writeto_mem(_i2c_address, current_i2c_offset + _i2c_set_speed, speed);

    current_direc = direction;
    current_speed = speed;

    // Calculating the speed to a nice value for the display (includes negative values etc.)
    if (direction == MOTOR_DIREC_BACK)
        speed *= -1;
    motor_speed = speed;
}

void motor::stop() { this->move_direction(0, MOTOR_DIREC_STOP); }

void motor::off() { this->move_direction(0, MOTOR_DIREC_OFF); }

void motor::enable(bool enabled) 
{
    _motor_i2c_enabled = enabled;
}

void motor::force_resend()
{
    // By changing the current direction / speed forcing the resend of motor values

    char save_current_speed = current_speed;
    char save_current_direction = current_direc;

    current_speed = current_speed + 1;
    current_direc = current_direc + 1;

    move_direction(save_current_speed, save_current_direction);
}

#include "motor.h"

motor::motor() {};

void motor::enable(bool enabled) 
{
    _motor_i2c_enabled = enabled;
}

#ifdef comm_v1
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

    if (current_speed != speed)
        writeto_mem(_i2c_address, current_i2c_offset + _i2c_set_speed, speed);
    if (current_direc != direction)
        writeto_mem(_i2c_address, current_i2c_offset + _i2c_set_direc, direction);

    current_direc = direction;
    current_speed = speed;

    // Calculating the speed to a nice value for the display (includes negative values etc.)
    if (direction == MOTOR_DIREC_BACK)
        speed *= -1;
    motor_speed = speed;
}

void motor::stop() { this->move_direction(0, MOTOR_DIREC_STOP); }

void motor::off() { this->move_direction(0, MOTOR_DIREC_OFF); }

void motor::force_resend()
{
    // By changing the current direction / speed forcing the resend of motor values

    char save_current_speed = current_speed;
    char save_current_direction = current_direc;

    current_speed = current_speed + 1;
    current_direc = current_direc + 1;

    move_direction(save_current_speed, save_current_direction);
}
#endif

#if defined(comm_v2) || defined(comm_v3)
void motor::init(char mnum) 
{
    logln("init motor %d", mnum);
    motor_num = mnum;
    move(0);
}

void motor::force_resend()
{
    // By changing the current direction / speed forcing the resend of motor values

    char save_current_speed = current_speed;

    current_speed = current_speed + 1;

    move(save_current_speed);
}
#endif

#ifdef comm_v2
void motor::move(int speed)
{
    #ifdef EXTENSIVE_DEBUG
    logln("move motor %d with speed %d", motor_num, speed);
    #endif
    if (speed != current_speed)
    {
        current_speed = speed;

        if (speed == 0)
        {
            stop_command stop_send;
            stop_send.motor_num = motor_num;
            stop_send.type = stop_type_stop;

            Wire.beginTransmission(_i2c_address);
            Wire.write((unsigned char*) &stop_send, sizeof(stop_send));
            Wire.endTransmission();
        }
        else
        {
            uint8_t direction = 0;
            if (speed < 0)
                direction = move_direction_backward;
            else if (speed > 0)
                direction = move_direction_forward;

            drive_command drive_send;
            drive_send.motor_num = motor_num;
            drive_send.direction = direction;
            drive_send.speed = (uint8_t) abs(speed);

            Wire.beginTransmission(_i2c_address);
            Wire.write((unsigned char*) &drive_send, sizeof(drive_send));
            Wire.endTransmission();
        }
    }
    motor_speed = speed;
}

void motor::move_steps(int speed, int steps)
{
    uint8_t direction = 0;
    if (speed < 0)
        direction = move_direction_backward;
    else if (speed > 0)
        direction = move_direction_forward;

    move_steps_command steps_send;
    steps_send.motor_num = motor_num;
    steps_send.direction = direction;
    steps_send.speed = (uint8_t) abs(speed);
    steps_send.steps = (uint16_t) abs(steps);

    logln("sending m_num: %d, dir: %d, speed: %d, steps: %d", motor_num, direction, abs(speed), abs(steps));

    Wire.beginTransmission(_i2c_address);
    Wire.write((unsigned char*) &steps_send, sizeof(steps_send));
    Wire.endTransmission();
}

void motor::off() 
{ 
    stop_command out;
    out.motor_num = motor_num;
    out.type = stop_type_off;

    Wire.beginTransmission(_i2c_address);
    Wire.write((unsigned char*) &out, sizeof(out));
    Wire.endTransmission();
}
#endif

#ifdef comm_v3
void motor::move(int speed)
{
    #ifdef EXTENSIVE_DEBUG
    logln("move motor %d with speed %d", motor_num, speed);
    #endif
    if (speed != current_speed)
    {
        current_speed = speed;

        uint8_t send_byte = 0; // Byte where all information will be stored
        send_byte |= (motor_num == 2) << 7; // Setting First Bit to be the motor_num
        
        if (abs(speed) > 63)
            logln("WARNING! Exceeding Speed Maximum of 63!");

        if (speed == 0)
        {
            send_byte |= stop_type_stop << 6; // Setting Second Bit to be the Stop Type
        }
        else
        {
            if (speed < 0)  // Setting Second Bit to be the Move Direction
                send_byte |= move_direction_backward << 6;
            else if (speed > 0)
                send_byte |= move_direction_forward << 6;
        }

        send_byte |= constrain(abs(speed), 0, 63);  // Setting Rest to be the absolute Speed

        Wire.beginTransmission(_i2c_address);
        Wire.write(send_byte);
        Wire.endTransmission();
    }
    motor_speed = speed;
}

void motor::off() 
{ 
    uint8_t send_byte = 0; // Byte where all information will be stored
    send_byte |= (motor_num == 2) << 7; // Setting First Bit to be the motor_num
    send_byte |= stop_type_off << 6; // Setting Second Bit to be the Stop Type

    Wire.beginTransmission(_i2c_address);
    Wire.write(send_byte);
    Wire.endTransmission();
}
#endif

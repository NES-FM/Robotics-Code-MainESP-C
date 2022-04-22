#include "servo_angles.h"

/*
 ______ ______ ______ ______
|UL11     10     9      8 UR|
|12                        7|
-                           -
|13                        6|
|                           |
-                           -
|14                        5|
|LL 1     2      3      4 LR|
 ------ ------ ------ ------

*/

#define ECKE_NONE 0
#define ECKE_LR 1
#define ECKE_UR 2
#define ECKE_UL 3
#define ECKE_LL 4

#define TURN_90_DEG_DELAY 500

int ecke = ECKE_NONE;
int ecke_possibility = ECKE_NONE;

int hole = ECKE_NONE;

void cust_delay(int millis)
{
    for (int i = 0; i < millis; i+= 10)
    {
        display.tick();
        delay(10);
    }
}

// returns true if corner found at pos
bool check(int pos)
{
    move(DRIVE_SPEED_RAUM, DRIVE_SPEED_RAUM);
    cuart.silver_line = false;
    cuart.green_line = false;
    while(!taster.get_state(taster.front_right) && !cuart.silver_line && !cuart.green_line)
    {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    if (cuart.silver_line || cuart.green_line)
    {
        hole = pos;
        move(-DRIVE_SPEED_RAUM, -DRIVE_SPEED_RAUM);
        cust_delay(500);
        return false;
    }
    cust_delay(200);
    if (!taster.get_state(taster.front_left))
    {
        ecke = pos;
        return true;
    }
    return false;
}

void be_straight()
{
    move(0, -DRIVE_SPEED_NORMAL-5);
    cust_delay(300);
    move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    cust_delay(200);
    move(-DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
    cust_delay(TURN_90_DEG_DELAY+160);
    move(DRIVE_SPEED_RAUM, DRIVE_SPEED_RAUM-5);
}

void put_away()
{
    move(DRIVE_SPEED_NORMAL, 0);
    while(!taster.get_state(taster.front_left))
    {
        display.tick();
        vTaskDelay(watchdog_delay);
    }
    move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    cust_delay(500);
    move(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    cust_delay(2* TURN_90_DEG_DELAY);
    move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    cust_delay(2000);
    move(0, 0);
    greifer_up.write(ANGLE_GREIFER_MID);
    cust_delay(500);
    greifer_zu.write(ANGLE_GREIFER_OPEN);
    cust_delay(500);
    greifer_zu.write(ANGLE_GREIFER_CLOSE);
    greifer_up.write(ANGLE_GREIFER_UP);

    // if(hole != 0)
    // {

    // }
}

void move_along_wall()
{
    if (check(ECKE_LR))
    {
        put_away();
    }
    else
    {
        be_straight();
        if (check(ECKE_UR))
        {
            put_away();
        }
        else
        {
            be_straight();
            if (check(ECKE_UL))
            {
                put_away();
            }
            else
            {
                be_straight();
                if (check(ECKE_LL))
                {
                    put_away();
                }
                else
                {
                    be_straight();
                    move_along_wall();
                }
            }
        }
    }
}

void drive_raum()
{
    move(-DRIVE_SPEED_RAUM, -DRIVE_SPEED_RAUM);
    cust_delay(300);
    move(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    cust_delay(TURN_90_DEG_DELAY);
    move_along_wall();
    move(0, 0);
    cust_delay(10000);
}

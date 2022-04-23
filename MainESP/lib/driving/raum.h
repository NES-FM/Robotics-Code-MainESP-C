#include "servo_angles.h"
#include "timer.h"

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

int count_of_balls = 0;

bool move_out_of_hole_now = false;

int raum_entry_angle = 0;

void cust_delay(int millis)
{
    for (int i = 0; i < millis; i+= 10)
    {
        display.tick();
        delay(10);
    }
}

bool block_detect_exit = false;

bool driving_to_ecke = false;

// returns true if corner found at pos
bool check(int pos)
{
    block_detect_exit = false;
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
        cuart.silver_line = false;
        cuart.green_line = false;
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

void turn_90_while_next_to_wall()
{
    move(0, -DRIVE_SPEED_NORMAL-5);
    cust_delay(300);
    move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    cust_delay(200);
    move(-DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
    cust_delay(TURN_90_DEG_DELAY+160);
    move(DRIVE_SPEED_RAUM, DRIVE_SPEED_RAUM);
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

    count_of_balls++;
    preferences.putInt("balls", count_of_balls);

    if (count_of_balls == 1)
    {
        move_out_of_hole_now = true;
    }
}

void move_along_wall()
{
    if (check(ECKE_LR))
    {
        if (count_of_balls == 0)
            put_away();
    }
    else
    {
        turn_90_while_next_to_wall();
        if (check(ECKE_UR))
        {
            if (count_of_balls == 0)
                put_away();
        }
        else
        {
            turn_90_while_next_to_wall();
            if (check(ECKE_UL))
            {
                if (count_of_balls == 0)
                    put_away();
            }
            else
            {
                turn_90_while_next_to_wall();
                if (check(ECKE_LL))
                {
                    if (count_of_balls == 0)
                        put_away();
                }
                else
                {
                    turn_90_while_next_to_wall();
                    move_along_wall();
                }
            }
        }
    }
}

timer disable_IR_timer(2000);

void move_along_wall_find_exit()
{
    move(DRIVE_SPEED_RAUM, DRIVE_SPEED_RAUM);
    IR_R.change_between_last_time();
    while(!taster.get_state(taster.front_right) && !cuart.silver_line && !cuart.green_line)
    {
        display.tick();        
        vTaskDelay(pdMS_TO_TICKS(50));
/*
        if (IR_R.get_cm() > 45)
        {
            // Maybe hole
            cust_delay(50);
            move(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
            cust_delay(TURN_90_DEG_DELAY);
            cuart.silver_line = false;
            cuart.green_line = false;
            move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
            while(!taster.get_state(taster.front_right) && !cuart.silver_line && !cuart.green_line)
            {
                display.tick();
                vTaskDelay(watchdog_delay);
            }
            if (cuart.green_line)
            {
                display.raum_mode = false;
                in_raum = false;
                return;
            }
            if (cuart.silver_line)
            {
                move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
                cust_delay(200);
            }
            move(-DRIVE_SPEED_RAUM, -DRIVE_SPEED_RAUM);
            cust_delay(500);
            move(-DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
            cust_delay(TURN_90_DEG_DELAY);
        }
        else if (IR_R.get_cm() < 2)
        {
            move(DRIVE_SPEED_RAUM-5, DRIVE_SPEED_RAUM+5);
        }
        else if (IR_R.get_cm() > 2)
        {
            move(DRIVE_SPEED_RAUM+5, DRIVE_SPEED_RAUM-5);
        }
        else
        {
            move(DRIVE_SPEED_RAUM, DRIVE_SPEED_RAUM);
        }
        */
       if (disable_IR_timer.has_reached_target() && IR_R.change_between_last_time() > 10/* && IR_R.change_direction == 1*/)
       {
           // Maybe hole
           move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
            cust_delay(400);
            move(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
            cust_delay(TURN_90_DEG_DELAY);
            cuart.silver_line = false;
            cuart.green_line = false;
            move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
            while(!(taster.get_state(taster.front_right) && taster.get_state(taster.front_left)) && !cuart.silver_line && !cuart.green_line)
            {
                display.tick();
                vTaskDelay(watchdog_delay);
            }
            if (cuart.green_line)
            {
                display.raum_mode = false;
                in_raum = false;
                move(0,  0);
                cust_delay(1000);
                move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
                return;
            }
            if (cuart.silver_line)
            {
                move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
                cust_delay(200);
                cuart.silver_line = false;
                disable_IR_timer.reset();
            }
            turn_90_while_next_to_wall();
            IR_R.change_between_last_time();
            cuart.silver_line = false;
       }
    //    else if (IR_R.change_direction == 1)
    //    {
    //        move(DRIVE_SPEED_RAUM + 5, DRIVE_SPEED_RAUM - 5);
    //    }
    }
    if (cuart.green_line)
    {
        display.raum_mode = false;
        in_raum = false;
        move(0,  0);
        cust_delay(1000);
        move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
        return;
    }
    if (cuart.silver_line)
    {
        move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
        cust_delay(200);
        cuart.silver_line = false;
        disable_IR_timer.reset();
    }
    turn_90_while_next_to_wall();
    
    move_along_wall_find_exit();
}

void drive_raum()
{
    count_of_balls = 0;//preferences.getInt("balls", 0);
    // ecke = preferences.getInt("ecke", 0);

    raum_entry_angle = compass.get_angle();

    move(-DRIVE_SPEED_RAUM, -DRIVE_SPEED_RAUM);
    cust_delay(300);
    move(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    cust_delay(TURN_90_DEG_DELAY);
    move_along_wall(); // Finding ecke / dropping off blue cube
    
    before_if:
    if (true)//move_out_of_hole_now) // If the robot is ready to exit the area
    {
        move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
        cust_delay(100);
        move(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
        cust_delay(1.5* TURN_90_DEG_DELAY);
        move(DRIVE_SPEED_RAUM, DRIVE_SPEED_RAUM);
        
        cuart.green_line = false;
        cuart.silver_line = false;
        while(!(taster.get_state(taster.front_right) || taster.get_state(taster.front_left)) && !cuart.silver_line && !cuart.green_line)
        {
            display.tick();
            vTaskDelay(watchdog_delay);
        }
        if (cuart.green_line)
        {
            in_raum = false;
            display.raum_mode = false;
            move(0,  0);
            cust_delay(1000);
            move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
            return;
        }
        if (cuart.silver_line)
        {
            move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
            cust_delay(200);
            cuart.silver_line = false;
        }
        turn_90_while_next_to_wall();

        move_along_wall_find_exit();
        return;
    }
    else // Contine with searching for balls
    {
        move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
        cust_delay(500);
        move(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
        cust_delay(0.5* TURN_90_DEG_DELAY);
        move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
        cust_delay(1000); // should now be standing against a wall
        driving_to_ecke = false;
        search_for_balls:
        move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
        cuart.silver_line = false;
        cuart.green_line = false;
        while(!taster.get_state(taster.front_right) && !cuart.silver_line && !cuart.green_line) // driving until the other wall...
        {
            display.tick();
            vTaskDelay(pdMS_TO_TICKS(50));

            if (IR_L.change_between_last_time() > 5) // ... except if there is an abrupt change in IR
            {
                float value_of_ball = IR_L.get_cm();
                move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
                cust_delay(100);
                float value_of_wall = IR_L.get_cm();
                float mid_between_wall_and_ball = abs(value_of_ball - value_of_wall) / 2; // then measuring values
                move(DRIVE_SPEED_CORNER, DRIVE_SPEED_CORNER);
                while(IR_L.get_cm() > mid_between_wall_and_ball) // trying to find ball again, but exact
                {
                    display.tick();
                    vTaskDelay(watchdog_delay);
                    // TODO: What if ball isnt found
                }
                move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL); // Trying to center robot to ball ...
                cust_delay(60);
                move(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
                cust_delay(TURN_90_DEG_DELAY); // ... and face the ball
                move(0, 0);
                greifer_zu.write(ANGLE_GREIFER_OPEN);
                cust_delay(500);
                greifer_up.write(ANGLE_GREIFER_DOWN);
                move(-DRIVE_SPEED_CORNER, -DRIVE_SPEED_CORNER);
                cust_delay(150*value_of_ball);
                greifer_zu.write(ANGLE_GREIFER_CLOSE);
                cust_delay(500);
                move(DRIVE_SPEED_CORNER, DRIVE_SPEED_CORNER);
                cust_delay(100);
                greifer_up.write(ANGLE_GREIFER_UP); // ball is picked up
                move(DRIVE_SPEED_CORNER, DRIVE_SPEED_CORNER);
                cust_delay((150*value_of_ball)-100);
                if (driving_to_ecke) // if driving while facing the ecke, we need to turn in a different direction
                {
                    move(-DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
                }
                else
                {
                    move(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
                }
                cust_delay(TURN_90_DEG_DELAY);
                move(DRIVE_SPEED_RAUM, DRIVE_SPEED_RAUM);
                while(!taster.get_state(taster.front_right) && !cuart.silver_line && !cuart.green_line) // driving to wall
                {
                    display.tick();
                    vTaskDelay(watchdog_delay);
                }
                if (cuart.silver_line || cuart.green_line)
                {
                    move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
                    cust_delay(500);
                    cuart.silver_line = false;
                    cuart.green_line = false;
                }
                turn_90_while_next_to_wall(); // getting straight to the wall
                while(!taster.get_state(taster.front_right)) // until the ecke is detected
                {
                    display.tick();
                    vTaskDelay(watchdog_delay);
                }
                put_away(); // drop off
                goto before_if; // go to before the if -> collect balls if necessary, else drive out
            }
        }
        if (cuart.silver_line || cuart.green_line)
        {
            move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
            cust_delay(500);
            cuart.silver_line = false;
            cuart.green_line = false;
        }
        turn_90_while_next_to_wall(); // Reached end of room: Moving to the side and turning around
        move(DRIVE_SPEED_NORMAL, DRIVE_SPEED_NORMAL);
        cust_delay(100);
        turn_90_while_next_to_wall();
        move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
        cust_delay(500);
        driving_to_ecke = true;
        goto search_for_balls;
    }
}

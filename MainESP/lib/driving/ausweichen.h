#pragma once

void ausweichen()
{
    move(-DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    delay(200);
    move(DRIVE_SPEED_NORMAL, -DRIVE_SPEED_NORMAL);
    delay(500);
    move(8, 35);
    delay(1000);
    //TBD
}

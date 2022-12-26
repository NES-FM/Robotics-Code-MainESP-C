#define MILLIMETERS_PER_MILLISECOND 0.1770833333

void drive_room()
{
    if (robot.cur_room_state == robot.ROOM_STATE_DEFAULT)
    {
        robot.move(0, 0);
    }
}

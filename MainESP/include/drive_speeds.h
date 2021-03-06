#ifndef DRIVE_SPEEDS_H
#define DRIVE_SPEEDS_H

#define DRIVE_SPEED_OFFSET_SMALL 5
#define DRIVE_SPEED_OFFSET_BIG 10

#define DRIVE_SPEED_NORMAL 30

#define DRIVE_SPEED_RAUM 40

#define DRIVE_SPEED_CORNER 20

#define DRIVE_SPEED_LOWER (DRIVE_SPEED_CORNER - DRIVE_SPEED_OFFSET_BIG)
#define DRIVE_SPEED_LOW (DRIVE_SPEED_CORNER - DRIVE_SPEED_OFFSET_SMALL)

#define DRIVE_SPEED_HIGH (DRIVE_SPEED_CORNER + DRIVE_SPEED_OFFSET_SMALL)
#define DRIVE_SPEED_HIGHER (DRIVE_SPEED_CORNER + DRIVE_SPEED_OFFSET_BIG)

#define DRIVE_SPEED_HALF (DRIVE_SPEED_NORMAL / 2)

#endif // DRIVE_SPEEDS_H

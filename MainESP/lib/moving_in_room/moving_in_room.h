#pragma once

#include "robot.h"
#include "bcuart.h"

#define ROTATE_TO_ANGLE_TOLERANCE 4
#define ROTATE_TO_ANGLE_SLOW_DOWN_TOLERANCE 10

class moving_in_room_step
{
    public:
        virtual bool tick(uint32_t delta_time)=0; // Returns True if finnished
        Robot* _robot;
        int motor_left_speed = 0;
        int motor_right_speed = 0;
        virtual ~moving_in_room_step() = default;
};

class moving_in_room_follow_ball : public moving_in_room_step
{
    public:
        bool tick(uint32_t delta_time) override;
        BCUART_class* _bcuart;
    private:
        bool moving_to_balls_ball_too_close = false;
};

class moving_in_room_follow_corner : public moving_in_room_step
{
    public:
        bool tick(uint32_t delta_time) override;
        BCUART_class* _bcuart;
        uint32_t time_after = 1000;
    private:
        bool moving_to_corner_corner_too_close = false;
};

class moving_in_room_rotate_to_deg : public moving_in_room_step
{
    public:
        bool tick(uint32_t delta_time) override;
        float target_angle = 0.0;
};

class moving_in_room_rotate_relative_degrees : public moving_in_room_step
{
    public:
        bool tick(uint32_t delta_time) override;
        float target_relative_angle = 0.0;
    private:
        moving_in_room_rotate_to_deg* rotate_to_deg;
        bool first_time = true;
};

class moving_in_room_distance_by_time : public moving_in_room_step
{
    public:
        bool tick(uint32_t delta_time) override;
        uint32_t time_left = 0;
        void calculate_time_by_distance(int distance_mm);
};

class moving_in_room_pick_up_ball : public moving_in_room_step
{
    public:
        bool tick(uint32_t delta_time) override;
};

class moving_in_room_goto_room_state : public moving_in_room_step
{
    public:
        bool tick(uint32_t delta_time) override;
        Robot::room_states target_room_state = Robot::ROOM_STATE_DEFAULT;
};

class moving_in_room_set_claw : public moving_in_room_step
{
    public:
        bool tick(uint32_t delta_time) override;
        Claw::State claw_state;
        bool force = false;
};

class moving_in_room_until_taster : public moving_in_room_step
{
    public:
        bool tick(uint32_t delta_time) override;
        uint32_t continue_afterwards = 0;
    private:
        bool taster_activated = false;
};

extern std::vector<moving_in_room_step*> moving_in_room_queue;

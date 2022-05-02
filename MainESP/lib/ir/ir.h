#pragma once
#include "analog_sensor.h"

class IR {
    public:
        IR(int pin) { as = new analog_sensor(pin, false); pinMode(pin, INPUT_PULLUP); };
        int get_raw() { return as->get_state(); };
        float get_cm() { float state = calculate_cm(as->get_state()); last_state = state;return state; }
        
        float change_between_last_time() { float state = get_cm();/* if (state > 45) {return 0;}*/float diff = abs(state - last_state); if((last_state-state)>0) { change_direction = -1;} else { change_direction = 1; } Serial.printf("state: %f, last_state: %f\r\n", state, last_state); last_state = state; return diff; }
        int change_direction = 0;
        bool is_ball() 
        { 
            float state = get_cm();
            float average_last_two = ((state + last_state) / 2);
            float substracted = abs(average_last_two - state_two_back);
            // Serial.printf("state: %f, Last2: %f, Last8: %f, sub: %f\r\n", state, average_last_two, average_last_eight, substracted);
            state_two_back = last_state;
            last_state = state;
            if (substracted > 10)
                return true;
            return false;
        }
        float last_state = 0.0f;
    private:
        analog_sensor* as;
        float state_two_back = 0.0f;
        float calculate_cm(int input) { return 46.68567 * exp(-0.001145 * input); };
};
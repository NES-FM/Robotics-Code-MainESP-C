#include "analog_sensor.h"

uint8_t valid_analog_pins[20] = {0,2,4,12,13,14,15,25,26,27,32,33,34,35,36,37,38,39};

analog_sensor::analog_sensor(int p)
{
    this->checkValidPin(p);
}

analog_sensor::analog_sensor(int p, bool av8) 
{ 
    average8_enabled = av8;
    this->checkValidPin(p);
}

void analog_sensor::checkValidPin(uint8_t p)
{
    for(uint8_t valid_pin : valid_analog_pins)
    {
        if (p == valid_pin)
        {
            pin = p; 
            adcAttachPin(pin);
            return;
        }
    }
    Serial.printf("Error Initializing Analog Sensor: Pin %d is not a valid ADC pin\r\n", p);
}

uint16_t analog_sensor::get_state()
{
    uint16_t state = analogRead(pin);
    
    if (average8_enabled)
    {
        average8_list[average8_index] = state;
        average8_index = (average8_index + 1) % 8;

        state = 0;

        for (auto t : average8_list)
        {
            state += t;
        }

        state /= 8;
    }
    
    last_state = state;
    return state; 
}

uint16_t analog_sensor::diff_to_last_state()
{
    uint16_t state = analogRead(pin);
    uint16_t diff = abs(state - last_state);
    last_state = state;
    return diff;
}

float analog_sensor::convert_to_battery_voltage()
{
    /*
    List of Points:
    (1200, 5.78)
    (1320, 6.3)
    (1450, 6.76)
    (1520, 7.1)
    (1590, 7.46)
    (1680, 7.9)
    (1780, 8.38)

    GeoGebra, different fits:
    l(x) = Trendlinie({H, I. J. K. L, M, N)) —> y = O.0044756x + 0.35819
    e(x) = TrendExp({H. Ii, K. L. M.N}) —> 2.6898 * e^(0.00063966x)
    q(x) = TrendPoly({I-1, I, J. K. L, M, N}, 2) —> 0.0000013422x² + 0.00048115x + 3.2830
    o(x) = TrendLog({H.I.J,K.L,M.N}) —> -40.756 + 6.5471*ln(x)

    Currently chosen: q(x) = TrendPoly
    */

    uint16_t state = this->get_state();

    //return (0.0000013422 * state * state) + (0.00048115 * state) + 3.2830;
    return (0.0050462030154 * state) + 0.8888827902453;
}


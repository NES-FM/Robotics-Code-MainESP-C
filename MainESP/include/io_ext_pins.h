#pragma once

enum io_ext_pins {
    EXT_D0 = 0,
    EXT_D1 = 1,
    EXT_D2 = 2,
    EXT_D3 = 3,
    EXT_D4 = 4,
    EXT_D5 = 5,
    EXT_D6 = 6,
    EXT_D7 = 7,
    EXT_D8 = 8,
    EXT_D9 = 9,
    EXT_D10 = 10,
    EXT_D11 = 11,
    EXT_D12 = 12,
    EXT_D13 = 13,
    EXT_A0 = 14,
    EXT_A1 = 15,
    EXT_A2 = 16,
    EXT_A3 = 17,
    EXT_A4 = 18,
    EXT_A5 = 19,
    EXT_A6 = 20,
    EXT_A7 = 21
};

struct lc02_answer
{
    uint32_t distance;
    uint8_t error_code;
};

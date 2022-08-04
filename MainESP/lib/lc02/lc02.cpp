#include "lc02.h"
int lc02::getDistance_mm()
{
    getData();
    return data.distance;
}
uint8_t lc02::getErrorCode()
{
    return data.error_code;
}

void lc02::getData()
{
    data = getArduinoLc02Distance();
}

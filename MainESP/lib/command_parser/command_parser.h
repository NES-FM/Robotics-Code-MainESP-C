#pragma once

#include <Arduino.h>
#include "logger.h"

int count_occurrences_in_string(String str, char chr);

struct splitstring
{
    String* data;
    unsigned int length;
    unsigned int start = 0;
};

splitstring split_string_at_space(String str);

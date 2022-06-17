#include "command_parser.h"

int count_occurrences_in_string(String str, char chr)
{
    int occ = 0;
    for (int i = 0; i < str.length(); i++)
    {
        if (str[i] == chr)
            occ++;
    }
    return occ;
}

splitstring split_string_at_space(String str)
{
    int num_spaces = count_occurrences_in_string(str, ' ');
    splitstring ret;
    ret.data = new String[num_spaces+1];
    int StringCount = 0;

    // Split the string into substrings
    while (str.length() > 0)
    {
        int index = str.indexOf(' ');
        if (index == -1) // No space found
        {
            ret.data[StringCount++] = str;
            break;
        }
        else
        {
            ret.data[StringCount++] = str.substring(0, index);
            str = str.substring(index+1);
        }
    }

    ret.length = StringCount;
    return ret;
}

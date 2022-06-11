#include "command_parser.h"

Robot* _logger_robot_ref;

void parser_set_logger_robot_ref(Robot* robot)
{
    _logger_robot_ref = robot;
}

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

struct splitstring
{
    String* data;
    unsigned int length;
    unsigned int start = 0;
};

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

String help_command()
{
    String ret = "";
    ret += "--Help--\r\n";
    ret += "get [sensor] [<subsensor>]\r\n";
    ret += "--------\r\n";
    return ret;
}

String get_command(String sensor, String subsensor)
{
    char* out = "unknown error";

    if (sensor == "")
        out = "too few arguments";
    else if (sensor == "help")
    {
        String ret = "";
        ret += "--get--\r\n";
        ret += "battery\r\n";
        ret += "dip [<dip1/dip2/wettkampfmodus>]\r\n";
        ret += "accel\r\n";
        ret += "compass\r\n";
        ret += "tof [<back/left/right>]\r\n";
        ret += "drive mode\r\n";
        ret += "-------\r\n";

        sprintf(out, "%s", ret.c_str());
    }
    else if (sensor == "battery")
        sprintf(out, "%fV", _logger_robot_ref->bat_voltage->convert_to_battery_voltage());
    else if (sensor == "dip")
    {
        if (subsensor == "")
            sprintf(out, "%s%s%s", _logger_robot_ref->dip->get_state(DIP::dip1) ? "1": "0", _logger_robot_ref->dip->get_state(DIP::dip2) ? "1": "0", _logger_robot_ref->dip->get_state(DIP::wettkampfmodus) ? "1": "0");
        else if (subsensor == "dip1")
            sprintf(out, "%s", _logger_robot_ref->dip->get_state(DIP::dip1) ? "1": "0");
        else if (subsensor == "dip2")
            sprintf(out, "%s", _logger_robot_ref->dip->get_state(DIP::dip2) ? "1": "0");
        else if (subsensor == "wettkampfmodus")
            sprintf(out, "%s", _logger_robot_ref->dip->get_state(DIP::wettkampfmodus) ? "1": "0");
        else
            out = "subsensor not found";
    }
    else if (sensor == "accel")
    {
        if (subsensor == "")
            sprintf(out, "%f", _logger_robot_ref->accel_sensor->get_roll_degrees());
        else
            out = "subsensor not found";
        // ToDo: Other Axis + Ramp modes
    }
    else if (sensor == "compass")
        sprintf(out, "%f", _logger_robot_ref->compass->get_angle());
    else if (sensor == "tof")
    {
        if (subsensor == "")
            out = "Specifying of subsensor needed";
        else if (subsensor == "right")
            sprintf(out, "%f", _logger_robot_ref->tof_right->getMeasurement());
        else if (subsensor == "left")
            sprintf(out, "%f", _logger_robot_ref->tof_left->getMeasurement());
        else if (subsensor == "back")
            sprintf(out, "%f", _logger_robot_ref->tof_back->getMeasurement());
        else
            out = "subsensor not found";
    }
    else if (sensor == "drive" && subsensor == "mode")
    {
        if (_logger_robot_ref->cur_drive_mode == Robot::ROBOT_DRIVE_MODE_LINE)
            out = "line";
        else if (_logger_robot_ref->cur_drive_mode == Robot::ROBOT_DRIVE_MODE_ROOM)
            out = "room";
        else
            out = "unknown drive mode";
    }
    // ToDo: Taster
    return out;
}

String move_command(String l, String r)
{
    
}

void logger_pasrse_command(String command)
{
    command.trim();
    logln("Parsing Command: %s", command.c_str());

    splitstring splitted = split_string_at_space(command);

    String top_level_command = splitted.data[0];
    String first_arg = "";
    String second_arg = "";

    if (splitted.length > 1)
        first_arg = splitted.data[1];
    if (splitted.length > 2)
        second_arg = splitted.data[2];

    String out;

    top_level_command.toLowerCase();
    first_arg.toLowerCase();
    second_arg.toLowerCase();

    if (top_level_command == "help")
        out = help_command();
    else if (top_level_command == "get")
        out = get_command(first_arg, second_arg);

    logln("%s", out.c_str());
}

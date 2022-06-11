#pragma once

#include <Arduino.h>
#include "logger.h"
#include "robot.h"

void logger_pasrse_command(String command);
void parser_set_logger_robot_ref(Robot* robot);


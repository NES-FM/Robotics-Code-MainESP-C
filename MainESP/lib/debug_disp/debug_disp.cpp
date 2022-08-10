#include "debug_disp.h"

debug_disp::debug_disp() {};

void debug_disp::init(CUART_class* c, Robot* r, bool* int_sit, bool* int_bi_left, bool* int_bi_right, bool* int_bi_both)
{
    // Getting references to all variables to be shown on the screen
    _cuart = c;
    _robot = r;
    _interesting_situation = int_sit;
    _int_bias_left = int_bi_left;
    _int_bias_right = int_bi_right;
    _int_bias_both = int_bi_both;


    if (!oled->begin(SSD1306_SWITCHCAPVCC, _i2c_address))
    {
        logln("SSD1306 allocation failed");
        this->enable(false);
    }
    if (_display_i2c_enabled)
    {
        // oled->setRotation(2);
        oled->setTextWrap(false);
        oled->clearDisplay();
        oled->display();
    }

    // Room Wall pieces
    // Bottom
    all_room_walls[0].mid.x_mm = 125;
    all_room_walls[0].mid.y_mm = 0;
    all_room_walls[1].mid.x_mm = 425;
    all_room_walls[1].mid.y_mm = 0;
    all_room_walls[2].mid.x_mm = 725;
    all_room_walls[2].mid.y_mm = 0;
    all_room_walls[2].entry = true;
    all_room_walls[3].mid.x_mm = 1025;
    all_room_walls[3].mid.y_mm = 0;
    // Right
    all_room_walls[4].mid.x_mm = _robot->room_width;
    all_room_walls[4].mid.y_mm = 125;
    all_room_walls[4].vertical = true;
    all_room_walls[5].mid.x_mm = _robot->room_width;
    all_room_walls[5].mid.y_mm = 425;
    all_room_walls[5].vertical = true;
    all_room_walls[5].entry = true;
    all_room_walls[6].mid.x_mm = _robot->room_width;
    all_room_walls[6].mid.y_mm = 725;
    all_room_walls[6].vertical = true;
    // Top
    all_room_walls[7].mid.x_mm = 1025;
    all_room_walls[7].mid.y_mm = _robot->room_height;
    all_room_walls[8].mid.x_mm = 725;
    all_room_walls[8].mid.y_mm = _robot->room_height;
    all_room_walls[9].mid.x_mm = 425;
    all_room_walls[9].mid.y_mm = _robot->room_height;
    all_room_walls[10].mid.x_mm = 125;
    all_room_walls[10].mid.y_mm = _robot->room_height;
    // Left
    all_room_walls[11].mid.x_mm = 0;
    all_room_walls[11].mid.y_mm = 725;
    all_room_walls[11].vertical = true;
    all_room_walls[12].mid.x_mm = 0;
    all_room_walls[12].mid.y_mm = 425;
    all_room_walls[12].vertical = true;
    all_room_walls[12].entry = true;
    all_room_walls[13].mid.x_mm = 0;
    all_room_walls[13].mid.y_mm = 125;
    all_room_walls[13].vertical = true;
    all_room_walls[13].exit = true;

    room_conversion_factor = float(_robot->room_height) / float(SCREEN_HEIGHT-10);
    screen_space_50 = 50.0f / room_conversion_factor;
    screen_space_125 = 125.0f / room_conversion_factor;
    screen_space_250 = 250.0f / room_conversion_factor;
    screen_space_300 = 300.0f / room_conversion_factor;
}

void debug_disp::draw_sensor_array(int x, int y, int element_width, int element_height)
{
    // Outline
    oled->drawRect(x, y, 2+element_width*24, element_height+2, SSD1306_WHITE);
    // Indentations for left, middle and right sensors
    oled->drawLine(x+(3*element_width)-1, y, x+(3*element_width)-1, y+element_height+2, SSD1306_BLACK);
    oled->drawLine(x+(10*element_width)-1, y, x+(10*element_width)-1, y+element_height+2, SSD1306_BLACK);
    oled->drawLine(x+(16*element_width), y, x+(16*element_width), y+element_height+2, SSD1306_BLACK);
    oled->drawLine(x+(23*element_width), y, x+(23*element_width), y+element_height+2, SSD1306_BLACK);

    // Main array
    for (int i = 0; i < 24; i++)
    {
        if (_cuart->sensor_array[i])
            oled->fillRect(x+1+(i*element_width), y+1, element_width, element_height, SSD1306_WHITE);
    }

    // Sensor Values
    oled->setTextSize(2);
    oled->setTextColor(SSD1306_WHITE);

    oled->setCursor(x+(3*element_width)+5, y+element_height+3);
    oled->print(_cuart->array_left_sensor);

    oled->setCursor(x+(10*element_width)+5, y+element_height+3);
    oled->print(_cuart->array_mid_sensor);

    oled->setCursor(x+(16*element_width)+7, y+element_height+3);
    oled->print(_cuart->array_right_sensor);
}

void debug_disp::draw_green_dots(int x, int y, int width, int height)
{
    oled->drawLine(x + (width/2), y, x + (width/2), y + height, SSD1306_WHITE);
    oled->drawLine(x, y + (height/2), x + width, y + (height/2), SSD1306_WHITE);

    int single_width = (width-3)/2;
    int single_height = (height-3)/2;

    if (_cuart->green_dots[0])
        oled->fillRect(x+1, y+1, single_width, single_height, SSD1306_WHITE);
    if (_cuart->green_dots[1])
        oled->fillRect(x+width-single_width, y+1, single_width, single_height, SSD1306_WHITE);
    if (_cuart->green_dots[2])
        oled->fillRect(x+1, y+height-single_height, single_width, single_height, SSD1306_WHITE);
    if (_cuart->green_dots[3])
        oled->fillRect(x+width-single_width, y+height-single_height, single_width, single_height, SSD1306_WHITE);
}

void debug_disp::draw_ltype(int x, int y)
{
    if (*_interesting_situation) oled->drawLine(x, y, x+14, y, SSD1306_WHITE);
    if (*_int_bias_left || *_int_bias_both) oled->drawLine(x, y+1, x+3, y+1, SSD1306_WHITE);
    if (*_int_bias_right || *_int_bias_both) oled->drawLine(x+11, y+1, x+14, y+1, SSD1306_WHITE);

    y += 4;
    oled->setCursor(x, y);
    oled->setTextSize(2);
    oled->setTextColor(SSD1306_WHITE);

    switch(_cuart->line_type)
    {
        // Every Character is one less then the desired one (I have no clue why)
        case CUART_LTYPE_STRAIGHT:
            oled->print((char)0xB9); // ║
            break;
        case CUART_LTYPE_90l:
            oled->print((char)0xBA); // ╗
            break;
        case CUART_LTYPE_90r:
            oled->print((char)0xC8); // ╔
            oled->drawRect(x+10, y+4, 4, 2, SSD1306_WHITE);
            oled->drawRect(x+10, y+8, 4, 2, SSD1306_WHITE);
            break;
        case CUART_LTYPE_tl:
            oled->print((char)0xB8); // ╣
            break;
        case CUART_LTYPE_tr:
            oled->print((char)0xCB); // ╠
            oled->drawRect(x+10, y+4, 4, 2, SSD1306_WHITE);
            oled->drawRect(x+10, y+8, 4, 2, SSD1306_WHITE);
            break;
        case CUART_LTYPE_t:
            oled->print((char)0xCA); // ╦
            oled->drawRect(x+10, y+4, 4, 2, SSD1306_WHITE);
            oled->drawRect(x+10, y+8, 4, 2, SSD1306_WHITE);
            break;
        case CUART_LTYPE_X:
            oled->print((char)0xCD); // ╬
            oled->drawRect(x+10, y+4, 4, 2, SSD1306_WHITE);
            oled->drawRect(x+10, y+8, 4, 2, SSD1306_WHITE);
            break;
        case CUART_LTYPE_SPACE:
            oled->print((char)0xB9); // ║
            oled->fillRect(x, y+4, 12, 8, SSD1306_BLACK);
            oled->fillRect(x+6, y+2, 2, 2, SSD1306_WHITE);
            oled->fillRect(x+6, y+12, 2, 2, SSD1306_WHITE);
            break;
        default:
            oled->print('?');
            break;
    }        
}

void debug_disp::draw_motor_values(int x, int y)
{
    oled->setCursor(x, y);
    oled->setTextSize(2);
    oled->setTextColor(SSD1306_WHITE);
    
    oled->printf("%+03d %+03d", _robot->motor_left->motor_speed, _robot->motor_right->motor_speed);
}

void debug_disp::draw_comp_accel(int x, int y)
{
    oled->setCursor(x, y);
    oled->setTextSize(1);
    oled->setTextColor(SSD1306_WHITE);

    oled->printf("%5.1f", _robot->accel_sensor->get_roll_degrees());

    this->draw_accel(x, y+8);
}

void debug_disp::draw_accel(int x, int y)
{
    oled->setCursor(x, y);
    oled->setTextColor(SSD1306_WHITE);
    oled->setTextSize(1);

    oled->printf(" %03d", int(_robot->compass->get_angle()));//int(_compass->getRelativeAngle()));
}

void debug_disp::draw_cuart(int x, int y)
{
    oled->setCursor(x, y);
    oled->setTextSize(1);
    oled->setTextColor(SSD1306_WHITE);

    oled->printf("%+02d", _cuart->line_angle);
    // oled->print(0xF8);

    oled->setCursor(x, y+8);

    oled->printf("%+02d", _cuart->line_midfactor);
    // oled->print(0xF8);

    // Serial.printf("%f  %5.1f\t\t%f %03d\r\n", _accelerometer->get_roll_degrees(), _accelerometer->get_roll_degrees(), _compass->get_angle(), int(_compass->get_angle()));
}

void debug_disp::draw_disabled_i2c_devices(int x, int y)
{
    oled->setCursor(x, y);
    oled->setTextSize(1);
    oled->setTextColor(SSD1306_WHITE);

    oled->print("I2C-D: ");
    oled->print(i2c_disabled_devices);
}

void debug_disp::draw_voltage(int x, int y)
{
    oled->setCursor(x, y);
    oled->setTextColor(SSD1306_WHITE);
    oled->setTextSize(2);

    oled->printf("%.1fV", _robot->bat_voltage->convert_to_battery_voltage());
    // oled->print(_voltage->get_state());
}

void debug_disp::draw_voltage_smol(int x, int y)
{
    oled->setCursor(x, y);
    oled->setTextColor(SSD1306_WHITE);
    oled->setTextSize(1);

    oled->printf("%.1fV", _robot->bat_voltage->convert_to_battery_voltage());
    // oled->print(_voltage->get_state());
}

void debug_disp::draw_dip(int x, int y)
{
    oled->drawRect(x, y, 14, 6, SSD1306_WHITE);
    if (_robot->dip->get_state(_robot->dip->wettkampfmodus))
        oled->fillRect(x+1, y+1, 4, 4, SSD1306_WHITE);
    if (_robot->dip->get_state(_robot->dip->dip1))
        oled->fillRect(x+5, y+1, 4, 4, SSD1306_WHITE);
    if (_robot->dip->get_state(_robot->dip->dip2))
        oled->fillRect(x+9, y+1, 4, 4, SSD1306_WHITE);
}

void debug_disp::draw_taster(int x, int y, int w, int h)
{
    oled->drawRect(x, y, w, h, SSD1306_WHITE);
    
    if (_robot->taster->get_state(_robot->taster->front_left))
        oled->fillRect(x+1, y+1, (w/2)-1, h/4, SSD1306_WHITE);
    if (_robot->taster->get_state(_robot->taster->front_right))
        oled->fillRect(x+(w/2), y+1, (w/2)-1, h/4, SSD1306_WHITE);
}

void debug_disp::draw_tof(int x, int y)
{
    oled->setTextSize(1);
    oled->setCursor(x, y);
    uint16_t left = _robot->tof_left->getMeasurement();
    uint16_t right = _robot->tof_right->getMeasurement();
    uint16_t back = _robot->tof_back->getMeasurement();
    uint16_t front = _robot->tof_front->getMeasurement();

    oled->print("L|B|R|F:");

    if (_robot->tof_left->getMeasurementError() == _robot->tof_left->TOF_ERROR_NONE)
        oled->print(left);
    else
        oled->print("Err");

    oled->print("|");

    if (_robot->tof_back->getMeasurementError() == _robot->tof_back->TOF_ERROR_NONE)
        oled->print(back);
    else
        oled->print("Err");

    oled->print("|");

    if (_robot->tof_right->getMeasurementError() == _robot->tof_right->TOF_ERROR_NONE)
        oled->print(right);
    else
        oled->print("Err");

    oled->print("|");

    if (_robot->tof_front->getMeasurementError() == _robot->tof_front->TOF_ERROR_NONE)
        oled->print(front);
    else
        oled->print("Err");

    // oled->print("=>");
    // oled->print(left+right);
}

void debug_disp::draw_closerange_tof(int x, int y)
{
    oled->setTextSize(1);
    oled->setCursor(x, y);
    uint16_t close = _robot->tof_closerange->getMeasurement();

    tof::tof_error_types error = _robot->tof_closerange->getMeasurementError();

    if (error == tof::tof_error_types::TOF_ERROR_MAX_DISTANCE || error == tof::tof_error_types::TOF_ERROR_NOCONVERGENCE)
        oled->print("Max");
    else if (error == tof::tof_error_types::TOF_ERROR_MIN_DISTANCE)
        oled->print("Min");
    else if (error == tof::tof_error_types::TOF_ERROR_NONE)
        oled->print(close);
    else
        oled->print("Err");
}

void debug_disp::draw_room()
{
    // Serial.printf("Drawing Room with: Conv: %f, w: %f, h: %f\r\n", conversion_factor, _robot->room_width / conversion_factor, _robot->room_height / conversion_factor);
    // int w = _robot->room_width / conversion_factor;
    // int h = _robot->room_height / conversion_factor;

    // oled->drawRect(x, y, w, h, SSD1306_WHITE);

    this->draw_robot_in_room_coordinates();

    if (_robot->room_corner_found)
        this->draw_room_corner();

    draw_room_wall();
}

void debug_disp::draw_robot_in_room_coordinates()
{
    Robot::point UL;
    UL.x_mm = (_robot->pos.x_mm - (0.5 * _robot->width));
    UL.y_mm = (_robot->pos.y_mm + (0.5 * _robot->height));
    UL = Robot::rotate_point(UL, _robot->pos, _robot->angle);
    // screen_point UL_screen;
    // UL_screen.x = bottom_left_x + (UL.x_mm / conversion_factor); // Translate to Screen space
    // UL_screen.y = bottom_left_y - (UL.y_mm / conversion_factor);

    Robot::point UR;
    UR.x_mm = (_robot->pos.x_mm + (0.5 * _robot->width));
    UR.y_mm = (_robot->pos.y_mm + (0.5 * _robot->height));
    UR = Robot::rotate_point(UR, _robot->pos, _robot->angle);
    // screen_point UR_screen;
    // UR_screen.x = bottom_left_x + (UR.x_mm / conversion_factor); // Translate to Screen space
    // UR_screen.y = bottom_left_y - (UR.y_mm / conversion_factor);

    Robot::point DL;
    DL.x_mm = (_robot->pos.x_mm - (0.5 * _robot->width));
    DL.y_mm = (_robot->pos.y_mm - (0.5 * _robot->height));
    DL = Robot::rotate_point(DL, _robot->pos, _robot->angle);
    // screen_point DL_screen;
    // DL_screen.x = bottom_left_x + (DL.x_mm / conversion_factor); // Translate to Screen space
    // DL_screen.y = bottom_left_y - (DL.y_mm / conversion_factor);

    Robot::point DR;
    DR.x_mm = (_robot->pos.x_mm + (0.5 * _robot->width));
    DR.y_mm = (_robot->pos.y_mm - (0.5 * _robot->height));
    DR = Robot::rotate_point(DR, _robot->pos, _robot->angle);
    // screen_point DR_screen;
    // DR_screen.x = bottom_left_x + (DR.x_mm / conversion_factor); // Translate to Screen space
    // DR_screen.y = bottom_left_y - (DR.y_mm / conversion_factor);

    Robot::point UM;
    UM.x_mm = (UL.x_mm + UR.x_mm) / 2;
    UM.y_mm = (UL.y_mm + UR.y_mm) / 2;
    // screen_point UM_screen;
    // UM_screen.x = (UL_screen.x + UR_screen.x) / 2; // UL and UR are already Screen space -> no need to convert again
    // UM_screen.y = (UL_screen.y + UR_screen.y) / 2;

    draw_room_space_line(UL, UR);
    draw_room_space_line(UR, DR);
    draw_room_space_line(DR, DL);
    draw_room_space_line(DL, UL);
    draw_room_space_line(UM, DL);
    draw_room_space_line(UM, DR);

    // oled->drawLine(UL_screen.x, UL_screen.y, UR_screen.x, UR_screen.y, SSD1306_WHITE);
    // oled->drawLine(UR_screen.x, UR_screen.y, DR_screen.x, DR_screen.y, SSD1306_WHITE);
    // oled->drawLine(DR_screen.x, DR_screen.y, DL_screen.x, DL_screen.y, SSD1306_WHITE);
    // oled->drawLine(DL_screen.x, DL_screen.y, UL_screen.x, UL_screen.y, SSD1306_WHITE);
    // oled->drawLine(UM_screen.x, UM_screen.y, DL_screen.x, DL_screen.y, SSD1306_WHITE);
    // oled->drawLine(UM_screen.x, UM_screen.y, DR_screen.x, DR_screen.y, SSD1306_WHITE);
}

void debug_disp::draw_room_corner()
{
    int corner_pos_x = _robot->room_corner_pos.x_mm;
    int corner_pos_y = _robot->room_corner_pos.y_mm;

    Robot::point corner_point_1;
    Robot::point corner_point_2;

    corner_point_1.x_mm = corner_pos_x - 150;
    corner_point_2.x_mm = corner_pos_x + 150;

    if ((corner_pos_x == ROOM_CORNER_POS_BL_X && corner_pos_y == ROOM_CORNER_POS_BL_Y) || (corner_pos_x == ROOM_CORNER_POS_TR_X && corner_pos_y == ROOM_CORNER_POS_TR_Y))
    {
        corner_point_1.y_mm = corner_pos_y + 150;
        corner_point_2.y_mm = corner_pos_y - 150;
    }
    else if ((corner_pos_x == ROOM_CORNER_POS_BR_X && corner_pos_y == ROOM_CORNER_POS_BR_Y) || (corner_pos_x == ROOM_CORNER_POS_TL_X && corner_pos_y == ROOM_CORNER_POS_TL_Y))
    {
        corner_point_1.y_mm = corner_pos_y - 150;
        corner_point_2.y_mm = corner_pos_y + 150;
    }

    draw_room_space_line(corner_point_1, corner_point_2);

    // screen_point corner_point_1_screen;
    // corner_point_1_screen.x = bottom_left_x + round(float(corner_point_1.x_mm) / conversion_factor); // Translate to Screen space
    // corner_point_1_screen.y = bottom_left_y - round(float(corner_point_1.y_mm) / conversion_factor);
    // screen_point corner_point_2_screen;
    // corner_point_2_screen.x = bottom_left_x + round(float(corner_point_2.x_mm) / conversion_factor); // Translate to Screen space
    // corner_point_2_screen.y = bottom_left_y - round(float(corner_point_2.y_mm) / conversion_factor);

    // oled->drawLine(corner_point_1_screen.x-1, corner_point_1_screen.y-1, corner_point_2_screen.x-1, corner_point_2_screen.y-1, SSD1306_WHITE);
}

void debug_disp::draw_room_wall()
{
    Robot::point point_1;
    Robot::point point_2;
    for (wall_piece piece : all_room_walls)
    {
        // if (piece.entry || piece.exit)
        // {
        //     if (piece.vertical)
        //     {
        //         point_1.x_mm = piece.mid.x_mm;
        //         point_2.x_mm = piece.mid.x_mm;

        //         point_1.y_mm = constrain(piece.mid.y_mm - 125, 0, _robot->room_height);
        //         point_2.y_mm = constrain(piece.mid.y_mm - 175, 0, _robot->room_height);
        //         draw_room_space_line(point_1, point_2);

        //         point_1.y_mm = constrain(piece.mid.y_mm + 125, 0, _robot->room_height);
        //         point_2.y_mm = constrain(piece.mid.y_mm + 175, 0, _robot->room_height);
        //         draw_room_space_line(point_1, point_2);
        //     }
        //     else
        //     {
        //         point_1.y_mm = piece.mid.y_mm;
        //         point_2.y_mm = piece.mid.y_mm;

        //         point_1.x_mm = constrain(piece.mid.x_mm - 125, 0, _robot->room_width);
        //         point_2.x_mm = constrain(piece.mid.x_mm - 175, 0, _robot->room_width);
        //         draw_room_space_line(point_1, point_2);

        //         point_1.x_mm = constrain(piece.mid.x_mm + 125, 0, _robot->room_width);
        //         point_2.x_mm = constrain(piece.mid.x_mm + 175, 0, _robot->room_width);
        //         draw_room_space_line(point_1, point_2);
        //     }
        // }

        if (_robot->room_entry_found && piece.mid.x_mm == _robot->room_entry_pos.x_mm && piece.mid.y_mm == _robot->room_entry_pos.y_mm) // Entry tile
            continue;

        if (_robot->room_exit_found && piece.mid.x_mm == _robot->room_exit_pos.x_mm && piece.mid.y_mm == _robot->room_exit_pos.y_mm) // Exit tile
            continue;

        // if (piece.entry)
        //     continue;

        // if (piece.exit)
        //     continue; // Draw dotted line

        if (piece.vertical)
        {
            point_1.x_mm = piece.mid.x_mm;
            point_2.x_mm = piece.mid.x_mm;

            point_1.y_mm = piece.mid.y_mm - 125;
            point_2.y_mm = piece.mid.y_mm + 125;
        }
        else
        {
            point_1.x_mm = piece.mid.x_mm - 125;
            point_2.x_mm = piece.mid.x_mm + 125;

            point_1.y_mm = piece.mid.y_mm;
            point_2.y_mm = piece.mid.y_mm;
        }
        draw_room_space_line(point_1, point_2);
    }

    // for (int i = 250; i < 900; i += 300)
    // {
    //     point_1.x_mm = i;
    //     point_1.y_mm = 0;
    //     point_2.x_mm = i + 50;
    //     point_2.y_mm = 0;
    //     draw_room_space_line(point_1, point_2);

    //     point_1.y_mm = _robot->room_height;
    //     point_2.y_mm = _robot->room_height;
    //     draw_room_space_line(point_1, point_2);
    // }

    // // Bottom
    // oled->drawFastHLine(bottom_left_x + round(screen_space_250), bottom_left_y, screen_space_50, SSD1306_WHITE);
    // oled->drawFastHLine(bottom_left_x + round(2*screen_space_250) + round(screen_space_50), bottom_left_y, screen_space_50, SSD1306_WHITE);
    // oled->drawFastHLine(bottom_left_x + round(3*screen_space_250) + round(2*screen_space_50), bottom_left_y, screen_space_50, SSD1306_WHITE);
    // // Top
    // oled->drawFastHLine(bottom_left_x + round(screen_space_250), bottom_left_y - round(float(_robot->room_width) / conversion_factor), screen_space_50, SSD1306_WHITE);
    // oled->drawFastHLine(bottom_left_x + round(2*screen_space_250) + round(screen_space_50), bottom_left_y - round(float(_robot->room_width) / conversion_factor), screen_space_50, SSD1306_WHITE);
    // oled->drawFastHLine(bottom_left_x + round(3*screen_space_250) + round(2*screen_space_50), bottom_left_y - round(float(_robot->room_width) / conversion_factor), screen_space_50, SSD1306_WHITE);
    // // Left
    // oled->drawFastVLine(bottom_left_x, bottom_left_y - round(screen_space_250), screen_space_50, SSD1306_WHITE);
    // oled->drawFastVLine(bottom_left_x, bottom_left_y - round(2*screen_space_250) - round(screen_space_50), screen_space_50, SSD1306_WHITE);
    // // Right
    // oled->drawFastVLine(bottom_left_x + round(float(_robot->room_width) / conversion_factor), bottom_left_y - round(screen_space_250), screen_space_50, SSD1306_WHITE);
    // oled->drawFastVLine(bottom_left_x + round(float(_robot->room_width) / conversion_factor), bottom_left_y - round(2*screen_space_250) - round(screen_space_50), screen_space_50, SSD1306_WHITE);
}

void debug_disp::draw_room_space_line(Robot::point point_1, Robot::point point_2)
{
    screen_point point_1_screen;
    point_1_screen.x = room_bottom_left_x + round(float(point_1.x_mm) / room_conversion_factor); // Translate to Screen space
    point_1_screen.y = room_bottom_left_y - round(float(point_1.y_mm) / room_conversion_factor);
    screen_point point_2_screen;
    point_2_screen.x = room_bottom_left_x + round(float(point_2.x_mm) / room_conversion_factor); // Translate to Screen space
    point_2_screen.y = room_bottom_left_y - round(float(point_2.y_mm) / room_conversion_factor);

    oled->drawLine(point_1_screen.x, point_1_screen.y, point_2_screen.x, point_2_screen.y, SSD1306_WHITE);
}

void debug_disp::tick()
{
    if (_display_i2c_enabled)
    {
        if (millis() - _tick_last_millis >= DISPLAY_REFRESH_TIME)
        {
            _tick_last_millis = millis();
            oled->fillScreen(SSD1306_BLACK);

            if (_robot->cur_drive_mode == Robot::ROBOT_DRIVE_MODE_LINE)
            {
                // Sensor array and numbers down below. 
                this->draw_sensor_array(0, 0, 3, 5); // W: 2+element_width*24  H: element_height+18
                
                // Ltype
                this->draw_ltype(80, 0);

                // cuart values
                this->draw_cuart(90, 24);

                // Green Dots
                this->draw_green_dots(100, 0, 22, 22);

                // Motor Values
                this->draw_motor_values(0, 24); // W: 108px

                // Accelerometer and Compass 
                this->draw_comp_accel(45, 48);

                // Disabled I2C Devices
                this->draw_disabled_i2c_devices(0, 40);

                // Battery Voltage
                this->draw_voltage(80,44);

                // Dip Switches
                this->draw_dip(0, SCREEN_HEIGHT-6);

                // Taster
                this->draw_taster(18, SCREEN_HEIGHT-16, 16, 16);
            }
            else if (_robot->cur_drive_mode == Robot::ROBOT_DRIVE_MODE_ROOM)
            {
                if (room_entry_found_before != _robot->room_entry_found)
                {
                    room_entry_found_before = _robot->room_entry_found;
                }
                if (room_exit_found_before != _robot->room_exit_found)
                {
                    room_exit_found_before = _robot->room_exit_found;
                }


                this->draw_tof(0, 0);

                this->draw_closerange_tof(100, 10);

                this->draw_room();
                
                this->draw_accel(100, 46);
                this->draw_voltage_smol(100, 54);
                // this->draw_motor_values(0, 24); // W: 108px
            }
            
            // Flashing Pixel in lower right corner
            heartbeat_state = !heartbeat_state;
            oled->drawPixel(SCREEN_WIDTH-1, SCREEN_HEIGHT-1, heartbeat_state ? SSD1306_WHITE : SSD1306_BLACK);

            oled->display();
        }
    }
}

void debug_disp::enable(bool enabled)
{
    _display_i2c_enabled = enabled;
}

void debug_disp::draw_star()
{
    if (_display_i2c_enabled)
    {
        oled->clearDisplay();

        oled->drawBitmap(
        (oled->width()  - star_width ) / 2,
        (oled->height() - star_height) / 2,
        star_bmp, star_width, star_height, 1);
        oled->display();
    }
}

void debug_disp::disable_i2c_device(String dev)
{
    i2c_disabled_devices += dev;
    i2c_disabled_devices += " ";
}

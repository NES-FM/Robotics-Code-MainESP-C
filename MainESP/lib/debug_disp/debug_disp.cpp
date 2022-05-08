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
        Serial.println(F("SSD1306 allocation failed"));
        this->enable(false);
    }
    if (_display_i2c_enabled)
    {
        // oled->setRotation(2);
        oled->setTextWrap(false);
        oled->clearDisplay();
        oled->display();
    }
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

    if (_robot->tof_left->getMeasurementError() == _robot->tof_left->TOF_ERROR_NONE)
        oled->print(left);
    else
        oled->print("Err");

    oled->print(" | ");

    if (_robot->tof_right->getMeasurementError() == _robot->tof_right->TOF_ERROR_NONE)
        oled->print(right);
    else
        oled->print("Err");

    oled->print(" => ");
    oled->print(left+right);
}

void debug_disp::draw_room(int x, int y, float conversion_factor)
{
    // Serial.printf("Drawing Room with: Conv: %f, w: %f, h: %f\r\n", conversion_factor, _robot->room_width / conversion_factor, _robot->room_height / conversion_factor);
    int w = _robot->room_width / conversion_factor;
    int h = _robot->room_height / conversion_factor;

    oled->drawRect(x, y, w, h, SSD1306_WHITE);

    this->draw_robot_in_room_coordinates(x, y + h, conversion_factor);

    for(Robot::point p: _robot->points)
    {
        if (p.x != 0 || p.y != 0)
        {
            oled->drawPixel(x + (p.x / conversion_factor), y - (p.y / conversion_factor), SSD1306_WHITE);
        }
    }
}

void debug_disp::draw_robot_in_room_coordinates(int bottom_left_x, int bottom_left_y, float conversion_factor)
{
    typedef struct robot_edge_point
    {
        float x;
        float y;
    };

    robot_edge_point r_center;
    r_center.x = float(_robot->posx);
    r_center.y = float(_robot->posy);
    float robot_angle_rad = DEG_TO_RAD * _robot->angle;

    robot_edge_point temp;
    temp.x = 0.0f;
    temp.y = 0.0f;

    // https://gamedev.stackexchange.com/questions/86755/how-to-calculate-corner-positions-marks-of-a-rotated-tilted-rectangle
    robot_edge_point UL;
    UL.x = (r_center.x - (0.5 * float(_robot->width)) - r_center.x);  // Translate Center Point to Origin
    UL.y = (r_center.y + (0.5 * float(_robot->height)) - r_center.y);
    temp.x = UL.x*cos(robot_angle_rad) + UL.y*sin(robot_angle_rad);  // Apply Rotation
    temp.y = -UL.x*sin(robot_angle_rad) + UL.y*cos(robot_angle_rad);
    UL.x = temp.x + r_center.x;  // Translate Back
    UL.y = temp.y + r_center.y;
    UL.x = bottom_left_x + (UL.x / conversion_factor); // Translate to Screen space
    UL.y = bottom_left_y - (UL.y / conversion_factor);

    robot_edge_point UR;
    UR.x = (r_center.x + (0.5 * float(_robot->width)) - r_center.x);  // Translate Center Point to Origin
    UR.y = (r_center.y + (0.5 * float(_robot->height)) - r_center.y);
    temp.x = UR.x*cos(robot_angle_rad) + UR.y*sin(robot_angle_rad);  // Apply Rotation
    temp.y = -UR.x*sin(robot_angle_rad) + UR.y*cos(robot_angle_rad);
    UR.x = temp.x + r_center.x;  // Translate Back
    UR.y = temp.y + r_center.y;
    UR.x = bottom_left_x + (UR.x / conversion_factor); // Translate to Screen space
    UR.y = bottom_left_y - (UR.y / conversion_factor);

    robot_edge_point DL;
    DL.x = (r_center.x - (0.5 * float(_robot->width)) - r_center.x);  // Translate Center Point to Origin
    DL.y = (r_center.y - (0.5 * float(_robot->height)) - r_center.y);
    temp.x = DL.x*cos(robot_angle_rad) + DL.y*sin(robot_angle_rad);  // Apply Rotation
    temp.y = -DL.x*sin(robot_angle_rad) + DL.y*cos(robot_angle_rad);
    DL.x = temp.x + r_center.x;  // Translate Back
    DL.y = temp.y + r_center.y;
    DL.x = bottom_left_x + (DL.x / conversion_factor); // Translate to Screen space
    DL.y = bottom_left_y - (DL.y / conversion_factor);

    robot_edge_point DR;
    DR.x = (r_center.x + (0.5 * float(_robot->width)) - r_center.x);  // Translate Center Point to Origin
    DR.y = (r_center.y - (0.5 * float(_robot->height)) - r_center.y);
    temp.x = DR.x*cos(robot_angle_rad) + DR.y*sin(robot_angle_rad);  // Apply Rotation
    temp.y = -DR.x*sin(robot_angle_rad) + DR.y*cos(robot_angle_rad);
    DR.x = temp.x + r_center.x;  // Translate Back
    DR.y = temp.y + r_center.y;
    DR.x = bottom_left_x + (DR.x / conversion_factor); // Translate to Screen space
    DR.y = bottom_left_y - (DR.y / conversion_factor);


    robot_edge_point UM;
    UM.x = (UL.x + UR.x) / 2; // UL and UR are already Screen space -> no need to convert again
    UM.y = (UL.y + UR.y) / 2;

    oled->drawLine(UL.x, UL.y, UR.x, UR.y, SSD1306_WHITE);
    oled->drawLine(UR.x, UR.y, DR.x, DR.y, SSD1306_WHITE);
    oled->drawLine(DR.x, DR.y, DL.x, DL.y, SSD1306_WHITE);
    oled->drawLine(DL.x, DL.y, UL.x, UL.y, SSD1306_WHITE);
    oled->drawLine(UM.x, UM.y, DL.x, DL.y, SSD1306_WHITE);
    oled->drawLine(UM.x, UM.y, DR.x, DR.y, SSD1306_WHITE);

    // https://stackoverflow.com/questions/644378/drawing-a-rotated-rectangle
    // UL  =  x + ( Width / 2 ) * cos A - ( Height / 2 ) * sin A ,  y + ( Height / 2 ) * cos A  + ( Width / 2 ) * sin A
    // UR  =  x - ( Width / 2 ) * cos A - ( Height / 2 ) * sin A ,  y + ( Height / 2 ) * cos A  - ( Width / 2 ) * sin A
    // BL =   x + ( Width / 2 ) * cos A + ( Height / 2 ) * sin A ,  y - ( Height / 2 ) * cos A  + ( Width / 2 ) * sin A
    // BR  =  x - ( Width / 2 ) * cos A + ( Height / 2 ) * sin A ,  y - ( Height / 2 ) * cos A  - ( Width / 2 ) * sin A
}


void debug_disp::tick()
{
    if (_display_i2c_enabled)
    {
        if (millis() - _tick_last_millis >= DISPLAY_REFRESH_TIME)
        {
            _tick_last_millis = millis();
            oled->fillScreen(SSD1306_BLACK);

            if (draw_mode == DISPLAY_DRAW_MODE_LINE)
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
            else if (draw_mode == DISPLAY_DRAW_MODE_ROOM)
            {
                this->draw_tof(0, 0);

                this->draw_room(0, 9, float(_robot->room_height) / float(SCREEN_HEIGHT-10));
                
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

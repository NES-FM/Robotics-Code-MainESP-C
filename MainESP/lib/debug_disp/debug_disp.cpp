#include "debug_disp.h"

debug_disp::debug_disp() {};

void debug_disp::init(bool* sensor_array, bool* green_dots, unsigned char* type, signed char* angle, signed char* midfactor, int* l_sens, int* m_sens, int* r_sens, int* lm_val, int* rm_val, bool* int_sit, bool* int_bi_left, bool* int_bi_right, bool* int_bi_both, compass_hmc* comp, accel* acc, analog_sensor* volt)
{
    // Getting references to all variables to be shown on the screen
    _local_cuart_sensor_array = sensor_array;
    _local_cuart_green_dots = green_dots;
    _local_cuart_line_type = type;
    _local_cuart_line_angle = angle;
    _local_cuart_line_midfactor = midfactor;
    _l_sensor = l_sens;
    _m_sensor = m_sens;
    _r_sensor = r_sens;
    _l_motor_value = lm_val;
    _r_motor_value = rm_val;
    _interesting_situation = int_sit;
    _int_bias_left = int_bi_left;
    _int_bias_right = int_bi_right;
    _int_bias_both = int_bi_both;
    _compass = comp;
    _accelerometer = acc;
    _voltage = volt;

    if (!oled->begin(SSD1306_SWITCHCAPVCC, _i2c_address))
    {
        Serial.println(F("SSD1306 allocation failed"));
        this->enable(false);
    }
    if (_display_i2c_enabled)
    {
        oled->setRotation(2);
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
        if (_local_cuart_sensor_array[i])
            oled->fillRect(x+1+(i*element_width), y+1, element_width, element_height, SSD1306_WHITE);
    }

    // Sensor Values
    oled->setTextSize(2);
    oled->setTextColor(SSD1306_WHITE);

    oled->setCursor(x+(3*element_width)+5, y+element_height+3);
    oled->print(*_l_sensor);

    oled->setCursor(x+(10*element_width)+5, y+element_height+3);
    oled->print(*_m_sensor);

    oled->setCursor(x+(16*element_width)+7, y+element_height+3);
    oled->print(*_r_sensor);
}

void debug_disp::draw_green_dots(int x, int y, int width, int height)
{
    oled->drawLine(x + (width/2), y, x + (width/2), y + height, SSD1306_WHITE);
    oled->drawLine(x, y + (height/2), x + width, y + (height/2), SSD1306_WHITE);

    int single_width = (width-3)/2;
    int single_height = (height-3)/2;

    if (_local_cuart_green_dots[0])
        oled->fillRect(x+1, y+1, single_width, single_height, SSD1306_WHITE);
    if (_local_cuart_green_dots[1])
        oled->fillRect(x+width-single_width, y+1, single_width, single_height, SSD1306_WHITE);
    if (_local_cuart_green_dots[2])
        oled->fillRect(x+1, y+height-single_height, single_width, single_height, SSD1306_WHITE);
    if (_local_cuart_green_dots[3])
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

    switch(*_local_cuart_line_type)
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
    
    oled->printf("%+03d %+03d", *_l_motor_value, *_r_motor_value);
}

void debug_disp::draw_comp_accel(int x, int y)
{
    oled->setCursor(x, y);
    oled->setTextSize(1);
    oled->setTextColor(SSD1306_WHITE);

    oled->printf("%5.1f", _accelerometer->get_roll_degrees());
    // oled->print(0xF8);

    oled->setCursor(x, y+8);

    oled->printf("  %03d", int(_compass->get_angle()));
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

    oled->printf("%.1fV", _voltage->convert_to_battery_voltage());
    // oled->print(_voltage->get_state());
}

void debug_disp::tick()
{
    if (_display_i2c_enabled)
    {
        if (millis() - _tick_last_millis >= DISPLAY_REFRESH_TIME)
        {
            _tick_last_millis = millis();
            oled->fillScreen(SSD1306_BLACK);

            // Sensor array and numbers down below. 
            this->draw_sensor_array(0, 0, 3, 5); // W: 2+element_width*24  H: element_height+18
            
            // Ltype
            this->draw_ltype(80, 0);

            // Green Dots
            this->draw_green_dots(100, 0, 22, 22);

            // Motor Values
            this->draw_motor_values(0, 24); // W: 108px

            // Accelerometer and Compass
            this->draw_comp_accel(90, 24);

            // Disabled I2C Devices
            this->draw_disabled_i2c_devices(0, 40);

            // Battery Voltage
            this->draw_voltage(80,44);

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

#ifndef LOGGER_H
#define LOGGER_H

#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define LOGGER_BUFFLEN 256

#define LOGGER_LOG_FORMAT(format) "[%s:%u] %s():\t\t" format "\r\n", pathToFileName(__FILE__), __LINE__, __FUNCTION__
#define LOGGER_LOG_FORMAT_INLINE(format) "[%s:%u] %s():\t\t" format, pathToFileName(__FILE__), __LINE__, __FUNCTION__

#define logln(format, ...) logger_log_formatted_string(LOGGER_LOG_FORMAT(format), ##__VA_ARGS__)

#define log_inline_begin() logger_log_formatted_string(LOGGER_LOG_FORMAT_INLINE(""))
#define log_inline(format, ...) logger_log_formatted_string(format, ##__VA_ARGS__)
#define log_inline_end() logger_log_formatted_string("\r\n")

void logger_init(bool bluetoothEnabled = true);
void logger_tick();
void logger_log_formatted_string(const char *format, ...);
void logger_bluetooth_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
void logger_pasrse_command(String command);

#endif

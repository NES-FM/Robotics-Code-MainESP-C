#ifndef OTA_H
#define OTA_H

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "debug_disp.h"

// #include "secrets.h"

class OTA {
    public:
        OTA(debug_disp* deb_disp, char* s, char* p);
        void init();
        void tick();
        void check_wifi_available();
        void enable(bool enabled);
    private:
        const char* hostname = "ROBOTICS_MAIN_ESP";

        IPAddress* local = new IPAddress(192,168,178,123);
        IPAddress* dns = new IPAddress(192,168,178,1);
        IPAddress* gateway = new IPAddress(192,168,178,1);
        IPAddress* subnet = new IPAddress(255,255,255,0);

        debug_disp* display;

        void onStart() { display->ota_on_start(); };
        void onEnd() { display->ota_on_end(); };
        void onProgress(unsigned int progress, unsigned int total) { display->ota_on_progress(progress, total); };
        void onError(ota_error_t error) { display->ota_on_error(error); };

        bool ota_enabled = false;

        const char* ssid = "";
        const char* password = "";
};

#endif

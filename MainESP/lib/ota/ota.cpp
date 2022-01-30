#include "ota.h"

OTA::OTA(debug_disp* deb_disp) { 
    display = deb_disp; 
};

void OTA::enable(bool enabled) 
{
    ota_enabled = enabled;
}
/*
void OTA::check_wifi_available()
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    int n = WiFi.scanNetworks(false, false, false, 300, 3);
    if (n > 0) {
        for (int i = 0; i < n; ++i) {
            if (WiFi.SSID(i) == ssid) {
                this->enable(true);
                return;
            }
        }
    }
    this->enable(false);
}
*/
void OTA::init()
{
    if (ota_enabled)
    {
        WiFi.mode(WIFI_STA);
        WiFi.config(*local, *gateway, *subnet, *dns);
        WiFi.setHostname(hostname);
        WiFi.begin(ssid, password);
        
        if (WiFi.waitForConnectResult() != WL_CONNECTED)
        {
            this->enable(false);
            return;
        }
        
        ArduinoOTA.setPort(3232);
        ArduinoOTA.setHostname(hostname);
        ArduinoOTA.onStart([this]() { this->onStart(); });
        ArduinoOTA.onEnd([this]() { this->onEnd(); });
        ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) { this->onProgress(progress, total); });
        ArduinoOTA.onError([this](ota_error_t error) { this->onError(error); });
        ArduinoOTA.begin();
    }
}

void OTA::tick()
{
    if (ota_enabled)
        ArduinoOTA.handle();
}


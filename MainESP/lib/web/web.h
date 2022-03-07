#ifndef web_h
#define web_h

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* PARAM_INPUT = "value";
String sliderValuel = "0";
String sliderValuer = "0";

const char index_html[1814] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP Web Server</title>
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 2.3rem;}
    p {font-size: 1.9rem;}
    body {max-width: 400px; margin:0px auto; padding-bottom: 25px;}
    .slider { -webkit-appearance: none; margin: 14px; width: 360px; height: 25px; background: #FFD65C;
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background: #003249; cursor: pointer;}
    .slider::-moz-range-thumb { width: 35px; height: 35px; background: #003249; cursor: pointer; } 
  </style>
</head>
<body>
  <h2>ESP Web Server</h2>
  <p><span id="motorlValue">%SLIDERVALUEL%</span></p>
  <p><input type="range" onchange="updateMotorL(this)" id="motorl" min="0" max="50" value="%SLIDERVALUEL%" step="1" class="slider"></p>
  <p><span id="motorrValue">%SLIDERVALUER%</span></p>
  <p><input type="range" onchange="updateMotorR(this)" id="motorr" min="0" max="50" value="%SLIDERVALUER%" step="1" class="slider"></p>
<script>
function updateMotorL(element) {
  var sliderValue = document.getElementById("motorl").value;
  document.getElementById("motorlValue").innerHTML = sliderValue;
  console.log(sliderValue);
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/motorl?value="+sliderValue, true);
  xhr.send();
}
function updateMotorR(element) {
  var sliderValue = document.getElementById("motorr").value;
  document.getElementById("motorrValue").innerHTML = sliderValue;
  console.log(sliderValue);
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/motorr?value="+sliderValue, true);
  xhr.send();
}
</script>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
    //Serial.println(var);
    if (var == "SLIDERVALUEL"){
        return sliderValuel;
    }
    else if (var == "SLIDERVALUER")
    {
        return sliderValuer;
    }
    return String();
}

class web {
    public:
        void init() {
             server = new AsyncWebServer(80);
            // Route for root / web page
            server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
                request->send_P(200, "text/html", index_html, processor);
            });

            // Send a GET request to <ESP_IP>/slider?value=<inputMessage>
            server->on("/motorl", HTTP_GET, [] (AsyncWebServerRequest *request) {
                String inputMessage;
                // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
                if (request->hasParam(PARAM_INPUT)) {
                inputMessage = request->getParam(PARAM_INPUT)->value();
                sliderValuel = inputMessage;
                }
                else {
                inputMessage = "No message sent";
                }
                Serial.println(inputMessage);
                request->send(200, "text/plain", "OK");
            });
            server->on("/motorr", HTTP_GET, [] (AsyncWebServerRequest *request) {
                String inputMessage;
                // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
                if (request->hasParam(PARAM_INPUT)) {
                inputMessage = request->getParam(PARAM_INPUT)->value();
                sliderValuel = inputMessage;
                }
                else {
                inputMessage = "No message sent";
                }
                Serial.println(inputMessage);
                request->send(200, "text/plain", "OK");
            });
        }
        int get_l() {return sliderValuel.toInt();}
        int get_r() {return sliderValuer.toInt();}
    private:
        


        AsyncWebServer* server;

};

#endif

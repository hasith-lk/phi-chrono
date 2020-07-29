#ifndef ACTIONWEBCONTROL_H
#define ACTIONWEBCONTROL_H

#include "FiSystem.h"

uint8_t ledPin;

// Replaces placeholder with LED state value
String processor(const String& var) {
  Serial.println(var);
  return "";
}

bool InitActionWebControl(){
  if (!WebServerRunning){
    // Route to set GPIO to LOW
    WebServer.on("/on", HTTP_GET, [](AsyncWebServerRequest * request) {
      if(request->hasArg("port_id")){
        String sport_id = request->arg("port_id");
        ledPin = GetPortIdToPinId(sport_id);
        digitalWrite(ledPin, LOW); // low level trigger relay
      }
      request->send(200,"text/html", "");
    });

    // Route to set GPIO to LOW
    WebServer.on("/off", HTTP_GET, [](AsyncWebServerRequest * request) {
      if(request->hasArg("port_id")){
        String sport_id = request->arg("port_id");
        ledPin = GetPortIdToPinId(sport_id);
        digitalWrite(ledPin, HIGH); // low level trigger relay
      }
      request->send(200,"text/html", "");
    });
  }
}

#endif

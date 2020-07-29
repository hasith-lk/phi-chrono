#ifndef ASYNCWEBSERVER_H
#define ASYNCWEBSERVER_H

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "FiSystem.h"

bool WebServerRunning = false;

// Create AsyncWebServer object on port 80
AsyncWebServer WebServer(80);

void SetupDefaultRoutes(){
    // Route for root / web page
    WebServer.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send(LittleFS, "index.html");
    });

    WebServer.onNotFound([](AsyncWebServerRequest * request){
        request->send(404, "text/plain", "Not found");
    });
}

void StartServer(){
    // https://www.html5rocks.com/en/tutorials/cors/
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    WebServer.begin();
    WebServerRunning = true;
    Printmsg2(SERVER_STARTED, GetLocalIpAddress());

    SetupDefaultRoutes();
    
}
#endif

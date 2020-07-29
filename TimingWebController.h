#ifndef TIMINGWEBCONTROL_H
#define TIMINGWEBCONTROL_H

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "AsyncWebServer.h"
#include "TimeConfig.h"

String stringPostJson;

void restGetHandler(AsyncWebServerRequest *request){
  String method = request->url();
  method.replace("/restAPI","");
  //Serial.println("method restGetHandler");
  
  if (method.startsWith("/port"))
  {
    if (request->method() == HTTP_OPTIONS)
    {
      // https://www.html5rocks.com/en/tutorials/cors/
      AsyncWebServerResponse *response = request->beginResponse(200);
      response->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT");
      response->addHeader("Access-Control-Allow-Headers", "Content-Type, X-Requested-With, X-authentication, X-client");
      response->setContentType("application/json;charset=utf-8");
      request->send(response);
    }
      
    int paramsNr = request->params();
    if(paramsNr > 0)
    {
      if (request->method() == HTTP_GET){
        for(int i=0; i<paramsNr; i++){
          AsyncWebParameter* p = request->getParam(i);
          if (p->name().equals("port_id")){
            AsyncWebServerResponse *response = request->beginResponse(200, "application/json;charset=utf-8", getTimePorts(p->value()));
            request->send(response);
          }
        }
      }
    }
    else
    {
      if (request->method() == HTTP_GET){
        AsyncWebServerResponse *response = request->beginResponse(200, "application/json;charset=utf-8", getTimePorts());
        request->send(response);
      }
    }
  }
  request->send(200,"text/html", request->url());
}

void restPostHandler(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
  String method = request->url();
  method.replace("/restAPI","");
  //Serial.println("method restPostHandler");

  if (method.startsWith("/port"))
  {
    //Serial.println("method restPostHandler");
    if (request->method() == HTTP_OPTIONS)
    {
      // https://www.html5rocks.com/en/tutorials/cors/
      AsyncWebServerResponse *response = request->beginResponse(200);
      response->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT");
      response->addHeader("Access-Control-Allow-Headers", "Content-Type, X-Requested-With, X-authentication, X-client");
      response->setContentType("application/json;charset=utf-8");
      request->send(response);
    }
    if (request->method() == HTTP_POST)
    {
      if(!index){
        stringPostJson = "";
      }
      for (size_t i = 0; i < len; i++) {
        stringPostJson += char(data[i]);
        //Serial.print(char(data[i]));
      }
      
      if(index + len == total){
        int responseCode;
        if (writeTimeConfigJSON(stringPostJson))
          responseCode = 202;
        else
          responseCode = 404;
          
        AsyncWebServerResponse *response = request->beginResponse(responseCode);
        request->send(response);
      }
    }
  }  
}

bool InitTimingWebControl(){
    if (!WebServerRunning){
        WebServer.on("/restAPI/*", HTTP_ANY, restGetHandler, NULL, restPostHandler);
        WebServer.on("/time/index.html", HTTP_GET, [](AsyncWebServerRequest * request) {
          request->send(LittleFS, "/time/index.html");
        });
        WebServer.on("/time/timeJS.js", HTTP_GET, [](AsyncWebServerRequest * request) {
          request->send(LittleFS, "/time/timeJS.js");
        });
    }
}

#endif

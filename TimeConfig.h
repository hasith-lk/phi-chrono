#ifndef TIMECONFIG_H
#define TIMECONFIG_H

#include <ArduinoJson.h>
#include <LittleFS.h>
#include <time.h>
#include "FiSystem.h"
#include "Messages.h"

#define DOC_SIZE 512

const char * FOLDER_PATH = "AppConfig/Time";

void readTimeConfigJSON(StaticJsonDocument<DOC_SIZE> *doc, String folderPath, String filename)
{
  String qFilePath = folderPath + "/" +filename;
  
  if (LittleFS.exists(qFilePath)){
    File file = LittleFS.open(qFilePath, "r");
    if (!file) {
      Printmsg(FAILED_OPEN_FILE);
    }
    
    DeserializationError error = deserializeJson(*doc, file);
    if (error)
      Printmsg(FILE_DESERIALIZE_FAILED);
    file.close();
  }
  else
    Printmsg(FILE_READ_FAILED);
}

void listConfigPorts(StaticJsonDocument<DOC_SIZE> *doc, String folderPath)
{
  Dir root = LittleFS.openDir(folderPath);
  JsonArray ports = doc->createNestedArray("ports");
  while (root.next()) {
    File file = root.openFile("r");
    String fileName = file.name();
    if (fileName.endsWith(".json"))
    {
      StaticJsonDocument<DOC_SIZE> localDoc;
      DeserializationError error = deserializeJson(localDoc, file);
      if (!error){
        String portName = localDoc["port_id"];
        ports.add(portName);
      }
    }
  }
}

String getTimePorts(){
  StaticJsonDocument<DOC_SIZE> doc;
  listConfigPorts(&doc, FOLDER_PATH);
  String retVal;
  serializeJson(doc, retVal);
  return retVal;
}

String getTimePorts(String portId){
  StaticJsonDocument<DOC_SIZE> doc;
  readTimeConfigJSON(&doc, FOLDER_PATH, "port_"+ portId +".json");
  String retVal;
  serializeJson(doc, retVal);
  return retVal;
}

TimeFormatArray getTimeForPort(String portId){
  
  StaticJsonDocument<DOC_SIZE> doc;
  readTimeConfigJSON(&doc, FOLDER_PATH, "port_"+ portId +".json");
  
  JsonObject portTiming = doc.as<JsonObject>();
  JsonArray timingData = portTiming["data"];
  
  TimeFormat* timeData = new TimeFormat[timingData.size()];
  for (int i=0; i<timingData.size(); i++)
  {
    JsonObject timeRecord = ((JsonVariant)timingData[i]).as<JsonObject>();

    String time = timeRecord["time"];
    uint8_t duration = timeRecord["duration"];

    String hour = time.substring(0, time.indexOf(":"));
    String minute = time.substring(time.indexOf(":")+1);
    timeData[i].Hour = hour.toInt();
    timeData[i].Minute = minute.toInt();
    timeData[i].Duration = duration;
    //Serial.println("hour " + hour +", minute " + minute);
  }
  TimeFormatArray timeArray;
  timeArray.array = timeData;
  timeArray.count = timingData.size();

  return timeArray;
}

// For DEBUG
void printlistConfigPorts(String folderPath){
  StaticJsonDocument<DOC_SIZE> doc;
  listConfigPorts(&doc, folderPath);
  serializeJsonPretty(doc, Serial);
}

void PrintConfigJSON(String folderPath, String filename){
  StaticJsonDocument<DOC_SIZE> doc;
  readTimeConfigJSON(&doc, folderPath, filename);
  serializeJsonPretty(doc, Serial);
}



bool writeTimeConfigJSON(String json)
{
  StaticJsonDocument<DOC_SIZE> doc;
  DeserializationError error = deserializeJson(doc, json);
  if(error)
    return false;

  //serializeJsonPretty(doc, Serial);
  JsonObject object = doc.as<JsonObject>();
  
  String portId = object["port_id"];
  //Serial.println("\nportId " + portId);
    
  String folderPath = FOLDER_PATH;
  String qFilePath = folderPath + "/port_"+ portId +".json";

  //Serial.println("qFilePath " + qFilePath);
  
  File file = LittleFS.open(qFilePath, "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return false;
  }
  if (file.print(json))
  {
    //Serial.println(json);
    file.close();
    return true;
  }
  else
  {
    file.close();
    return false;
  }
  return true;
}


void listDir(const char * dirname) {
  Serial.printf("Listing directory: %s\n", dirname);

  Dir root = LittleFS.openDir(dirname);

  while (root.next()) {
    File file = root.openFile("r");
    Serial.print("  FILE: ");
    Serial.print(root.fileName());
    Serial.print("  SIZE: ");
    Serial.print(file.size());
    time_t t = file.getLastWrite();
    struct tm * tmstruct = localtime(&t);
    file.close();
    Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
  }
}

#endif

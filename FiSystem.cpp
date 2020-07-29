#include "Arduino.h"
#include "FiSystem.h"
#include "FiConfig.h"
#include "Messages.h"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <ESP8266mDNS.h>        // Include the mDNS library
#include "ClockConfig.h"
//#include "LcdConfig.h"

struct PortMapping_t{
  String port_id;
  uint8_t pin_id;
};

struct SysConfig_t{
  String network_id;
  String network_pwd;
  uint8_t network_attempts;
  String device_id;
  String device_key;
  PortMapping_t *portMapping;
  uint8_t port_count;
}*sysConfig;

IPAddress systemIp;

bool SetupFileSystem(void);
void InitSysConfig(void);
bool InitDevices(void);
bool SetupOutPorts();
void SetPortDefault();

void InitConfig(){
    Serial.begin(115200);
    // Set port defaults
    SetPortDefault();
    InitDevices();
    
    // Setup File System
    if (!SetupFileSystem()) {
      Printmsg(FILE_SYSTEM_SETUP_FAILED);
      return;
    }
    // Read System configuration file data
    InitSysConfig();  
    // Set port mapping
    SetupOutPorts();
}

void readSysConfigJSON(StaticJsonDocument<SYS_DOC_SIZE> *doc, String folderPath, String filename)
{
  String qFilePath = folderPath + "/" +filename;
  Printmsg("file path " + qFilePath);
  
  if (LittleFS.exists(qFilePath)){
    File file = LittleFS.open(qFilePath, "r");
    if (!file) {
      Printmsg(FAILED_OPEN_SYS_FILE);
    }
    
    DeserializationError error = deserializeJson(*doc, file);
    if (error)
      Printmsg(FAILED_SYS_FILE_JSON);
    file.close();
  }
  else
    Printmsg(FAILED_SYS_FILE);
}

void InitSysConfig(){
  StaticJsonDocument<SYS_DOC_SIZE> sysConfigDoc;
  readSysConfigJSON(&sysConfigDoc, SYS_CONFIG_FOLDER_PATH, SYS_CONFIG_FILE_NAME);

  JsonObject sysObject = sysConfigDoc.as<JsonObject>();
  
  String network_id = sysObject["network_id"];
  String network_pwd = sysObject["network_pwd"];
  uint8_t network_attempts = sysObject["network_attempts"];
  String device_id = sysObject["device_id"];
  String device_key = sysObject["device_key"];
  
  JsonArray jMappingArray = sysObject["pin_mapping"];
  PortMapping_t *portMapping = new PortMapping_t[jMappingArray.size()];
  PortMapping_t *portMappingOriginal = portMapping;

  for (int i=0; i<jMappingArray.size(); i++)
  {
    JsonObject portMappingObject = ((JsonVariant)jMappingArray[i]).as<JsonObject>();

    String port_id = portMappingObject["port_id"];
    uint8_t pin_id = portMappingObject["pin_id"];    
    portMapping->port_id = port_id;
    portMapping->pin_id = pin_id;

    // Printmsg("Pin Id " + portMapping->pin_id + " port id " + portMapping->port_id);
    portMapping++;
  }

  // portMapping = portMappingOriginal;
  /*for (int j=0; j<jMappingArray.size(); j++){
    Printmsg("j " + j);
    Printmsg("Pin Id X " + (portMapping+j)->pin_id + " port id X " + (portMapping+j)->port_id);
  }*/

  sysConfig = new SysConfig_t;
  sysConfig->network_id = network_id;
  sysConfig->network_pwd = network_pwd;
  sysConfig->network_attempts = network_attempts;
  sysConfig->device_id = device_id;
  sysConfig->device_key = device_key;
  sysConfig->port_count = jMappingArray.size();
  sysConfig->portMapping = portMappingOriginal;
  
  //Printmsg(sysConfig->network_id);
  //Printmsg(sysConfig->network_pwd);
}

void Printmsg(String msg){
  Serial.println(msg);
  if (USE_LCD)
    LcdPrint(msg);
}

void Printmsg2(String msg1, String msg2){
  Serial.println(msg1);
  Serial.println(msg2);
  if (USE_LCD)
    LcdPrint(msg1, msg2);
}

void PrintAppendmsg(String msg){
  Serial.println(msg);
  if (USE_LCD)
    LcdPrint(msg, true);
}

bool SetupFileSystem(void){
  if (!LittleFS.begin())
  {
    Printmsg(FILE_SYS_FAILED);
    return false;
  }
  else
    return true;
}

bool ConnectToNetwork(){  
  // Connect to Wi-Fi
  WiFi.begin(sysConfig->network_id, sysConfig->network_pwd);
  int attemptCount = 0;

  while (WiFi.status() != WL_CONNECTED && attemptCount < sysConfig->network_attempts ) {
    delay(1000);
    Printmsg2(CONNECT_WIFI, String(attemptCount+1));
    attemptCount++;
  }

  systemIp = WiFi.localIP();
  // Print ESP32 Local IP Address
  // Serial.println(WiFi.localIP());
  return (WiFi.status() != WL_CONNECTED);
}

void SetPortDefault(){
  digitalWrite(D5, HIGH);
  digitalWrite(D6, HIGH);
  digitalWrite(D7, HIGH);
}

bool SetupOutPorts(){
  PortMapping_t *portMapping = sysConfig->portMapping;
  for (int j=0; j<sysConfig->port_count; j++){
    pinMode((portMapping+j)->pin_id, OUTPUT);
    // Printmsg("Pin Id X " + (portMapping+j)->pin_id + " port id X " + (portMapping+j)->port_id);
  }
}

bool StartMDNS(){
  if (!MDNS.begin(sysConfig->device_id)) {             // Start the mDNS responder for esp8266.local
    Printmsg(MDNS_FAIL);
  }
  else
    Printmsg(MDNS_SUCCESS);
}

bool InitDevices(void){
  InitLcd();
  InitRtcTime();
  return true;
}

uint8_t GetPortCount(){
  return sysConfig->port_count;
}

uint8_t *GetPortPinList(){
  uint8_t *pinList = new uint8_t[sysConfig->port_count];
  PortMapping_t *portMapping = sysConfig->portMapping;
  for (int j=0; j<sysConfig->port_count; j++){
    pinList[j] = (portMapping+j)->pin_id;
  }
  return pinList;
}

String *GetPortList(){
  String *portList = new String[sysConfig->port_count];
  PortMapping_t *portMapping = sysConfig->portMapping;
  for (int j=0; j<sysConfig->port_count; j++){
    portList[j] = (portMapping+j)->port_id;
  }
  return portList;
}

uint8_t GetPortIdToPinId(String portId){
  PortMapping_t *portMapping = sysConfig->portMapping;
  for (int j=0; j<sysConfig->port_count; j++){
    if((portMapping+j)->port_id == portId)
      return (portMapping+j)->pin_id;
  }
}

TimeFormat GetCurrentTimeInFormat(){
  TimeFormat t;
  GetCurrentHourMinute(&t.Hour, &t.Minute);
  t.Duration = 0;
  return t;
}

String GetLocalIpAddress(){
  return String(systemIp[0]) + String(".") +\
  String(systemIp[1]) + String(".") +\
  String(systemIp[2]) + String(".") +\
  String(systemIp[3]); 
}

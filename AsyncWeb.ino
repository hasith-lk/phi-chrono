// Import required libraries


#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>

#include "FiConfig.h"
#include "FiSystem.h"
#include "Messages.h"
#include "TimingWebController.h"
#include "ActionWebController.h"

#include "TimerTask.h"
#include "TimeConfig.h"

void setup() {
  
  // Init Serial port for debugging purposes / Read Config
  InitConfig();

  // Connect to Wi-Fi
  ConnectToNetwork();
  // Start DNS
  StartMDNS();

  InitTimingWebControl();
  InitActionWebControl();
  
  StartServer();
  PrintAppendmsg(GetCurrentTimeStr());
  //Printmsg(SERVER_STARTED);
  //LcdPrint(SERVER_STARTED, "LCD OK");

  // This should be the last line
  InitTimerTasks();

}

void loop() {

}

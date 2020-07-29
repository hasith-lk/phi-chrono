#ifndef FISYSTEM_H
#define FISYSTEM_H

#define SYS_DOC_SIZE 512

#include "LcdConfig.h"

typedef struct TimeFormat{
  uint8_t Hour;
  uint8_t Minute;
  uint8_t Duration;

  bool operator>(TimeFormat t) const{
    if(Hour > t.Hour || (Hour == t.Hour && Minute > t.Minute))
      return true;
    else
      return false;
  }

  bool operator<(TimeFormat t) const{
    if(Hour < t.Hour || (Hour == t.Hour && Minute < t.Minute))
      return true;
    else
      return false;
  }

  bool operator==(TimeFormat t) const{
    if(Hour == t.Hour && Minute == t.Minute)
      return true;
    else
      return false;
  }
};

typedef struct TimeFormatArray{
  TimeFormat* array;
  uint8_t count;
};

extern "C" void InitConfig();
extern "C" void Printmsg(String msg);
extern "C" void PrintAppendmsg(String msg);
extern "C" void Printmsg2(String msg1, String msg2);
extern "C" bool ConnectToNetwork();
extern "C" bool SetupOutPorts();
extern "C" bool StartMDNS();
extern "C" String GetLocalIpAddress();
extern "C" String GetCurrentTimeStr();

extern "C" String GetNetworkId();
extern "C" String GetDeviceId();
extern "C" String GetDeviceKey();
extern "C" uint8_t GetPortCount();
extern "C" uint8_t *GetPortPinList();
extern "C" String *GetPortList();
extern "C" uint8_t GetPortIdToPinId(String portId);
extern "C" TimeFormat GetCurrentTimeInFormat();
#endif

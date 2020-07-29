#ifndef CLOCKCONFIG_H
#define CLOCKCONFIG_H

#define countof(a) (sizeof(a) / sizeof(a[0]))

#include <Wire.h>
#include <RtcDS3231.h>
extern "C" void Printmsg(String msg);

RtcDS3231<TwoWire> Rtc(Wire);

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

String GetCurrentTimeStr(){
  RtcDateTime now = Rtc.GetDateTime();
  char time_[10];
  snprintf_P(time_, 
          countof(time_),
          PSTR("%02u:%02u:%02u"),
          now.Hour(),
          now.Minute(),
          now.Second());
  return String(time_);
}

uint8_t GetCurrentHour(){
  RtcDateTime now = Rtc.GetDateTime();
  return now.Hour();
}

uint8_t GetCurrentMinute(){
  RtcDateTime now = Rtc.GetDateTime();
  return now.Minute();
}

void GetCurrentHourMinute(uint8_t *hour, uint8_t *minute){
  RtcDateTime now = Rtc.GetDateTime();
  *hour = now.Hour();
  *minute = now.Minute();
}

bool InitRtcTime()
{
  bool updateTimeFromServer = SETUP_TIME ;
  if (updateTimeFromServer){
    updateTimeFromServer = false;
  }

  Rtc.Begin(SDA, SCL);
  if (!Rtc.IsDateTimeValid()) 
  {
      if (Rtc.LastError() != 0)
      {
          // we have a communications error
          // see https://www.arduino.cc/en/Reference/WireEndTransmission for 
          // what the number means
          String msg = ERROR_CODE_ON_RTC_TIME + Rtc.LastError() ;
          Printmsg(msg);
          return false;
      }
      else
      {
        Printmsg(ERROR_ON_RTC_TIME);
        return false;
      }
  }

  if (!Rtc.GetIsRunning())
  {
      Printmsg(STARTING_RTC_TIME);
      Rtc.SetIsRunning(true);
      delay(100);
  }

  //Serial.println(GetCurrentTimeStr());
  Printmsg("Current Time " + GetCurrentTimeStr());

  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);
  return true;
}
#endif

#ifndef TIMERTASK_H
#define TIMERTASK_H

#include <Scheduler.h>
#include "FiSystem.h"
#include "TimeConfig.h"

#define RELAY_ON LOW
#define RELAY_OFF HIGH

class TimerTask : public Task {

  public:
    TimerTask(uint8_t pin_id, String portId) {
      this->pin_id = pin_id;
      this->portId = portId;
      this->timingArray = getTimeForPort(portId);
    }

  protected:
    void setup() {
      state = RELAY_OFF;

      pinMode(pin_id, OUTPUT);
      digitalWrite(pin_id, state);
    }

    void loop() {
      /*state = state == HIGH ? LOW : HIGH;
      digitalWrite(pin_id, state);
      int delayTimeLocal = delayTime;
      delay(delayTimeLocal);*/
      TimeFormat currentTime = GetCurrentTimeInFormat();
      TimeFormat minTime = currentTime;
      minTime.Minute = minTime.Minute - 1;
      TimeFormat maxTime = currentTime;
      maxTime.Minute = maxTime.Minute + 1;
      
      TimeFormat *timing = timingArray.array;
      for (int i=0; i < timingArray.count; i++)
      {
        if ((timing[i] > minTime) && (timing[i] < maxTime))
        {
          state = RELAY_ON;
          digitalWrite(pin_id, state);
          Printmsg("Port "+ portId +" On");
          delay(timing[i].Duration*60*1000);
          state = RELAY_OFF;
          digitalWrite(pin_id, state);
          Printmsg("Port "+ portId +" Off");
        }
      }
      delay(delayTime);
    }

  private:
    uint8_t state; // Since relay use low level trigger ON state = LOW
    uint8_t pin_id;
    String portId;
    long delayTime = 60*1000;
    TimeFormatArray timingArray;
};

bool InitTimerTasks(){
  uint8_t *pinList = GetPortPinList();
  String *portList = GetPortList();
  uint8_t count = GetPortCount();

  for (int i=0; i< count; i++){
    uint8_t pinId = pinList[i];
    String portId = portList[i];
    
    TimerTask *localTask = new TimerTask(pinId, portId);
    Scheduler.start(localTask);
  }
  Scheduler.begin();
}

#endif

#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "Process.h"

class BuzzerProcess : public Process
{  
  public:
  
  BuzzerProcess(int buzzPin);
 
  virtual void setup();
  void turnTone(int buzzFrequency,int buzzDuration);
  void turnTone(int buzzFrequency);
    
  void turnOn();
  void turnOff();

  bool isActive();

  virtual void loopStep();
  
  
  protected:
  
  enum         {BUZZER_OFF, BUZZER_CHIRP, BUZZER_ON};
  int          pin;
  int          state;
  unsigned int buzzSwitchTimestamp;
  unsigned int buzzerFrequency;
  int          timeOn;
 };

 #endif


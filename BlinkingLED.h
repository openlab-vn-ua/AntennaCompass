#ifndef __BLINKINGLED_H__
#define __BLINKINGLED_H__

#include "Process.h"

class BlinkingLED : public Process
{  
  public:
  
  BlinkingLED(int ledPin);
 
  virtual void setup();
  void turnBlink(int blinkOnTimeInMs,int blinkOffTimeInMs);
  void turnBlink(int blinkPeroidInMs);
    
  void turnOn();
  void turnOff();

  bool isActive();

  virtual void loopStep();
  
  protected:
  
  enum         {LED_OFF, LED_BLINKING, LED_ON};
  int          pin;
  int          state;
  unsigned int blinkSwitchTimestamp;
  int          timeOn;
  int          timeOff;  
 };

 #endif


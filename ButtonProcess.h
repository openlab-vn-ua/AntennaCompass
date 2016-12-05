#ifndef __BUTTON_PROCESS_H__
#define __BUTTON_PROCESS_H__

#include "Process.h"
#include "Button.h"
#include "CompassProcess.h"
#include "Enerlib.h"
#include "LowPower.h"
#include "BlinkingLed.h"


class ButtonProcess : public Process
{  
  public:
  
  ButtonProcess(Button *button, Energy *energy, LowPowerClass *LowPower, CompassProcess *compass, BlinkingLED *led);
  void handleInterrupt();
 
  virtual void setup();

  virtual void loopStep();

  
  protected:
  
  enum         {STATE_WAKING_UP, STATE_CALIBRATING, STATE_MEASURING,  STATE_SLEEPING, STATE_GOING_SLEEP};
  
  Energy        *energy;
  LowPowerClass *LowPower;
  Button        *button;
  CompassProcess       *compass;
  BlinkingLED   *led;
  int           pin;
  long          lastChangeTime;
  long          startCalibrationTime;
  long          pressTime;
  int           state;
  bool          wasPress;
  
  void          startCompassAutoCalibration();
 };

 #endif

#ifndef __BEARING_DETECTOR_H__
#define __BEARING_DETECTOR_H__

#include "Process.h"
#include "BlinkingLED.h"
#include "CompassProcess.h"
#include "BuzzerProcess.h"

class BearingDetector : public Process
{     
  public:

  BearingDetector(BlinkingLED *led, BuzzerProcess *buzz, CompassProcess *comp);
 
  virtual void setup();

  virtual void loopStep();
  
  protected:

  BlinkingLED  *led;
  CompassProcess *comp;
  BuzzerProcess  *buzz;
  long         startTime;
  int          count;
  float        summ;
  float        current_angle;

  private :
  
  bool bearingIsOutsideRange(float angle);
  void signalOutsideRange();

  bool bearingIsInFarRange(float angle);
  void signalInFarRange(float angle);

  bool bearingIsInAzimutRange(float angle);
  void signalInAzimutRange();

  int detectBlinkingFrequency(int angle);

  float filter (float current, float real);
};

#endif

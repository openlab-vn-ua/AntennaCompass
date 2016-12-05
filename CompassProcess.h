#ifndef __COMPASS_H__
#define __COMPASS_H__

#include "Process.h"
#include <SPHMC8337L2.h>

class CompassProcess : public Process
{  
  public:

  CompassProcess(int Compas_i2cAddr, int PowerPin);
  long  startCalibration;
  int durationOfCalibrating;
  
  void setup();
  void setCalibration(float calibrX, float calibrY, float calibrZ);
  void configureDevice();

  void startCompassCalibration (long duration);
  bool isCalibrating () const;

  float getBearing();

  void loopStep();

  void turnOff();
  void turnOn();
  bool isOn();
  
  protected:

  float fromRadiansToDegress (float degreeInRad);
  float calcBearing (float radX, float radY, float radZ);
  SPHMC5883L  compass;
  bool isCalibratingNow;  
  int powerPin;
  bool isON;
};

#endif

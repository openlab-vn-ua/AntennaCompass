#include <Arduino.h>
#include <SPHMC8337L2.h>
#include "CompassProcess.h"
#include <Wire.h>

extern "C" 
{
#include "utility/twi.h"
}

#define COMPASS_CONFIGURATION_DELAY   (134)
#define COMPASS_CALM_DOWN_TIME        (50)
#define ADDRESS_CALIBRATION_DATA      (0)
  
  CompassProcess::CompassProcess(int Compass_i2cAddr, int PowerPin)
  : compass(Compass_i2cAddr)
  {
    powerPin = PowerPin;
    isCalibratingNow = false;
    startCalibration = 0;
    durationOfCalibrating = 0;
    isON = true;
  }
  
 void CompassProcess::setup()
 {
   pinMode(powerPin, OUTPUT);
   digitalWrite(powerPin, HIGH);
   delay(COMPASS_CALM_DOWN_TIME);
   if (compass.hasSavedCalibrationData(ADDRESS_CALIBRATION_DATA))
   {
     compass.loadCalibrationData(ADDRESS_CALIBRATION_DATA);
   }
   configureDevice();
 }
 
 void CompassProcess::setCalibration(float calibrX, float calibrY, float calibrZ)
 {
  compass.setCalibration(calibrX,calibrY,calibrZ);
 }

 void CompassProcess::configureDevice()
 {
  compass.configure(SPHMC5883L::SAMPLES_8,
                    SPHMC5883L::DATA_RATE_15,
                    SPHMC5883L::GAIN5,
                    SPHMC5883L::MODE_CONTINUOUS);
   
   delay(COMPASS_CONFIGURATION_DELAY);
 }

  void CompassProcess::startCompassCalibration (long duration)
  {
   startCalibration = millis();
   isCalibratingNow = true;
   durationOfCalibrating = duration;
   compass.startAutoCalibration();
  }
  
  bool CompassProcess::isCalibrating () const
  {
    return isCalibratingNow;
  }
 
 float CompassProcess::fromRadiansToDegress (float degreeInRad)
 {
  return (degreeInRad * 180.0f / 3.1415927f);
 }

 float CompassProcess::calcBearing (float radX, float radY, float radZ)
 {
  return (atan2f(radY,radX));
 }

 float CompassProcess::getBearing()
 {
  float x,y,z;

  if (!isOn()) return 0.0f;
  
  compass.readXYZ(&x,&y,&z);

  float bearing = fromRadiansToDegress(calcBearing(x, y, z));

  if (bearing < 0.0f)
   {
    bearing += 360.0f;
   }

  return bearing;
 }

 void CompassProcess::turnOff()
 {
  isON = false;
  twi_disable();
  delay(COMPASS_CALM_DOWN_TIME);
  digitalWrite(powerPin, LOW);
 }

  void CompassProcess::turnOn()
 {
  isON = true;
  digitalWrite(powerPin, HIGH);  
  delay(COMPASS_CALM_DOWN_TIME);
  Wire.begin(); 
  configureDevice();
 }

 bool CompassProcess::isOn()
 {
  return isON;
 }

  void CompassProcess::loopStep()
  {
    if (!isOn()) return;
    if (isCalibrating())
    {
      if (millis() - startCalibration < durationOfCalibrating)
      {
       int16_t x;
       int16_t y;
       int16_t z;
       compass.readXYZRaw(&x, &y, &z);
      }
      else
      {                  
       float offsetX,offsetY,offsetZ;
       
       compass.getAutoCalibration(&offsetX,&offsetY,&offsetZ); 
       compass.setCalibration(offsetX,offsetY,offsetZ);                         
       compass.saveCalibrationData (ADDRESS_CALIBRATION_DATA);
       compass.stopAutoCalibration();
       isCalibratingNow = false;
      }
    }  
  }




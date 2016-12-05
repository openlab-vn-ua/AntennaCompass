#include <SPHMC8337L2.h>

#include <Arduino.h>

#include "BearingDetector.h"

#define minLimit (180)
#define maxLimit (220)
#define minAzimut (195)
#define maxAzimut (205)
#define usualFrequency (1000)
#define azimutFrequency (50)
#define buzzFrequensyInFarLimit (500)
#define buzzDurationInFarLimit (1000)
#define buzzFrequensyInAzimutRange (1000)
#define buzzDurationInAzimutRange (200)
#define minRoundingTolerance (10)
#define maxRoundingTolerance (maxAngleInDegress - minRoundingTolerance)
#define maxAngleInDegress (360)
#define PERIOD (200)
#define k 0.5f


 BearingDetector::BearingDetector(BlinkingLED *led, BuzzerProcess *buzz, CompassProcess *comp)
 {
  this->led = led;
  this->comp = comp;
  this->buzz = buzz;
  startTime = 0;
  count = 0;
  summ = 0.0f;
  current_angle = 0.0f;
 }

 bool BearingDetector::bearingIsOutsideRange(float angle)
 {
   return ((angle < minLimit)||(angle > maxLimit));
 }
 
 void BearingDetector::signalOutsideRange()
 {
  led->turnBlink(usualFrequency);
  buzz->turnOff();
 }

 bool BearingDetector::bearingIsInFarRange(float angle)
 {
  return ((angle < minAzimut)||(angle > maxAzimut));
 }
 
 void BearingDetector::signalInFarRange(float angle)
 {
  if (angle < minAzimut)
  {
    angle = minAzimut - angle;
  }
  else 
  {
    angle = angle - maxAzimut;
  }
  
  led->turnBlink(detectBlinkingFrequency(angle));
  buzz->turnTone(buzzFrequensyInFarLimit, buzzDurationInFarLimit);
 }

 bool BearingDetector::bearingIsInAzimutRange(float angle)
 {
  return ((angle > minAzimut) && (angle < maxAzimut));
 }
 
 void BearingDetector::signalInAzimutRange()
 {
  led->turnBlink(azimutFrequency);
  buzz->turnTone(buzzFrequensyInAzimutRange, buzzDurationInAzimutRange);
 }

 int BearingDetector::detectBlinkingFrequency (int angle)
 {
  return (pow(angle,2) + 50);
 }

 
 void BearingDetector::setup()
 {
 }

 float BearingDetector::filter (float current, float real)
 {
  float result;
  
  if ((current > maxRoundingTolerance) && (real < minRoundingTolerance))    
  {
    real += maxAngleInDegress;
    result = (current * k + real * (1 - k)) - maxAngleInDegress;
  }
  else if ((current < minRoundingTolerance) && (real > maxRoundingTolerance))
  {
    current += maxAngleInDegress;
    result = (current * k + real * (1 - k)) - maxAngleInDegress;
  }
  else
  {
    result = current * k + real * (1 - k);    
  }

 if (result < 0)
 {
  result += maxAngleInDegress;
 }
 
 return result;
 }

 void BearingDetector::loopStep()
 {
  float real_angle;

  if (!comp->isCalibrating())    
  {  
    Serial.print(" bearing(degrees): "); 
    Serial.println(comp->getBearing()); 
  }
  
  real_angle = filter(current_angle, comp->getBearing());

    if (bearingIsOutsideRange(real_angle))
     {
      signalOutsideRange();
     }
    else if (bearingIsInFarRange(real_angle))
     {
      signalInFarRange(real_angle);
     }
    else if (bearingIsInAzimutRange(real_angle))
     {
      signalInAzimutRange();
     }  
     
  current_angle = real_angle; 
 }

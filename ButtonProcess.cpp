#include <LowPower.h>
#include <Enerlib.h>
#include <Wire.h>
#include <SPHMC8337L2.h>
#include <Arduino.h>
#include "ButtonProcess.h"

extern "C" 
{
#include "utility/twi.h"
}

#define GOING_CALIBRATION_TIME        (5000)
#define CALIBRATION_TIME              (30000)

static ButtonProcess *buttonProcess = NULL;

static void INT0_ISR ()
{
  buttonProcess->handleInterrupt();
}

 ButtonProcess::ButtonProcess(Button *button, Energy *energy, LowPowerClass *LowPower, CompassProcess *compass, BlinkingLED *led)
  {
    this->energy = energy;
    this->button = button;
    this->compass = compass;
    this->led = led;
    this->LowPower = LowPower;
    state = STATE_MEASURING;
    buttonProcess = this;
    lastChangeTime = 0;
    pressTime = 0;
    startCalibrationTime = 0;
    wasPress = false;
  }

  void ButtonProcess::setup()
  {
  }
 
  void ButtonProcess::handleInterrupt()
  {
    Serial.print("INTERRUPT IS CALLING");
    Serial.println();
    Serial.print("new state:   STATE_WAKING_UP");
    Serial.println();
    state = STATE_WAKING_UP;
  }


  void ButtonProcess::loopStep()
  {  
   if (state == STATE_MEASURING)
   {         
      if (button->isButtonPressed())
      {
       Serial.print("new state:   STATE_GOING_SLEEP");
       Serial.println();
       state = STATE_GOING_SLEEP;
      }
   }
   else if (state == STATE_CALIBRATING)
   {
    if (!compass->isCalibrating ())
    {     
     startCalibrationTime = millis();
     compass->startCompassCalibration(CALIBRATION_TIME); 
    }
    else if (millis() - startCalibrationTime > CALIBRATION_TIME)
    {
     state = STATE_MEASURING;
     Serial.print("new state:   STATE_MEASURING");
     Serial.println();
    }
   }
   else if (state == STATE_WAKING_UP)
   {
    if (!compass->isOn())  
    {
     compass->turnOn();
    } 
    if (button->isButtonPressed())
    {
      if (button->getDuration() > GOING_CALIBRATION_TIME)
      {
       Serial.print("new state:   STATE_CALIBRATING");
       Serial.println();
       state = STATE_CALIBRATING;
      }
    }
    else if (button->getDuration() >= 100)
    {
     Serial.print("new state:   STATE_MEASURING");
     Serial.println();
     Serial.print(button->getDuration());
     Serial.println();
     state = STATE_MEASURING;
    }
   }   
   else if (state == STATE_GOING_SLEEP)  
   {
    if (!button->isButtonPressed())
    {
     Serial.print("new state:   STATE_SLEEPING");
     Serial.println();
     compass->turnOff();
     led->turnOff();
     attachInterrupt(0, INT0_ISR, FALLING);
     LowPower->powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
     detachInterrupt(0);
    }
   }
   else if (state == STATE_SLEEPING)
   {
     //nothing to do
   }  
  }   
 

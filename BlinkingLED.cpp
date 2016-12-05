#include <Arduino.h>
 
#include "BlinkingLED.h"
 
  BlinkingLED::BlinkingLED(int ledPin)
  {
    pin = ledPin;
    timeOn = 0;
    timeOff = 0;
    state = LED_OFF;
  }
  
  void BlinkingLED::setup()
  {
    pinMode(pin,OUTPUT);
  }

  void BlinkingLED::turnBlink(int blinkOnTimeInMs,int blinkOffTimeInMs)
  {
   if ((state == LED_BLINKING)&&(timeOn == blinkOnTimeInMs)&&(timeOff == blinkOffTimeInMs))
    { 
    }
   else
   {
    state = LED_BLINKING;
    timeOn  = blinkOnTimeInMs;
    timeOff = blinkOffTimeInMs;
    digitalWrite(pin,HIGH);
    blinkSwitchTimestamp = millis();
   }
  }

  void BlinkingLED::turnBlink(int blinkPeroidInMs)
  {
    turnBlink(blinkPeroidInMs, blinkPeroidInMs);
  }   
    
  void BlinkingLED::turnOn()
  {
    state = LED_ON;
    digitalWrite(pin,HIGH);   
  }

  void BlinkingLED::turnOff()
  {
    state = LED_OFF;
    digitalWrite(pin,LOW);
  }

  bool BlinkingLED::isActive()
  { 
   return (state != LED_OFF);
  }

  void BlinkingLED::loopStep()
  {
    if (state == LED_ON)
    {
      // OK, nothing to do 
    }      
    else if (state == LED_BLINKING)
    {
      // We already blinking
      int now = millis();
      int spentTime = (now - blinkSwitchTimestamp);
      
      if ((digitalRead(pin) == HIGH) && (spentTime < timeOn))
      {
        // OK, keep the state
      }
      else if ((digitalRead(pin) == LOW) && (spentTime < timeOff))
      {
       // OK, keep state
      }      
      else
      {
        blinkSwitchTimestamp = millis();
        if (digitalRead(pin) == LOW)
        {
          digitalWrite(pin,HIGH);
        }
        else
        {
          digitalWrite(pin,LOW);
        }         
      }      
    }  
  }




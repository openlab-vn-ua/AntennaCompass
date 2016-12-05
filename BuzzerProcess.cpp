#include <Arduino.h>
 
#include "BuzzerProcess.h"

#define TURN_ON_FREQUENCY      (1000)
  
  BuzzerProcess::BuzzerProcess(int buzzPin)
  {
    pin = buzzPin;
    timeOn = 0;
    state = BUZZER_OFF;
    buzzerFrequency = 0;
  }
  
  void BuzzerProcess::setup()
  {
    pinMode(pin,OUTPUT);
  }

  void BuzzerProcess::turnTone(int buzzFrequency,int buzzDuration)
  {
   if ((state == BUZZER_CHIRP)&&(timeOn == buzzDuration))
    { 
     //nothing to do
    }
   else
   {
    state = BUZZER_CHIRP;
    timeOn  = buzzDuration;

    buzzerFrequency = buzzFrequency;
    
    tone(pin, buzzFrequency);
    
    buzzSwitchTimestamp = millis();
   }
  }
 
    
  void BuzzerProcess::turnOn()
  {
    state = BUZZER_ON;
    tone(pin, TURN_ON_FREQUENCY);   
  }

  void BuzzerProcess::turnOff()
  {
    state = BUZZER_OFF;
    noTone(pin);
  }

  bool BuzzerProcess::isActive()
  { 
   return (state != BUZZER_OFF);
  }

  void BuzzerProcess::loopStep()
  {
    if (state == BUZZER_ON)
    {
      // OK, nothing to do 
    }      
    else if (state == BUZZER_CHIRP)
    {
      int now = millis();
      int spentTime = (now - buzzSwitchTimestamp);
      
      if ((digitalRead(pin) == HIGH) && (spentTime < timeOn))
      {
        // OK, keep the state
      }
      else if ((digitalRead(pin) == LOW) && (spentTime < timeOn))
      {
       // OK, keep state
      }      
      else
      {
        buzzSwitchTimestamp = millis();
        if (digitalRead(pin) == LOW)
        {
          tone(pin,buzzerFrequency);
        }
        else
        {
          noTone(pin);
        }         
      }      
    }  
  }

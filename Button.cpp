#include <Enerlib.h>
#include <Wire.h>
#include <Arduino.h>
#include "Button.h"
  
  Button::Button(int buttonPin)
  {
    pin = buttonPin;
    pressed = false;
    duration = 0;
    lastChangeTime = 0;
  }
  
  void Button::setup()
  {
    pinMode(pin, INPUT_PULLUP); 
  }
  
  bool Button::isButtonPressed () const
  {
    return pressed;
  }

  int Button::getDuration () const
  {
    return duration;
  }

 // static bool pressFlag = true; 
  
  void Button::loopStep()
  {      
     bool isPressedNow = digitalRead(pin) == LOW;
     
     if (isPressedNow == pressed)
     {
      duration = millis() - lastChangeTime;
     }
     else
     {
      duration = 0;
      lastChangeTime = millis();
     }
    
    pressed = isPressedNow;
  }

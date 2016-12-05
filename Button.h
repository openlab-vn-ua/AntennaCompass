#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "Process.h"

class Button : public Process
{  
  public:
  
  Button       (int buttonPin);
  
  virtual void setup();

  virtual void loopStep();

  bool         isButtonPressed ( /*long *duration*/) const;
  int          getDuration () const;
  
  protected:

  int          pin;  

  private:

  bool         pressed;
  long         duration;
  long         lastChangeTime;
 };

 #endif

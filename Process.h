#ifndef __PROCESS_H__
#define __PROCESS_H__

class Process
{
  public:

  virtual void setup() = 0;
  virtual void loopStep() = 0;
};

#endif


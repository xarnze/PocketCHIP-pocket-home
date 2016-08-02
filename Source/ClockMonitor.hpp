#include "../JuceLibraryCode/JuceHeader.h"

#ifndef CLOCKMONITOR_H
#define CLOCKMONITOR_H
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>

using namespace juce;

class ClockMonitor : public Thread{
public:
  ClockMonitor();
  ~ClockMonitor();
  
  void run() override;
  Label& getLabel();

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClockMonitor)
  char formatted[10];
  CriticalSection lock;
  ScopedPointer<Label> clock;
};

#endif

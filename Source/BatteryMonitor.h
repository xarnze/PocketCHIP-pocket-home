#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "Utils.h"
#include <string>

struct BatteryStatus {
  int percentage;
  int isCharging;
};
class BatteryMonitor: public juce::Thread {
private:
  BatteryStatus status;
  
  Array<int> percentages;
  int addAndCalculateAverage(int);
  
public:
  BatteryMonitor();
  ~BatteryMonitor();
  
  const BatteryStatus& getCurrentStatus();
  void updateStatus();
  
  virtual void run();
};
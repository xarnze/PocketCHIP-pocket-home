#include "BatteryMonitor.h"
#include "Utils.h"

#include <sys/types.h>
#include <unistd.h>
       
BatteryMonitor::BatteryMonitor( )
  : Thread( "BatteryMonitor" ) {
  status.percentage = 0;
  status.isCharging = 0;
}

BatteryMonitor::~BatteryMonitor( ) {
}

const BatteryStatus& BatteryMonitor::getCurrentStatus( ) {
  return status;
}

int BatteryMonitor::addAndCalculateAverage(int cur){
  //If the array already contains 10 elements, we need to pop the eldest
  if(percentages.size() >= 10)
    percentages.remove(0);
  percentages.add(cur);
  
  int i, size = percentages.size();
  float sum = 0;
  for(i = 0; i < size; i++)
    sum += percentages[i];
  return (int) (sum/size);
}

void BatteryMonitor::updateStatus() {
  
  //Now, we use i2cget to get the battery status (Charging or not)
  String command = "/usr/sbin/i2cget -y -f 0 0x34 0x00";
  ChildProcess stat;
  stat.start(command);
  stat.waitForProcessToFinish(2);
  String strstatus = stat.readAllProcessOutput();
  //We need to get the exit status to avoid having a defunct process
  stat.getExitCode();
  strstatus = strstatus.trim();
  int value = std::stoul(strstatus.toRawUTF8(), nullptr, 16);
  status.isCharging = (value != 0);
  
  //Now, we use i2cget to get the battery percentage, it's more accurate
  String cmd = "/usr/sbin/i2cget -y -f 0 0x34 0x0b9";
  ChildProcess perc;
  perc.start(cmd);
  perc.waitForProcessToFinish(2);
  String strpercentage = perc.readAllProcessOutput();
  //We need to get the exit status to avoid having a defunct process
  perc.getExitCode();
  strpercentage = strpercentage.trim();
  int x = std::stoul(strpercentage.toRawUTF8(), nullptr, 16);
  
  //No need to have a modulo of 5 now, let's get the average
  x = addAndCalculateAverage(x);
    
  status.percentage = x;
}

void BatteryMonitor::run( ) {
  while( !threadShouldExit() ) {
    updateStatus();
    wait(2000);
  }
}
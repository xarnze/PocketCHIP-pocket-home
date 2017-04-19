#include "BatteryMonitor.h"
#include "Utils.h"

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
       
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
  //Open the i2c bus
  int file_i2c = open("/dev/i2c-0", O_RDWR);
  //Failed to open
  if (file_i2c < 0) return;
  // Get access to the battery charging info
  if (ioctl(file_i2c, I2C_SLAVE_FORCE, 0x34) < 0) return;

  // create a buffer for the registers
  unsigned char reg[512];
    
  // Set the charging status
  int loc=0;
  if (write(file_i2c, &loc, 1) < 0) return;
  read(file_i2c, reg, 0xff);
  status.isCharging = (reg[0] != 0);

  // Set the battery percentage  
  loc=0xb9;
  if (write(file_i2c, &loc, 1) < 0) return;
  read(file_i2c, reg, 0xff);
  status.percentage = addAndCalculateAverage(reg[0]);
}

void BatteryMonitor::run( ) {
  while( !threadShouldExit() ) {
    updateStatus();
    wait(2000);
  }
}

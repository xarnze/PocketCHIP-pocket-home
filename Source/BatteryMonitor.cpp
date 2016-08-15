#include "BatteryMonitor.h"
#include "Utils.h"

BatteryMonitor::BatteryMonitor( )
  : Thread( "BatteryMonitor" ) {
  status.percentage = 0;
  status.isCharging = 0;
  
  auto voltageFileName = absoluteFileFromPath( "/usr/lib/pocketchip-batt/voltage" );
  auto chargingFileName = absoluteFileFromPath( "/usr/lib/pocketchip-batt/charging" );
  
  voltageFile = File( voltageFileName );
  chargingFile = File( chargingFileName );
}

BatteryMonitor::~BatteryMonitor( ) {
}

const BatteryStatus& BatteryMonitor::getCurrentStatus( ) {
  return status;
}

void BatteryMonitor::updateStatus() {
  /*if( chargingFile.exists() ) {
    auto chargingValue = chargingFile.loadFileAsString();
    status.isCharging = chargingValue.getIntValue();
    //if(status.isCharging != 0) status.isCharging = 1;
  }*/
  
  //Now, we use i2cget to get the battery status (Charging or not)
  String command = "/usr/sbin/i2cget -y -f 0 0x34 0x00";
  ChildProcess stat;
  stat.start(command);
  stat.waitForProcessToFinish(5000);
  String strstatus = stat.readAllProcessOutput();
  strstatus = strstatus.trim();
  int value = std::stoul(strstatus.toRawUTF8(), nullptr, 16);
  status.isCharging = (value != 0);
  
  //Now, we use i2cget to get the battery percentage, it's more accurate
  String cmd = "/usr/sbin/i2cget -y -f 0 0x34 0x0b9";
  ChildProcess perc;
  perc.start(cmd);
  perc.waitForProcessToFinish(5000);
  String strpercentage = perc.readAllProcessOutput();
  strpercentage = strpercentage.trim();
  int x = std::stoul(strpercentage.toRawUTF8(), nullptr, 16);
  
  //Let's make the percentage a modulo of 5
  int mod = x%5;
  if(x>2) x += 5-mod;
  else x -= mod;
  
  status.percentage = x;
  /*
  if( voltageFile.exists() ) {
    auto voltageValue = voltageFile.loadFileAsString();
    
    float voltageOffset = (voltageValue.getFloatValue()*.001) - minVoltage;
    float maxOffset = maxVoltage - minVoltage;
    
    // turn voltage into a percentage we can use
    status.percentage = (voltageOffset * 100)/maxOffset;
    
    // only show lowest percentage graphic if battery is at least 15%
    if ( status.percentage <= 25 && status.percentage > 15) {
      status.percentage = 15;
    }
    // limit range to [0:100]
    if(status.percentage>100) status.percentage = 100;
    if(status.percentage<0) status.percentage = 0;
  }*/
}

void BatteryMonitor::run( ) {
  while( !threadShouldExit() ) {
    updateStatus();
    wait(2000);
  }
}
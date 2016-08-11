#include "ClockMonitor.hpp"

ClockMonitor::ClockMonitor() :
Thread("Clock"), 
clock(new Label("clock")),
ampm(false)
{
  clock->setFont(Font(16.5f));
}

ClockMonitor::~ClockMonitor(){ }

void ClockMonitor::setAmMode(bool mode){
  ampm = mode;
}

void ClockMonitor::run(){
  while(!threadShouldExit()){
    struct timeval tv;
    int error = gettimeofday(&tv, NULL);
    if(error) perror("Time of the day");
    struct tm res;
    localtime_r(&tv.tv_sec, &res);
    if(!ampm)
      sprintf(formatted, "%02d:%02d", res.tm_hour, res.tm_min);
    else{
      const char* moment = (res.tm_hour>12)?"pm":"am";
      int hour = (res.tm_hour>12)?res.tm_hour-12:res.tm_hour;
      sprintf(formatted, "%02d:%02d %s", hour, res.tm_min, moment);
    }
    lock.enter();
    clock->setText(String(formatted),
                   NotificationType::dontSendNotification);
    lock.exit();
    Thread::sleep(1000);
  }
}

Label& ClockMonitor::getLabel(){
  return *clock;
}

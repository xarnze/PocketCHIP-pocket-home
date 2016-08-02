#include "ClockMonitor.hpp"

ClockMonitor::ClockMonitor() :
Thread("Clock"), 
clock(new Label("clock"))
{
  clock->setFont(Font(16.5f));
}

ClockMonitor::~ClockMonitor(){ }

void ClockMonitor::run(){
  while(!threadShouldExit()){
    struct timeval tv;
    int error = gettimeofday(&tv, NULL);
    if(error) perror("Time of the day");
    struct tm res;
    localtime_r(&tv.tv_sec, &res);
    sprintf(formatted, "%02d:%02d", res.tm_hour, res.tm_min);
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

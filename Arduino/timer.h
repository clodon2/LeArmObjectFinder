#ifndef TIMER_H
#define TIMER_H
#include "Arduino.h"


class Timer {
  public:
    bool isDone();
    void update();
    void add(int milli);
    int getTime();
  private:
    int _time;
    int _last_mill;
};


#endif

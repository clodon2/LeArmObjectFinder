#include "Arduino.h"
#include "timer.h"


// check if the timer has hit 0
bool Timer::isDone() {
  if (_time == 0) {
    return true;
  }
  return false;
}


// update the timer with the last milliseconds
void Timer::update() {
  _time -= (millis() - _last_mill); // subtract time since last update
  if (_time < 0) { // prevent negative time on timer
    _time = 0;
  }
  _last_mill = millis();
}


// add time to the timer
void Timer::add(int milli) {
  _time += milli;
}


// get the current time left on the timer
int Timer::getTime() {
  return _time;
}
#include "Arduino.h"
#include "sonarFind.h"
#include "handler.h"
#include "timer.h"
#include <LobotServoController.h>

// you will likely need all of these includes for your function ^

// 0 = obj not found, 1 = obj found, 2 = algorithm ended

// constants for our function
int h_range[] = {500,2500};

bool moved = false;


// search primarily horizontally using the sonar sensor
int sonar_find_horizontal(armHandler* handler) {
  // in c++, we have to use pointers to refer to objects when we pass them as arguments, which why you see "->" instead of "."
  Timer* time = handler->getTimeObject(); // get the timer object in arm handler so we can check time
  LobotServo* servos = handler->getServos(); // get the servo array from the arm handler so we can move the arm
  // prevent updating our movement unless we are ready for the next
  if (!time->isDone()) {
    return 0;
  }
  // moving the arm
  servos[5].Position += 100;
  int move_time = 300;
  // reset arm back to 500 and repeat if at max movement
  if (servos[5].Position > 2500) {
    servos[5].Position = 500;
    // move slower back so arm doesnt go crazy
    move_time = 6000;
  }
  // actually move the arm
  handler->moveServos(move_time);
  // return obj not found
  return 0;
}
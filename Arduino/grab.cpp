#include "Arduino.h"
#include "grab.h"
#include "handler.h"
#include "timer.h"
#include <LobotServoController.h>
#include <VL53L0X.h>


int simple_grab(armHandler* handler) {
  // in c++, we have to use pointers to refer to objects when we pass them as arguments, which why you see "->" instead of "."
  Timer* time = handler->getTimeObject(); // get the timer object in arm handler so we can check time
  LobotServo* servos = handler->getServos(); // get the servo array from the arm handler so we can move the arm

  // prevent updating our movement unless we are ready for the next
  if (!time->isDone()) {
    return 0;
  }

  VL53L0X* ir = handler->getIR();
  int distance = ir->readRangeContinuousMillimeters();

  Serial.println(distance);

  // if we are able to grab, let's do it
  if ((distance > 50) && (distance < 228)  && !(servos[0].Position == 2000)) {
    Serial.println("start grab");
    servos[0].Position = 2000;
    handler->moveServos(500);
  }
  if ((distance > 50) && (distance < 228) && (servos[0].Position == 2000)) {
    Serial.println("finish grab");
    servos[4].Position = 1500;
    handler->moveServos(2000);
    return 2;
  }

  // move toward object
  if ((servos[3].Position >= 600) && (servos[4].Position == 1500)) {
    servos[3].Position -= 100;
    servos[2].Position += 100;
  }
  else if (servos[4].Position <= 2500) {
    servos[4].Position += 100;
    servos[3].Position += 100;
  }
  // unable to grab (probably out of range)
  else if (!((distance > 50) && (distance < 228))) {
    handler->reset();
    return 2;
  }

  handler->moveServos(300);

  return 0;
}

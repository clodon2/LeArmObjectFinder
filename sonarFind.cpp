#include "Arduino.h"
#include "sonarFind.h"
#include "handler.h"
#include "timer.h"
#include <LobotServoController.h>
#include <NewPing.h>
// you will likely need all of these includes for your function ^

// 0 = obj not found, 1 = obj found, 2 = algorithm ended

// constants for our function
int h_range[] = {500,2500};
bool first_iteration = true;
bool moved = false;

unsigned long past_distance = 0;
// distnace measurements vary by this percent (differences within it will be ignored)
float dist_variance = .3;
int x_increment = 50;

// count objects that appear in a row
int object_counter = 0;


// search primarily horizontally using the sonar sensor
int sonar_find_horizontal(armHandler* handler) {
  // in c++, we have to use pointers to refer to objects when we pass them as arguments, which why you see "->" instead of "."
  Timer* time = handler->getTimeObject(); // get the timer object in arm handler so we can check time
  LobotServo* servos = handler->getServos(); // get the servo array from the arm handler so we can move the arm

  // reset arm to start if algorithm not started yet
  if (first_iteration) {
    first_iteration = !sonar_find_reset(handler);
  }

  // prevent updating our movement unless we are ready for the next
  if (!time->isDone()) {
    return 0;
  }

  // if we have searched all around, we can end algorithm
  if (servos[2].Position >= 1000) {
    first_iteration = true;
    x_increment = 50;
    handler->reset();
    return 2;
  }

  // see and compare distance
  NewPing* sonar = handler->getSonar();
  unsigned long distance = sonar->ping_median();
  if (past_distance == 0) {
    past_distance = distance;
  }
  Serial.print(distance);
  Serial.print(" ");
  Serial.print(past_distance);
  Serial.print(" ");
  Serial.println(is_object(distance, past_distance));
  if (is_object(distance, past_distance)) {
    return 1;
  }
  past_distance = distance;

  // moving the arm
  servos[5].Position += x_increment;
  // move arm opp direction if at limit
  if (500 >= servos[5].Position or servos[5].Position >= 2500) {
    x_increment *= -1;
    servos[2].Position += 100;
    past_distance = 0;
  }
  // actually move the arm
  handler->moveServos(150);

  // return obj not found
  return 0;
}


bool sonar_find_reset(armHandler* handler) {
  Serial.println("sonar reset");
  // in c++, we have to use pointers to refer to objects when we pass them as arguments, which why you see "->" instead of "."
  Timer* time = handler->getTimeObject(); // get the timer object in arm handler so we can check time
  LobotServo* servos = handler->getServos(); // get the servo array from the arm handler so we can move the arm
  // prevent updating our movement unless we are ready for the next
  if (!time->isDone()) {
    return false;
  }
  servos[0].Position = 1500;
  servos[2].Position = 500;
  servos[3].Position = 1000;
  servos[5].Position = 500;
  handler->moveServos(5000);
  return true;
}


// see if dist2 is within variance to dist 1
bool is_object(unsigned long dist1, unsigned long dist2) {
  // throw out if either is 0 since scan is invalid
  if ((dist1 == 0) or (dist2 == 0)) {
    return false;
  }
  unsigned long dist1_lower = dist1 * (1 - dist_variance);
  unsigned long dist1_higher = dist1 * (1 + dist_variance);
  // dist1_lower < dist2 < dist1_higher
  if ((dist1_lower < dist2) && (dist1_higher > dist2)) {
    return false;
  }
  return true;
}
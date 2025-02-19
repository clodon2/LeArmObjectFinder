#include "Arduino.h"
#include "handler.h"
#include "sonarFind.h"
#include "grab.h"
#include "timer.h"
#include <LobotServoController.h>
#include <SoftwareSerial.h>
#include <NewPing.h>

// 0 = obj not found, 1 = obj found, 2 = algorithm ended

// sonar stuff
#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 300 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define ROUNDING_ENABLED False

// rx/tx pins for arm communication
#define arm_rx 5
#define arm_tx 6 

// create our serial communication, hard to do inside the class itself
SoftwareSerial armSerial(arm_rx, arm_tx);
LobotServoController myse(armSerial);


armHandler::armHandler() : sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE) {
  // functions here, grab function last, add new functions before grab but after the rest
  _functions[0] = sonar_find_horizontal;
  _functions[2] = simple_grab;
  _enabled = -1; // which function starts as enabled, -1 for none

  _obj_found = false;

  // setup servo array
  servos[0].ID = 1;
  servos[1].ID = 2;
  servos[2].ID = 3;
  servos[3].ID = 4;
  servos[4].ID = 5;
  servos[5].ID = 6;
  for (int i = 0; i < 6; i++) {
    servos[i].Position = 1500;
  }
}


// begin serial communication and reset arm
void armHandler::setup() {
  armSerial.begin(9600);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}


// runs in main loop, controls arm functions
void armHandler::update() {
  _delay_timer.update();
  if (_enabled >= 0) {
    int result = _functions[_enabled](this); // run function that is enabled, include timer for updates
    _obj_found = (result == 1); // update if obj was found
    if (result == 2) {
      _enabled = -1; // if we are done running algorithm, disable
    }
  }

  // if we found object, switch to grabbing mode
  if (_obj_found && (_enabled != (FUN_NUMBER - 1) )) {
    _enabled = FUN_NUMBER - 1;
  }
}


// reset the arm to the default position
void armHandler::reset() {
  _enabled = -1;
  for (int i = 0; i < 6; i++) {
    servos[i].Position = 1500;
  }
  moveServos(10000);
}


// enable a function index, -1 for none, FUN_NUMBER - 1 for grab, rest are custom
void armHandler::enable(int function_number) {
  _enabled = function_number;
}


// move the servos (edited in this object's "servos" array) over move_time milliseconds, also updates timer
bool armHandler::moveServos(long move_time) {
  // if arm is already moving, return false
  if (!_delay_timer.isDone()) {
    return false;
  }
  myse.moveServos(servos, 6, move_time);
  _delay_timer.add(move_time);
  return true;
}


// get the timer object for the arm associated with this object
Timer* armHandler::getTimeObject() {
  return &_delay_timer;
}


// get the servo array for this object
LobotServo* armHandler::getServos() {
  return servos;
}

NewPing* armHandler::getSonar() {
  return &sonar;
}
#include <LobotServoController.h>
#include <SoftwareSerial.h>
#include <NewPing.h>
#include <IRremote.hpp>
#include <elapsedMillis.h>
#include "timer.h"
#include "handler.h"

#define IR_RECEIVE_PIN 10 // IR remote pin

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 300 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define ROUNDING_ENABLED false

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

// used to reset arm between operations
elapsedMillis arm_reset;
int ARM_R_COOLDOWN = 3000;

// timer for checking arm movement
Timer armTimer;
armHandler arm;


void setup() {
  arm.setup(); // serial begins are in here, as well as arm reset
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}


void loop() {
  unsigned long mill = millis();
  // receive IR signal after cooldown, send code received (if any) to IR functions
  IR_check();
  arm.update();
}

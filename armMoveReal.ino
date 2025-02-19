#include <LobotServoController.h>
#include <SoftwareSerial.h>
#include <NewPing.h>
#include <IRremote.hpp>
#include <elapsedMillis.h>
#include "timer.h"
#include "handler.h"

#define IR_RECEIVE_PIN 10 // IR remote pin


// used to reset arm between operations
elapsedMillis arm_reset;
int ARM_R_COOLDOWN = 3000;

// timer for checking arm movement
Timer armTimer;
armHandler arm;


void setup() {
  arm.setup(); // serial begins are in here, as well as arm reset
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  delay(2000);
}


void loop() {
  unsigned long mill = millis();
  // receive IR signal after cooldown, send code received (if any) to IR functions
  IR_check();
  arm.update();
}

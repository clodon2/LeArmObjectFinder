#include <LobotServoController.h>
#include <SoftwareSerial.h>
#include <IRremote.hpp>
#include <elapsedMillis.h>
#include "timer.h"
#include "handler.h"

#define IR_RECEIVE_PIN 10 // IR remote pin

armHandler arm;


void setup() {
  Serial.println("Loading");
  arm.setup(); // serial begins are in here, as well as arm reset
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  delay(2000);
  Serial.println("ready");
}


void loop() {
  // receive IR signal after cooldown, send code received (if any) to IR functions
  IR_check();
  arm.update();
  arm.enable(1);
}

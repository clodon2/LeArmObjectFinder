#include <LobotServoController.h>
#include <SoftwareSerial.h>
#include <IRremote.hpp>
#include <elapsedMillis.h>
#include "timer.h"
#include "handler.h"

#define IR_RECEIVE_PIN 10 // IR remote pin

armHandler arm;


void setup() {
  pinMode(2, OUTPUT); // setup XSHUT pin for VL53L0X
  pinMode(13, OUTPUT); // Set onboard LED as outpu
  delay(1000);
  digitalWrite(13, HIGH);
  Serial.println("Loading");
  arm.setup(); // serial begins are in here, as well as arm reset
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  delay(2000);
  Serial.println("ready");
  digitalWrite(13, LOW);
}


void loop() {
  // receive IR signal after cooldown, send code received (if any) to IR functions
  IR_check();
  arm.update();
}

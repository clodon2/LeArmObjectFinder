#include "Arduino.h"
#include "sonarFind.h"
#include "handler.h"
#include "timer.h"
#include <Wire.h>
#include <VL53L0X.h>

int middle_threshold = 50;
int object_right_threshold = (1280/2) + middle_threshold;
int object_left_threshold = (1280/2) + middle_threshold;


int camera_find(armHandler* handler) {
  // in c++, we have to use pointers to refer to objects when we pass them as arguments, which why you see "->" instead of "."
  Timer* time = handler->getTimeObject(); // get the timer object in arm handler so we can check time
  LobotServo* servos = handler->getServos(); // get the servo array from the arm handler so we can move the arm

  int* object_coordinates = handler->getCameraObjects();
  if (object_coordinates[0] == -1){
    return 0;
  }
  Serial.print("obj detected: ");
  for (int i=0; i < 4; i++) {
    Serial.print(object_coordinates[i]);
    Serial.print(", ");
  }
  Serial.print("\n");
  int object_middle_px = (object_coordinates[2] + object_coordinates[0]) / 2;
  int object_width_px = abs(object_coordinates[2] - object_coordinates[0]);
  int object_height_px = abs(object_coordinates[3] - object_coordinates[1]);
  Serial.println("camera_find results: ");
  Serial.print(object_width_px);
  Serial.print(" ");
  Serial.println(object_height_px);

  if (object_middle_px > object_right_threshold) {
    servos[5].Position += 50;
  }
  else if (object_middle_px < object_left_threshold) {
    servos[5].Position -= 50;
  }

  handler->moveServos(100);

  return 0;
}
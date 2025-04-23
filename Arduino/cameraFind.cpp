#include "Arduino.h"
#include "cameraFind.h"
#include "handler.h"
#include "timer.h"
#include <Wire.h>
#include <VL53L0X.h>

// object detection constants
int servo_move_increment = 10;
int middle_threshold = 10;
int object_right_threshold = (1280/2) + middle_threshold - 50;
int object_left_threshold = (1280/2) - middle_threshold - 50;
int object_up_threshold = (720/2) - middle_threshold;
int object_down_threshold = (720/2) + middle_threshold + 200;

// resetting detection info
bool cam_first = true;
int no_object_scans = 0;
int object_centered = 0;


int camera_find(armHandler* handler) {
  // in c++, we have to use pointers to refer to objects when we pass them as arguments, which why you see "->" instead of "."
  Timer* time = handler->getTimeObject(); // get the timer object in arm handler so we can check time
  LobotServo* servos = handler->getServos(); // get the servo array from the arm handler so we can move the arm

  if (cam_first) {
    camera_find_reset(handler);
  }

  // prevent updating our movement unless we are ready for the next
  if (!time->isDone()) {
    return 0;
  }

  int* object_coordinates = handler->getCameraObjects();
  // if no object detected
  if (object_coordinates[0] == -1){
    no_object_scans += 1;

    // if no object detected for some frames, move to look at new area
    if (no_object_scans >= 30) {
      servos[5].Position += 500;
      if (servos[5].Position > 2500) {
        cam_first = true;
        return 2;
      }
      else {
        handler->moveServos(1000);
        no_object_scans = 0;
        return 0;
      }
    }
    return 0;
  }
  

  int move_time = 10;

  // print object detection info, calculate object width/height/middle
  Serial.print("obj detected: ");
  for (int i=0; i < 4; i++) {
    Serial.print(object_coordinates[i]);
    Serial.print(", ");
  }
  Serial.print("\n");
  int object_middle_px_x = (object_coordinates[2] + object_coordinates[0]) / 2;
  int object_middle_px_y = (object_coordinates[3] + object_coordinates[1]) / 2;
  int object_width_px = abs(object_coordinates[2] - object_coordinates[0]);
  int object_height_px = abs(object_coordinates[3] - object_coordinates[1]);

  if ( ((object_left_threshold < object_middle_px_x) && (object_middle_px_x < object_right_threshold)) && 
  ((object_down_threshold > object_middle_px_y) && (object_middle_px_y > object_up_threshold)) ) {
    object_centered += 1;
    servo_move_increment = 2;
  }
  else {
    object_centered = 0;
  }

  // if object is centered enough, activate grab
  if (( ((object_left_threshold < object_middle_px_x) && (object_middle_px_x < object_right_threshold)) && 
  ((object_down_threshold > object_middle_px_y) && (object_middle_px_y > object_up_threshold)) ) && (object_centered > 10) ) {
    Serial.println("object centered");
    cam_first = true;
    VL53L0X* ir = handler->getIR();
    ir->setMeasurementTimingBudget(200000);
    int obj_dist = ir->readRangeSingleMillimeters();
    while (obj_dist == 8191 || obj_dist == 8190) {
      handler->resetIR();
      delay(200);
      ir->setMeasurementTimingBudget(200000);
      obj_dist = ir->readRangeSingleMillimeters();
      Serial.println(obj_dist);
    }
    Serial.print("PX size: ");
    Serial.print(object_width_px);
    Serial.print(" ");
    Serial.println(object_height_px);
    Serial.print("Dist: ");
    Serial.println(obj_dist);
    float real_object_width = (2 * obj_dist * tan(1.91986/2) * float(object_width_px / 1920.00));
    float real_object_height = (2 * obj_dist * tan(1.0472/2) * float(object_height_px / 720.00));
    Serial.print("Real size: ");
    Serial.print(real_object_width);
    Serial.print(" ");
    Serial.println(real_object_height);
    ir->setMeasurementTimingBudget(50000);
    if (obj_dist <= 450) {
      return 1;
    }
    else {
      Serial.println("object too far");
      return 2;
    }
  }


  // move arm to center object in camera pov if not centered
  if (object_middle_px_x > object_right_threshold) {
    Serial.println("object right");
    servos[5].Position -= servo_move_increment;
  }
  else if (object_middle_px_x < object_left_threshold) {
    Serial.println("object left");
    servos[5].Position += servo_move_increment;
  }

  if (object_middle_px_y < object_up_threshold) {
    Serial.println("object above");
    servos[2].Position += servo_move_increment;
  }
  else if (object_middle_px_y > object_down_threshold) {
    Serial.println("object below");
    servos[2].Position -= servo_move_increment;
  }

  // if moved beyond arm bounds, set to max/min movement

  if (servos[5].Position < 500) {
    servos[5].Position = 500;
  }
  else if (servos[5].Position > 2500) {
    servos[5].Position = 2500;
  }

  if (servos[2].Position < 500) {
    servos[2].Position = 500;
  }
  else if (servos[2].Position > 2500) {
    servos[2].Position = 2500;
  }


  // commit movement
  Serial.println(move_time);
  handler->moveServos(move_time);

  return 0;
}


bool camera_find_reset(armHandler* handler) {
  Serial.println("camera reset");
  // in c++, we have to use pointers to refer to objects when we pass them as arguments, which why you see "->" instead of "."
  Timer* time = handler->getTimeObject(); // get the timer object in arm handler so we can check time
  LobotServo* servos = handler->getServos(); // get the servo array from the arm handler so we can move the arm
  // prevent updating our movement unless we are ready for the next
  if (!time->isDone()) {
    return false;
  }
  servos[0].Position = 1500;
  servos[2].Position = 500;
  servos[3].Position = 500;
  servos[4].Position = 900;
  servos[5].Position = 1500;
  handler->moveServos(4000);
  cam_first = false;
  no_object_scans = 0;
  object_centered = 0;
  return true;
}
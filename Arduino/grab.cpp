#include "Arduino.h"
#include "grab.h"
#include "handler.h"
#include "timer.h"
#include <LobotServoController.h>
#include <VL53L0X.h>


bool first_grab = true;
int grab_distance = 120;
int grab_power = 2090;

int simple_grab(armHandler* handler) {
  // in c++, we have to use pointers to refer to objects when we pass them as arguments, which why you see "->" instead of "."
  Timer* time = handler->getTimeObject(); // get the timer object in arm handler so we can check time
  LobotServo* servos = handler->getServos(); // get the servo array from the arm handler so we can move the arm

  // prevent updating our movement unless we are ready for the next
  if (!time->isDone()) {
    return 0;
  }

  VL53L0X* ir = handler->getIR();
  if (first_grab) {
    Serial.println("new grab started");
    ir->startContinuous(80);
    first_grab = false;
    delay(10);
    servos[0].Position = 1500;
  }

  int distance = ir->readRangeContinuousMillimeters();
  int grab_adjustment = round( atan2 (65, distance) * 180/3.14159265 );
  Serial.print("grab_adjustment: ");
  Serial.println(grab_adjustment);
  Serial.print("Distance: ");
  Serial.println(distance);


// (distance > 30) && (distance < grab_distance) && 
  if ((servos[0].Position == grab_power)) {
    Serial.println("finish grab");
    servos[4].Position = 1500;
    handler->moveServos(2000);
    ir->stopContinuous();
    first_grab = true;
    return 2;
  }

  // if we are able to grab, let's do it
  if ((distance < grab_distance) && !(servos[0].Position == grab_power)) {
    servos[2].Position = servos[2].Position + 100;
    Serial.println("start grab");
    servos[0].Position = grab_power;
    handler->moveServos(1000);
  }

  // move toward object
  if ((servos[3].Position >= 600) && (servos[4].Position == 1500)) {
    Serial.println("second phase");
    servos[3].Position -= 100;
    servos[2].Position += 100;
  }
  else if (servos[4].Position <= 2500 && (servos[2].Position >= 1500)) {
    Serial.println("third phase");
    servos[3].Position += 100;
    servos[2].Position -= 120;
  }
  else if (servos[4].Position <= 2500) {
    Serial.println("first phase");
    servos[4].Position += 100;
    servos[2].Position += 100;
  }
  // unable to grab (probably out of range)
  else if (!((distance > 30) && (distance < grab_distance)) && (servos[0].Position != grab_power)) {
    handler->reset();
    ir->stopContinuous();
    first_grab = true;
    return 2;
  }

  handler->moveServos(900);

  return 0;
}


// object detection constants
int horizontal_increment = 10;
int vertical_increment = 40;

// resetting detection info
bool cam_grab_first = true;


int camera_grab(armHandler* handler) {
  // in c++, we have to use pointers to refer to objects when we pass them as arguments, which why you see "->" instead of "."
  Timer* time = handler->getTimeObject(); // get the timer object in arm handler so we can check time
  LobotServo* servos = handler->getServos(); // get the servo array from the arm handler so we can move the arm

  // prevent updating our movement unless we are ready for the next
  if (!time->isDone()) {
    return 0;
  }
  
  int middle_threshold = 10;
  int object_right_threshold = (1280/2) + middle_threshold - 50;
  int object_left_threshold = (1280/2) - middle_threshold - 50;
  int object_up_threshold = (720/2) - middle_threshold;
  int object_down_threshold = (720/2) + middle_threshold + 200;

  if (cam_grab_first) {
    cam_grab_first = false;
  }

  if (servos[0].Position == grab_power) {
    servos[4].Position = 1500;
    handler->moveServos(2000);
    return 2;
  }

  int* object_coordinates = handler->getCameraObjects();

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

  // if object is centered enough, activate grab
  if (( ((object_left_threshold < object_middle_px_x) && (object_middle_px_x < object_right_threshold)) && 
  ((object_down_threshold > object_middle_px_y) && (object_middle_px_y > object_up_threshold)) )) {
    Serial.println("object centered");
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
    if (obj_dist > 150) {
      servos[4].Position += vertical_increment;
    }
    else {
      servos[0].Position = grab_power; 
      handler->moveServos(300);
    }
  }

  // move arm to center object in camera pov if not centered
  if (object_middle_px_x > object_right_threshold) {
    Serial.println("object right");
    servos[5].Position -= horizontal_increment;
  }
  else if (object_middle_px_x < object_left_threshold) {
    Serial.println("object left");
    servos[5].Position += horizontal_increment;
  }

  if (object_middle_px_y < object_up_threshold) {
    Serial.println("object above");
    if (servos[4].Position <= 2000) {
      servos[3].Position += vertical_increment;
    }
    else {
      servos[2].Position += vertical_increment;
    }
  }
  else if (object_middle_px_y > object_down_threshold) {
    Serial.println("object below");
    if (servos[2].Position > 500) {
      servos[2].Position -= vertical_increment;
    }
    else {
      servos[2].Position = 500;
      servos[3].Position -= vertical_increment;
    }
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
  handler->moveServos(move_time);

  return 0;
}
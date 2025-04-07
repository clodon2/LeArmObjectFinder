#include "Arduino.h"
#include "handler.h"
#include "sonarFind.h"
#include "cameraFind.h"
#include "grab.h"
#include "timer.h"
#include <LobotServoController.h>
#include <SoftwareSerial.h>
#include <VL53L0X.h>
#include <Wire.h>

// 0 = obj not found, 1 = obj found, 2 = algorithm ended

// sonar stuff
#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 300 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define ROUNDING_ENABLED False

// rx/tx pins for arm communication
#define arm_rx 5
#define arm_tx 6 

// rx/tx pins for camera connection
#define cam_rx 8
#define cam_tx 9

// create our serial communication, hard to do inside the class itself
SoftwareSerial armSerial(arm_rx, arm_tx);
SoftwareSerial camSerial(cam_rx, cam_tx);
LobotServoController myse(armSerial);


armHandler::armHandler() {
  Serial.println("not cooked");
  // functions here, grab function last, add new functions before grab but after the rest
  _functions[0] = sonar_find_horizontal;
  _functions[1] = camera_find;
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
  Serial.println("test");
  armSerial.begin(9600);
  camSerial.begin(31250);
  Serial.begin(9600);
  Wire.begin();
  // setup IR
  IR_sensor.setTimeout(500);
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

VL53L0X* armHandler::getIR() {
  return &IR_sensor;
}

bool armHandler::toggleIR() {
  return true;
}

int* armHandler::getCameraObjects() {
  camSerial.listen();

  char obj_type[32];
  static int coordinates[4] = {-1, -1, -1, -1};

  int count = 0;

  // while there is data coming in, read it

  // and send to the hardware serial port:
  /*
  bool reading_data = false;
  char start_char = '<';
  char end_char = '>';
  char received;
  const byte char_num = 32;
  char received_full[char_num];
  char received_temp[char_num];
  int received_index = 0;

  Serial.println("Cam Info: ");
  if (camSerial.available() > 0) {
    received = camSerial.read();
    if (received == start_char) {
      Serial.println("started");
      reading_data = true;
      String object_type = camSerial.readStringUntil("\n");
      int top_left_x = camSerial.parseInt();
      int top_left_y = camSerial.parseInt();
      int bottom_right_x = camSerial.parseInt();
      int bottom_right_y = camSerial.parseInt();
      camSerial.read();
      reading_data = false;
    }
    
    while (reading_data) {
      received = camSerial.read();
      if (received == end_char) {
        received_full[received_index] = '\0';
        reading_data = false;
      }
      else {
        received_full[received_index] = received;
        received_index += 1;
        if (received_index >= char_num) {
          received_index = char_num - 1;
        }
      }
    }
    
  }
  
  Serial.println(received_full);
  strcpy(received_temp, received_full);

  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(received_temp,"\n");      // get the first part - the string
  strcpy(obj_type, strtokIndx); // copy it to messageFromPC

  for (int i=0; i < 4; i++) {
    strtokIndx = strtok(NULL, "\n"); // this continues where the previous call left off
    coordinates[i] = atoi(strtokIndx);     // convert this part to an integer
  }

  Serial.println("non-working: ");
  Serial.print(coordinates[1]);
  Serial.println();
  */

  
  if (camSerial.available() > 0) {
    int top_left_x = camSerial.parseInt();
    int top_left_y = camSerial.parseInt();
    int bottom_right_x = camSerial.parseInt();
    int bottom_right_y = camSerial.parseInt();

    coordinates[0] = top_left_x;
    coordinates[1] = top_left_y;
    coordinates[2] = bottom_right_x;
    coordinates[3] = bottom_right_y;
  }
  
  for (int i=0; i < 4; i++) {
    Serial.print(coordinates[i]);
    Serial.print(", ");
  }
  Serial.print("\n");
  return coordinates;
}
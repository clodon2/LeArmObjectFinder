#include <LobotServoController.h>
#include <NewPing.h>
#include <IRremote.hpp>
#include <elapsedMillis.h>

#define IR_RECEIVE_PIN 10 // IR remote pin

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 300 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define ROUNDING_ENABLED false

LobotServoController myse(Serial);
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

LobotServo servos[6]; // setup arm servo array

// used to reset arm between operations
elapsedMillis arm_reset;
int ARM_R_COOLDOWN = 3000;


// used as parent for any arm function class
class armFunction {
  public:
    bool enabled = false;
    void update() {}
};


// old sonar find that searches ^v primarily, with increments
class sonarFindOld : public armFunction{
  private:
    bool found_obj = false;

    int serv_5_range[2] = {500, 2500};
    int serv_2_range[2] = {500, 900};

    int ARM_COOLDOWN = 500;
    elapsedMillis arm_move;

  public:
    bool enabled = false;

    bool update() {
      // return true if done searching
      // stop if full rotation and nothing found
      if (servos[5].Position >= serv_5_range[1]) {
        return true;
      }
      // if object not found, try to find
      if (!found_obj) {
        found_obj = find_object();
      }
      unsigned int distance = sonar.ping_median();
      if (distance >= 5 && found_obj) {
        grab_object();
      }
      else if (distance < 5 && found_obj) {
        return true;
      }
      return false;
    }

    void reset_arm() {
      if (arm_reset >= ARM_R_COOLDOWN) {
        servos[2].Position = 500;
        servos[3].Position = 1000;
        servos[5].Position = 500;
        myse.moveServos(servos, 6, ARM_R_COOLDOWN);
        arm_reset = 0;
      }
    }

    bool find_object() {
      // if arm on cooldown, skip
      if (arm_move < ARM_COOLDOWN) {
        return false;
      }

      if ((servos[5].Position < serv_5_range[1]) && (servos[2].Position >= serv_2_range[1])) {
        servos[5].Position += 100;
        servos[2].Position = serv_2_range[0];
      }
      if (servos[2].Position < serv_2_range[1]) {
        servos[2].Position += 100;

        // calculate current distance and expected distance
        float current_distance = float(sonar.ping_median()) / US_ROUNDTRIP_CM;
        int serv_angle = servos[3].Position - 500 + servos[2].Position;
        float expected_diff = (22 / cos(calc_angle(serv_angle))) - 14;
        
        // check if object found (distance anomaly)
        if (! (expected_diff == 0)) {
          float w_difference = current_distance - expected_diff;
          Serial.println(expected_diff);
          Serial.println(current_distance);
        }
        // move arm, update cooldown
        myse.moveServos(servos, 6, ARM_COOLDOWN);
        arm_move = 0;
      }
      
      return false;
    }

    float calc_angle(int servo_move) {
      float move_percent = float(servo_move - 500) / 2000;
      return (M_PI) * move_percent;
    }

    void grab_object() {
      if (arm_move < ARM_COOLDOWN) {
        return;
      }
      servos[2].Position += 50;
      servos[4].Position += 50;
      myse.moveServos(servos, 6, ARM_COOLDOWN);
      arm_move = 0;
    }
};


class horizontalScan : public armFunction {
  private:
    bool found_obj = false;
    bool moving = false;

    float last_distance = 0;

    int serv_5_range[2] = {500, 2500};
    int serv_2_range[2] = {500, 900};

    int sev_5_change = 30; //

    float DEADZONE = 1; // amount of error between distance scans (increase if picking up objects that aren't there)
    int SCAN_SPEED = 7000; // in milliseconds, time for arm to complete horizontal scan
    int SCAN_COOLDOWN = 125; // scanner can do 40 per/sec, we do 5 at once so 40/5=8 1000/8 = 125 delay between reads
    int ARM_COOLDOWN = 200;
    elapsedMillis sonar_reset;
    elapsedMillis arm_move;

  public:
    bool update(){
      // return true if done searching
      // stop if full search and nothing found
      if (servos[2].Position >= serv_2_range[1]) {
        return true;
      }
      // if object not found, try to find
      if (!found_obj) {
        found_obj = find_object();
      }
      unsigned int distance = sonar.ping_median();
      if (distance >= 5 && found_obj) {
        grab_object();
      }
      else if (distance < 5 && found_obj) {
        return true;
      }
      return false;
    }

    void reset_arm() {
      if (arm_reset >= ARM_R_COOLDOWN) {
        servos[2].Position = 500;
        servos[3].Position = 1000;
        servos[5].Position = 500;
        myse.moveServos(servos, 6, ARM_R_COOLDOWN);
        arm_reset = 0;
      }
    }


    bool find_object() {
      // if sonar on cooldown, skip
      if (sonar_reset < SCAN_COOLDOWN) {
        return false;
      }

      float distance = float(sonar.ping_median()) / US_ROUNDTRIP_CM;

      // if we have a scanned before and the distance has changed more than deadzone, we found an object
      if ((last_distance != 0) && !((last_distance - DEADZONE) > distance > (last_distance + DEADZONE))) {
        return true;
      }

      last_distance = distance;

      bool stopped = myse.waitForStopping(uint32_t 20000);

      if (stopped) {
        servos[5].Position = serv_5_range[1];
        servos[2].Position = serv_2_range[0];
        myse.moveServos(servos, 6, SCAN_SPEED);
      }

      // if already moving, dont need to move
      if (!moving){
        servos[5].Position = serv_5_range[1];
        servos[2].Position = serv_2_range[0];
        myse.moveServos(servos, 6, SCAN_SPEED);
        moving = true;
      }
      
      return false;
    }


    void grab_object() {
      if (arm_move < ARM_COOLDOWN) {
        return;
      }
      servos[2].Position += 50;
      servos[4].Position += 50;
      myse.moveServos(servos, 6, ARM_COOLDOWN);
      arm_move = 0;
    }
};


// used to handle all arm functions
class armFunctions {
  private:
    sonarFindOld sfo;
    // add functions to this list
    armFunction functions[1] = {sfo};

  public:
    void update() {
        if (sfo.enabled) {
          sfo.update();
        }
      for (armFunction arm_function : functions) {
        if (arm_function.enabled) {
          arm_function.update();
        }
      }
    }


    void oldSonarFind() {
      sfo.reset_arm();
      sfo.enabled = true;
    }
};


void setup() {
  armFunctions arm;
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  servos[0].ID = 1;
  servos[1].ID = 2;
  servos[2].ID = 3;
  servos[3].ID = 4;
  servos[4].ID = 5;
  servos[5].ID = 6;
  for (int i = 0; i <= 5; i++) {
    servos[i].Position = 1500;
  }
}


armFunctions arm;


void loop() {
  unsigned long mill = millis();
  // receive IR signal after cooldown, send code received (if any) to IR functions
  IR_check();
  arm.update();
}

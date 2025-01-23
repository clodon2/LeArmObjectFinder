#include <LobotServoController.h>
#include <NewPing.h>

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 300 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define ROUNDING_ENABLED false

LobotServoController myse(Serial);
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

LobotServo servos[6];

unsigned long prev_mill = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
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

void loop() {
  unsigned long mill = millis();
  //Serial.println(50000);

  bool found_object = find_object();
  unsigned int distance = sonar.ping_median();
  if (distance >= 5 && found_object) {
    servos[2].Position += 50;
    servos[4].Position += 50;
    myse.moveServos(servos, 6, 600);
  }
  delay(1000);
}


bool find_object() {
  float current_distance = float(sonar.ping_median()) / US_ROUNDTRIP_CM;
  delay(400);
  servos[2].Position = 500;
  servos[3].Position = 1000;
  servos[5].Position = 500;
  myse.moveServos(servos, 6, 1000);
  delay(1000);
  for (int i = 500; i <= 2500; i += 100) {
    servos[5].Position = i;
    for (int j = 500; j <= 900; j += 100) {
      current_distance = float(sonar.ping_median()) / US_ROUNDTRIP_CM;
      int serv_angle = servos[3].Position - 500 + j;
      float expected_diff = (22 / cos(calc_angle(serv_angle))) - 14;
      if (! (expected_diff == 0)) {
        float w_difference = current_distance - expected_diff;
        Serial.println(expected_diff);
        Serial.println(current_distance);
      }
      servos[2].Position = j;
      myse.moveServos(servos, 6, 500);
      delay(500);
    }
  }
  return false;
}


void get_object(unsigned int distance) {
  int cur_pos = servos[4].Position;
  for (int i = cur_pos; i >= 500; i -= 30) {
    distance = sonar.ping_cm();
    if (distance <= 3) {
      break;
    }
    servos[4].Position = i;
    myse.moveServos(servos, 6, 400);
    delay(400);
  }
}


float calc_angle(int servo_move) {
  float move_percent = float(servo_move - 500) / 2000;
  return (M_PI) * move_percent;
}


void safe_print(int p) {
  Serial.println();
  Serial.println(p);
  Serial.println();
}
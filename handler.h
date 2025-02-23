#ifndef HANDLER_H
#define HANDLER_H
#include "Arduino.h"
#include <LobotServoController.h>
#include <SoftwareSerial.h>
#include "timer.h"
#include <NewPing.h>

#define FUN_NUMBER 3 //number of arm functions in armHandler, update if some are added


class armHandler {
  private:
    LobotServo servos[6]; // setup arm servo array
    // add functions to this list
    int (*_functions[FUN_NUMBER])(armHandler*); // hold all arm functions, GRAB_OBJ IS LAST
    int _enabled; // which function is enabled in ^, -1 for none
    bool _obj_found;
    Timer _delay_timer; // timer to track arm movement
    NewPing sonar;

  public:
    armHandler();
    void setup();
    void update();
    void reset();
    void enable(int function_number);
    bool moveServos(long move_time);
    Timer* getTimeObject();
    LobotServo* getServos();
    NewPing* getSonar();
};


#endif
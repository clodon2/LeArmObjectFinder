#ifndef SONARFIND_H
#define SONARFIND_H
#include "Arduino.h"
#include "handler.h"

int sonar_find_horizontal(armHandler*);


bool sonar_find_reset(armHandler*);


bool is_object(unsigned long, unsigned long);


void reset_values();

#endif
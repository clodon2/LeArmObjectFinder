# Autonomous Robot Arm Program for Arduino
This project utilizes an arduino, sonar sensor, and AMB82-Mini camera to control a HiWonder Learm robotic arm. The arm can detect objects either with a [simple sonar search algorithm](sonarFind.cpp) or with computer/machine vision using the AMB82-Mini camera. Switching between arm functionality is facilitated by an 1838 infrared receiver and an infrared remote. The main program loop can be found in [armMoveReal](Arduino/armMoveReal.ino). 

## Demo to be put here

# Modules

## Arm Handler
Working with the included documentation, I created a wrapper for the arm that allows IR signals to always be received while also updating arm commands. This handler is seen in [handler.cpp](handler.cpp) and [handler.h](Arduino/handler.h). 

## Timer
I created a timer class to track if the arm was still moving, which works in conjunction with the handler. This prevents multiple commands from being sent to the arm (which causes motor spasms). See [timer.cpp](Arduino/timer.cpp) and [timer.h](Arduino/timer.h).

## Infrared Functionality 
The arm can be controlled with a wireless remote using basic infrared commands. This allows us to showcase multiple object detection methods and shut off the arm if something goes wrong (unplugging it works too). See [IR.ino](IR.ino).

## Sonar 
The original searching method for the arm is with a sonar distance sensor. We scan horizontally and see if the distance changes, if it does there is an object there. See [sonarFind.cpp](sonarFind.cpp) and [sonarFind.h](sonarFind.h).

## Grabbing
The grabbing algorithm scans around the object that was detected and determines roughly how big it is and how to grab it. See [grab.cpp](grab.cpp) and [grab.h](grab.h).

## Camera
A camera with on-board processing (AMB82-Mini) will be implemented. This camera will be coded with object detection algorithms in openCV/Yolo. Over software serial communication, we will take the data gathered from the camera and send it to the arduino, and then send it to the grab function.

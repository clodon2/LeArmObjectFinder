# Autonomous Robot Arm Program for Arduino
This project utilizes an arduino, sonar sensor, and AMB82-Mini camera to control a HiWonder Learm robotic arm.

## Arm Handler
Working with the included documentation, I created a wrapper for the arm that allows IR signals to always be received while also updating arm commands. This handler is seen in [handler.cpp](handler.cpp) and handler.h. 

## Timer
I created a timer class to track if the arm was still moving, which works in conjunction with the handler. This prevents multiple commands from being sent to the arm (which causes motor spasms).

## Infrared Functionality 
The arm can be controlled with a wireless remote using basic infrared commands. This allows us to showcase multiple object detection methods and shut off the arm if something goes wrong (unplugging it works too).

## Sonar 
The original searching method for the arm is with a sonar distance sensor. We scan horizontally and see if the distance changes, if it does there is an object there.

## Grabbing
The grabbing algorithm scans around the object that was detected and determines roughly how big it is and how to grab it.

## Camera
A camera with on-board processing (AMB82-Mini) will be implemented. This camera will be coded with object detection algorithms in openCV/Yolo. Over software serial communication, we will take the data gathered from the camera and send it to the arduino, and then send it to the grab function.

# Autonomous Robot Arm Program for Arduino
<img src="https://github.com/user-attachments/assets/20f55610-b40d-4bfd-8ba0-e4f257556019" width="400" height="400">  <img src="https://github.com/user-attachments/assets/9ea08274-d14b-4c68-b7d0-52d0e9bcb841" width="600" height="400"> <br/>
This project utilizes an arduino, sonar sensor, and AMB82-Mini camera to control a HiWonder Learm robotic arm. The arm can detect objects either with a [simple distance search algorithm](Arduino/sonarFind.cpp) or with computer/machine vision using the AMB82-Mini camera. Switching between arm functionality is facilitated by an 1838 infrared receiver and an infrared remote. The main program loop can be found in [armMoveReal](Arduino/armMoveReal.ino). 

## Early Demo
![1000003804-ezgif com-optimize](https://github.com/user-attachments/assets/6106c50e-e7bc-4af0-9b6b-cd6c22fea8c7)

# Modules

## Arm Handler
Working with the included documentation, I created a wrapper for the arm that allows IR signals to always be received while also updating arm commands. This handler is seen in [handler.cpp](Arduino/handler.cpp) and [handler.h](Arduino/handler.h). 

## Timer
I created a timer class to track if the arm was still moving, which works in conjunction with the handler. This prevents multiple commands from being sent to the arm (which causes motor spasms). See [timer.cpp](Arduino/timer.cpp) and [timer.h](Arduino/timer.h).

## Infrared Functionality 
The arm can be controlled with a wireless remote using basic infrared commands. This allows us to showcase multiple object detection methods and shut off the arm if something goes wrong (unplugging it works too). See [IR.ino](Arduino/IR.ino).

## Infrared Distance Sensor
![image](https://github.com/user-attachments/assets/1b7e5c75-6b7e-471f-8c76-503ed2366f56)
Distance to objects is determined with a VL53L0/1XV2 IR distance sensor, which is a stunningly accurate and small sensor. Implemented with the VL53L0X.h library, we use it for grabbing objects, in the camera pickup algorithm, and the modified sonar sensor search (working with IR). 

## Grabbing
The grabbing algorithm scans around the object that was detected and determines roughly how big it is and how to grab it. See [grab.cpp](Arduino/grab.cpp) and [grab.h](Arduino/grab.h).

## Camera
![image](https://github.com/user-attachments/assets/e7640de8-f189-46aa-a87a-02aad169303a)
A camera with on-board processing (AMB82-Mini) is being implemented. Currently, the camera has a serial connection (tx pin 21, rx pin 22) to the arduino through a software serial connection (tx pin 9, rx pin 8). This allows the camera to feed object position information to the arduino, which controls the arm and calculates movement. The camera also sends a live video feed through USB to a connected computer which can be viewed with PotPlayer's webcam/other device streamer. See [Camera](AMB82-Mini).

## Sonar 
The original searching method for the arm was with a sonar distance sensor. We scan horizontally and see if the distance changes, if it does there is an object there. See [sonarFind.cpp](Arduino/sonarFind.cpp) and [sonarFind.h](Arduino/sonarFind.h). **This was deprecated due to the sonar sensor being wildly inaccurate.**

## Connections
Connect arm 5v/gnd to breadboard if you want it to run on its own. Camera must be connected via usb to a computer and opened with potplayer. <br/>
![image](https://github.com/user-attachments/assets/a366f590-74c2-4102-9031-019bb60866e3)



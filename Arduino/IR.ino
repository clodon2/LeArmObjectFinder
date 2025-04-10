int IR_COOLDOWN = 1000; // cooldown between IR information receiving in milliseconds
elapsedMillis IR_prev;

// receive IR signal after cooldown, send code received (if any) to IR functions
void IR_check(){
  if (IrReceiver.decode()) {
    if (IR_prev >= IR_COOLDOWN){
      IR_prev = 0;
      IR_input_map(IrReceiver.decodedIRData.command);
    }
  }
  IrReceiver.resume(); // Enable receiving of the next value
}


void IR_input_map(int code){
  Serial.print("Recieved: ");
  Serial.println(code);
  // codes for the IR remote are as follows:
  // CH- = 69, CH = 70, CH+ = 71, |<< = 68, >>| = 64, >|| = 67, - = 7, + = 21, EQ = 9, 100+ = 25, 200+ = 13, 
  // 0 = 22, 1 = 12, 2 = 24, 3 = 94, 4 = 8, 5 = 28, 6 = 90, 7 = 66, 8 = 82, 9 = 74
  if (code == 69){
    Serial.println("deactivated arm");
    arm.enable(-1);
  }
  // reset arm
  if (code == 22){
    Serial.println("reset arm");
    arm.reset();
  }
  if (code == 12){
    Serial.println("horizontal search algorithm");
    arm.enable(0);
  }
  if (code == 24){
    Serial.print("Camera Object Search");
    arm.enable(1);
  }
  if (code == 94){
    Serial.println("Camera object Position");
    int* arr = arm.getCameraObjects();
  }
  // add activation for new function here
}


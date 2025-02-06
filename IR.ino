int IR_COOLDOWN = 1000; // cooldown between IR information receiving in milliseconds
elapsedMillis IR_prev;

// receive IR signal after cooldown, send code received (if any) to IR functions
void IR_check(){  
  if (IR_prev >= IR_COOLDOWN){
      if (IrReceiver.decode()) {
        IrReceiver.resume(); // Enable receiving of the next value
        IR_prev = 0;
        IR_input_map(IrReceiver.decodedIRData.command);
      }
  }
}


void IR_input_map(int code){
  // CH- = 69, CH = 70, CH+ = 71, |<< = 68, >>| = 64, >|| = 67, - = 7, + = 21, EQ = 9, 100+ = 25, 200+ = 13, 
  // 0 = 22, 1 = 12, 2 = 24, 3 = 94, 4 = 8, 5 = 28, 6 = 90, 7 = 66, 8 = 82, 9 = 74
  if (code == 12){
    Serial.println(9000);
    arm.oldSonarFind();
  }
  if (code == 0){
    // code to cancel movement here
  }
}
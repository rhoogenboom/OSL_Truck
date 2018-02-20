void receiveControllerInfo() {
  if (radio.available()) {
    radio.read(&packet, sizeof(packet));
    int servoPosition = packet.controller3;
  
    if (servoPosition != 0) {
        updateServoPositions(limitToMaxPositionsServo(servoPosition));
    }       
  }
  
}


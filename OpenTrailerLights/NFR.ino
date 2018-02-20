void receiveControllerInfo() {
  if (radio.available()) {
    radio.read(&packet, sizeof(packet));
    Serial.println(packet.controller3);
  }
  int servoPosition = packet.controller3;

  if (servoPosition != 0) {
      Serial.println(servoPosition);
      updateServoPositions(limitToMaxPositionsServo(servoPosition));
  }       
  
}


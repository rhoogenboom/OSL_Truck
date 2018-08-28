void receiveControllerInfo() {
  if (radio.available()) {
    radio.read(&packet, sizeof(packet));
    int servoPosition = packet.controller3;
  
    if (servoPosition != 0) {
        updateServoPositions(limitToMaxPositionsServo(servoPosition));
    } 

    SetLights();
  //printPacketContents();  
  }
}

void printPacketContents() {
  Serial.print(F("Controller: ")); Serial.println(packet.controller3);
  for (int i=0;i<12;i++) {
    Serial.print(F("Light:  ")); Serial.println(packet.lights[i].state);
  }
}


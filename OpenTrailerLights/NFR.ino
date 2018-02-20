void receiveControllerInfo() {
  if (radio.available()) {
    radio.read(&controller, sizeof(controller));
    Serial.println(controller.controller1);
  }

  if (controller.controller1 != 0) {
      Serial.println(controller.controller1);
      updateServoPositions(limitToMaxPositionsServo(controller.controller1));
  }       
  
}


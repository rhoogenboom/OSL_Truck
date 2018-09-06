void transmitControllerInfo() {
  // Transmits the OSL controller info over wifi
  // RHO TODO: only transmit dirty data? consumes less bandwidth, but can OSL deal with it on the other end?  

  radio.write(&packet, sizeof(packet));

  if (DEBUG_OUTPUT) {
    Serial.print(F("Steering position"));
    Serial.print(F("   "));
    Serial.println(F(""));
    Serial.print(packet.controller3); 
    Serial.print(F("   "));
    Serial.println(F("")); 
  }  
}


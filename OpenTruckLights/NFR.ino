void transmitControllerInfo(char whatToTransmit) {
  // Transmits the OSL controller info over wifi
  // RHO TODO: only transmit dirty data? consumes less bandwidth, but can OSL deal with it on the other end?  
  char text[32];
  
  switch (whatToTransmit) {
    case '1' :
      controller.getControllerInfo().toCharArray(text, 32);
      radio.write(&text, sizeof(text));    
      Serial.println("1");
    break;
    case '2' :
       //write light 0-4
      controller.getLightsInfo1_4().toCharArray(text, 32);
      radio.write(&text, sizeof(text));
      delay(10);
    
      //write light 5-7
      controller.getLightsInfo5_8().toCharArray(text, 32);
      radio.write(&text, sizeof(text));   
      Serial.println("2");
    break;
    default:
    break;
  }
}


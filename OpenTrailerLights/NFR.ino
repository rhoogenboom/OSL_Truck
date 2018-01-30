void transmitControllerInfo() {
  // Transmits the OSL controller info over wifi
  // RHO TODO: only transmit dirty data? consumes less bandwidth, but can OSL deal with it on the other end?  

  //write controller info
  char text[32];
  controller.getControllerInfo().toCharArray(text, 32);
  radio.write(&text, sizeof(text));
 
  //write light 0-4
  controller.getLightsInfo1_4().toCharArray(text, 32);
  radio.write(&text, sizeof(text));

  //write light 5-7
  controller.getLightsInfo5_8().toCharArray(text, 32);
  radio.write(&text, sizeof(text));
}



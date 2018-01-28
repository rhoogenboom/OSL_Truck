void transmitControllerInfo() {
  // Transmits the OSL controller info over wifi
  // RHO TODO: only transmit dirty data? consumes less bandwidth, but can OSL deal with it on the other end?  

  //write controller info
  String text = controller.getControllerInfo();
  radio.write(&text, sizeof(text));

  //write light 0-4
  text = controller.getLightsInfo1_4();
  radio.write(&text, sizeof(text));

  //write light 5-7
  text = controller.getLightsInfo5_8();
  radio.write(&text, sizeof(text));
}


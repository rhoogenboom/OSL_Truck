void transmitControllerInfo() {
  // Transmits the OSL controller info over wifi
  // RHO TODO: only transmit dirty data? consumes less bandwidth, but can OSL deal with it on the other end?  

  radio.write(&packet, sizeof(packet));
}


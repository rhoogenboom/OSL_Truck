//check if input is within allowed range, if not, then receiver might be disconnected so we just return the middle
int limitToMaxPositionsFromReceiver(int input) {
  if (input == 0 || input < MIN_CHANNEL || input > MAX_CHANNEL) {
    return MixSteering_PulseCenter;
  } else {
    //TODO fix so we can use full stick range but still link to plate range for movement
    
    //below limits the receiver input range to the pot range so we can we can subtract or add them together
    return input;
  }
}

//translate the potmeter input to the same values as the receiver input so they can be easily subtracted from each other.
int convertPotmeterToReceiverValues(int input) {
  return map(input, minValueMeasuredForPot, maxValueMeasuredForPot, MIN_CHANNEL, MAX_CHANNEL);
}

//Mix the manual steering from receiver with the pot input and come to final angle for wheels
int mixPlateAndReceiverInput(int receiver, int potmeter) {
  //if receiver is centered we can just get out of here with just the potmeter
  if ( receiver == 0 || receiver < (CHANNEL_CENTER - CHANNEL_DEADCENTER) || receiver > (CHANNEL_CENTER + CHANNEL_DEADCENTER)) {
    //receiver is not centered, so do something with it
    if (receiver <= CHANNEL_CENTER) {
      // we need to subtract it from potmeter
      return (potmeter - abs((CHANNEL_CENTER-receiver)));
    } else {
      //we need to add it to potmeter
      return (potmeter + (receiver-CHANNEL_CENTER));
    }
  } else {
    return potmeter;
  }
}

int CalculateRearAxlePosition(int receiver) {
  // reads the value of the potentiometer
  int potInput = analogRead(PotMeter_Pin); //input: 0-1024 
  int analogPotmeterInput = convertPotmeterToReceiverValues(potInput); //input range: 0-1023 limit: 875-2125

   //validat input from receiver
  int analogReceiverInput = limitToMaxPositionsFromReceiver(receiver); //input range: 875-2125 limit: 875-2125
  
  //mix the receiver and the pot together but only when the sticks are out of center
  int resultingSteering = mixPlateAndReceiverInput(analogReceiverInput, analogPotmeterInput); //input: 875-2125 / 875-2125; 

  if (DEBUG_INPUT) {
    Serial.print(F("Potmeter Input"));
    Serial.print(F("   "));
    Serial.print(F("Potmeter Converted"));
    Serial.print(F("   "));
    Serial.print(F("Steering Input"));
    Serial.print(F("   "));
    Serial.print(F("Steering Result"));
    Serial.print(F("   "));
    Serial.println(F("Trailer steering"));
    Serial.print(potInput); 
    Serial.print(F("             "));
    Serial.print(analogPotmeterInput); 
    Serial.print(F("                  "));
    Serial.print(analogReceiverInput); 
    Serial.print(F("                  "));
    Serial.print(resultingSteering); 
    Serial.print(F("               "));
    Serial.println(map(resultingSteering, MIN_CHANNEL, MAX_CHANNEL, maxPositionLeftFrontServo, maxPositionRightFrontServo)); 
    delay(1000);
  }
  return map(resultingSteering, MIN_CHANNEL, MAX_CHANNEL, maxPositionLeftFrontServo, maxPositionRightFrontServo);
}


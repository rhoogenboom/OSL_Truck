//limits the range which we consider the full movement between truck and trailer angle, either left of right
int limitToMaxPositionsOnPlate(int input) {
  //if input is smaller than the minimum we would accept, we update to minimum
  if (input < potMaxPositionLeft) {
    input = potMaxPositionLeft;
  }
  //if input is larger than the maximum we would accept, we update to maximum
  if (input > potMaxPositionRight) {
    input = potMaxPositionRight;
  }
  return input;
}

//check if input is within allowed range, if not, then receiver might be disconnected so we just return the middle
int limitToMaxPositionsFromReceiver(int input) {
  if (input == 0 || input < MIN_CHANNEL || input > MAX_CHANNEL) {
    return map(MixSteering_PulseCenter, MIN_CHANNEL, MAX_CHANNEL, minValueMeasuredForPot, maxValueMeasuredForPot);
  } else {
    //TODO fix so we can use full stick range but still link to plate range for movement
    
    //below limits the receiver input range to the pot range so we can we can subtract or add them together
    return map(input, MIN_CHANNEL, MAX_CHANNEL, minValueMeasuredForPot, maxValueMeasuredForPot);
  }
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
  int analogPotmeterInput = analogRead(PotMeter_Pin); //input: 0-1024 
  int potInput = analogPotmeterInput;

   //translate the receiver input to the same range of the pot input
  int analogReceiverInput = limitToMaxPositionsFromReceiver(receiver); //input range: 875-2125 limit: 0-1024
  //mix the receiver and the pot together but only when the sticks are out of center
  analogPotmeterInput = mixPlateAndReceiverInput(analogReceiverInput, analogPotmeterInput); //input: 0-1024 / 0-1024; 

  // limit the pot values to what we expect them to be max left and right
  analogPotmeterInput = limitToMaxPositionsOnPlate(analogPotmeterInput);

  // translate plate position to relative position between max left and right
  int positionPotmeter = map(analogPotmeterInput, potMaxPositionLeft, potMaxPositionRight, minValueMeasuredForPot, maxValueMeasuredForPot);
  if (DEBUG_INPUT) {
    Serial.print(F("Potmeter Input"));
    Serial.print(F("   "));
    Serial.print(F("Potmeter Converted"));
    Serial.print(F("   "));
    Serial.print(F("Steering Input"));
    Serial.print(F("   "));
    Serial.println(F("Trailer steering"));
    Serial.print(potInput); 
    Serial.print(F("             "));
    Serial.print(analogPotmeterInput); 
    Serial.print(F("                  "));
    Serial.print(analogReceiverInput); 
    Serial.print(F("               "));
    Serial.println(map(positionPotmeter, 0, maxValueMeasuredForPot, maxPositionLeftFrontServo, maxPositionRightFrontServo)); 
    delay(1000);
  }
  return map(positionPotmeter, 0, maxValueMeasuredForPot, maxPositionLeftFrontServo, maxPositionRightFrontServo);
}


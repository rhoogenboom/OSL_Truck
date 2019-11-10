//check if input is min max range and limit it
int limitToMaxPositionsFromPotmeter(int input) {
  if (input <= minValueMeasuredForPot) {
    return minValueMeasuredForPot;
  } 
  else {
    if (input > maxValueMeasuredForPot) {
      return maxValueMeasuredForPot;
    }
    else {
        return input;
    }
  }
}

//check if input is within allowed range, if not, then receiver might be disconnected so we just return the middle
int limitToMaxPositionsFromReceiver(int input) {
  if (input == 0 || input < MIN_CHANNEL || input > MAX_CHANNEL) {
    return MixSteering_PulseCenter;
  } else {
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
  if ( receiver == 0 || (receiver > (RC_CHANNEL_CENTER - CHANNEL_DEADCENTER) && receiver < (RC_CHANNEL_CENTER + CHANNEL_DEADCENTER)) ) {
    return potmeter;
  } else {
    //receiver is not centered, so do something with it
    if (receiver <= RC_CHANNEL_CENTER) {
      // we need to subtract it from potmeter
      return (potmeter - abs((RC_CHANNEL_CENTER-receiver)));
    } else {
      //we need to add it to potmeter
      return (potmeter + (receiver-RC_CHANNEL_CENTER));
    }
  }
}

// takes the adjusted offset stored which corrects the middle of the magnet to the middle of the potmeter position
int ConvertPotmeterToMiddlePosition(int potMeterInput) {
  int convertedInput = potMeterInput + potMeterMiddleAdjustment;

  //if real middle is a lower value than 512 compensate on the over 1024 area
  if (potMeterMiddleAdjustment > 0) {
    // figure out if we need compensating calculation as we might have gone beyond the 1024
    if ( potMeterInput > (1024 - potMeterMiddleAdjustment) ) {
      // compensate as we went over the end of the sensor so need to work back to 1 to x value
      convertedInput = (potMeterInput + potMeterMiddleAdjustment) - 1024;
    }
  } else {
    //if real middle is a higher value than 512 compensate on the smaller than 0 area
    if ( potMeterInput <= abs(potMeterMiddleAdjustment) ) {
      // compensate as we went over the end of the sensor so need to work back to 1 to x value
      convertedInput = (1024 + potMeterMiddleAdjustment) + potMeterInput;
    }
  }
  return convertedInput;
}


int CalculateRearAxlePosition(int receiver) {
  // reads the value of the potentiometer
  // run the value through a conversion because the magnet might just be a little bit off center
  //potInput = analogRead(PotMeter_Pin); //input: 0-1024 
  potInput = ConvertPotmeterToMiddlePosition(analogRead(PotMeter_Pin)); //input: 0-1024 

  int limitedPotmeterInput = limitToMaxPositionsFromPotmeter(potInput); //input range: 0-1023 limit: min and max for potmeter
  
  int analogPotmeterInput = convertPotmeterToReceiverValues(limitedPotmeterInput); //input range: 0-1023 limit: 875-2125

   //validat input from receiver
  int analogReceiverInput = limitToMaxPositionsFromReceiver(receiver); //input range: 875-2125 limit: 875-2125
  
  //mix the receiver and the pot together but only when the sticks are out of center
  int resultingSteering = mixPlateAndReceiverInput(analogReceiverInput, analogPotmeterInput); //input: 875-2125 / 875-2125; 
  debugCounter++;
  
  if (DEBUG_INPUT && ((debugCounter % 20) == 0)) {
    debugCounter=0;
    Serial.print(F("Min pot value - Max pot value : "));
    Serial.print(minValueMeasuredForPot); Serial.print(F(" - ")); Serial.println(maxValueMeasuredForPot);
    
    Serial.print(F("Potmeter Input"));
    Serial.print(F("   "));
    Serial.print(F("Potmeter Limit"));
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
    Serial.print(limitedPotmeterInput); 
    Serial.print(F("                  "));
    Serial.print(analogPotmeterInput); 
    Serial.print(F("                  "));
    Serial.print(analogReceiverInput); 
    Serial.print(F("                  "));
    Serial.print(resultingSteering); 
    Serial.print(F("               "));
    Serial.println(map(resultingSteering, MIN_CHANNEL, MAX_CHANNEL, servoMinPulse, servoMaxPulse)); 
  }
  return map(resultingSteering, MIN_CHANNEL, MAX_CHANNEL, servoMinPulse, servoMaxPulse);
}


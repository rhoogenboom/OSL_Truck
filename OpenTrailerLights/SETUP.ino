void LightMode() {
  
}

void SetupMode() {
  pinMode(MAGNET_POT, INPUT);
  pinMode(SERVO_POT, INPUT);
  pinMode(MAGNET_LEFT_LED_RED, OUTPUT);
  pinMode(MAGNET_RIGHT_LED_GREEN, OUTPUT);
}

void ReadPotMeters() {
  //read magnet pot 
  int potInput = analogRead(MAGNET_POT); //input: 0-1024 
  Serial.print("Pot input: "); Serial.println(potInput);
  
  //adjust magnet min and max
  potMaxPositionLeft = potInput;
  potMaxPositionRight = potMiddlePosition + (potMiddlePosition - potMaxPositionLeft); //maximal range potmeter movement right

  // read servo pot
  potInput = analogRead(SERVO_POT); //input: 0-1024 
  Serial.print("Pot input: "); Serial.println(potInput);

  //adjust servo min and max
  servoMinPulse = map(potInput, MIN_VALUE_MEASURED_FOR_POT, MAX_VALUE_MEASURED_FOR_POT, SERVO_MIN_POSITION, SERVO_MAX_POSITION);
  servoMaxPulse = SERVO_MIDDLE_POSITION + (SERVO_MIDDLE_POSITION - servoMinPulse);
}

void SetMagnetLEDs(int potInput) {
  int output = 0;
  if (potInput > CHANNEL_CENTER) {
    //past middle to the right

    //set left full
    analogWrite(MAGNET_LEFT_LED_RED, 0);

    //scale right down
    output = map((potInput - CHANNEL_CENTER), 0, CHANNEL_CENTER, 0, 255);
  }
  else {
    //before the middle to the left

    //set right full
    analogWrite(MAGNET_RIGHT_LED_GREEN, 255);

    //scale down the left
    output = map(potInput, 0, CHANNEL_CENTER, 255, 0);    
  }
  
}













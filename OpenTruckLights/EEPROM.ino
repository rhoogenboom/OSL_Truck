void Initialize_EEPROM()
{ // If EEPROM has not been used before, we initialize to some sensible, yet conservative, default values.
  // The first time a radio setup is performed, these will be overwritten with actual values, and never referred to agian.
  // Because the  radio setup is the first thing a user should do, these in fact should not come into play.
  servoMinPulse = 1200;
  minValueMeasuredForPot = 350;
  potMeterMiddleAdjustment = 0;

  eeprom_write(servoMinPulse, E_servoMinPulse);
  eeprom_write(minValueMeasuredForPot, E_minValueMeasuredForPot);
  eeprom_write(potMeterMiddleAdjustment, E_potMeterMiddleAdjustment);

  // This is our initialization constant
  eeprom_write(EEPROM_Init, E_InitNum);

  // Finally - we still need to set up our variables, so now we call Load_EEPROM
  Load_EEPROM();
}

//load all our saved values at startup
void Load_EEPROM()
{
  eeprom_read(servoMinPulse, E_servoMinPulse);
  eeprom_read(minValueMeasuredForPot, E_minValueMeasuredForPot);
  eeprom_read(potMeterMiddleAdjustment, E_potMeterMiddleAdjustment);

  maxValueMeasuredForPot = potMiddlePosition + (potMiddlePosition - minValueMeasuredForPot);
  servoMaxPulse = SERVO_MIDDLE_POSITION + (SERVO_MIDDLE_POSITION - servoMinPulse);

  Serial.print("Potmeter center adjustment: "); Serial.println(potMeterMiddleAdjustment);
  Serial.print("Servo min pulse: "); Serial.println(servoMinPulse);
  Serial.print("Servo max pulse: "); Serial.println(servoMaxPulse);
  Serial.print("Pot max pos left(min): "); Serial.println(minValueMeasuredForPot);
  Serial.print("Pot max pos right(max): "); Serial.println(maxValueMeasuredForPot);
}

void Write_EEPROM() {
  // This is our initialization constant
  eeprom_write(EEPROM_Init, E_InitNum);
  eeprom_write(servoMinPulse, E_servoMinPulse);
  eeprom_write(minValueMeasuredForPot, E_minValueMeasuredForPot);
  eeprom_write(potMeterMiddleAdjustment, E_potMeterMiddleAdjustment);

  Serial.println("Wrote to EEPROM: "); 
  Serial.print("Servo min pulse: "); Serial.println(servoMinPulse);
  Serial.print("Pot max pos left(min): "); Serial.println(minValueMeasuredForPot);
  Serial.print("Potmeter center adjustment: "); Serial.println(potMeterMiddleAdjustment);
}


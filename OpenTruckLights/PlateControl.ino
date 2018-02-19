const int CHANNEL_CENTER = 511;
const int CHANNEL_DEADCENTER = 10;
const int MIN_CHANNEL = 875;
const int MAX_CHANNEL = 2125;

int deadCentreWidth = 2;
int potDeviation = 25;

int previousPlatePosition;

const int minValueMeasuredForPot = 0;
const int maxValueMeasuredForPot = 1023;
int potMiddlePosition = 514; //overruled the middle position of maxValueMeasuredForPot divided by two 2 due to the magnetic center

const int potMaxPositionLeft = 350; //minimale range potmeter movement left
const int potMaxPositionRight = potMiddlePosition + (potMiddlePosition - potMaxPositionLeft); //maximal range potmeter movement right

const int servoMinPulse = 1200;
const int servoMaxPulse = 1800;

int maxPositionLeftFrontServo = servoMinPulse; //maximale uitslag naar links voorste servo
int maxPositionRightFrontServo = servoMaxPulse; //maximale uitslag naar rechts voorste servo

int maxPositionLeftRearServo = servoMinPulse; //maximale uitslag naar links achterste servo
int maxPositionRightRearServo = servoMaxPulse; //maximale uitslag naar rechts achterste servo


int limitToMaxPositionsOnPlate(int input) {
  //below limits the range which we consider the full movement between truck and trailer angle, either left of right
  
  //if input is smaller than the minimum we would accept, update to minimum
  if (input < potMaxPositionLeft) {
    input = potMaxPositionLeft;
  }
  //if input is larger than the maximum we would accept, update to maximum
  if (input > potMaxPositionRight) {
    input = potMaxPositionRight;
  }
  return input;
}

int limitToMaxPositionsFromReceiver(int input) {

  //check if input is within allowed range, if not, the receive might be disconnected so we just return the middle
  if (input == 0 || input < MIN_CHANNEL || input > MAX_CHANNEL) {
    return Channel3PulseCenter;
  } else {
    //TODO fix so we can use full stick range but still link to plate range for movement
    
    //below limits the receiver input range to the pot range so we can we can subtract or add them together
    return map(input, MIN_CHANNEL, MAX_CHANNEL, minValueMeasuredForPot, maxValueMeasuredForPot);
  }
}

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
  int analogPotmeterInput = analogRead(PotMeter); //input: 0-1024 

   //translate the receiver input to the same range of the pot input
  int analogReceiverInput = limitToMaxPositionsFromReceiver(receiver); //input: 875-2125? range: 875-2125 limit: 0-1024

  //mix the receiver and the pot together but only when the sticks are out of center
  analogPotmeterInput = mixPlateAndReceiverInput(analogReceiverInput, analogPotmeterInput); //input: 0-1024 / 0-1024; 

  // limit the pot values to what we expect them to be max left and right
  analogPotmeterInput = limitToMaxPositionsOnPlate(analogPotmeterInput);
   
  // translate plate position to relative position between max left and right
  int positionPotmeter = map(analogPotmeterInput, potMaxPositionLeft, potMaxPositionRight, minValueMeasuredForPot, maxValueMeasuredForPot);
  return map(positionPotmeter, 0, maxValueMeasuredForPot, maxPositionLeftFrontServo, maxPositionRightFrontServo);
}


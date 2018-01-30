int SERVO_VOOR_PIN = 48;        //Arduino pin to which the servo is attached D48/pin36
int SERVO_ACHTER_PIN = 49;      //Arduino pin to which the servo is attached D49/pin35

bool startFound = false;
bool endFound = false;

const int servoMinPulse = 1200;
const int servoMaxPulse = 1800;

Servo servoFront;               // front servo (3 axles)
Servo servoRear;                // rear servo (2 axles)

int oldPosition = -1;

void updateServoPositions(int relativePosition) {
  servoFront.write(relativePosition);
  servoRear.write(relativePosition);
}

int limitToMaxPositionsServo(int input) {
  //below limits the range which we consider the full movement between truck and trailer angle, either left of right
  
  //if input is smaller than the minimum we would accept, update to minimum
  if (input < servoMinPulse) {
    input = servoMinPulse;
  }
  //if input is larger than the maximum we would accept, update to maximum
  if (input > servoMaxPulse) {
    input = servoMaxPulse;
  }
  return input;
}


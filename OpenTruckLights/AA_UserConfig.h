//ms transmission interval
#define transmissionInterval 500  

// Set this to true to receive debugging messages out the serial port. NOTE: This will make the program less responsive, 
#define DEBUG_INPUT true         
#define DEBUG_OUTPUT false         

// Set baud rate here if you know what you're doing and don't like the default value
#define BaudRate 9600// 38400          

// Use interupts for IO
#define INTERUPT_IO true

//Middle point of the mix steering channel 
#define CHANNEL_CENTER 511

//Bandwidth around the center of the mix steering channel where we ignore it and just use the potmeter input
#define CHANNEL_DEADCENTER 10

//Minimum and maximum valid inputs for mix steering channel
#define MIN_CHANNEL 875
#define MAX_CHANNEL 2125
#define MixSteering_PulseCenter 1500 //1472

//Minimum and maximum valid inputs for potmeter
#define minValueMeasuredForPot 0
#define maxValueMeasuredForPot 1023

//overruled the middle position of maxValueMeasuredForPot divided by two 2 due to the magnetic center
#define potMiddlePosition 514

//Limit the pot input up to this minimal value
#define potMaxPositionLeft 350 

//Limit the pot input up to this maximum value
#define potMaxPositionRight (potMaxPositionLeft + (2 * (potMiddlePosition - potMaxPositionLeft)))

//Limit the steering servos to these maximum values for left and right
#define servoMinPulse 1200
#define servoMaxPulse 1800

//Maintain same min and max for front servo
#define maxPositionLeftFrontServo servoMinPulse
#define maxPositionRightFrontServo servoMaxPulse

#define maxPositionLeftRearServo servoMinPulse
#define maxPositionRightRearServo servoMaxPulse



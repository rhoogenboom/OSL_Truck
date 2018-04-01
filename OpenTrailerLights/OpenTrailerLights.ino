 /*
 * Open Trailer Lights - Truck  
 * An Arduino-based program to control LED lights and servos on RC utility vehicles and via wifi their trailer. 
 * Last Updated:        06/01/2018 - rhoogenboom
 *  
 * GitHub Repository OSL_Truck:
 * https://github.com/rhoogenboom/OSL_Truck
 *
 *
 * Version history
 * 6 Jan 2018 - v1.0 - initial project initialization
 * 
 * 
 *
 * For more information on OSL, see the RCGroups thread: 
 * http://www.rcgroups.com/forums/showthread.php?t=1539753
 * 
 * GitHub Repository OSL_Original:
 * https://github.com/OSRCL/OSL_Original
 * 
 *
 * To compile select               Tools -> Board     -> Arduino Nano
 * Also select correct processor   Tools -> Processor -> ATmega328 (P)
 *
 *
 * CREDITS!    CREDITS!    CREDITS!
 *----------------------------------------------------------------------------------------------------------------------------------------------------->
 * Several people have contributed code to this project
 * 
 *  taken from OSL_Original 2.10 by Luke Middleton
 *
 * Richard & Nick       RCGroups username "Rbhoogenboom" and "NickSegers"
 *                          June 2016 - created an Excel file to simplify the light setups. It will automatically generate the entire AA_LIGHT_SETUP file. 
 *                          Download from the first post of the thread linked above. 
 * Sergio Pizzotti      RCGroups username "wormch"
 *                          March 2015 - Made several impressive changes specifically for drift cars. Wrote all the code related to the backfiring and Xenon effects. 
 *                          Made ChangeSchemeMode more user-friendly, it can only be entered after the car has been stopped several seconds. 
 *                          Also fixed some bugs and taught me the F() macro!
 * Patrik               RCGroups username "Orque"
 *                          March 2015 - Expanded the Channel 3 functionality to read up to a 5 position switch (previously only worked to 3 positions)        
 * Jens                 RCGroups username "learningarduino"
 *                          October 2014 - Fixed bugs related to pin initialization and debug printing. 
 * Peter                RCGroups username "4x4_RC_Pit"
 *                          September 2014 - Fixed several bugs in the RadioSetup routine. Also the first person to post a video of OSL in action.
 * JChristensen         We are using Christensen's button library unchanged. It has been renamed from Button to OSL_Button simply because there are many Arduino
 *                      button libraries, and we don't want the install of this one to conflict with others you may already have. See JChristensen's project page here:
 *                      https://github.com/JChristensen/Button
 *
 * Open Source Lights is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v3 as published by
 * the Free Software Foundation (http://www.gnu.org/licenses/)
 *
*/

#include "AA_UserConfig.h"
#include "Defines.h"
#include <OSL_SimpleTimer.h>
#include <EEPROM.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include <Servo.h>

//#include <OSLController.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const byte NFR_CE              =    3;                       // The Arduino pin connected to the NFR CE pin D3/pin7
const byte NFR_CSN             =    4;                       // The Arduino pin connected to the NFR CSN pin D4/pin1

/*
 * NFR wiring
 * VCC - 3.3v!!!!
 * GND - GND
 * CSN / CE see above
 * Nano:
 * SCK - pin 13
 * MOSI - pin 11
 * MISO = pin 12
 * 
 * 
 */

int SERVO_VOOR_PIN = 6;        //Arduino nano pin to which the servo is attached D48/pin36
int SERVO_ACHTER_PIN = 7;      //Arduino pin to which the servo is attached D49/pin35
        

const int NA                   =    -1;                 // For each of the 8 states the light can have the following settings: On, Off, NA, Blink, FastBlink, or Dim. On/Off are defined below
const int BLINK                =    -2;                 // These give us numerical values to these names which makes coding easier, we can just type in the name instead of the number. 
const int FASTBLINK            =    -3;
const int SOFTBLINK            =    -4;
const int DIM                  =    -5;
const int FADEOFF              =    -6;
const int XENON                =    -7;
const int BACKFIRE             =    -8;

const byte ON = 1;
const byte OFF = 0;
const byte YES = 1;
const byte NO = 0;

OSL_SimpleTimer                   timer;                 // Instantiate a SimpleTimer named "timer"
boolean TimeUp                 =  true;

boolean Startup                =  true;                 // This lets us run a few things in the main loop only once instead of over and over

int CurrentScheme;                                      // Indicates which scheme is presently selected and active. Number from 1 to NumSchemes. 
                                                        // Note that the actual schemes are zero-based (0 to NumSchemes-1) but don't worry about that,
                                                        // the code takes care of it. 
#define NumLights                    12                 // The number of light outputs available on the board
#define NumStates                    10                 // There are 10 possible states a light can be by: 

int ActualDimLevel;                                             // We allow the user to enter a Dim level from 0-255. Actually, we do not want them using numbers 0 or 1. The ActualDimLevel corrects for this.
                                                                // In practice, it is unlikely a user would want a dim level of 1 anyway, as it would be probably invisible. 

//Nano pins:
int LightPin[NumLights] = {2,5,8,9,10,A0,A1,A2,A3,A4,A5,A6};       // These are the Arduino pins to the lights  
int Dimmable[NumLights] = {0,1,0,1,1,  0, 0, 0, 0, 0, 0, 0};            // This indicates which of these pins are capable of ouputting PWM, in order. 

//Mega pins:
//int LightPin[NumLights] = {9,10,11,6,7,8,12,13,46,5,17,45};       // These are the Arduino pins to the lights  
//int Dimmable[NumLights] = {1,1,1,1,1,1,1,1,1,1,1,1};            // This indicates which of these pins are capable of ouputting PWM, in order. 

                                                                // Dimmable must be true in order for the light to be capable of DIM, FADEOFF, or XENON settings
int LightSettings[NumLights][NumStates];                        // An array to hold the settings for each state for each light. 
int PriorLightSetting[NumLights][NumStates];                    // Sometimes we want to temporarily change the setting for a light. We can store the prior setting here, and revert back to it when the temporary change is over.
int PWM_Step[NumLights] = {0,0,0,0,0,0,0,0,0,0,0,0};            // What is the current PWM value of each light. 

// FadeOff effect
int FadeOff_EffectDone[NumLights] = {0,0,0,0,0,0,0,0,0,0,0,0};  // For each light, if = 1, then the Fade  effect is done, don't do it again until cleared (Fade_EffectDone = 0)

// Xenon effect
int Xenon_EffectDone[NumLights] = {0,0,0,0,0,0,0,0,0,0,0,0};    // For each light, if = 1, then the Xenon effect is done, don't do it again until cleared (Xenon_EffectDone = 0)
int Xenon_Step[NumLights]       = {0,0,0,0,0,0,0,0,0,0,0,0};    // Save the current step variable for the Xenon light effect
unsigned long Xenon_millis[NumLights] = {0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long Xenon_interval    = 25;                   // The interval between the various step of the Xenon effect

// Blinking effect
boolean Blinker                =  true;                 // A flip/flop variable used for blinking
boolean FastBlinker            =  true;                 // A flip/flop variable used for fast blinking
boolean IndividualLightBlinker[NumLights] = {true, true, true, true, true, true, true, true, true, true, true, true};   // A flip/flop variable but this time one for each light. Used for SoftBlink.

unsigned int BlinkOffID;                                // SimpleTimer ID number for the blinking lights
static boolean Blinking;                                // Are the lights blinking? 
static boolean State;                                   // If blinking, are they blinking on (true) or off (false)? 
static boolean PriorState;                              // Blinking state in the prior iteration
static int TimesBlinked;                                // How many times have the lights blinked

const byte address[6] = {0x66,0x68,0x7b,0x4a,0x63};   

RF24 radio(NFR_CE, NFR_CSN); // NFR CE, CSN connections
//NFR pin connections:
//Arduino
//Pin   Pin Name            Mapped  Pin   Name       Connected To  On Device
//1     PG5 ( OC0B )        Digital pin 4 (PWM)       CSN           NFR
//7     PE5 ( OC3C/INT5 )   Digital pin 3 (PWM)       CE            NFR
//11    GND GND GND NFR
//20    PB1 ( SCK/PCINT1 )  Digital pin  (SCK)        SCK           NFR
//21    PB2 ( MOSI/PCINT2 ) Digital pin  (MOSI)       MOSI          NFR
//22    PB3 ( MISO/PCINT3 ) Digital pin  (MISO)       MISO          NFR

typedef struct
{
  int8_t state;
} OSLLight;

typedef struct
{
//  uint16_t controller1;
//  uint16_t controller2;
  uint16_t controller3;
  OSLLight lights[12];
} OSLLightPacket;

volatile OSLLightPacket packet;

// front servo (3 axles)
Servo servoFront;   
// rear servo (2 axles)
Servo servoRear;    
//Servo pin connections:
//Arduino
//Pin   Pin Name        Mapped  Pin   Name      Connected To  On Device
//                      Digital pin 6           Signal (Orange)  Servo
//                      Digital pin 7           Signal (Orange)  Servo
//                      GND                     Ground  (Brown)   Servo
//                                              PWR     (Red)     Servo



bool startFound = false;
bool endFound = false;

const int servoMinPulse = 1200;
const int servoMaxPulse = 1800;

void setup()
{
  Serial.begin(BaudRate);  

  for (int i=0; i<NumLights; i++)                             
  {
      packet.controller3 = 1500;
      packet.lights[i].state = OFF;
      pinMode(LightPin[i], OUTPUT);                       // Set all the external light pins to outputs
      TurnOffLight(i);                                    // Start with all lights off                        
  }

  SetupLights(CurrentScheme);                             // Set the lights to the Scheme last saved in EEPROM
  FixDimLevel();                                          // Takes care of a bug that only occurs if a user sets the Dim level to 1 (unlikely)

  // first steering servo at the front
  servoFront.attach(SERVO_VOOR_PIN);  
  // second steering servo at the front
  servoRear.attach(SERVO_ACHTER_PIN); 
  
  updateServoPositions(1500);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW); //RF24_PA_MIN = 0,RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX, RF24_PA_ERROR
  radio.setDataRate(RF24_2MBPS); //RF24_1MBPS = 0, RF24_2MBPS, RF24_250KBPS  
  radio.openReadingPipe(0, address);
  radio.startListening();
}



// ====================================================================================================================================================>
//  MAIN LOOP
// ====================================================================================================================================================>

void loop()
{
  if (Startup)
  {       
//      if (DEBUG) { DumpConfiguration(); }  //todo - dump current configuration to serial
      timer.setInterval(BlinkInterval, BlinkLights);
      timer.setInterval(FastBlinkInterval, FastBlinkLights);
      Startup = false;
  }
  timer.run();
  
  receiveControllerInfo();
} 



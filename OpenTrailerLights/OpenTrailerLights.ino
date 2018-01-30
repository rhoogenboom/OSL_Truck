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

//RHO TODO
// remove backfire light code

// fix check code of ch3 is a multiprop channel
// fix setup code for multiprop channel



// ====================================================================================================================================================>
//  INCLUDES 
// ====================================================================================================================================================>
    #include "AA_UserConfig.h"
    #include "Defines.h"
    #include <OSL_SimpleTimer.h>
    #include <EEPROM.h>
    #include <OSL_Button.h>  
    #include <avr/eeprom.h>
    #include <avr/pgmspace.h>

    #include <Servo.h>

    #include <OSLController.h>
    #include <SPI.h>
    #include <nRF24L01.h>
    #include <RF24.h>

   
// ====================================================================================================================================================>
//  GLOBAL VARIABLES
// ====================================================================================================================================================>
    // PINs
        //receiver items and model inputs
        const byte ThrottleChannel_Pin =    21;                      // The Arduino pin connected to the throttle channel input - D21/pin43
        const byte SteeringChannel_Pin =    20;                      // The Arduino pin connected to the steering channel input - D20/pin44
        const byte Channel3_Pin        =    19;                      // The Arduino pin connected to the multichannel input - D19/pin45
        const byte MixSteeringChannel_Pin = 18;                      // The Arduino pin connected to the steering channel for mixing the rear axles - D18/pin46
        const byte PotMeter            =    A1;                      // The Arduino pin connected to the potentio-meter from the 5th wheel A1/pin96

        //OSL board items
        const byte GreenLED            =    A4;                      // The Arduino pin connected to the on-board Green LED A4/pin93
        const byte RedLED              =    A5;                      // The Arduino pin connected to the on-board Red LED A5/pin92 
        const byte SetupButton         =    A0;                      // The Arduino pin connected to the on-board push button A0/pin97 

        const byte NFR_CSN             =    4;                       // The Arduino pin connected to the NFR CSN pin D4/pin1
        const byte NFR_CE              =    3;                       // The Arduino pin connected to the NFR CE pin D3/pin7
        
#define RC_Throttle_Index  0
#define RC_Steering_Index  1
#define RC_RearAxles_Index  3


    // Useful names 
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
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
        const byte PRESSED = 0;                                 // Used for buttons pulled-up to Vcc, who are tied to ground when pressed
        const byte RELEASED = 1;                                // Used for buttons pulled-up to Vcc, who are tied to ground when pressed

    // SIMPLE TIMER 
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
        OSL_SimpleTimer                   timer;                 // Instantiate a SimpleTimer named "timer"
        boolean TimeUp                 =  true;

    // STARTUP
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
        boolean Startup                =  true;                 // This lets us run a few things in the main loop only once instead of over and over
        int loopCounter = 0;

    // DRIVING
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
        boolean Braking                = false;                 // Are we braking            
        boolean Decelerating           = false;                 // Are we sharply decelerating 
        boolean Accelerating           = true;                  // Are we sharply accelerating
        boolean StoppedLongTime        = false;                 // Have we been stopped for a long time (actual length of time set on the UserConfig tab - LongStopTime_mS)

        typedef char DRIVEMODES; 
        #define UNKNOWN     0
        #define STOP        1
        #define FWD         2
        #define REV         3
        #define LAST_MODE   REV

        const __FlashStringHelper *printMode(DRIVEMODES Type);     //Returns a character string that is name of the drive mode.
        
        // Little function to help us print out actual drive mode names, rather than numbers. 
        // To use, call something like this:  Serial.print(printMode(DriveModeCommand));
        const __FlashStringHelper *printMode(DRIVEMODES Type) {
            if(Type>LAST_MODE) Type=UNKNOWN;
            const __FlashStringHelper *Names[LAST_MODE+1]={F("UNKNOWN"),F("STOP"),F("FORWARD"),F("REVERSE")};
            return Names[Type];
        };

        //RC channel variables 
        const int channelCount = 3;
        volatile unsigned long receiver_pulse_start[channelCount];     // Array to keep track of 3 timer values for calculating the pulse width of a receiver channel
        volatile int pulse_time[channelCount];                         //calcSignal is the interrupt handler 

        // Throttle
        int ThrottleCommand            =     0;                 // A mapped value of ThrottlePulse to (0, MapPulseFwd/Rev) where MapPulseFwd/Rev is the maximum FWD/REV speed (100, or less if governed)
        volatile int ThrottlePulse;                             // Positive = Forward, Negative = Reverse <ThrottlePulseCenter - ThrottlePulseMin> to <0> to <ThrottlePulseCenter + ThrottlePulseMax>
        int ThrottlePulseMin;                                   // Will ultimately be determined by setup procedure to read max travel on stick, or from EEPROM if setup complete
        int ThrottlePulseMax;                                   // Will ultimately be determined by setup procedure to read max travel on stick, or from EEPROM if setup complete
        int ThrottlePulseCenter;                                // EX: 1000 + ((2000-1000)/2) = 1500. If Pulse = 1000 then -500, 1500 = 0, 2000 = 500
        int ThrottleCenterAdjust       =     0;                 // If small throttle commands do not result in movement due to gearbox/track resistance, increase this number. FOR NOW, LEAVE AT ZERO. IF SET, MUST BE SMALLER THAN THROTTLEDEADBAND
        boolean ThrottleChannelReverse;                         // Can be used to reverse the throttle channel if they don't have reversing on radio
        int MaxFwdSpeed                =   100;                 // 
        int MaxRevSpeed                =  -100;                 // 

        // Steering
        boolean SteeringChannelPresent;                         // On startup we check to see if this channel is connected, if not, this variable gets set to False and we don't bother checking for it again until reboot
        int TurnCommand                =     0;                 // A mapped value of ThrottlePulse from (TurnPulseMin/TurnPulseMax) to MaxLeft/MaxRight turn (100 each way, or less if governed)
        volatile int TurnPulse;                                 // Positive = Right, Negative = Left <TurnPulseCenter - TurnPulseMin> to <0> to <TurnPulseCenter + TurnPulseMax>
        volatile int MixedTurnPulse;                            // 

        int TurnPulseMin;                                       // Will ultimately be determined by setup procedure to read max travel on stick, or from EEPROM if setup complete
        int TurnPulseMax;                                       // Will ultimately be determined by setup procedure to read max travel on stick, or from EEPROM if setup complete
        int TurnPulseCenter;                                    // EX: 1000 + ((2000-1000)/2) = 1500. If Pulse = 1000 then -500, 1500 = 0, 2000 = 500
        int TurnCenterAdjust           =     0;                 // Leave at ZERO for now
        boolean TurnChannelReverse;                             // Can be used to reverse the steering channel if they don't have reversing on radio
        int MaxRightTurn               =   100;                 // 
        int MaxLeftTurn                =  -100;
        boolean TurnSignal_Enable      =  true;                 // If the user decides to restrict turn signals only to when the car is stopped, they can further add a delay that begins
                                                                // when the car first stops, and until this delay is complete, the turn signals won't come on. This flag indicates if the delay
                                                                // is up. Initialize to true, otherwise turn signals won't work until the car has driven forward once and stopped.
        int TurnSignalOverride         =     0;                 // The other situation we want to modify the turn signal is when starting from a stop, while turning. In this case we leave the turn signals on
                                                                // for a period of time even after the car has started moving (typically turn signals are disabled while moving). This mimics a real car where
                                                                // the turn signal isn't cancelled until after the steering wheel turns back the opposite way. In our case we don't check the steering wheel, 
                                                                // we just wait a short amount of time (user configurable in AA_UserConfig.h, variable TurnFromStartContinue_mS)

        // Channel 3
        boolean Channel3Present;                                // On startup we check to see if this channel is connected, if not, this variable gets set to False and we don't bother checking for it again until reboot
        int Channel3Pulse;                                      // 
        int Channel3PulseMin;        
        int Channel3PulseMax;
        int Channel3PulseCenter;
        int Channel3                   =   OFF;                 // State of the Channel 3 switch: On (1), Off (0), Disconnected (-1)
        boolean Channel3Reverse;
        #define Pos1                         0                  // Position defines for Channel 3 switch (can be up to 5 positions)
        #define Pos2                         1
        #define Pos3                         2
        #define Pos4                         3
        #define Pos5                         4

        // Channel 3 - multiprop channel
        volatile unsigned long MultiPropItemTime= 0;
        volatile unsigned long multiPropStartTime = 0;
        volatile int multi_prop[9];
        int MultipropChannelPulse[9];
        volatile int current_multiprop_channel = 0;        

    
    // LIGHTS 
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
        int CurrentScheme;                                      // Indicates which scheme is presently selected and active. Number from 1 to NumSchemes. 
                                                                // Note that the actual schemes are zero-based (0 to NumSchemes-1) but don't worry about that,
                                                                // the code takes care of it. 
        #define NumLights                    12                 // The number of light outputs available on the board
        #define NumStates                    10                 // There are 10 possible states a light can be by: 
                                                                // - Set through 1 or more switches on the multiprop channel, 
                                                                // - Forward, Reverse, Stop, Stop Delay, Brake (from Throttle Channel), 
                                                                // - Right Turn, Left Turn (from Turn Channel)
                                                                // - Accelerating - 
                                                                // - Decelerating - special state that occurs on heavy deceleration (from Throttle Channel)

        const byte StateFwd            =    1; //5;                 // Moving forward
        const byte StateRev            =    2; //6;                 // Moving backwards
        const byte StateStop           =    3; //7;                 // Stopped
        const byte StateStopDelay      =    4; //8;                 // Stopped for a user-defined length of time
        const byte StateBrake          =    5; //9;                 // Braking
        const byte StateRT             =    6; //10;                // Right turn
        const byte StateLT             =    7; //11;                // Left turn
        const byte StateAccel          =    8; //12;                // Acceleration
        const byte StateDecel          =    9; //13;                // Deceleration
       
        int ActualDimLevel;                                             // We allow the user to enter a Dim level from 0-255. Actually, we do not want them using numbers 0 or 1. The ActualDimLevel corrects for this.
                                                                        // In practice, it is unlikely a user would want a dim level of 1 anyway, as it would be probably invisible. 
        int LightPin[NumLights] = {9,10,11,6,7,8,12,13,46,5,17,45};       // These are the Arduino pins to the 8 lights  
                                                                
        int Dimmable[NumLights] = {1,1,1,1,1,1,1,1,1,1,1,1};            // This indicates which of these pins are capable of ouputting PWM, in order. 
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

        // Backfire effect
        unsigned long backfire_interval;                        // Will save the random interval for the backfire effect
        unsigned long backfire_timeout;                         // Will save the random timeout interval to turn off the LED
        unsigned long Backfire_millis  =  0;                    // will store last time LED was updated
        boolean canBackfire            = false;                 // Is the backfiring effect currently active?

        // Overtaking effect
        boolean Overtaking             = false;
        
        // Blinking effect
        boolean Blinker                =  true;                 // A flip/flop variable used for blinking
        boolean FastBlinker            =  true;                 // A flip/flop variable used for fast blinking
        boolean IndividualLightBlinker[NumLights] = {true, true, true, true, true, true, true, true, true, true, true, true};   // A flip/flop variable but this time one for each light. Used for SoftBlink.

    // RC CHANNEL INPUTS
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
        boolean Failsafe               = false;                 // If we loose contact with the Rx this flag becomes True
        unsigned long ServoTimeout     = 35000;                 // Value in microseconds (uS) - length of time to wait for a servo pulse. Measured on Eurgle/HK 3channel at ~20-22ms between pulses
                                                                // Up to version 2.03 of OSL code this value was 21,000 (21ms) and it worked fine. However with the release of Arduino IDE 1.6.5, 
                                                                // something has changed about the pulseIn function, or perhaps the way it is compiled. At 21ms, pulseIn would return 0 every other read.
                                                                // Increasing the timeout to 30ms seems to have fixed it. LM - 7/15/2015
        int PulseMin_Bad               =   700;                 // Pulse widths lower than this amount are considered bad 
        int PulseMax_Bad               =  2300;                 // Pulse widths greater than this amount are considered bad
 
    // BOARD OBJECTS
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
        // Button Object
        OSL_Button InputButton = OSL_Button(SetupButton, true, true, 25);   // Initialize a button object. Set pin, internal pullup = true, inverted = true, debounce time = 25 mS

    // CHANGE-SCHEME-MODE MENU VARIABLES
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
        boolean canChangeScheme     = false;                    // Are we allowed to enter Change Scheme Mode? (This is set to true after being in the STOP state for several seconds)
        unsigned int BlinkOffID;                                // SimpleTimer ID number for the blinking lights
        static boolean Blinking;                                // Are the lights blinking? 
        static boolean State;                                   // If blinking, are they blinking on (true) or off (false)? 
        static boolean PriorState;                              // Blinking state in the prior iteration
        static int TimesBlinked;                                // How many times have the lights blinked
        static boolean ChangeSchemeMode = false;                    // A flag to indicate if we are in Change-Scheme-Mode or not

    // EEPROM
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
        const long EEPROM_Init         = 0xAA43;                // Change this any time the EEPROM content changes
        struct __eeprom_data {                                  // __eeprom_data is the structure that maps all of the data we are storing in EEPROM
          long E_InitNum;                                       // Number that indicates if EEPROM values have ever been initialized 
          int E_ThrottlePulseMin;
          int E_ThrottlePulseMax;
          int E_ThrottlePulseCenter;
          int E_TurnPulseMin;
          int E_TurnPulseMax;
          int E_TurnPulseCenter;
          int E_Channel3PulseMin;
          int E_Channel3PulseMax;
          int E_Channel3PulseCenter;
          boolean E_ThrottleChannelReverse;
          boolean E_TurnChannelReverse;
          boolean E_Channel3Reverse;
          int E_CurrentScheme;
        };

    // WIFI
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
        RF24 radio(NFR_CE, NFR_CSN); // NFR CE, CSN connections
        //NFR pin connections:
        //Arduino
        //Pin   Pin Name            Mapped Pin Name       Connected To  Device
        //1     PG5 ( OC0B )        Digital pin 4 (PWM)   CSN           NFR
        //7     PE5 ( OC3C/INT5 )   Digital pin 3 (PWM)   CE            NFR
        //11    GND GND GND NFR
        //20    PB1 ( SCK/PCINT1 )  Digital pin 52 (SCK)  SCK           NFR
        //21    PB2 ( MOSI/PCINT2 ) Digital pin 51 (MOSI) MOSI          NFR
        //22    PB3 ( MISO/PCINT3 ) Digital pin 50 (MISO) MISO          NFR
        const byte address[6] = {0x66,0x68,0x7b,0x4a,0x63};   

        OSLController controller;         // Wrapper object around 3 RC channels, drive mode and 8 light switches


// ====================================================================================================================================================>
//  SETUP
// ====================================================================================================================================================>
void setup()
{
    // SERIAL
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
       Serial.begin(BaudRate);  

    // PINS
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
        pinMode(RedLED, OUTPUT);                                // Set RedLED pin to output
        pinMode(GreenLED, OUTPUT);                              // Set GreenLED pin to output
        RedLedOff();                                            // Turn off board LEDs to begin with
        GreenLedOff();

        for (int i=0; i<NumLights; i++)                             
        {
            pinMode(LightPin[i], OUTPUT);                       // Set all the external light pins to outputs
            TurnOffLight(i);                                    // Start with all lights off                        
        }

        // Timer values for receiver channels
        for (int i=0;i<channelCount;i++) {
          receiver_pulse_start[i] = 0;
        }

        if (INTERUPT_IO) { 
          // Set these pins to input - not required
//          pinMode(ThrottleChannel_Pin, INPUT);             
//          pinMode(SteeringChannel_Pin, INPUT);
//          pinMode(Channel3_Pin, INPUT);
//          pinMode(MixSteeringChannel_Pin, INPUT);
          // Hook up interrupt handler functions for when data comes in
          attachInterrupt(digitalPinToInterrupt(ThrottleChannel_Pin), calcChannel1, CHANGE);
          attachInterrupt(digitalPinToInterrupt(SteeringChannel_Pin), calcChannel2, CHANGE);
          attachInterrupt(digitalPinToInterrupt(MixSteeringChannel_Pin), calcChannel3, CHANGE);
        } else {
          pinMode(ThrottleChannel_Pin, INPUT_PULLUP);
          pinMode(SteeringChannel_Pin, INPUT_PULLUP);
          pinMode(MixSteeringChannel_Pin, INPUT_PULLUP);
        }

        attachInterrupt(digitalPinToInterrupt(Channel3_Pin), calcMultiPropChannel, CHANGE);

    // LOAD VALUES FROM EEPROM    
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
        long Temp;
        eeprom_read(Temp, E_InitNum);                           // Get our EEPROM Initialization value
        if(Temp != EEPROM_Init)                                 // If EEPROM has never been initialized before, do so now
        {    Initialize_EEPROM();  }
        else
        {    Load_EEPROM();        }                            // Otherwise, load the values from EEPROM

    // RUN LIGHT SETUP
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
        SetupLights(CurrentScheme);                             // Set the lights to the Scheme last saved in EEPROM
        FixDimLevel();                                          // Takes care of a bug that only occurs if a user sets the Dim level to 1 (unlikely)

    // first steering servo at the front
    servoFront.attach(SERVO_VOOR_PIN);  
    // second steering servo at the front
    servoRear.attach(SERVO_ACHTER_PIN); 
  
    updateServoPositions(1500);

    //WIFI
        radio.begin();
        radio.openReadingPipe(0, address);
        radio.setPALevel(RF24_PA_MIN);
        radio.startListening();
}



// ====================================================================================================================================================>
//  MAIN LOOP
// ====================================================================================================================================================>

void loop()
{
    // LOCAL VARIABLES
    // ------------------------------------------------------------------------------------------------------------------------------------------------>    
    // Drive Modes
    static int DriveModeCommand = UNKNOWN;                      // The Drive Mode presently being commanded by user. At startup we don't know, so initialize to UNKNOWN.
    static int DriveMode = STOP;                                // As opposed to DriveModeCommand, this is the actual DriveMode being implemented. 
    static int DriveMode_Previous = STOP;                       // The previous Drive Mode implemented. There is no "previous" when first started, initialize to STOP
    static int DriveModeCommand_Previous = STOP;                // There is no "previous" command when we first start. Initialize to STOP
    static int DriveMode_LastDirection = STOP;
    static boolean DriveModeTransition = false;                 // Are we in the midst of a drive mode transition event? 
    static int TransitionMode;                                  // Which transition are we doing? 
    static int ReverseTaps = 0;
    
    static int ThrottleCommand_Previous;
    
    // Shifting Direction
    static int DriveFlag = NO;                                  // We start with movement allowed
    static unsigned long TransitionStart;                       // A marker which records the time when the shift transition begins
    static unsigned long StopCMDLength;                         // Counter to determine how long we have been commanding Stop

    // Stop time
    static unsigned long TimeStopped;                           // When did we stop - will be used to initiate a delay after stopping before turn signals can be used, or when stop-delay light settings can take effect

    // Scheme change variables
    static int MaxTurn;                                         // This will end up being 90 percent of Turn Command, we consider this a full turn
    static int MinTurn;                                         // This will end up being 10-20 percent of Turn Command, this is the minimum movement to be considered a turn command during Change-Scheme-Mode
    static byte RightCount = 0;                                 // We use these to count up the number of times the user has cranked the
    static byte LeftCount = 0;                                  // steering wheel completely over. 
    static int ChangeModeTime_mS = 2000;                        // Amount of time user has to enter Change-Scheme-Mode 
    static boolean ChangeSchemeTimerFlag = false;               // Has the timer begun
    static unsigned int TurnTimerID;                            // An ID for the timer that counts the turns of the wheel                      
    static int ExitSchemeFlag = 0;                              // A flag to indicate whether or not to exit Change-Scheme-Mode
    static int TimeToWait_mS = 1200;                            // Time to wait between change-scheme commands (otherwise as long as you held the wheel over it would keep cycling through rapidly)
    static int TimeToExit_mS = 3000;                            // How long to hold the wheel over until Change-Scheme-Mode is exited
    static unsigned long ExitStart;                             // Start time of the exit waiting period
    static boolean TimeoutFlag;
    static boolean HoldFlag; 
    static unsigned long HoldStart;

    // Backfire
    static unsigned int BackfireTimerID = 0; 
    // Overtaking
    static unsigned int OvertakeTimerID = 0;

    // Temp vars
    static unsigned long currentMillis;        
    static boolean WhatState = true;
    int i;    
    


    // STARTUP - RUN ONCE
    // ------------------------------------------------------------------------------------------------------------------------------------------------>    
    if (Startup)
    {       
        if (DEBUG) { DumpControllerValues(true); }                             // This puts some useful information to the Serial Port
        
        timer.setInterval(BlinkInterval, BlinkLights);          // This will call the function BlinkLights every BlinkInterval milliseconds
        timer.setInterval(FastBlinkInterval, FastBlinkLights);  // This will call the function FastBlinkLights every FastBlinkInterval milliseconds
        
        currentMillis = millis();                               // Initializing some variables 
        TransitionStart = currentMillis;  
        StopCMDLength = currentMillis;  
        TimeStopped = currentMillis;
        TurnSignal_Enable = false;
        StoppedLongTime = false;
        
        canChangeScheme = false;    
        ChangeSchemeTimerFlag = false;
        ChangeSchemeMode = false;
        MaxTurn = (int)((float)MaxRightTurn * 0.9);             // This sets a turn level that is near max, we use sequential back-and-forth max turns to enter Change-Scheme-Mode
        MinTurn = (int)((float)MaxRightTurn * 0.2);             // This is the minimum amount of steering wheel movement considered to be a command during Change-Scheme-Mode
        RightCount = 0;
        LeftCount = 0;

        Startup = false;                                        // This ensures the startup stuff only runs once

   }

    timer.run();

    receiveControllerInfo();

    if (controller.getController3() != -1) {
      updateServoPositions(limitToMaxPositionsServo(controller.getController3()));
    }  
        
    //DEBUG
    DumpControllerValues(false);
} 

void DumpControllerValues(bool overwrite) {
  if (PRINTDEBUG(overwrite)) {
    Serial.println(controller.printDebugInfo());
    loopCounter = 1;
  }
  loopCounter++;  
}

bool PRINTDEBUG(bool overwrite) {
   if (loopCounter % 8 == 0 || overwrite) {
    loopCounter = 1;
    return true;
  }
  loopCounter++;   
  return false;
}


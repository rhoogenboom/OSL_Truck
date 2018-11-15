//RHO TODO
#include "AA_UserConfig.h"
#include <OSL_SimpleTimer.h>
#include <PinChangeInterrupt.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//receiver items and model inputs
const byte MixSteeringChannel_Pin = 2;      // steering channel for mixing the rear axles - Mega:D18/pin46 - Nano: D2/
const byte PotMeter_Pin = A7;                // 5th wheel Mega: A1/pin96 - Nano: A7/
const byte NFR_CE  = 3;                      // NFR CE pin D3/pin7
const byte NFR_CSN = 4;                      // NFR CSN pin D4/pin1

// The number of light outputs available on the board
#define NumLights                    12                 

OSL_SimpleTimer timer;                 
unsigned int transmissionTimerID;
boolean TimeUp =  true;

boolean Startup =  true; 

volatile int pulse_time;  
volatile unsigned long receiver_pulse_start;                     
volatile int MixedTurnPulse;                            

unsigned long ServoTimeout     = 35000;                 // Value in microseconds (uS) - length of time to wait for a servo pulse. Measured on Eurgle/HK 3channel at ~20-22ms between pulses
                                                        // Up to version 2.03 of OSL code this value was 21,000 (21ms) and it worked fine. However with the release of Arduino IDE 1.6.5, 
                                                        // something has changed about the pulseIn function, or perhaps the way it is compiled. At 21ms, pulseIn would return 0 every other read.
                                                        // Increasing the timeout to 30ms seems to have fixed it. LM - 7/15/2015
        
// WIFI
RF24 radio(NFR_CE, NFR_CSN); // NFR CE, CSN connections
//NFR pin connections:
//Pin   Pin Name            Mapped Pin Name       Connected To  Device
//1     PG5 ( OC0B )        Digital pin 4 (PWM)   CSN           NFR
//7     PE5 ( OC3C/INT5 )   Digital pin 3 (PWM)   CE            NFR
//11    GND GND GND NFR
//20    PB1 ( SCK/PCINT1 )  Digital pin 52 (SCK)  SCK           NFR
//21    PB2 ( MOSI/PCINT2 ) Digital pin 51 (MOSI) MOSI          NFR
//22    PB3 ( MISO/PCINT3 ) Digital pin 50 (MISO) MISO          NFR


const byte address[6] = {0x66,0x68,0x7b,0x4a,0x63};   

//controller structure for NFR transmission
typedef struct
{
  uint8_t state;
} OSLLight;

typedef struct
{
  uint16_t controller3;
  OSLLight lights[12];
} OSLLightPacket;

volatile OSLLightPacket packet;

void setup()
{
  Serial.begin(BaudRate);  

  if (INTERUPT_IO) { 
    // Hook up interrupt handler functions for when data comes in
    attachInterrupt(digitalPinToInterrupt(MixSteeringChannel_Pin), calcMixSteeringChannel, CHANGE);
  } else {
      pinMode(MixSteeringChannel_Pin, INPUT_PULLUP);
  }
       
  pinMode(PotMeter_Pin, INPUT); 

  //setup default controller values
  packet.controller3 = 0;
  for (int i=0; i<NumLights; i++)
  {
      packet.lights[i].state = 0;                 
  }

  //WIFI
  radio.begin();
  radio.setPALevel(RF24_PA_HIGH); //RF24_PA_MIN = 0,RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX, RF24_PA_ERROR
  radio.setDataRate(RF24_2MBPS); //RF24_1MBPS = 0, RF24_2MBPS, RF24_250KBPS
  radio.openWritingPipe(address);
  radio.stopListening();
}


// ====================================================================================================================================================>
//  MAIN LOOP
// ====================================================================================================================================================>

void loop()
{
  //RUN ONCE
  if (Startup)
  {       
      transmissionTimerID = timer.setInterval(transmissionInterval, transmitControllerInfo);
      Startup = false;                                        
  }

  //ETERNAL LOOP

  timer.run();
      
  GetRxCommands();
} 


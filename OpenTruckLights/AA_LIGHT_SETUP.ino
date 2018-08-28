// ------------------------------------------------------------------------------------------------------------------------------------------------>
// THIS IS WHERE YOU DETERMINE THE SETTINGS FOR YOUR LIGHTS
// ------------------------------------------------------------------------------------------------------------------------------------------------>

    // SETUP INSTRUCTIONS 

    // EXPLANATION OF STATES
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
    // Each light can respond to 14 different conditions, or states. These are: 
    // - Channel 3 - Position 1    
    // - Channel 3 - Position 2
    // - Channel 3 - Position 3    (This is the middle position if using a 3-position switch)
    // - Channel 3 - Position 4
    // - Channel 3 - Position 5    (This is position "3" if using a 3-position switch, or position "2" if using a 2-position switch)
    // - Forward
    // - Reverse
    // - Stop
    // - Stop Delay                (stopped for LongStopTime_mS milliseconds, set by user on UserConfig tab)
    // - Brake
    // - Right Turn
    // - Left Turn
    // - Accelerating - this is a subset of the Forward state, and occurs during heavy acceleration (defined in UserConfig)
    // - Decelerating - this is a subset of the Forward state, and occurs during deceleration (defined in UserConfig)
    //
    // State Notes: 
    // - At the very least, you must plug in the Throttle channel. Steering and Channel 3 are optional
    // - If you do not plug in a steering channel, then obviously you will never encounter the Right Turn & Left Turn States
    // - If you do not plug in a third channel, the program will only use the values specified in Position 1 as the default. In other words, it acts
    //   as if you have a one-position switch that is always in Position 1
    // - If your third channel is only a 2-position switch, the program will switch between Position 1 and Position 5
    // - If your third channel is a 3-position switch, you will be able to switch between Positions 1, 3, and 5
    // - If your third channel is a 5-position switch, you will be able to select all five Positions
    // - For the third channel to operate correctly, and in fact, for all channels to operate correctly, you need to run through the Radio Setup once.

    // EXPLANATION OF SETTINGS
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
    // For each light, you need to specify what will happen in each state - this is called the setting. The possible settings are: 
    // - ON
    // - OFF
    // - FADEOFF        * Fades off slowly
    // - NA
    // - BLINK            Will blink at the rate set in UserConfig.h (BlinkInterval)
    // - SOFTBLINK        Will blink at the rate set in UserConfig.h (BlinkInterval), but the light will fade-in and fade-out. The timing of the fade can be adjusted, see UserConfig 
    // - FASTBLINK        Will blink at the fast rate set in UserConfig.h
    // - DIM            * Will dim to the level set in UserConfig.h
    // - XENON          * Turns a light on but with a special effect that looks like a xenon bulb turning on
    // - BACKFIRE         Special effect that blinks a light randomly for a short period of time (only works under Decelerating state - use for tailpipe LEDs)
    //
    // Settings Notes: 
    // - For the positions determined by Channel 3, it is best to specify an explicit setting, in other words, you probably don't want any of them to be NA - 
    //   they should be ON, OFF, whatever. 
    // - The opposite is true for the other states - if you don't want anything to happen during the Forward state for example, set it to NA, not OFF. 
    //   Otherwise, your light will turn off when going forward. 
    // * Some settings require a special feature known as PWM. These are marked above with an asterisks (*). Not all of the lights on the board are capable of implementing PWM, 
    //   only the first 6 sockets. If you look at the underside of the physical board, these lights are marked with an asterisks (*). If you want to use these special settings,
    //   they must be on lights 1-6. Otherwise if you specify one of these settings on lights 7 or 8, the program will simply turn them OFF isntead. 
   
    // EXPLANATION OF SCHEMES
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
    // A Scheme defines every setting for every light in every state. You may have as many Schemes as code space allows. Two are provided below to start, 
    // if you want more, add them below, but remember to also update the NumSchemes variable in AA_UserConfig.h.     
   
    // HOW TO SETUP YOUR LIGHTS
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
    // Below you will see the lighting schemes. Each Scheme has a single row for each of the eight lights. The columns represent the states. The values
    // in the individual tables represent the settings for that light at that state. 
    // 
    // OK, YOU'RE READY. TRY NOT TO MESS UP THE LAYOUT. JUST CHANGE THE SETTINGS. REMEMBER THE POSSIBLE VALUES: 
    // ON, OFF, FADEOFF, BLINK, FASTBLINK, DIM, XENON, FADEOFF, NA

    const PROGMEM uint16_t Schemes[NumSchemes][NumLights][NumStates] =
    {
        {
        //     SCHEME ONE - EXAMPLE PROVIDED
        //     Pos 1      Pos 2      Pos 3      Pos 4       Pos 5     Forward     Reverse     Stop      StopDelay   Brake     Right Turn     Left Turn      Accelerating   Decelerating
        // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//old:
//{0,OFF,OFF,ON,OFF,ON,NA,NA,NA,ON},  // light 1 - Brake
//{0,NA,BLINK,OFF,BLINK,NA,BLINK,NA,NA,NA}, // light 2 - Right Turn
//{0,NA,BLINK,OFF,BLINK,NA,NA,BLINK,NA,NA}, // light 3 - Left Turn
//{0,OFF,ON,OFF,OFF,NA,NA,NA,NA,NA},  // light 4 - Reverse
//{128,NA,NA,NA,NA,NA,NA,NA,NA,NA}, // light 5 - Headlight 1
//{64,NA,NA,NA,NA,NA,NA,NA,NA,NA},  // light 6 - Headlight 2
//{32,NA,NA,NA,NA,NA,NA,NA,NA,NA},  // light 7 - Low Fog Front
//{16,NA,NA,NA,NA,NA,NA,NA,NA,NA},  // light 8 - High Fog Front
//{8,NA,NA,NA,NA,NA,NA,NA,NA,NA}, // light 9  - Rear Fog
//{4,NA,NA,NA,NA,NA,NA,NA,NA,NA}, // light 10  - Emergency
//{2,NA,NA,NA,NA,NA,NA,NA,NA,NA}, // light 11 - Emergency Trailer
//{1,NA,NA,NA,NA,NA,NA,NA,NA,NA}, // light 12 - Extra lights 1

//multifuction:
{0,NA,OFF,OFF,ON,OFF,ON,NA,NA,NA,ON},  // light 1 - Brake  // light 1  9
{0,NA,NA,BLINK,OFF,BLINK,NA,BLINK,NA,NA,NA},  // light 2 - Right Turn // light 2  10
{0,NA,NA,BLINK,OFF,BLINK,NA,NA,BLINK,NA,NA},  // light 3 - Left Turn  // light 3  11
{0,NA,OFF,ON,OFF,OFF,NA,NA,NA,NA,NA}, // light 4 - Reverse  // light 4  6
{128,XENON,NA,NA,NA,NA,NA,NA,NA,NA,NA}, // light 5 - Headlight 1  // light 5  7
{64,XENON,NA,NA,NA,NA,NA,NA,NA,NA,NA},  // light 6 - Headlight 2  // light 6  8
{32,ON,NA,NA,NA,NA,NA,NA,NA,NA,NA}, // light 7 - Low Fog Front  // light 7  12
{16,ON,NA,NA,NA,NA,NA,NA,NA,NA,NA}, // light 8 - High Fog Front // light 8  13
{8,ON,NA,NA,NA,NA,NA,NA,NA,NA,NA},  // light 9  - Rear Fog  // light 9  46
{4,FASTBLINK,NA,NA,NA,NA,NA,NA,NA,NA,NA}, // light 10  - Emergency  // light 10   5
{2,ON,NA,NA,NA,NA,NA,NA,NA,NA,NA},  // light 11 - Emergency Trailer // light 11 17
{1,ON,NA,NA,NA,NA,NA,NA,NA,NA,NA} // light 12 - Extra lights 1  // light 12 45


}
    };





void GetRxCommands()
{
    static int LastThrottleCommand;
    
    // We always check the Throttle channel
    ThrottleCommand = GetThrottleCommand();

    while(Failsafe)
    {
        if (DEBUG) {
          Serial.println(F("RX Disconnected!"));
          DumpControllerValues();
        }
        ToggleAllLights();                                       // If the receiver isn't connected, pause the program and flash all the lights
        delay(150);
        GetThrottleCommand();
    }    

    // But to save time, we only check the Steering and Channel 3 channels 
    // if they were detected at startup. Otherwise we ignore them. 
    if (SteeringChannelPresent) { 
      TurnCommand = GetTurnCommand(); 
    } else { 
      TurnCommand = 0; // We set Turn to nothing if not being used                
    }    

    GetMixedSteeringInput(); 
}


int GetThrottleCommand()
{
    int ThrottleCommand;

    if (INTERUPT_IO) {
      ThrottlePulse = pulse_time[RC_Throttle_Index];
    } else {
      ThrottlePulse = pulseIn(ThrottleChannel_Pin, HIGH, ServoTimeout);  
    }
    //controller.controller1 = ThrottlePulse;
    
    if ((ThrottlePulse == 0) || (ThrottlePulse > PulseMax_Bad) || (ThrottlePulse < PulseMin_Bad))
    {   // Timed out waiting for a signal, or measured a bad signal
        // Set Failsafe flag, set Throttle to 0
        //controller.controller1 = 0;
        Failsafe = true;
        ThrottleCommand = 0;
    }
    else
    {
        Failsafe = false;
        if (ThrottlePulse >= ThrottlePulseCenter + ThrottleDeadband) 
        { 
            if (ThrottleChannelReverse == false)
            {
                // Without channel reversing, we consider PPM pulse values greater than Center to be forward throttle commands. We now map the radio command to our forward throttle range
                ThrottleCommand = map(ThrottlePulse, (ThrottlePulseCenter + ThrottleDeadband), ThrottlePulseMax, ThrottleCenterAdjust, MaxFwdSpeed);
            }
            else
            {
                // With channel reversing, we consider PPM pulse values greater than Center to be reverse throttle commands. We now map the radio command to our reverse throttle range
                ThrottleCommand = map(ThrottlePulse, (ThrottlePulseCenter + ThrottleDeadband), ThrottlePulseMax, -ThrottleCenterAdjust, MaxRevSpeed);
            }
        }
        else if (ThrottlePulse <= (ThrottlePulseCenter - ThrottleDeadband))
        { 
            if (ThrottleChannelReverse == false)
            {
                // Without channel reversing, we consider PPM pulse values less than Center to be reverse throttle commands. We now map the radio command to our reverse throttle range
                ThrottleCommand = map(ThrottlePulse, ThrottlePulseMin, (ThrottlePulseCenter - ThrottleDeadband), MaxRevSpeed, -ThrottleCenterAdjust);
            }
            else
            {
                // With channel reversing, we consider PPM pulse values less than Center to be forward throttle commands. We now map the radio command to our forward throttle range
                ThrottleCommand = map(ThrottlePulse, ThrottlePulseMin, (ThrottlePulseCenter - ThrottleDeadband), MaxFwdSpeed, ThrottleCenterAdjust);
            }
        }
        else
        {   // In this case we are within the ThrottleDeadband setting, so Command is actually Zero (0)
            ThrottleCommand = 0;
        }
        return constrain(ThrottleCommand, MaxRevSpeed, MaxFwdSpeed);
        // After all this, ThrottleCommand is now some value from -255 to +255 where negative equals REV and positive equals FWD. (The values can actually be less if top forward or 
        // reverse speeds have been limited by user)
    }    
}


int GetMixedSteeringInput()
{
    //read the input from the RC receiver overruling the 5th wheel position
    if (INTERUPT_IO) {
      MixedTurnPulse = pulse_time[RC_RearAxles_Index];
    } else {
      MixedTurnPulse = pulseIn(MixSteeringChannel_Pin, HIGH, ServoTimeout);
    }

    //update the controller object for sending the data over
    packet.controller3 = CalculateRearAxlePosition(MixedTurnPulse);  
}  


int GetTurnCommand()
{
    int TurnCommand;
    if (INTERUPT_IO) {
      TurnPulse = pulse_time[RC_Steering_Index];
    } else {
      TurnPulse = pulseIn(SteeringChannel_Pin, HIGH, ServoTimeout);
    }
    //controller.controller2 = TurnPulse;
    if ((TurnPulse == 0) || (TurnPulse > PulseMax_Bad) || (TurnPulse < PulseMin_Bad))
    {   // In this case, there was no signal found on the turn channel
        TurnCommand = 0;    // If no TurnPulse, we set Turn to 0 (no turn)
    }
    else
    {
        if (TurnPulse >= TurnPulseCenter + TurnDeadband) 
        { 
            if (TurnChannelReverse == false)
            {
                // Without channel reversing, we consider PPM pulse values greater than Center to be Right Turn commands. We now map the radio command to our Right Turn range
                TurnCommand = map(TurnPulse, (TurnPulseCenter + TurnDeadband), TurnPulseMax, TurnCenterAdjust, MaxRightTurn);
            }
            else
            {
                // With channel reversing, we consider PPM pulse values greater than Center to be Left Turn commands. We now map the radio command to our Left Turn range
                TurnCommand = map(TurnPulse, (TurnPulseCenter + TurnDeadband), TurnPulseMax, -TurnCenterAdjust, MaxLeftTurn);
            }
        }
        else if (TurnPulse <= (TurnPulseCenter - TurnDeadband))
        { 
            if (TurnChannelReverse == false)
            {
                // Without channel reversing, we consider PPM pulse values less than Center to be Left Turn commands. We now map the radio command to our Left Turn range
                TurnCommand = map(TurnPulse, TurnPulseMin, (TurnPulseCenter - TurnDeadband), MaxLeftTurn, -TurnCenterAdjust);
            }
            else
            {
                // With channel reversing, we consider PPM pulse values less than Center to be Right Turn commands. We now map the radio command to our Right Turn range
                TurnCommand = map(TurnPulse, TurnPulseMin, (TurnPulseCenter - TurnDeadband), MaxRightTurn, TurnCenterAdjust);
            }
        }
        else
        {   // In this case we are within the TurnDeadband setting, so Command is actually Zero (0)
            TurnCommand = 0;
        }
        return constrain(TurnCommand, MaxLeftTurn, MaxRightTurn);
        // After all this, TurnCommand is now some value from -100 to +100 where negative equals LEFT and positive equals RIGHT.
    }
}

void calcMultiPropChannel()
{
  if (digitalRead(MultiChannel_Pin) == HIGH) {
    multiPropStartTime = micros();
  } else {
    MultiPropItemTime = (uint16_t)(micros() - multiPropStartTime);
    current_multiprop_channel++;
    
    if (MultiPropItemTime < 1000) {
      current_multiprop_channel = 0;
    }

    if (MultiPropItemTime > 2000) {
      current_multiprop_channel = 0;
    }
    
    if (current_multiprop_channel < 10) {
      multi_prop[current_multiprop_channel] = MultiPropItemTime;
    }
  }
}

void calcChannel(int channel, int pinNumber) 
{
    //if the pin has gone HIGH, record the microseconds since the Arduino started up 
    if(digitalRead(pinNumber) == HIGH) 
    { 
        receiver_pulse_start[channel] = micros();
    } 
    //otherwise, the pin has gone LOW 
    else
    { 
        //only worry about this if the timer has actually started
        if(receiver_pulse_start[channel] != 0)
        { 
            //record the pulse time
            pulse_time[channel] = ((volatile int)micros() - receiver_pulse_start[channel]);
            //restart the timer
            receiver_pulse_start[channel] = 0;
        }
    } 
} 

void calcChannel1() { calcChannel(RC_Throttle_Index, ThrottleChannel_Pin); }
void calcChannel2() { calcChannel(RC_Steering_Index, SteeringChannel_Pin); }
void calcChannel3() { calcChannel(RC_RearAxles_Index, MixSteeringChannel_Pin); }




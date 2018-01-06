# Open Source RC Lights Truck

This library is a copy of the OSL_Original project by LukeZ. OSL is an Arduino-based, open source project for controlling lights in RC cars and trucks. It uses an ATmega 328 as the processors and has 8 light outputs

OSL_Truck library was created to provide more switching functions for utility vehicles. The OSL_Original library didn't provide enough swithing functions to cope with all the lights and servos.  

Differences between OSL_Original and OSL_Truck:
  * Reads and decodes multiprop sub-channels from channel 3
  * Implement normal driving lights for trucks (Headlight 1, headlight 2, braking, reverse, indicator left and right) and combine 8 additional light/servo switches. (10 would be the maximun number of outputs limited by the number of pins available)

[You can find the OSL_Original github project here](https://github.com/OSRCL/OSL_Original)

**--> [COMPLETE OSL DETAILS AND INSTRUCTIONS AVAILABLE HERE](http://www.rcgroups.com/forums/showthread.php?t=1539753) <--**

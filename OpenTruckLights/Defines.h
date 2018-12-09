// EEPROM defines
#define eeprom_read_to(dst_p, eeprom_field, dst_size) eeprom_read_block(dst_p, (void *)offsetof(__eeprom_data, eeprom_field), MIN(dst_size, sizeof((__eeprom_data*)0)->eeprom_field))
#define eeprom_read(dst, eeprom_field) eeprom_read_to(&dst, eeprom_field, sizeof(dst))
#define eeprom_write_from(src_p, eeprom_field, src_size) eeprom_write_block(src_p, (void *)offsetof(__eeprom_data, eeprom_field), MIN(src_size, sizeof((__eeprom_data*)0)->eeprom_field))
#define eeprom_write(src, eeprom_field) { typeof(src) x = src; eeprom_write_from(&x, eeprom_field, sizeof(x)); }

// Useful functions
#define MIN(x,y) ( x > y ? y : x )
#define MAX(x,y) ( x > y ? x : y )

#define MIN_CHANNEL 875 //max left of RC controller
#define MAX_CHANNEL 2125 //max right of RC controller
#define CHANNEL_DEADCENTER 10

#define SERVO_MIN_POSITION 1000
#define SERVO_MAX_POSITION 1500  //TODO should this be 1500? not 2000???
#define SERVO_MIDDLE_POSITION 1500

#define MIN_VALUE_MEASURED_FOR_POT 0
#define MAX_VALUE_MEASURED_FOR_POT 1024
#define CHANNEL_CENTER 512 //middle of pot meter

//setup pins and lights
#define SETUP_BUTTON A0 //nano: 20
#define MAGNET_POT 8 
#define SERVO_POT 5
#define MAGNET_LEFT_LED_RED 9
#define MAGNET_RIGHT_LED_GREEN 10

//receiver items and model inputs
#define MixSteeringChannel_Pin 2 // steering channel for mixing the rear axles - Mega:D18/pin46 - Nano: D2/
#define PotMeter_Pin A7 // 5th wheel Mega: A1/pin96 - Nano: A7/

#define POWER_5V 2 //TODO figure out other pin used by lights

//light unit

// The number of light outputs available on the board
#define NumLights                    12                 

//receiver 
#define ServoTimeout 35000 // Value in microseconds (uS) - length of time to wait for a servo pulse.


#ifndef _COMMON_DEF_H_ 
#define _COMMON_DEF_H_ 

/**********************************************************
 *  INCLUDES
 *********************************************************/
#include <stdio.h>


/**********************************************************
 *  TYPES 
 *********************************************************/

// list of commands to be send
enum command {NO_CMD=0, SET_HEAT_CMD=1, READ_SUN_CMD=2, 
	      READ_TEMP_CMD=3, READ_POS_CMD=4};

// structure of command message
struct command_msg {
    short int cmd;  	      // command to execute
    short int set_heater;     // c
};

// structure of response message
struct response_msg {
    short int cmd;  	     // command to respond to
    short int error;         // boolean to state if execution went well
    long int sunlight_on;   // boolean to state if sunlight is on
    float temperature;       // value of the temperature
    float orbit_position[3]; // value of the position
};

#endif /* _COMMON_DEF_H_ */


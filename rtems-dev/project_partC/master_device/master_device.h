#ifndef _MASTER_DEVICE_H_ 
#define _MASTER_DEVICE_H_ 

/**********************************************************
 *  INCLUDES
 *********************************************************/
#include <stdio.h>
#include "common_def.h"


/**********************************************************
 *  CONSTANTS
 *********************************************************/

// Represents the maximum temperature for the satellite
static const double MAX_TEMPERATURE = 90.0;

// Represents the minimum temperature for the satellite
static const double MIN_TEMPERATURE = -10.0;

// Represents the average temperature for the satellite
static const double AVG_TEMPERATURE = 40.0;


/**********************************************************
 *  TYPES 
 *********************************************************/

// structure of master state
struct master_status {
    int heater_on;            // boolean to state if heater is on
    int sunlight_on;          // boolean to state if sunlight is on
    double temperature;       // value of the temperature
    double orbit_position[3]; // value of the position
};


/**********************************************************
 *  GLOBAL VARIABLES
 **********************************************************/

// next command message to be send
extern struct command_msg ready_send_command;

// last response message received  
extern struct response_msg last_recv_response;

// set of variables that comprises the master status
extern struct master_status master_status;

//---------------------------------------------------------------------------
//                           MAIN FUNCTIONS
//---------------------------------------------------------------------------

/**********************************************************
 *  Function: init_master_state
 *********************************************************/
void init_master_state ();

/**********************************************************
 *  Function: control_temperature
 *********************************************************/
void control_temperature ();


//------------ FUNCTIONS TO CREATE COMMANDS --------------

/**********************************************************
 *  Function: create_command_no_cmd
 *********************************************************/
void create_command_no_cmd ();

/**********************************************************
 *  Function: create_set_heat_cmd
 *********************************************************/
void create_command_set_heat_cmd ();

/**********************************************************
 *  Function: create_read_sun_cmd
 *********************************************************/
void create_command_read_sun_cmd ();

/**********************************************************
 *  Function: create_read_temp_cmd
 *********************************************************/
void create_command_read_temp_cmd ();

/**********************************************************
 *  Function: create_read_pos_cmd
 *********************************************************/
void create_command_read_pos_cmd ();


//------------ FUNCTIONS TO PROCESS RESPONSES -------------

/**********************************************************
 *  Function: process_response_no_cmd
 *********************************************************/
void process_response_no_cmd ();

/**********************************************************
 *  Function: process_response_set_heat_cmd
 *********************************************************/
void process_response_set_heat_cmd ();

/**********************************************************
 *  Function: process_response_read_sun_cmd
 *********************************************************/
void process_response_read_sun_cmd ();

/**********************************************************
 *  Function: process_response_read_temp_cmd
 *********************************************************/
void process_response_read_temp_cmd ();

/**********************************************************
 *  Function: process_response_read_pos_cmd
 *********************************************************/
void process_response_read_pos_cmd ();

#endif /* _MASTER_DEVICE_H_ */


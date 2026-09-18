/**********************************************************
 *  INCLUDES
 *********************************************************/

#include <stdio.h>
#include <time.h>

#include "master_device.h"

/**********************************************************
 *  GLOBAL VARIABLES
 **********************************************************/

// next command message to be send
struct command_msg ready_send_command = {NO_CMD, 0};

// last response message received  
struct response_msg last_recv_response = {NO_CMD, 0, 0, 0.0, 0.0};

// set of variables that comprises the master status
struct master_status master_status;

//---------------------------------------------------------------------------
//                           MAIN FUNCTIONS
//---------------------------------------------------------------------------

/**********************************************************
 *  Function: init_master_state
 *********************************************************/
void init_master_state ()
{
    master_status.heater_on = 0;          
    master_status.sunlight_on = 0;    
    master_status.temperature = 0.0; 
    master_status.orbit_position[0] = 3000.0;     
    master_status.orbit_position[1] = 0.0;     
    master_status.orbit_position[2] = 12000.0; 
}

/**********************************************************
 *  Function: control_temperature
 *********************************************************/
void control_temperature () 
{
    // check if temperature is lower or higher
    if (master_status.temperature < AVG_TEMPERATURE) {
	// set heater
	master_status.heater_on = 1;
    } else if (master_status.temperature >= AVG_TEMPERATURE) {
	// unset heater
	master_status.heater_on = 0;
    }
}


//------------ FUNCTIONS TO CREATE COMMANDS --------------

/**********************************************************
 *  Function: create_command_no_cmd
 *********************************************************/
void create_command_no_cmd ()
{
}


/**********************************************************
 *  Function: create_set_heat_cmd
 *********************************************************/
void create_command_set_heat_cmd ()
{
}

/**********************************************************
 *  Function: create_read_sun_cmd
 *********************************************************/
void create_command_read_sun_cmd ()
{
}

/**********************************************************
 *  Function: create_read_temp_cmd
 *********************************************************/
void create_command_read_temp_cmd ()
{
}

/**********************************************************
 *  Function: create_read_pos_cmd
 *********************************************************/
void create_command_read_pos_cmd ()
{
}


//------------ FUNCTIONS TO PROCESS RESPONSES -------------

/**********************************************************
 *  Function: process_response_no_cmd
 *********************************************************/
void process_response_no_cmd ()
{
}
 
/**********************************************************
 *  Function: process_response_set_heat_cmd
 *********************************************************/
void process_response_set_heat_cmd ()
{
}

/**********************************************************
 *  Function: process_response_read_sun_cmd
 *********************************************************/
void process_response_read_sun_cmd ()
{
}

/**********************************************************
 *  Function: process_response_read_temp_cmd
 *********************************************************/
void process_response_read_temp_cmd ()
{
}
/**********************************************************
 *  Function: process_response_read_pos_cmd
 *********************************************************/
void process_response_read_pos_cmd ()
{
}

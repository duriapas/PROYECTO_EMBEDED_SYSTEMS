/**********************************************************
 *  INCLUDES
 *********************************************************/

#include <stdio.h>
#include <time.h>

#include "slave_device.h"


/**********************************************************
 *  GLOBAL VARIABLES
 **********************************************************/

// last command message received 
struct command_msg last_recv_command = {NO_CMD, 0};

// next response message to be send
struct response_msg ready_send_response = {NO_CMD, 0, 0, 0.0, 0.0};

// set of variables that comprises the slave status
struct slave_status slave_status;


//---------------------------------------------------------------------------
//                           AUXILIAR FUNCTIONS 
//---------------------------------------------------------------------------

/**********************************************************
 *  Function: getClock
 *********************************************************/
double getClock()
{
    struct timespec tp;
    double reloj;

    clock_gettime (CLOCK_REALTIME, &tp);
    reloj = ((double)tp.tv_sec) +
	    ((double)tp.tv_nsec) / ((double)1000000000);

    return (reloj);
}

//---------------------------------------------------------------------------
//                           MAIN FUNCTIONS
//---------------------------------------------------------------------------

/**********************************************************
 *  Function: init_slave_state
 *********************************************************/
void init_slave_state ()
{
    slave_status.heater_on = 0;          
    slave_status.sunlight_on = 0;    
    slave_status.temperature = 0.0; 
    slave_status.time_temperature = getClock();
    slave_status.orbit_position[0] = 3000.0;     
    slave_status.orbit_position[1] = 0.0;     
    slave_status.orbit_position[2] = 12000.0; 
    slave_status.orbit_init_time = getClock();
}


/**********************************************************
 *  Function: get_temperature
 *********************************************************/
void get_temperature ()
{
}

/**********************************************************
 *  Function: get_position
 *********************************************************/

void get_position ()
{
}

/**********************************************************
 *  Function: execute_command_and_create_response
 *********************************************************/

void execute_command_and_create_response ()
{
}        




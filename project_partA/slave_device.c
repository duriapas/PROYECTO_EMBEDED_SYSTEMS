/**********************************************************
 *  INCLUDES
 *********************************************************/

#include <stdio.h>
#include <time.h>
#include <math.h>    // So that fmod() and floor() can be used

#include "slave_device.h"


/**********************************************************
 *  GLOBAL VARIABLES
 **********************************************************/

//ESTE COMENTARIO ES UN TEST
//woowowowoowowo

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
    double elapsed_time = getClock() - slave_status.time_temperature;

    slave_status.time_temperature = getClock();

    double power = HEAT_POWER_LOSS;

    if (slave_status.heater_on){
        power = power + HEATER_POWER;
    }

    if (slave_status.sunlight_on){
        power = power + SUNLIGHT_POWER;
    }

    double energy = power * elapsed_time;

    slave_status.temperature = (energy / (SHIP_SPECIFIC_HEAT * SHIP_MASS)) + slave_status.temperature;
}

/**********************************************************
 *  Function: get_position
 ***************)******************************************/

void get_position ()
{
    double relative_time = getClock() - slave_status.orbit_init_time;
    relative_time = fmod(relative_time,ORBIT_ROUND_TIME); 

    int orbit_first_index = floor(relative_time * (ORBIT_POINTS_ARRAY_SIZE /ORBIT_ROUND_TIME));

    int orbit_second_index = (orbit_first_index+1)%ORBIT_POINTS_ARRAY_SIZE;

    double orbit_offset_ratio = (relative_time / (ORBIT_ROUND_TIME /ORBIT_POINTS_ARRAY_SIZE)) - orbit_first_index;

    slave_status.orbit_position[0] =
    ORBIT_POINTS_ARRAY[orbit_first_index][0] * (1.0 - orbit_offset_ratio)
    + ORBIT_POINTS_ARRAY[orbit_second_index][0] * orbit_offset_ratio;

    slave_status.orbit_position[1] =
    ORBIT_POINTS_ARRAY[orbit_first_index][1] * (1.0 - orbit_offset_ratio)
    + ORBIT_POINTS_ARRAY[orbit_second_index][1] * orbit_offset_ratio;

    slave_status.orbit_position[2] =
    ORBIT_POINTS_ARRAY[orbit_first_index][2] * (1.0 - orbit_offset_ratio)
    + ORBIT_POINTS_ARRAY[orbit_second_index][2] * orbit_offset_ratio;
}

/**********************************************************
 *  Function: execute_command_and_create_response
 *********************************************************/

void execute_command_and_create_response ()
{
    // check no cmd msg and process it
    if (last_recv_command.cmd == NO_CMD) {
        //set cmd and no error for response message
        ready_send_response.cmd = NO_CMD;
        ready_send_response.error = 0;

    // check set heater msg and process it
    } else if (last_recv_command.cmd == SET_HEAT_CMD) {
        //set cmd and no error for response message
        ready_send_response.cmd = SET_HEAT_CMD;
        ready_send_response.error = 0;
        //update heater value on slave status
        slave_status.heater_on = last_recv_command.set_heater;

    // check read sunlight msg and process it
    } else if (last_recv_command.cmd == READ_SUN_CMD) {
        //set cmd and no error for response message
        ready_send_response.cmd = READ_SUN_CMD;
        ready_send_response.error = 0;
        //set sunlight sensor value on response message
        ready_send_response.sunlight_on = slave_status.sunlight_on;

    // check read temperature msg and process it
    } else if (last_recv_command.cmd == READ_TEMP_CMD) {
        //set cmd and no error for response message
        ready_send_response.cmd = READ_TEMP_CMD;
        ready_send_response.error = 0;
        //set temperature value on response message
        ready_send_response.temperature = slave_status.temperature;

    // check read position msg and process it
    } else if (last_recv_command.cmd == READ_POS_CMD) {
        //set cmd and no error for response message
        ready_send_response.cmd = READ_POS_CMD;
        ready_send_response.error = 0;
        //set orbit position value on response message
        ready_send_response.orbit_position[0] = slave_status.orbit_position[0];
        ready_send_response.orbit_position[1] = slave_status.orbit_position[1];
        ready_send_response.orbit_position[2] = slave_status.orbit_position[2];
    }   
}        



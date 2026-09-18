#ifndef _SLAVE_DEVICE_H_ 
#define _slave_DEVICE_H_ 

/**********************************************************
 *  INCLUDES
 *********************************************************/
#include <stdio.h>
#include "common_def.h"


/**********************************************************
 *  CONSTANTS
 *********************************************************/

// Represents the specific heat of the satellite
static const double SHIP_SPECIFIC_HEAT = 0.9;

// Represents the mass of the satellite
static const double SHIP_MASS = 10.0; // Kg

// Represents the energy the heater can emit every second
static const double HEATER_POWER = 150.0; // J/sec

// Represents the energy the sun light can emit towards the satellite every second
static const double SUNLIGHT_POWER = 50.0; // J/sec

// Represents the energy the satellite losses every second
static const double HEAT_POWER_LOSS = -100.0; // J/sec

// Represents the number of orbit positions stored in the array
static const int ORBIT_POINTS_ARRAY_SIZE = 20;

// Represents the time to travel a complete orbit
static const double ORBIT_ROUND_TIME = 300.0; //sec

// position data for the orbit
static const double ORBIT_POINTS_ARRAY[20][3]= {
    {3000.00, 0.0, 12000.0},
    {2853.169548885460, 1854.101966249680, 11412.678195541800},
    {2427.050983124840, 3526.711513754840, 9708.203932499370},
    {1763.355756877420, 4854.101966249680, 7053.423027509680},
    {927.050983124842, 5706.339097770920, 3708.203932499370},
    {0.0, 6000.0, 0.0},
    {-927.050983124842, 5706.339097770920, -3708.203932499370},
    {-1763.355756877420, 4854.101966249680, -7053.423027509680},
    {-2427.050983124840, 3526.711513754840, -9708.203932499370},
    {-2853.169548885460, 1854.101966249680, -11412.678195541800},
    {-3000.0, 0.0, -12000.0},
    {-2853.169548885460, -1854.101966249690, -11412.678195541800},
    {-2427.050983124840, -3526.711513754840, -9708.203932499370},
    {-1763.355756877420, -4854.101966249680, -7053.423027509680},
    {-927.050983124843, -5706.339097770920, -3708.203932499370},
    {0.0, -6000.0, 0.0},
    {927.050983124842, -5706.339097770920, 3708.203932499370},
    {1763.355756877420, -4854.101966249690, 7053.423027509680},
    {2427.050983124840, -3526.711513754840, 9708.203932499370},
    {2853.169548885460, -1854.101966249690, 11412.678195541800}};

/**********************************************************
 *  TYPES 
 *********************************************************/

// structure of slave state
struct slave_status {
    int heater_on;            // boolean to state if heater is on
    int sunlight_on;          // boolean to state if sunlight is on
    double temperature;       // value of the temperature
    double time_temperature;  // value of the last time temperature was computed
    double orbit_position[3]; // value of the position
    double orbit_init_time;   // value of the time instant the first orbit started
};


/**********************************************************
 * GLOBAL VARIABLES
 **********************************************************/

// last command message received  
extern struct command_msg last_recv_command;

// next response message to be send
extern struct response_msg ready_send_response;

// set of variables that comprises the slave status
extern struct slave_status slave_status;

//---------------------------------------------------------------------------
//                           AUXILIAR FUNCTIONS 
//---------------------------------------------------------------------------

/**********************************************************
 *  Function: getClock
 *********************************************************/
double getClock();

//---------------------------------------------------------------------------
//                           MAIN FUNCTIONS
//---------------------------------------------------------------------------

/**********************************************************
 *  Function: init_slave_state
 *********************************************************/
void init_slave_state ();

/**********************************************************
 *  Function: get_temperature
 *********************************************************/
void get_temperature ();

/**********************************************************
 *  Function: get_position
 *********************************************************/
void get_position ();

/**********************************************************
 *  Function: execute_command_and_create_response
 *********************************************************/
void execute_command_and_create_response ();

#endif /* _SLAVE_DEVICE_H_ */

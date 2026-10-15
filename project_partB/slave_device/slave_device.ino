/**********************************************************
 *  INCLUDES
 *********************************************************/
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

// Hardware connections on the slave Arduino UNO.
static const int SUN_SENSOR_PIN = A3;
static const int HEATER_PIN = 13;
static const int SUNLIGHT_THRESHOLD = 500;

// The cyclic executive uses a 50 ms minor cycle and a 200 ms major cycle.
static const unsigned long SCHEDULER_SLOT_MS = 50UL;
static unsigned long next_slot_release = 0;
static unsigned char scheduler_slot = 0;

/**********************************************************
 *  TYPES
 *********************************************************/

// list of commands to be send
enum command {NO_CMD=0, SET_HEAT_CMD=1, READ_SUN_CMD=2,
          READ_TEMP_CMD=3, READ_POS_CMD=4};

// structure of command message
struct command_msg {
    short int cmd;            // command to execute
    short int set_heater;     // c
};

// structure of response message
struct response_msg {
    short int cmd;           // command to respond to
    short int error;         // boolean to state if execution went well
    long  int sunlight_on;   // boolean to state if sunlight is on
    float temperature;       // value of the temperature
    float orbit_position[3]; // value of the position
};

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
    return ((double) millis()) / 1000.0;
}

// --------------------------------------
// Function: delayClock
// --------------------------------------
void delayClock(double time)
{
    delay(time * 1000.0);
}

// --------------------------------------
// Function: comm_server
// --------------------------------------
int comm_server()
{
   static bool command_in_process = false;
   static struct command_msg aux_command = {NO_CMD, 0};
   static unsigned char cmd_parity = (unsigned char) 0;
   static int count = 0;
   bool new_command = false;

   unsigned char car_aux;

   // If there were a received msg, send the processed answer or ERROR if none.
   // then reset for the next request.
   // NOTE: this requires that between two calls of com_server all possible
   //       answers have been processed.
   if (command_in_process) {
      // compute parity
      unsigned char res_parity = (unsigned char) 0;
      for (int i=0; i<sizeof(struct response_msg); i++) {
         // compute parity (xor)
         res_parity = res_parity ^ ((unsigned char *)(&ready_send_response))[i];
      }
      Serial.write((unsigned char *)(&ready_send_response), sizeof(struct response_msg));
      Serial.write((unsigned char *)(&res_parity), 1);
      // reset flags and buffers
      command_in_process = false;
      memset((unsigned char *)(&aux_command), 0, sizeof(struct command_msg));
      memset((unsigned char *)(&last_recv_command), 0, sizeof(struct command_msg));
      memset((unsigned char *)(&ready_send_response), 0, sizeof(struct response_msg));
   }

   while (Serial.available()) {
      // read one character
      car_aux =Serial.read();
      if ((count == 0) && (car_aux > 4)) {
         continue;
      }
      // Check if it is the last byte of the msg or not.
      if (count == sizeof(struct command_msg)) {
         // check parity error
         if (cmd_parity != car_aux) {
            // set error answer
            last_recv_command.cmd = NO_CMD;
            last_recv_command.set_heater = 0;
            command_in_process = true;
            ready_send_response.cmd = NO_CMD;
            ready_send_response.error = 2;
            cmd_parity = (unsigned char) 0;
            count = 0;
            // end loop
            break;
         }
         // finish reading msg
         cmd_parity = (unsigned char) 0;
         count = 0;
         memcpy((unsigned char *)(&last_recv_command), (unsigned char *)(&aux_command), sizeof(struct command_msg));
         memset((unsigned char *)(&ready_send_response), 0, sizeof(struct response_msg));
         command_in_process = true;
         new_command = true;
         
         break;
      } else {
         //Store the character
         ((unsigned char *) (&aux_command))[count] = car_aux;
         // compute parity (xor)
         cmd_parity = cmd_parity ^ car_aux;
      }

      // Increment the count
      count++;
   }

   return new_command ? 1 : 0;
}

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
 *********************************************************/

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
    ready_send_response.cmd = last_recv_command.cmd;
    ready_send_response.error = 1; 
    ready_send_response.sunlight_on = 0;
    ready_send_response.temperature = 0.0f;
    ready_send_response.orbit_position[0] = 0.0f;
    ready_send_response.orbit_position[1] = 0.0f;
    ready_send_response.orbit_position[2] = 0.0f;

    switch (last_recv_command.cmd) {

        case NO_CMD:
            ready_send_response.cmd = NO_CMD;
            ready_send_response.error = 0;
            break;

        case SET_HEAT_CMD:
            slave_status.heater_on = (last_recv_command.set_heater != 0) ? 1 : 0;
            ready_send_response.cmd = SET_HEAT_CMD;
            ready_send_response.error = 0;
            break;

        case READ_SUN_CMD:
            ready_send_response.cmd = READ_SUN_CMD;
            ready_send_response.sunlight_on = (slave_status.sunlight_on != 0) ? 1 : 0;
            ready_send_response.error = 0;
            break;

        case READ_TEMP_CMD:
            ready_send_response.cmd = READ_TEMP_CMD;
            ready_send_response.temperature = (float) slave_status.temperature;
            ready_send_response.error = 0;
            break;

        case READ_POS_CMD:
            ready_send_response.cmd = READ_POS_CMD;
            ready_send_response.orbit_position[0] = (float) slave_status.orbit_position[0];
            ready_send_response.orbit_position[1] = (float) slave_status.orbit_position[1];
            ready_send_response.orbit_position[2] = (float) slave_status.orbit_position[2];
            ready_send_response.error = 0;
            break;

        default:
            ready_send_response.error = 1;
            break;
    }
}

/**********************************************************
 *  Function: read_sun_sensor
 *********************************************************/
void read_sun_sensor ()
{
    int light_value = analogRead(SUN_SENSOR_PIN);

    if (light_value >= SUNLIGHT_THRESHOLD) {
        slave_status.sunlight_on = 1;
    } else {
        slave_status.sunlight_on = 0;
    }
}

/**********************************************************
 *  Function: set_heater
 *********************************************************/
void set_heater ()
{
    if (slave_status.heater_on) {
        digitalWrite(HEATER_PIN, HIGH);
    } else {
        digitalWrite(HEATER_PIN, LOW);
    }
}

//---------------------------------------------------------------------------
//                           TASK FUNCTIONS (CREATE AS MANY AS NEEDED)
//---------------------------------------------------------------------------

/**********************************************************
 *  Function: task A
 *********************************************************/
void Task_C()
{
    read_sun_sensor();
    set_heater();
}

void Task_B()
{
    get_position();
    get_temperature();
}

void Task_A()
{
    // Execute only a command whose complete message and parity were received.
    if (comm_server()) {
        execute_command_and_create_response();
    }
}

//---------------------------------------------------------------------------
//                           MAIN ARDUINO FUNCTIONS
//---------------------------------------------------------------------------

// --------------------------------------
// Function: setup
// --------------------------------------
void setup()
{
    Serial.begin(9600);
    pinMode(SUN_SENSOR_PIN, INPUT);
    pinMode(HEATER_PIN, OUTPUT);
    init_slave_state();
    set_heater();
    next_slot_release = millis();
}

// --------------------------------------
// Function: loop
// --------------------------------------
void loop()
{
    // Wait for the absolute release time so execution time does not accumulate
    // as drift in the task periods. Signed subtraction handles millis() wrap.
    while ((long)(millis() - next_slot_release) < 0) {
        delay(1);
    }

    // Four secondary cycles form the 200 ms major cycle.
    switch (scheduler_slot) {
        case 0: // 0-50 ms: 50 ms and 100 ms tasks
            Task_C();
            Task_B();
            break;

        case 1: // 50-100 ms: 50 ms and 200 ms tasks
            Task_C();
            Task_A();
            break;

        case 2: // 100-150 ms: 50 ms and 100 ms tasks
            Task_C();
            Task_B();
            break;

        case 3: // 150-200 ms: 50 ms tasks
            Task_C();
            break;

        default:
            scheduler_slot = 0;
            break;
    }

    scheduler_slot = (scheduler_slot + 1) % 4;
    next_slot_release += SCHEDULER_SLOT_MS;
}

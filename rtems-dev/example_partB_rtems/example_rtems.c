
//-Uncomment to compile with arduino support
//#define ARDUINO

// --------------------------------------
// Includes
// --------------------------------------
#include <termios.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include <sys/errno.h>
#include <sys/stat.h>

#include <rtems.h>
#include <rtems/termiostypes.h>
#include <bsp.h>

// --------------------------------------
// Constants
// --------------------------------------
#define SLAVE_ADDR 0x8

// --------------------------------------
// TYPES
// --------------------------------------

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
    long int sunlight_on;   // boolean to state if sunlight is on
    float temperature;       // value of the temperature
    float orbit_position[3]; // value of the position
};

// --------------------------------------
// PUBLIC STATUS (GLOBAL VARIABLES)
// --------------------------------------
// file descriptor for I2C
int file_desc;

// boolean with the status of the heater
int heater_on = 0;

// next command message to be send
struct command_msg ready_send_command = {NO_CMD, 0};

// last response message received
struct response_msg last_recv_response = {NO_CMD, 0, 0, 0.0, 0.0};

//---------------------------------------------------------------------------
//                           AUXILIAR FUNCTIONS 
//---------------------------------------------------------------------------

// --------------------------------------
// Function: send_msg
// --------------------------------------
void send_msg ()
{

    // compute parity
    unsigned char res_parity = (unsigned char) 0;
    int ret = 0;

    for (int i=0; i<sizeof(struct command_msg); i++) {
       // compute parity (xor)
       res_parity = res_parity ^ ((unsigned char *)(&ready_send_command))[i];
    }

    // send message to slave
    ret = write(file_desc,(char *)&ready_send_command,sizeof(struct command_msg));
    if( ret < sizeof(struct command_msg)){
        printf("ERROR: write Request: ret=%d \n", ret);
    }
    // send parity to slave
    ret = write(file_desc,(char *)&res_parity,1);
    if(ret < 1){
        printf("ERROR: write parity: ret=%d \n", ret);
        sleep(5);
        exit (-1);
    }
}


// --------------------------------------
// Function: recv_msg
// --------------------------------------
void recv_msg ()
{

    // compute parity
    unsigned char res_parity = (unsigned char) 0;
    unsigned char parity = (unsigned char) 0;
    int ret = 0;

    // send message to slave
    int leido=0;
    while (leido<sizeof(struct response_msg)) {
      ret = read(file_desc,((char *)(&last_recv_response))+leido,sizeof(struct response_msg)-leido);
      if( ret <= 0){
        printf("ERROR: read Request: ret=%d \n", ret);
        sleep(5);
        exit (-1);
      }
      leido = leido +ret;
    }

    for (int i=0; i<sizeof(struct response_msg); i++) {
       // compute parity (xor)
       res_parity = res_parity ^ ((unsigned char *)(&last_recv_response))[i];
    }

    // send parity to slave
    ret = 0;
    while (ret <= 0) {
      ret = read(file_desc,(char *)&parity,1);
      if (ret < 1) {
        printf("ERROR: read parity: ret=%d \n", ret);
        sleep(5);
        exit (-1);
      }
    }
    if (parity != res_parity) {
        printf("ERROR: received wrong parity\n");
        last_recv_response.cmd = NO_CMD;
        last_recv_response.error = 2;
    }
}
//---------------------------------------------------------------------------
//                           MAIN FUNCTIONS
//---------------------------------------------------------------------------
// --------------------------------------
// Function: send_cmd_msg
// --------------------------------------
void send_cmd_msg (int cmd)
{
    // set no command
    if (cmd == NO_CMD) {
        ready_send_command.cmd = NO_CMD;

    // set set heater command msg
    } else if (cmd == SET_HEAT_CMD) {
       heater_on = 1 - heater_on;
       ready_send_command.cmd = SET_HEAT_CMD;
       ready_send_command.set_heater = heater_on;

    // set read sunlight command msg
    } else if (cmd == READ_SUN_CMD) {
        ready_send_command.cmd = READ_SUN_CMD;

    // set read temperature command msg
    } else if (cmd == READ_TEMP_CMD) {
        ready_send_command.cmd = READ_TEMP_CMD;

    // set read position command msg
    } else if (cmd == READ_POS_CMD) {
        ready_send_command.cmd = READ_POS_CMD;
    }

}

// --------------------------------------
// Function: recv_res_msg
// --------------------------------------
void recv_res_msg ()
{
    //printf("sizeof(short)= %ld\n",sizeof(float));
    //printf("sizeof(long)= %ld\n",sizeof(float));
    //printf("sizeof(float)= %ld\n",sizeof(float));
    //printf("sizeof(struct command_msg)= %ld\n",sizeof(struct command_msg));
    //printf("sizeof(struct response_msg)= %ld\n",sizeof(struct response_msg));

    // unpack no command
    if (last_recv_response.cmd == NO_CMD) {
        printf("Answer: NO_CMD\n");
    // unpack set heater response msg
    } else if (last_recv_response.cmd == SET_HEAT_CMD) {
        printf("Answer: SET_HEAT_CMD\n");
        printf("Status: %d\n",last_recv_response.error);
        printf("Heater: %d\n",heater_on);
    // unpack read sunlight response msg
    } else if (last_recv_response.cmd == READ_SUN_CMD) {
        printf("Answer: READ_SUN_CMD\n");
        printf("Status: %d\n",last_recv_response.error);
        printf("Sun Sensor: %ld\n",last_recv_response.sunlight_on);

    // unpack read temperature response msg
    } else if (last_recv_response.cmd == READ_TEMP_CMD) {
        printf("Answer: READ_TEMP_CMD\n");
        printf("Status: %d\n",last_recv_response.error);
        printf("Temperature: %f\n",last_recv_response.temperature);

    // unpack read position response msg
    } else if (last_recv_response.cmd == READ_POS_CMD) {
        printf("Answer: READ_POS_CMD\n");
        printf("Status: %d\n",last_recv_response.error);
        printf("Position: %f, %f, %f\n",last_recv_response.orbit_position[0],
                                        last_recv_response.orbit_position[1],
                                        last_recv_response.orbit_position[2]);
    }

    //set response to no command
    last_recv_response.cmd = NO_CMD;
    return;

}


// --------------------------------------
// Function: loop
// --------------------------------------
void loop()
{
    struct timespec sleep_time;

    //set 400 msec wait between request and answer
    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = 400000000;

    printf("\nCommands menu\n");
    printf("-------------\n\n");
    printf("0: Null cmd\n");
    printf("1: Set heater on/off\n");
    printf("2: Read sun sensor\n");
    printf("3: Read temperature\n");
    printf("4: Read Position\n\n");
    printf("Enter your option: \n");
    int option = getchar() - '0';
    getchar();

    // send cmd
    send_cmd_msg(option);
#if defined(ARDUINO)
    send_msg();
#endif

    // wait 400 ms
    nanosleep(&sleep_time,NULL);

    //receive answer
#if defined(ARDUINO)
    recv_msg();
#endif
    recv_res_msg();
}

// --------------------------------------
// Function: Init
// --------------------------------------
rtems_task Init (rtems_task_argument ignored)
{

#if defined(ARDUINO)
    /* Open serial port */
    char serial_dev[]="/dev/com1";
    file_desc = open (serial_dev, O_RDWR);
    if (file_desc < 0) {
        printf("open: error opening serial %s\n", serial_dev);
        sleep(5);
        exit(-1);
    }

    struct termios portSettings;
    speed_t speed=B9600;

    tcgetattr(file_desc, &portSettings);
    cfsetispeed(&portSettings, speed);
    cfsetospeed(&portSettings, speed);
    cfmakeraw(&portSettings);
    tcsetattr(file_desc, TCSANOW, &portSettings);
#endif

    // endless loop
    while (1) {
        loop();
    }
    exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_SEMAPHORES 10
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 30
#define CONFIGURE_MAXIMUM_DIRVER 10
#define CONFIGURE_MAXIMUM_POSIX_THREADS 2
#define CONFIGURE_MAXIMUM_POSIX_TIMERS 1

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

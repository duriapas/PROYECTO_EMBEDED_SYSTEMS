/**********************************************************
 *  INCLUDES
 *********************************************************/
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

#include "master_device.h"

// --------------------------------------
// Constants
// --------------------------------------

// Represents the number of nsec. per sec.
static const unsigned long NS_PER_S = 1000000000;

// --------------------------------------
// Global Variables
// --------------------------------------

// file descriptor for I2C
int file_desc;

//---------------------------------------------------------------------------
//                           AUXILIAR FUNCTIONS
//---------------------------------------------------------------------------

// --------------------------------------
// Function: getClock
// --------------------------------------
double getClock()
{
    struct timespec tp;
    double reloj;

    clock_gettime (CLOCK_REALTIME, &tp);
    reloj = ((double)tp.tv_sec) +
        ((double)tp.tv_nsec) / ((double)NS_PER_S);

    return (reloj);
}

// --------------------------------------
// Function: delay
// --------------------------------------
void delayClock(double time)
{
    struct timespec sleep_time;

    //set double into struct timespec
    double floor_time = (double)((unsigned long) time);
    sleep_time.tv_sec = (unsigned long) time;
    sleep_time.tv_nsec = (unsigned long) ((time - floor_time)* (double)(NS_PER_S));

    // sleep
    nanosleep(&sleep_time,NULL);
}

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
//                           TASK FUNCTIONS
//---------------------------------------------------------------------------

// --------------------------------------
// Function: task_no_cmd
// This example is for cycling scheduling
// must be modify for priority scheduling
// --------------------------------------
void task_no_cmd ()
{
    printf("Task no cmd\n");

    // prepare request buffer
    create_command_no_cmd ();

    // send message to slave
    send_msg();

    // wait until answer is ready
    delayClock(0.400);

    //receive the answer from the slave
    recv_msg();

    //parse response
    process_response_no_cmd ();
}

// --------------------------------------
// Function: task_set_heat_cmd
// --------------------------------------
void task_set_heat_cmd ()
{
}

// --------------------------------------
// Function: task_read_sun_cmd
// --------------------------------------
void task_read_sun_cmd ()
{
}

// --------------------------------------
// Function: task_read_temp_cmd
// --------------------------------------
void task_read_temp_cmd ()
{
}

// --------------------------------------
// Function: task_read_pos_cmd
// --------------------------------------
void task_read_pos_cmd ()
{
}

// --------------------------------------
// Function: task_print_state
// --------------------------------------
void task_print_state ()
{
}

// --------------------------------------
// Function: task_control_temperature
// --------------------------------------
void task_control_temperature ()
{
}

//---------------------------------------------------------
//                           SCHED AND MAIN FUNCTIONS
//---------------------------------------------------------

//-------------------------------------
//-  Function: scheduler
// initial implementation
// must be modify for cycling or priority scheduling
//-------------------------------------
void *scheduler(void *arg)
{
    // Endless loop
   while (1) {
       task_no_cmd ();
    }
}

//-------------------------------------
//-  Function: Init
//-------------------------------------
rtems_task Init (rtems_task_argument ignored)
{
    pthread_t thread_ctrl;
    sigset_t alarm_sig;
    int i;
    char aux_car;

    /* Block all real time signals so they can be used for the timers.
     Note: this has to be done in main() before any threads are created
     so they all inherit the same mask. Doing it later is subject to
     race conditions */
    sigemptyset (&alarm_sig);
    for (i = SIGRTMIN; i <= SIGRTMAX; i++) {
        sigaddset (&alarm_sig, i);
    }
    sigprocmask (SIG_BLOCK, &alarm_sig, NULL);

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
    
    // wait until press enter
    printf("Press ENTER:");
    scanf("%c",&aux_car);

    /* Create first thread */
    pthread_create(&thread_ctrl, NULL, scheduler, NULL);
    pthread_join (thread_ctrl, NULL);
    exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_SEMAPHORES 10
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 30
#define CONFIGURE_MAXIMUM_DIRVER 10
#define CONFIGURE_MAXIMUM_POSIX_THREADS 10
#define CONFIGURE_MAXIMUM_POSIX_TIMERS 10

#define CONFIGURE_INIT
#include <rtems/confdefs.h>


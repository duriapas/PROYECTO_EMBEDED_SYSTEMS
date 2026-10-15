// --------------------------------------
// Include files
// --------------------------------------
#include <string.h>
#include <stdio.h>
#include <time.h>

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
    long  int sunlight_on;   // boolean to state if sunlight is on
    float temperature;       // value of the temperature
    float orbit_position[3]; // value of the position
};

// --------------------------------------
// PUBLIC STATUS (GLOBAL VARIABLES)
// --------------------------------------

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
    ret = Serial.write((char *)&ready_send_command,sizeof(struct command_msg));
    if( ret < sizeof(struct command_msg)){
        Serial.println("ERROR: write Request");
    }
    // send parity to slave
    ret = Serial.write((char *)&res_parity,1);
    if(ret < 1){
        Serial.println("ERROR: write parity");
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
    memset(&last_recv_response, 0, sizeof(last_recv_response));
    ret = Serial.readBytes((char *)&last_recv_response,sizeof(struct response_msg));
    if( ret < sizeof(struct response_msg)){
        Serial.println("ERROR: read Response");
    }

    for (int i=0; i<sizeof(struct response_msg); i++) {
       // compute parity (xor)
       res_parity = res_parity ^ ((unsigned char *)(&last_recv_response))[i];
    }

    // send parity to slave
    ret = Serial.readBytes((char *)&parity,1);
    if (ret < 1) {
        Serial.println("ERROR: read parity");
    } else if (parity != res_parity) {
        Serial.println("ERROR: read wrong parity");
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
    char buffer[64];
    char num_str1[10];
    char num_str2[10];
    char num_str3[10];

    memset(buffer,'\0', 64);
    memset(num_str1,'\0', 10);
    memset(num_str2,'\0', 10);
    memset(num_str3,'\0', 10);

    // unpack no command
    if (last_recv_response.cmd == NO_CMD) {
        Serial.println("Ans: NO_CMD");
        sprintf(buffer,"Error: %d",last_recv_response.error);
        Serial.println(buffer);
    // unpack set heater response msg
    } else if (last_recv_response.cmd == SET_HEAT_CMD) {
        Serial.println("Ans: SET_HEAT_CMD");
        sprintf(buffer,"Error: %d",last_recv_response.error);
        Serial.println(buffer);
        sprintf(buffer,"Heat: %d",heater_on);
        Serial.println(buffer);
    // unpack read sunlight response msg
    } else if (last_recv_response.cmd == READ_SUN_CMD) {
        Serial.println("Ans: READ_SUN_CMD");
        sprintf(buffer,"Error: %d",last_recv_response.error);
        Serial.println(buffer);
        sprintf(buffer,"Sun: %d",last_recv_response.sunlight_on);
        Serial.println(buffer);

    // unpack read temperature response msg
    } else if (last_recv_response.cmd == READ_TEMP_CMD) {
        Serial.println("Ans: READ_TEMP_CMD");
        sprintf(buffer,"Error: %d",last_recv_response.error);
        Serial.println(buffer);
        dtostrf(last_recv_response.temperature,9,2,num_str1);
        sprintf(buffer,"Temp: %s",num_str1);
        Serial.println(buffer);

    // unpack read position response msg
    } else if (last_recv_response.cmd == READ_POS_CMD) {
        Serial.println("Ans: READ_POS_CMD");
        sprintf(buffer,"Error: %d",last_recv_response.error);
        Serial.println(buffer);
        dtostrf(last_recv_response.orbit_position[0],9,2,num_str1);
        dtostrf(last_recv_response.orbit_position[1],9,2,num_str2);
        dtostrf(last_recv_response.orbit_position[2],9,2,num_str3);
        sprintf(buffer,"Pos: %s, %s, %s",num_str1, num_str2, num_str3);
        Serial.println(buffer);
    }

    //set response to no command
    last_recv_response.cmd = NO_CMD;
    return;

}

// --------------------------------------
// Function: setup
// --------------------------------------
void setup()
{
  // Hardware UART on pins 0 (RX) and 1 (TX), as in the project diagram.
  Serial.begin(9600);
}

// --------------------------------------
// Function: loop
// --------------------------------------
void loop()
{
    static int option = 0;

    switch (option) {
      case 0: 
        Serial.println("Send: Null cmd");
        break;
      case 1: 
        Serial.println("Send: Set heater on/off");
        break;
      case 2: 
        Serial.println("Send: Read sun sensor");
        break;
      case 3: 
        Serial.println("Send: Read temperature");
        break;
      case 4: 
        Serial.println("Send: Read position");
        break;       
    }

    // wait 1000 ms
    delay(1000);

    // send cmd
    send_cmd_msg(option);
    send_msg();

    // The slave scheduler can take up to 400 ms to receive, process and return
    // a request. Leave a small margin before reading the response.
    delay(450);

    //receive answer
    recv_msg();
    recv_res_msg();

    // wait 1000 ms
    delay(2000);
    
    option = (option + 1) % 5;
}

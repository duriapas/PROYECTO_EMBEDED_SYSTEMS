/**********************************************************
 *  INCLUDES
 *********************************************************/
#include <gtest/gtest.h>
#include <unistd.h>
#include <stdio.h>

extern "C" {
#include "master_device.h"
#include "slave_device.h"
}

// To check the whole master status at the same time
static void expect_master_status(int heater_on, int sunlight_on, double temperature,
                                 double x, double y, double z)
{
    EXPECT_EQ(heater_on, master_status.heater_on);
    EXPECT_EQ(sunlight_on, master_status.sunlight_on);
    EXPECT_DOUBLE_EQ(temperature, master_status.temperature);
    EXPECT_DOUBLE_EQ(x, master_status.orbit_position[0]);
    EXPECT_DOUBLE_EQ(y, master_status.orbit_position[1]);
    EXPECT_DOUBLE_EQ(z, master_status.orbit_position[2]);
}

TEST(test_protocol, basic)
{
    init_master_state();
    init_slave_state();
 
    ready_send_command.cmd = NO_CMD;
    ready_send_command.set_heater = 0;
    last_recv_command.cmd = NO_CMD;
    last_recv_command.set_heater = 0;
 
    // Random test data 
    master_status.heater_on = 1;
    slave_status.sunlight_on = 1;
    slave_status.temperature = 25.0;
    slave_status.orbit_position[0] = 0.0;       
    slave_status.orbit_position[1] = 6000.0;
    slave_status.orbit_position[2] = 0.0;
 
    // test NO_CMD
    create_command_no_cmd();
    EXPECT_EQ(NO_CMD, ready_send_command.cmd);
    EXPECT_EQ(0, ready_send_command.set_heater);
 
     
    last_recv_command = ready_send_command;     
    execute_command_and_create_response();
    EXPECT_EQ(NO_CMD, ready_send_response.cmd);
    EXPECT_EQ(0, ready_send_response.error);
 
    
    last_recv_response = ready_send_response;   
    process_response_no_cmd();
    expect_master_status(1, 0, 0.0, 3000.0, 0.0, 12000.0);   // nothing changes in the maste status
    
 
 
    // test SET_HEAT_CMD
    create_command_set_heat_cmd();
    EXPECT_EQ(SET_HEAT_CMD, ready_send_command.cmd);
    EXPECT_EQ(1, ready_send_command.set_heater);             
 
    last_recv_command = ready_send_command;      
    execute_command_and_create_response();
    EXPECT_EQ(SET_HEAT_CMD, ready_send_response.cmd);
    EXPECT_EQ(0, ready_send_response.error);
    EXPECT_EQ(1, slave_status.heater_on);                    
 
    last_recv_response = ready_send_response;   
    process_response_set_heat_cmd();
    expect_master_status(1, 0, 0.0, 3000.0, 0.0, 12000.0);   // heater on

 
    // test READ_SUN_CMD
    create_command_read_sun_cmd();
    EXPECT_EQ(READ_SUN_CMD, ready_send_command.cmd);
    EXPECT_EQ(0, ready_send_command.set_heater);
 
    last_recv_command = ready_send_command;      
    execute_command_and_create_response();
    EXPECT_EQ(READ_SUN_CMD, ready_send_response.cmd);
    EXPECT_EQ(0, ready_send_response.error);
    EXPECT_EQ(1, ready_send_response.sunlight_on);
 
    last_recv_response = ready_send_response;   
    process_response_read_sun_cmd();
    expect_master_status(1, 1, 0.0, 3000.0, 0.0, 12000.0);   // sunlight on

 
    // test READ_TEMP_CMD
    create_command_read_temp_cmd();
    EXPECT_EQ(READ_TEMP_CMD, ready_send_command.cmd);
    EXPECT_EQ(0, ready_send_command.set_heater);
 
    last_recv_command = ready_send_command;      
    execute_command_and_create_response();
    EXPECT_EQ(READ_TEMP_CMD, ready_send_response.cmd);
    EXPECT_EQ(0, ready_send_response.error);
    EXPECT_DOUBLE_EQ(25.0, ready_send_response.temperature);
 
    last_recv_response = ready_send_response;   
    process_response_read_temp_cmd();
    expect_master_status(1, 1, 25.0, 3000.0, 0.0, 12000.0);  // temperature updated
 
    // test READ_POS_CMD
    create_command_read_pos_cmd();
    EXPECT_EQ(READ_POS_CMD, ready_send_command.cmd);
    EXPECT_EQ(0, ready_send_command.set_heater);
 
    last_recv_command = ready_send_command;      
    execute_command_and_create_response();
    EXPECT_EQ(READ_POS_CMD, ready_send_response.cmd);
    EXPECT_EQ(0, ready_send_response.error);
    EXPECT_DOUBLE_EQ(0.0, ready_send_response.orbit_position[0]);
    EXPECT_DOUBLE_EQ(6000.0, ready_send_response.orbit_position[1]);
    EXPECT_DOUBLE_EQ(0.0, ready_send_response.orbit_position[2]);
 
    last_recv_response = ready_send_response;   
    process_response_read_pos_cmd();
    expect_master_status(1, 1, 25.0, 0.0, 6000.0, 0.0);      // position updated
}


/**********************************************************
 *  Funtion: main
 *********************************************************/

int main(int argc, char **argv) 
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


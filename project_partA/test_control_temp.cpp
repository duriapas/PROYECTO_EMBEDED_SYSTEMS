/**********************************************************
 *  INCLUDES
 *********************************************************/
#include <gtest/gtest.h>
#include <unistd.h>
#include <stdio.h>

extern "C" {
#include "master_device.h"
}

/**********************************************************
 *  Test: control_temperature 
 *********************************************************/

TEST(test_control_temperature, basic) 
{ 
    // init master state
    init_master_state();
    
    // test 1
    master_status.temperature = 20;
    control_temperature();
    ASSERT_EQ(1, master_status.heater_on);

    // test 1
    master_status.temperature = 60;
    control_temperature();
    ASSERT_EQ(0, master_status.heater_on);
}


TEST(test_control_temperature, Above_average_turns_heater_off)
{

    // init master state
    init_master_state();
    master_status.temperature = 45;
    master_status.heater_on   = 1;

    control_temperature();
    ASSERT_EQ(0, master_status.heater_on);

}

TEST(test_control_temperature, Just_Average)
{

    // With heater off
    init_master_state();
    master_status.temperature = 40;
    master_status.heater_on   = 0;

    control_temperature();
    ASSERT_EQ(0, master_status.heater_on);

    // With heater on
    init_master_state();
    master_status.temperature = 40;
    master_status.heater_on   = 1;

    control_temperature();
    ASSERT_EQ(0, master_status.heater_on);

}

TEST(test_control_temperature, heater_already_correct)
{
    // below 40 already on stays on
    init_master_state();
    master_status.temperature = 35;
    master_status.heater_on   = 1;

    control_temperature();
    ASSERT_EQ(1, master_status.heater_on);

    // above 40 already off stays off
    init_master_state();
    master_status.temperature = 45;
    master_status.heater_on   = 0;

    control_temperature();
    ASSERT_EQ(0, master_status.heater_on);
}


/**********************************************************
 *  Funtion: main
 *********************************************************/

int main(int argc, char **argv) 
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


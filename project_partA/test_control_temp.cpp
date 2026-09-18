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
 *  Test: control_temperature -> basic
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

/**********************************************************
 *  Funtion: main
 *********************************************************/

int main(int argc, char **argv) 
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


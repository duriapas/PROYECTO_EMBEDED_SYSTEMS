/**********************************************************
 *  INCLUDES
 *********************************************************/
#include <gtest/gtest.h>
#include <unistd.h>
#include <stdio.h>

extern "C" {
#include "slave_device.h"
}

// Start and end times for testing
static double call_start;
static double call_end;

/**********************************************************
 *  Test: get_temperature 
 *********************************************************/

TEST(test_get_temperature, basic)
{
    // init slave state
    init_slave_state();

    slave_status.heater_on = 1;

    get_temperature();
    printf("temperature: %f\n", slave_status.temperature);
    EXPECT_NEAR(0.0, slave_status.temperature, 0.005);

    sleep(5);

    get_temperature();
    printf("temperature: %f\n", slave_status.temperature);
    EXPECT_NEAR(27.7778, slave_status.temperature, 0.01);
}

TEST(test_get_temperature, heater_off_sunlight_off)
{
    init_slave_state();
    slave_status.heater_on   = 0;
    slave_status.sunlight_on = 0;
    slave_status.temperature = 30;
    slave_status.time_temperature = getClock() - 10;

    get_temperature();

    EXPECT_NEAR(-81.1111, slave_status.temperature, 0.05);
    EXPECT_NEAR(getClock(), slave_status.time_temperature, 0.005);

}

TEST(test_get_temperature, heater_off_sunlight_on)
{
    init_slave_state();
    slave_status.heater_on   = 0;
    slave_status.sunlight_on = 1;
    slave_status.temperature = 30;
    slave_status.time_temperature = getClock() - 10;

    get_temperature();
    printf("temperature: %f\n", slave_status.temperature);

    EXPECT_NEAR(-80.1111, slave_status.temperature, 0.05);
    //EXPECT_NEAR(getClock(), slave_status.time_temperature, 0.005);
}

TEST(test_get_temperature, heater_on_sunlight_on)
{
    init_slave_state();
    slave_status.heater_on   = 1;
    slave_status.sunlight_on = 1;
    slave_status.temperature = 30;
    slave_status.time_temperature = getClock() - 10;

    get_temperature();
    printf("temperature: %f\n", slave_status.temperature);
    
    EXPECT_NEAR(86.5556, slave_status.temperature, 0.05);
}

TEST(test_get_temperature, heat_then_cool)
{
    init_slave_state();
    slave_status.heater_on   = 1;
    slave_status.sunlight_on = 0;
    slave_status.temperature = 20;
    slave_status.time_temperature = getClock() - 10;

    get_temperature();
    printf("temperature: %f\n", slave_status.temperature);
    EXPECT_NEAR(75.5556, slave_status.temperature, 0.05);

    // heater switched off, 5 more seconds pass
    slave_status.heater_on = 0;
    slave_status.time_temperature = getClock() - 5.0;
    get_temperature();

    EXPECT_NEAR(20.0, slave_status.temperature, 0.05);
}

/**********************************************************
 *  Funtion: main
 *********************************************************/

int main(int argc, char **argv) 
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


/**********************************************************
 *  INCLUDES
 *********************************************************/
#include <gtest/gtest.h>
#include <unistd.h>
#include <stdio.h>

extern "C" {
#include "slave_device.h"
}



// Checks the three coordinates.
static void expect_position(double x, double y, double z, double seconds)
{
    EXPECT_NEAR(x, slave_status.orbit_position[0], 1) << "x at t = " << seconds << " s";
    EXPECT_NEAR(y, slave_status.orbit_position[1], 1) << "y at t = " << seconds << " s";
    EXPECT_NEAR(z, slave_status.orbit_position[2], 1) << "z at t = " << seconds << " s";
}

TEST(test_position, basic)
{
    init_slave_state();
 
    get_position();
    printf("position: x:%f, y:%f, z:%f\n",
           slave_status.orbit_position[0],
           slave_status.orbit_position[1],
           slave_status.orbit_position[2]);
    expect_position(3000.0, 0.0, 12000.0, 0.0);
 
    //sleep (300); // Line below does the same withou actually waiting 300 seconds
                // if using this, the tolerance must be higher because of time discrepanciess
                // with the actual time.
    slave_status.orbit_init_time = getClock() - 300.0;   

    get_position();
    printf("position: x:%f, y:%f, z:%f\n",
           slave_status.orbit_position[0],
           slave_status.orbit_position[1],
           slave_status.orbit_position[2]);
    expect_position(3000.0, 0.0, 12000.0, 300.0);
}

TEST(test_position, halfway_between_points)
{

    init_slave_state();
    slave_status.orbit_init_time = getClock() - 7.5; 
    get_position();
    expect_position( 2926.58,  927.05,  11706.34, 7.5);
}

TEST(test_position, eighty_percent_between_points)
{

    init_slave_state();
    slave_status.orbit_init_time = getClock() - 12; 
    get_position();
    expect_position(  2882.54, 1483.28, 11530.14, 12.0);
}

// test to see if multiple calls work and if calling after the first orbit works
TEST(test_position, multiple_calls_multiple_orbits)
{

    init_slave_state();
    slave_status.orbit_init_time = getClock() - 7.5; 
    get_position();
    expect_position( 2926.58,  927.05,  11706.34, 7.5);

    slave_status.orbit_init_time = getClock() - 82.5; 
    get_position();
    expect_position( -463.53, 5853.17, -1854.10, 82.5);

    slave_status.orbit_init_time = getClock() - 307.5;   // second orbit
    get_position();
    expect_position(2926.58, 927.05, 11706.34, 307.5);

}

/**********************************************************
 *  Funtion: main
 *********************************************************/

int main(int argc, char **argv) 
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


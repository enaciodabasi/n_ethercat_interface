/**
 * @file timer_test.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "ethercat_interface/ethercat_interface.hpp"

#include <iostream>

int main(int argc, char** argv)
{

    TimeTracker tracker;

    bool flag = true;
    std::timespec wakeuptime;
    clock_gettime(CLOCK_MONOTONIC, &wakeuptime); 
    int count = 0;
    constexpr std::timespec period{0, (long int)(NanoSecPerSec * 0.002)};

    while(flag)
    {   
        if(count >= 10000){
            flag = false;
            break;
        }
        wakeuptime = addTimespec(wakeuptime, period);

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &wakeuptime, NULL);
        tracker.measureTimings(wakeuptime);

        auto measurements = tracker.getMeasurementVars();

        tracker.updateEndTime();
        count += 1;
    }

    return 0;

}
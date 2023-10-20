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


#include "ethercat_interface/time_operations.hpp"

#include <iostream>

class TimerUser
{
    public:
    TimerUser(){}
    ~TimerUser(){}

    void timerCallback(int* i)
    {
        (*i) += 10;
    }    
};


int main(int argc, char** argv)
{

    Timer sqtimer;
    TimerUser user;
    int count = 0;
    sqtimer.start(1, &TimerUser::timerCallback, user, &count);
    
    /* timer.setCallback<>(1.0, &timerCallback); */
    bool flag = true;
    while(flag)
    {   
        if(count >= 10000){
            flag = false;
            break;
        }
        
        std::cout << "Count: " << count << std::endl;
    }

    /* TimeTracker tracker;
    

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
        std::cout << measurements.m_Exec;
        tracker.updateEndTime();
        count += 1;
    } */

    return 0;

}
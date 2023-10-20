/**
 * @file time_operations.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ethercat_interface/time_operations.hpp"


std::timespec addTimespec(const std::timespec& t1, const std::timespec& t2)
{
    std::timespec result;
    int64_t nanosecOverflow = t1.tv_nsec + t2.tv_nsec;
    if(nanosecOverflow >= NanoSecPerSec){
        result.tv_sec = t1.tv_sec + t2.tv_sec + 1;
        result.tv_nsec = t1.tv_nsec + t2.tv_nsec - (int64_t)NanoSecPerSec;
        return result;/* {t1.tv_sec + t2.tv_sec + 1, t1.tv_nsec + t2.tv_nsec - (int64_t)NanoSecPerSec}; */ 
    }

    /* return {t1.tv_sec + t2.tv_sec, t1.tv_nsec + t2.tv_nsec}; */

    result.tv_sec = t1.tv_sec + t2.tv_sec;
    result.tv_nsec = t1.tv_nsec + t2.tv_nsec;
    return result;
}
CyclicTaskTimer::CyclicTaskTimer()
{

}

CyclicTaskTimer::CyclicTaskTimer(int64_t period_ns, ClockType clock_to_use)
    : m_PeriodNanoSec(period_ns), m_ClockToUse(clock_to_use)
{
    m_CyclePeriod = std::timespec({0, m_PeriodNanoSec});
}

void CyclicTaskTimer::init()
{
    clock_gettime(m_ClockToUse, &m_WakeupTime);
}

void CyclicTaskTimer::sleep()
{   
    m_WakeupTime = addTimespec(m_WakeupTime, m_CyclePeriod);
    clock_nanosleep(m_ClockToUse, TIMER_ABSTIME, &m_WakeupTime, nullptr);
}

CyclicTaskTimerDC::CyclicTaskTimerDC()
    : CyclicTaskTimer()
{

}

void CyclicTaskTimerDC::writeAppTimeToMaster(ec_master_t* master_ptr)
{
    ecrt_master_application_time(
        master_ptr,
        timespectoNanoSec(m_WakeupTime)
    );
}

void CyclicTaskTimerDC::syncSlaveClocks(ec_master_t* master_ptr)
{
    ecrt_master_sync_slave_clocks(master_ptr);
}

void CyclicTaskTimerDC::syncReferenceClock(ec_master_t* master_ptr)
{
    if(m_SyncRefCounter != 0){
        m_SyncRefCounter = 0;
    }
    else{
        m_SyncRefCounter = 1;
        std::timespec currentTime;
        clock_gettime(m_ClockToUse, &currentTime);
        ecrt_master_sync_reference_clock_to(master_ptr, timespectoNanoSec(currentTime));
    }
}

const uint64_t timespectoNanoSec(const std::timespec& t)
{
    return uint64_t((t.tv_sec * NanoSecPerSec) + t.tv_nsec);
}

/*
---------------------------------------------
---------------------------------------------
---------------------------------------------
Callback based timer class.
---------------------------------------------
---------------------------------------------
---------------------------------------------
*/
Timer::Timer()
{   

}

Timer::~Timer()
{
    m_IsActive = false;
}

/*
*/

TimeTracker::TimeTracker()
{
    
}
TimeTracker::~TimeTracker()
{
}
void TimeTracker::setWakeUpTime(const std::timespec& wakeup_time_ts)
{
    m_WakeUpTime = timeSpecToTimepoint(wakeup_time_ts);
}
void TimeTracker::measureTimings(const std::timespec& wakeup_time_ts)
{
    m_WakeUpTime = timeSpecToTimepoint(wakeup_time_ts);
    auto startTime = std::chrono::system_clock::now();
    m_MeasurementVars.m_Latency = std::chrono::duration_cast<std::chrono::milliseconds>(m_WakeUpTime - startTime).count();
    m_MeasurementVars.m_Period = std::chrono::duration_cast<std::chrono::milliseconds>(m_LastStartTime - startTime).count();
    m_MeasurementVars.m_Exec = std::chrono::duration_cast<std::chrono::milliseconds>(m_LastStartTime - m_EndTime).count();
    if(m_MeasurementVars.m_Latency > m_MeasurementVars.m_MaxLatency){m_MeasurementVars.m_MaxLatency = m_MeasurementVars.m_Latency;}
    if(m_MeasurementVars.m_Latency < m_MeasurementVars.m_MinLatency){m_MeasurementVars.m_MinLatency = m_MeasurementVars.m_Latency;}
    
    if(m_MeasurementVars.m_Period > m_MeasurementVars.m_MaxPeriod){m_MeasurementVars.m_MaxPeriod = m_MeasurementVars.m_Period;}
    if(m_MeasurementVars.m_Period < m_MeasurementVars.m_MinPeriod){m_MeasurementVars.m_MinPeriod = m_MeasurementVars.m_Period;}
    if(m_MeasurementVars.m_Exec > m_MeasurementVars.m_MaxExec){m_MeasurementVars.m_MaxExec = m_MeasurementVars.m_Exec;}
    if(m_MeasurementVars.m_Exec < m_MeasurementVars.m_MinExec){m_MeasurementVars.m_MinExec = m_MeasurementVars.m_Exec;}
    
}
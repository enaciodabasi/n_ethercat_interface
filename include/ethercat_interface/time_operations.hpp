/**
 * @file time_operations.hpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef TIME_OPERATIONS_HPP_
#define TIME_OPERATIONS_HPP_

#include <time.h>
#include <string>
#include <chrono>
#include <thread>
#include <functional>
#include <atomic>
#include <iostream>

#include "ecrt.h"

constexpr auto NanoSecPerSec = 1e+9; 

std::timespec addTimespec(const std::timespec& t1, const std::timespec& t2);

const uint64_t timespectoNanoSec(const std::timespec& t);

    enum ClockType : int
    {
        Realtime, // CLOCK_REALTIME: 0
        Monotonic, // CLOCK_MONOTONIC : 1
        ProcessCPU, // CLOCK_PROCESS_CPUTIMER: 2
        ThreadCPU // CLOCK_THREAD_CPUTIMER: 3
    };

    /**
     * @brief Helper struct to use in order to sync the loop to a frequency. 
     * 
     */
    struct CyclicTaskTimer
    {   
        protected:

        int64_t m_PeriodNanoSec;
        std::timespec m_CyclePeriod;

        std::timespec m_WakeupTime;

        int m_ClockToUse;

        public:
        
        CyclicTaskTimer();
        CyclicTaskTimer(
            int64_t period_ns,
            ClockType clock_to_use = ClockType::Monotonic
        );

        void init();

        void sleep();
    };

    struct CyclicTaskTimerDC : public CyclicTaskTimer
    {
        public:

        CyclicTaskTimerDC();

        /* ~CyclicTaskTimerDC(){
            
        }; */

        /**
         * @brief Writes the application time to the EtherCAT master
         * 
         * @param master_ptr Pointer to the ec_master struct inside the Master class 
         */
        void writeAppTimeToMaster(ec_master_t* master_ptr);

        /**
         * @brief Sync the slaves's clocks.
         * 
         * @param master_ptr Pointer to the ec_master struct inside the Master class
         */
        void syncSlaveClocks(ec_master_t* master_ptr);

        void syncReferenceClock(ec_master_t* master_ptr);

        private:

        uint16_t m_SyncRefCounter = 0;
    };

    class Timer
    {
        public:
        Timer();
        ~Timer();
        std::atomic<bool> m_IsActive{true};
        template<class Func, class... arguments>
        void setCallback(double interval, Func&& callback_function, arguments&&... args);
        private:
    };


        struct MeasurementVars
        {
            uint64_t m_Period = 0;
            uint64_t m_Exec = 0;
            uint64_t m_Latency = 0;
            
            uint64_t m_MinPeriod = 0;
            uint64_t m_MinExec = 0;
            uint64_t m_MinLatency = 0;

            uint64_t m_MaxPeriod = 0;
            uint64_t m_MaxExec = 0;
            uint64_t m_MaxLatency = 0;
        };
        
        constexpr std::chrono::nanoseconds timespecToNanoSecDuration(const std::timespec& ts)
        {   
            return std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::move(std::chrono::seconds{ts.tv_sec} + std::chrono::nanoseconds{ts.tv_nsec})
            );
        }
        struct TimeTracker
        {
            public:

            TimeTracker();

            //TimeTracker(double interval);

            ~TimeTracker();

            const MeasurementVars getMeasurementVars() const
            {
                return m_MeasurementVars;
            }

            void measureTimings(const std::timespec& wakeup_time_ts);
        
            void setWakeUpTime(const std::timespec& wakeup_time_ts);

            void updateEndTime(){
                m_EndTime = std::chrono::system_clock::now();
            }

            private:

            MeasurementVars m_MeasurementVars;

            std::chrono::time_point<std::chrono::system_clock> m_WakeUpTime;
            std::chrono::time_point<std::chrono::system_clock> m_LastStartTime;
            std::chrono::time_point<std::chrono::system_clock> m_EndTime;

            constexpr std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> timeSpecToTimepoint(const std::timespec& ts)
            {
                return std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>(
                    std::chrono::duration_cast<std::chrono::system_clock::duration>(
                        timespecToNanoSecDuration(ts)
                    )
                );
            } 
        };


#endif // TIME_OPERATIONS_HPP_
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
#include <chrono>


#include "ecrt.h"

namespace time_operations
{

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


}

#endif // TIME_OPERATIONS_HPP_
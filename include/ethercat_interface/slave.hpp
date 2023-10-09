/**
 * @file slave.hpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SLAVE_HPP_
#define SLAVE_HPP_

#include <unordered_map>

#include "ec_common_defs.hpp"

namespace ec
{
    namespace slave
    {

        typedef std::pair<uint16_t, ec_pdo_entry_info_t*> PDO_Mapping;

        class Slave
        {      
            public:

            Slave(){};
            
            Slave(const SlaveInfo& slave_info);

            Slave(Slave&& s) noexcept
                : 
                    m_SlaveConfigPtr(nullptr),
                    m_RxPDOs(nullptr),
                    m_TxPDOs(nullptr),
                    m_SyncManagerConfig(nullptr)
            {
                m_SlaveInfo = s.m_SlaveInfo;
                m_SlaveConfigPtr = s.m_SlaveConfigPtr;
                m_SlaveState = s.m_SlaveState;
                m_RxPDOs = s.m_RxPDOs;
                m_TxPDOs = s.m_TxPDOs;
                m_RxMappings = s.m_RxMappings;
                m_TxMappings = s.m_TxMappings;
                m_SyncManagerConfig = s.m_SyncManagerConfig;
                m_Offsets = s.m_Offsets;

                s.m_SlaveConfigPtr = nullptr;
                s.m_RxPDOs = nullptr;
                s.m_TxPDOs = nullptr;
                s.m_SyncManagerConfig = nullptr;
            }

            virtual ~Slave(){};

            /**
             * @brief Virtual initialization function for each slave type, because each has it's own unique initalization process.
             * @param master_ptr Pointer to the ec_master_t struct
             * @param domain_ptr Pointer to the ec_domain_t struct
             * @return true if no errors are encountered
             * @return false if an errors is encountered
             */
            virtual bool init(ec_master_t* master_ptr, ec_domain_t* domain_ptr = nullptr);
            /* virtual bool init(ec_master_t* master_ptr); */

            const SlaveInfo getSlaveInfo() const
            {
                return m_SlaveInfo;
            }

            std::optional<uint*> getOffsetPtr(const std::string& offset_name)
            {
                auto found = m_Offsets.find(offset_name);
                if(found == m_Offsets.end()){
                    return std::nullopt;
                }

                return &found->second;
            }

            bool configurePDOs();

            protected: // Protected member variables

            SlaveInfo m_SlaveInfo;

            // EtherCAT spesifics variables:

            /**
             * @brief Might be changed if using one for each slave does not work as intended.
             * 
             */
            ec_slave_config_t* m_SlaveConfigPtr = nullptr;

            /**
             * @brief Holds the current state of the slave
             * 
             */
            ec_slave_config_state_t m_SlaveState;

            /**
             * @brief 
             * 
             */
            ec_pdo_info_t* m_RxPDOs;

            ec_pdo_info_t* m_TxPDOs;

            std::vector<PDO_Mapping> m_RxMappings;

            std::vector<PDO_Mapping> m_TxMappings;

            ec_sync_info_t* m_SyncManagerConfig;

            std::unordered_map<std::string, uint> m_Offsets;

            protected: // Protected member functions

            virtual bool createSlaveConfigPtr(ec_master_t* master_ptr);

            virtual bool createSlaveSyncManagerConfig();
            
        };

        /**
         * @brief Represents the motor driver slaves. Inherits from the base Slave class.
         * 
         */
        class Driver : public Slave
        {
            public:
            
            Driver(const SlaveInfo& slave_info);

            ~Driver();

            private:

            /**
             * @brief 
             * 
             * @return true if no errors are encountered 
             * @return false if an errors is encountered
             */
            /* bool init(ec_master_t* master_ptr, ec_domain_t* domain_ptr = nullptr) override; */

            bool configurePDOs();
            
            
        };

        /**
         * @brief Represents the Input/Output slaves. Inherits from the base Slave class.
         * 
         */
        class IO : public Slave
        {
            public:

            IO(const SlaveInfo& slave_info);

            ~IO();

            private:

        };

        /**
         * @brief Represents the PLC slaves. Inherits from the base Slave class.
         * 
         */
        class PLC : public Slave
        {
            public:

            PLC(const SlaveInfo& slave_info);

            ~PLC();

            private:
            
        };

        /**
         * @brief Represents the Coupler slaves. Inherits from the base Slave class.
         * Does not require any type of configuration other than ecrt_master_slave_config.
         * 
         */
        class Coupler : public Slave
        {   
            public:

            Coupler(const SlaveInfo& slave_info);
            ~Coupler();

            bool init(ec_master_t* master_ptr, ec_domain_t* domain_ptr = nullptr) override;

            private:
        };

    } // End of namespace slave
} // End of namespace ec




#endif // SLAVE_HPP_
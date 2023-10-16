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

#include <optional>
#include <unordered_map>

#include "data.hpp"
#include "ec_common_defs.hpp"

namespace ec
{
    namespace slave
    {

        typedef std::pair<uint16_t, ec_pdo_entry_info_t*> PDO_Mapping;

        class Slave
        {      
            public:

            /**
             * @brief Default constructor.
             * 
             */
            Slave(){};
            
            /**
             * @brief Constructor that sets the slave information
             * 
             * @param slave_info : SlaveInfo struct parsed by the master via parser.cpp
             */
            Slave(const SlaveInfo& slave_info);
            
            /**
             * @brief Move constructor.
             * 
             * @param s 
             */
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

            template<typename T>
            bool write(
                const std::string& entry_name,
                T value
            )
            {
                auto entryQueryOffset = m_Offsets.find(entry_name);
        
                if(entryQueryOffset == m_Offsets.end()){
                    return false;
                }

                unsigned int entryOffset = entryQueryOffset->second;

                uint8_t* dataInPtr = m_DomainDataPtr + entryOffset;    

                return this->writeToEtherCAT(dataInPtr, value);

            }
    
            template<typename T>
            std::optional<T> read(const std::string& entry_name)
            {

                auto entryQueryOffset = m_Offsets.find(entry_name);
        
                if(entryQueryOffset == m_Offsets.end()){
                    return false;
                }

                unsigned int entryOffset = entryQueryOffset->second;

                uint8_t* dataInPtr = m_DomainDataPtr + entryOffset;

                return this->readFromEtherCAT<T>(dataInPtr);
            }
            
            /**
             * @brief Return the slave information struct stored inside the slave object.
             * 
             * @return const SlaveInfo 
             */
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
            
            /**
             * @brief Sets member variable pointer to the pointer to the EtherCAT domain.
             * 
             * @param domain_data_ptr 
             */
            void setDomainDataPtr(uint8_t* domain_data_ptr)
            {
                m_DomainDataPtr = domain_data_ptr;
            }

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

            std::shared_ptr<data::DataMap> m_SharedDataMap;

            protected: // Protected member functions

            virtual bool createSlaveConfigPtr(ec_master_t* master_ptr);

            virtual bool createSlaveSyncManagerConfig();

            /**
             * @brief Sets and initializes the m_SharedDataMap object, must be called after calling the init function of the slave object.
             * 
             * @param data_map_shared_ptr 
             * @return true If all PDO's are set.
             * @return false otherwise.
             */
            bool setSharedDataMap(std::shared_ptr<data::DataMap>& data_map_shared_ptr);

            template<typename T>
            bool writeToEtherCAT(uint8_t* point_to_write, T value);

             /**
             * @brief Helper function to deduce the requested data type and call the appropriate EC_READ macro.
             * 
             * @tparam T 
             * @param entry_name 
             * @return std::optional<T> 
             */
            template<typename T>
            std::optional<T> readFromEtherCAT(const std::string& entry_name);

            uint8_t* m_DomainDataPtr = nullptr;
            
        };
        
        template<typename T>
        bool Slave::writeToEtherCAT(uint8_t* point_to_write, T value)
        {
             if constexpr (std::is_same_v<uint8_t, T>)
            {
               EC_WRITE_U8(
                    point_to_write,
                    value
               ); 
            }
            else if constexpr (std::is_same_v<uint16_t, T>)
            {
                EC_WRITE_U16(
                    point_to_write,
                    value
                );
            }
            else if constexpr (std::is_same_v<uint32_t, T>)
            {
                EC_WRITE_U32(
                    point_to_write,
                    value
                );
            }
            else if constexpr (std::is_same_v<uint64_t, T>)
            {
                EC_WRITE_U64(
                    point_to_write,
                    value
                );
            }
            else if constexpr (std::is_same_v<int8_t, T>)
            {
                EC_WRITE_S8(
                    point_to_write,
                    value
                );
            }
            else if constexpr (std::is_same_v<int16_t, T>)
            {
                EC_WRITE_S16(
                    point_to_write,
                    value
                );
            }
            else if constexpr (std::is_same_v<int32_t, T>)
            {
                EC_WRITE_S32(
                    point_to_write,
                    value
                );
            }
            else if constexpr (std::is_same_v<int64_t, T>)
            {
                EC_WRITE_S64(
                    point_to_write,
                    value
                );
            }
            else if constexpr (std::is_same_v<float, T>)
            {
                EC_WRITE_REAL(
                    point_to_write,
                    value
                );
            }
            else if constexpr (std::is_same_v<double, T>)
            {
                EC_WRITE_LREAL(
                    point_to_write,
                    value
                );
            }
            else
            {
               return false;
            }
            
            return true;
        }

        template<typename T>
        std::optional<T> readFromEtherCAT(uint8_t* point_to_read_from)
        {
            if constexpr (std::is_same_v<uint8_t, T>)
            {
               return EC_READ_U8(
                    point_to_read_from
               ); 
            }
            else if constexpr (std::is_same_v<uint16_t, T>)
            {
                return EC_READ_U16(
                    point_to_read_from
                );
            }
            else if constexpr (std::is_same_v<uint32_t, T>)
            {
                return EC_READ_U32(
                    point_to_read_from
                );
            }
            else if constexpr (std::is_same_v<uint64_t, T>)
            {
                return EC_READ_U64(
                    point_to_read_from
                );
            }
            else if constexpr (std::is_same_v<int8_t, T>)
            {
                return EC_READ_S8(
                    point_to_read_from
                );
            }
            else if constexpr (std::is_same_v<int16_t, T>)
            {
                return EC_READ_S16(
                    point_to_read_from
                );
            }
            else if constexpr (std::is_same_v<int32_t, T>)
            {
                return EC_READ_S32(
                    point_to_read_from
                );
            }
            else if constexpr (std::is_same_v<int64_t, T>)
            {
                return EC_READ_S64(
                    point_to_read_from
                );
            }
            else if constexpr (std::is_same_v<float, T>)
            {
                return EC_READ_REAL(
                    point_to_read_from
                );
            }
            else if constexpr (std::is_same_v<double, T>)
            {
                return EC_READ_LREAL(
                    point_to_read_from
                );
            }

            return std::nullopt;
        }

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

/*             bool configurePDOs();
 */            
            
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
/**
 * @file slave.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ethercat_interface/slave.hpp"

namespace ec
{

    namespace slave
    {

        Slave::Slave(const SlaveInfo& slave_info)
        {
            m_SlaveInfo = slave_info;
        }

        bool Slave::init(ec_master_t* master_ptr, ec_domain_t* domain_ptr)
        {
            if(!createSlaveConfigPtr(master_ptr)){
                return false;
            }

            if(!configurePDOs()){
                return false;
            }

            if(!createSlaveSyncManagerConfig()){
                return false;
            }

            uint numSyncs = 0;
            if(m_SlaveInfo.syncManagerConfig.size() == 4){
                numSyncs = EC_END;
            }
            else{
                if(m_SlaveInfo.syncManagerConfig.size() < 4)
                numSyncs = m_SlaveInfo.syncManagerConfig.size();
            }
            
            if(ecrt_slave_config_pdos(m_SlaveConfigPtr, numSyncs, m_SyncManagerConfig) != 0){
                return false;
            }
        }

        bool Slave::createSlaveConfigPtr(ec_master_t* master)
        {
            m_SlaveConfigPtr = ecrt_master_slave_config(master, m_SlaveInfo.alias, m_SlaveInfo.position, m_SlaveInfo.vendorID, m_SlaveInfo.productCode);

            return ((m_SlaveConfigPtr != NULL) ? 1 : 0);
        }

        bool Slave::createSlaveSyncManagerConfig()
        {
            const std::size_t syncManagerSize = m_SlaveInfo.syncManagerConfig.size();
            const auto syncManagerConfig = m_SlaveInfo.syncManagerConfig;
            m_SyncManagerConfig = new ec_sync_info_t[syncManagerSize];
            m_SyncManagerConfig[0] = {
                0,
                syncManagerConfig.at(0).syncManagerDirection,
                0,
                NULL,
                syncManagerConfig.at(0).watchdogMode
            };

            m_SyncManagerConfig[1] = {
                1,
                syncManagerConfig.at(1).syncManagerDirection,
                0,
                NULL,
                syncManagerConfig.at(1).watchdogMode
            };

            m_SyncManagerConfig[2] = {
                2,
                syncManagerConfig.at(2).syncManagerDirection,
                (unsigned int)m_RxMappings.size(),
                m_RxPDOs
            };  

            m_SyncManagerConfig[3] = {
                3,
                syncManagerConfig.at(3).syncManagerDirection,
                (unsigned int)m_TxMappings.size(),
                m_TxPDOs
            };
        }

        Driver::Driver(const SlaveInfo& slave_info)
            : Slave(slave_info)
        {
            
        }

        /* bool Driver::init(ec_master_t* master_ptr, ec_domain_t* domain_ptr)
        {

            if(!createSlaveConfigPtr(master_ptr)){
                return false;
            }

            if(!configurePDOs()){
                return false;
            }

            if(!createSlaveSyncManagerConfig()){
                return false;
            }

            uint numSyncs = 0;
            if(m_SlaveInfo.syncManagerConfig.size() == 4){
                numSyncs = EC_END;
            }
            else{
                if(m_SlaveInfo.syncManagerConfig.size() < 4)
                numSyncs = m_SlaveInfo.syncManagerConfig.size();
            }
            
            if(ecrt_slave_config_pdos(m_SlaveConfigPtr, numSyncs, m_SyncManagerConfig) != 0){
                return false;
            }

            
            return true;
        } */

        bool Slave::configurePDOs()
        {
            
            const std::size_t rxPdoSize = m_SlaveInfo.rxPDOs.size(); 
            const std::size_t txPdoSize = m_SlaveInfo.txPDOs.size();
            
            /*
                If PDO mapping sizes are equal to zero:
                

                Else resize the PDO pointers according to the given size.
            */
            if(rxPdoSize == 0){

            }
            else{
                m_RxPDOs = new ec_pdo_info_t[rxPdoSize];
            }
    
            if(txPdoSize == 0){

            }else{
                m_TxPDOs = new ec_pdo_info_t[rxPdoSize];
                
            }

            for(std::size_t i = 0; i < rxPdoSize; i++)
            {   
                // Get the current PDO info
                auto& pdo = m_SlaveInfo.rxPDOs.at(i);
                const std::size_t numEntries = pdo.entries.size();
                // Create temp mapping
                PDO_Mapping mapping;
                mapping.first = pdo.pdoAddress;
                // Resize the mapping pointer
                mapping.second = new ec_pdo_entry_info_t[numEntries];
                for(std::size_t j = 0; j < numEntries; j++)
                {
                    // Get the current PDO entry info
                    const auto currEntry = pdo.entries.at(j);
                    // Populate the value at index j
                    mapping.second[j] = {
                        currEntry.index,
                        currEntry.subindex,
                        currEntry.bitlength
                    };
                    // Add the PDO entry to the Offset map
                    m_Offsets.insert_or_assign(currEntry.entryName, uint());
                }

                // Save the mapping
                m_RxMappings.push_back(mapping);
                // Save the PDO mapping inside the ec_pdo_info_t pointer
                m_RxPDOs[i].index = m_RxMappings.at(i).first;
                m_RxPDOs[i].n_entries = (unsigned int)pdo.entries.size();
                m_RxPDOs[i].entries = m_RxMappings.at(i).second;
            }

            for(std::size_t i = 0; i < txPdoSize; i++)
            {   
                // Get the current PDO info
                auto& pdo = m_SlaveInfo.txPDOs.at(i);
                const std::size_t numEntries = pdo.entries.size();
                // Create temp mapping
                PDO_Mapping mapping;
                mapping.first = pdo.pdoAddress;
                // Resize the mapping pointer
                mapping.second = new ec_pdo_entry_info_t[numEntries];
                for(std::size_t j = 0; j < numEntries; j++)
                {
                    // Get the current PDO entry info
                    const auto currEntry = pdo.entries.at(j);
                    // Populate the value at index j
                    mapping.second[j] = {
                        currEntry.index,
                        currEntry.subindex,
                        currEntry.bitlength
                    };
                    // Add the PDO entry to the Offset map
                    m_Offsets.insert_or_assign(currEntry.entryName, uint());
                }

                // Save the mapping
                m_TxMappings.push_back(mapping);
                // Save the PDO mapping inside the ec_pdo_info_t pointer
                m_TxPDOs[i].index = m_RxMappings.at(i).first;
                m_TxPDOs[i].n_entries = (unsigned int)pdo.entries.size();
                m_TxPDOs[i].entries = m_RxMappings.at(i).second;
            }


            // Put in a check for the number of PDOs
            // If it's 0 => Use default mappning (How?)            

        }


        Coupler::Coupler(const SlaveInfo& slave_info)
            : Slave(slave_info)
        {

        }

        Coupler::~Coupler()
        {

        }

        bool Coupler::init(ec_master_t* master_ptr, ec_domain_t* domain_ptr)
        {
            if(!createSlaveConfigPtr(master_ptr)){
                return false;
            }
        }

    } // End of namespace slave

} // End of namespace ec

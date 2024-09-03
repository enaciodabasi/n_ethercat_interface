/**
 * @file ec_common_defs.hpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef EC_COMMON_TYPES_HPP_
#define EC_COMMON_TYPES_HPP_

#include <string>
#include <vector>
#include <optional>
#include <map>

#include "ecrt.h"

namespace ec
{

    typedef uint16_t Index;
    typedef uint8_t Subindex;
    typedef uint8_t Bitlength;

    struct ProgramConfig;

    struct SlaveInfo;
    struct PDO;
    struct PDO_Entry;
    struct DistributedClockConfig;
    struct SyncManagerConfig;

    //enum class ProgramConfigErrorTypes
    //{
    //    FailedParse,
    //    DomainEntryError,
    //};
    
    enum class DataType
    {
        UINT8,
        INT8,
        UINT16,
        INT16,
        UINT32,
        INT32,
        UINT64,
        INT64,
        FLOAT,
        DOUBLE,
        UNKNOWN
    };

    enum class SlaveType
    {
        Driver,
        PLC,
        Coupler,
        IO
    };

    enum class PDO_Type
    {
        RxPDO,
        TxPDO
    };

    struct ProgramConfig
    {
        uint16_t cyclePeriod;

        std::vector<SlaveInfo> slaveConfigurations;
    };

    struct PDO
    {
        PDO_Type pdoType;

        uint16_t pdoAddress;

        std::vector<PDO_Entry> entries;
    };  

    struct PDO_Entry
    {

        std::string entryName;

        Index index;
        
        Subindex subindex;

        Bitlength bitlength;

        DataType type;
    };

    struct DistributedClockConfig
    {
        uint16_t assignActivate;

        uint32_t sync0Activate;

        int32_t sync0Shift;

        uint32_t sync1Activate;

        int32_t sync1Shift;

    };

    struct SyncManagerConfig
    {
        uint8_t index;
        
        ec_direction_t syncManagerDirection;

        ec_watchdog_mode_t watchdogMode;
    };

    /**
     * @brief Struct that holds all the information about the slave.
     * 
     */
    struct SlaveInfo
    {
        std::string slaveName;

        std::string domainName;

        SlaveType slaveType;

        uint16_t alias;

        uint16_t position;

        uint32_t vendorID;

        uint32_t productCode;
        
        std::vector<PDO> rxPDOs;

        std::vector<PDO> txPDOs;

        std::vector<SyncManagerConfig> syncManagerConfig;

        std::optional<DistributedClockConfig> distributedClockConfig;

        const std::string toString() const
        {
            std::string str;
            
            str += "Slave name: " + slaveName + " \n";
            str += "Alias: " + std::to_string(alias) + " Position: " + std::to_string(position) + "\n";
            str += "Vendor ID: " + std::to_string(vendorID) + " Product Code: " + std::to_string(productCode) + "\n";
            
            str += "RxPDOs: \n";
            for(const auto& rxPDO : rxPDOs)
            {
                str += "PDO Mapping Address: " + std::to_string(rxPDO.pdoAddress) + "\n";
                for(const auto& entry : rxPDO.entries)
                {
                    str += "Entry name: " + entry.entryName +"\n";
                    str += "Index: " + std::to_string(entry.index) + " Subindex: " + std::to_string((uint16_t)entry.subindex) + " Bit length: " +  std::to_string((uint16_t)entry.bitlength) + "\n";
                } 
            }

            str += "TxPDOs: \n";
            for(const auto& txPDO : txPDOs)
            {
                str += "PDO Mapping Address: " + std::to_string(txPDO.pdoAddress) + "\n";
                for(const auto& entry : txPDO.entries)
                {
                    str += "Entry name: " + entry.entryName + "\n";
                    str += "Index: " + std::to_string(entry.index) + " Subindex: " + std::to_string((uint16_t)entry.subindex) + " Bit length: " + std::to_string((uint16_t)entry.bitlength) + "\n";
                } 
            }

            if(distributedClockConfig){
                str += "Distributed Clock configuration: \n";
                const auto dc = distributedClockConfig.value();
                str += "Assign Activate: " + std::to_string(dc.assignActivate) + "\n";
                str += "Sync0 Cycle: " + std::to_string(dc.sync0Activate) + " Sync0 Shift: " + std::to_string(dc.sync0Shift) + "\n";
                str += "Sync1 Cycle: " + std::to_string(dc.sync1Activate) + " Sync1 Shift: " + std::to_string(dc.sync1Shift) + "\n";   
            }

            return str;

        }

    };

    

}

#endif // EC_COMMON_TYPES_HPP_
/**
 * @file parser.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ethercat_interface/parser.hpp"

namespace ec
{
    namespace parser
    {
        std::optional<ProgramConfig> parseConfigFile(const std::string& path_to_config_file)
        {
            const auto configDocs = YAML::LoadAllFromFile(path_to_config_file);
            if(configDocs.empty()){
                return std::nullopt;
            }
            ProgramConfig pConf;

            for(const YAML::Node& doc : configDocs)
            {
                if(const auto& program_config = doc["program_config"])
                {

                    continue;
                }
                
                auto slaveInformationExp = parseSlaveConfig(doc);
                if(slaveInformationExp.has_value()){
                    const auto slaveInformation = slaveInformationExp.value();
                    if(std::holds_alternative<std::vector<SlaveInfo>>(slaveInformation)){
                        const auto& slaveInfoVec = std::get<std::vector<SlaveInfo>>(slaveInformation);
                        for(auto slaveInfoIter= slaveInfoVec.cbegin(); slaveInfoIter != slaveInfoVec.cend(); slaveInfoIter++)
                        {
                            pConf.slaveConfigurations.push_back(std::move(*slaveInfoIter));
                        }   
                    }
                    else if(std::holds_alternative<SlaveInfo>(slaveInformation)){
                        pConf.slaveConfigurations.push_back(std::move(
                            std::get<SlaveInfo>(slaveInformation)
                        ));
                    }
                }
                else
                {
                  continue;
                } 

            }

            return pConf;
        }

        std::optional<SlaveInformation> parseSlaveConfig(const YAML::Node& slave_node)
        {
            SlaveInfo slaveInfo;

            uint16_t slaveCount = slave_node["slave_count"].as<uint16_t>();

            slaveInfo.slaveName = slave_node["slave_name"].as<std::string>();
            
            std::map<std::string, SlaveType>::const_iterator found = slaveTypes.find(slave_node["slave_type"].as<std::string>());
            if(found != slaveTypes.end()){
                slaveInfo.slaveType = found->second;
            }
            else{ 
                return std::nullopt;
            }

            slaveInfo.alias = slave_node["alias"].as<uint16_t>();
            
            slaveInfo.position = slave_node["position"].as<uint16_t>();

            slaveInfo.vendorID = slave_node["vendor_id"].as<uint32_t>();

            slaveInfo.productCode = slave_node["product_code"].as<uint32_t>();

            slaveInfo.domainName = slave_node["domain_name"].as<std::string>();

            if(const auto dc_node = slave_node["dc_config"]){
                DistributedClockConfig dcConfig;
                dcConfig.assignActivate = dc_node["assign_activate"].as<uint16_t>();
                dcConfig.sync0Activate = dc_node["sync0_activate"].as<uint32_t>();
                dcConfig.sync0Shift = dc_node["sync0_shift"].as<int32_t>();
                dcConfig.sync1Activate = dc_node["sync1_activate"].as<uint32_t>();
                dcConfig.sync1Shift = dc_node["sync1_shift"].as<int32_t>();
                slaveInfo.distributedClockConfig = dcConfig;
            }
            else{
                slaveInfo.distributedClockConfig = std::nullopt;
            }

            for(const YAML::Node& sync_manager : slave_node["sync_manager_config"])
            {
                SyncManagerConfig smConfig;

                smConfig.index =  sync_manager["index"].as<uint8_t>();
                const std::string smDir = sync_manager["direction"].as<std::string>();
                if(smDir == "input"){
                    smConfig.syncManagerDirection =  EC_DIR_INPUT;
                }
                else if(smDir == "output"){
                    smConfig.syncManagerDirection =  EC_DIR_OUTPUT;
                }
                else{
                    smConfig.syncManagerDirection =  EC_DIR_INVALID;
                }

                const std::string wd = sync_manager["watchdog_mode"].as<std::string>();
                if(wd == "default"){
                    smConfig.watchdogMode = EC_WD_DEFAULT;
                }
                else if(wd == "enabled"){
                    smConfig.watchdogMode = EC_WD_ENABLE;
                }
                else if(wd == "disabled"){
                    smConfig.watchdogMode = EC_WD_DISABLE;
                }

                slaveInfo.syncManagerConfig.emplace_back(std::move(smConfig));
            }

            constexpr uint16_t maxNumOfPdoMappings = 8;
            
            std::string pdoMappingYamlNodeName = "pdo_mapping_";
             
            for(uint16_t pdoMappingIter = 1; pdoMappingIter <= maxNumOfPdoMappings; pdoMappingIter++)
            {
                pdoMappingYamlNodeName += std::to_string(pdoMappingIter);

                if(YAML::Node pdoMappingNode = slave_node[pdoMappingYamlNodeName]){
                    uint16_t address = pdoMappingNode["addr"].as<uint16_t>();
                    std::string pdoType = pdoMappingNode["type"].as<std::string>();
                    PDO pdo;
                    pdo.pdoAddress = address;
                    for(const YAML::Node& entry : pdoMappingNode["pdos"])
                    {
                        PDO_Entry pdoEntry;
                        pdoEntry.entryName = entry["name"].as<std::string>();
                        pdoEntry.index = entry["index"].as<uint16_t>();
                        pdoEntry.subindex = entry["subindex"].as<uint8_t>();
                        pdoEntry.bitlength = entry["bitlength"].as<uint16_t>();
                        pdoEntry.type = [&entry]() -> DataType {
                        const std::string typeStr = entry["type"].as<std::string>();
                        const auto typeFound = dataTypes.find(typeStr);
                        if(typeFound == dataTypes.end()){
                            return DataType::UNKNOWN;
                        }

                            return typeFound->second;
                        }();
                        pdo.entries.emplace_back(pdoEntry);
                    }

                    if(pdoType == "rx"){
                        pdo.pdoType = PDO_Type::RxPDO;
                        slaveInfo.rxPDOs.emplace_back(pdo);
                    }
                    else if(pdoType == "tx"){
                        pdo.pdoType = PDO_Type::TxPDO;
                        slaveInfo.txPDOs.emplace_back(pdo);
                    }

                    pdoMappingYamlNodeName = "pdo_mapping_";
                }   
                else{
                    break;
                }
            }
            
            if(slaveCount > 1){
                const auto slaveTags = slave_node["slave_tags"].as<std::vector<std::string>>();
                std::vector<SlaveInfo> multipleSlaveInformation;
                int slavePosition = slaveInfo.position - 1;
                for(std::vector<std::string>::const_iterator slaveTagIter = slaveTags.cbegin(); slaveTagIter != slaveTags.cend(); slaveTagIter++)
                {  
                    // Override the slave name with the tag given in the config file
                    SlaveInfo currInfo = slaveInfo;
                    currInfo.slaveName = *slaveTagIter;
                    // Increment the position of the slaves in the bus.
                    currInfo.position = slavePosition + 1;
                    slavePosition += 1;
                    // Leave the rest of the parameters untouched
                    multipleSlaveInformation.push_back(currInfo);
                } 

                if(slaveTags.size() != multipleSlaveInformation.size()){
                    return std::nullopt;
                }
                
                return multipleSlaveInformation;
            }

            return slaveInfo;

        }

    } // End of namespace parser
} // End of namespace ec
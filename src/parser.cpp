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
                
                auto inf = parseSlaveConfig(doc);
                if(inf){
                    pConf.slaveConfigurations.emplace_back(std::move(inf.value()));
                }
            }

            return pConf;
        }

        std::optional<SlaveInfo> parseSlaveConfig(const YAML::Node& slave_node)
        {
            SlaveInfo slaveInfo;

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
                else if(wd == "enable"){
                    smConfig.watchdogMode = EC_WD_ENABLE;
                }
                else if(wd == "disable"){
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
                    for(const YAML::Node entry : pdoMappingNode["pdos"])
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
            /* if(const YAML::Node& pdos_node = slave_node["pdos"]){

                std::vector<PDO> rxPDOs;
                std::vector<PDO> txPDOs;

                std::unique_ptr<PDO> tempPDO;
                for(const YAML::Node& pdo_info : pdos_node)
                {
                    if(const auto& pdoAddress = pdo_info["address"]){
                        if(tempPDO){
                            if(tempPDO->pdoType == PDO_Type::RxPDO){
                                PDO pdo = *tempPDO;
                                rxPDOs.push_back(pdo);
                            }
                            else if(tempPDO->pdoType == PDO_Type::TxPDO){
                                PDO pdo = *tempPDO;
                                txPDOs.push_back(pdo);
                            }
                            tempPDO.reset();        
                        }
                        tempPDO = std::make_unique<PDO>();
                        tempPDO->pdoAddress = pdoAddress.as<uint16_t>();

                        const std::string pdoTypeStr = pdo_info["pdo_type"].as<std::string>();
                        if(pdoTypeStr == "rx"){
                            tempPDO->pdoType = PDO_Type::RxPDO;
                        }
                        else if(pdoTypeStr == "tx"){
                            tempPDO->pdoType = PDO_Type::TxPDO;
                            
                        }
                        continue; 
                    }

                    PDO_Entry entry;
                    entry.entryName = pdo_info["name"].as<std::string>();
                    entry.index = pdo_info["index"].as<uint16_t>();
                    entry.subindex = pdo_info["subindex"].as<uint8_t>();
                    entry.bitlength = pdo_info["bitlength"].as<uint8_t>();

                    entry.type = [&pdo_info]() -> DataType {
                        const std::string typeStr = pdo_info["type"].as<std::string>();
                        const auto typeFound = dataTypes.find(typeStr);
                        if(typeFound == dataTypes.end()){
                            return DataType::UNKNOWN;
                        }

                        return typeFound->second;
                    }();

                    if(tempPDO){
                        tempPDO->entries.push_back(entry);
                    }
                    
                }
                if(tempPDO){
                    if(tempPDO){
                    if(tempPDO->pdoType == PDO_Type::RxPDO){
                        PDO pdo = *tempPDO;
                        rxPDOs.push_back(pdo);
                    }
                    else if(tempPDO->pdoType == PDO_Type::TxPDO){
                        PDO pdo = *tempPDO;
                        txPDOs.push_back(pdo);
                    }
                    tempPDO.reset();        
                }
                }
                slaveInfo.rxPDOs = rxPDOs;
                slaveInfo.txPDOs = txPDOs;
            } */
            
            return slaveInfo;

        }

    } // End of namespace parser
} // End of namespace ec
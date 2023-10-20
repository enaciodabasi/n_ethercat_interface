/**
 * @file master.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef MASTER_HPP_
#define MASTER_HPP_

#include <map>
#include <vector>
#include <memory>
#include <functional>

#include "ec_common_defs.hpp"
#include "comm_interface.hpp"
#include "slave.hpp"
#include "parser.hpp"
#include "time_operations.hpp"

using namespace ec::slave;

/**
 * @brief 
 * 
 */
using SharedData = std::shared_ptr<std::map<std::string, std::shared_ptr<ec::data::DataMap>>>;

typedef std::map<std::string, Slave*> Slaves;

using CommunicationInterfacePtr = CommunicationInterface*;

struct Domain
{
    ec_domain_t* domainPtr;
    uint8_t* domainDataPtr;
    std::vector<std::string> domainSlaves;
    ec_pdo_entry_reg_t* domainEntries;

    Domain();
    ~Domain();

    bool registerPDOs();
    bool createDomainData();
};

class Master
{

    public:

    typedef std::function<void(void)> UpdateFunction;

    /**
     * @brief Default constructor.
     * 
     */
    Master();

    /**
     * @brief Constructs a Master object and sets the configuration file path for the program.
     * 
     * @param config_file_path 
     */
    Master(const std::string& config_file_path);

    ~Master();

    /**
     * @brief Initializes the EtherCAT master with the given program and slave configurations.
     * 
     * @return true If no error occurs
     * @return false otherwise
     */
    bool init();

    void update();

    /**
     * @brief Sets the string that is used to fetch the configuration file
     * 
     * @param config_file_path Absolute path to the configuration file.
     */
    inline void setConfigFilePath(const std::string& config_file_path)
    {
        m_PathToConfigurationFile = config_file_path;
    }

    void setUpdateFunction(UpdateFunction update_function);

    void setCommunicationInterface(CommunicationInterface* interface);

    /**
     * @brief Get the Slave object pointer in it's derived pointer format. 
     * This enables to use the derived-slave spesific methods in the user code.
     * 
     * @tparam T Type of the slave: Driver*, IO* etc.
     * @param slave_name Name of the slave
     * @return std::optional<T> 
     */
    template<class T>
    std::optional<T> getSlave(const std::string& slave_name)
    {
        auto slaveExists = m_RegisteredSlaves.find(slave_name);
        if(slaveExists == m_RegisteredSlaves.end()){
            return std::nullopt;
        }

        return dynamic_cast<T>(slaveExists->second);
    }

    /**
     * @brief Sets the specified data inside the shared data map
     * 
     * @tparam T Type of the data to be set
     * @param slave_name Name of the slave to set the data for
     * @param data_name Name of the data to be set
     * @param data_value Value to be set
     * @return true 
     * @return false 
     */
    template<typename T>
    bool setSharedData(
        const std::string& slave_name,
        const std::string& data_name, 
        T data_value
    )
    {
        if( auto slaveFound = m_SharedData->find(slave_name); 
            slaveFound != m_SharedData->end()){
            return slaveFound->second->set(data_name, data_value);
        }
        
        return false;
    }

    /**
     * @brief Gets the specified data from the shared data map
     * 
     * @tparam T Type of the data to get
     * @param slave_name Name of the slave to get the data from
     * @param data_name Name of the data to get
     * @return const std::optional<T> 
     */
    template<typename T>
    const std::optional<T> getSharedData(
        const std::string& slave_name,
        const std::string& data_name
    )
    {
        if( auto slaveFound = m_SharedData->find(slave_name);
            slaveFound != m_SharedData->end()){
            return slaveFound->second->get<T>(data_name);
        }

        return std::nullopt;
    }

    ///**
    // * @brief 
    // * 
    // * @tparam T: any primitive data type 
    // * @param slave_name 
    // * @param data_name 
    // * @param val 
    // * @return true 
    // * @return false 
    // */
    //template<typename T>
    //bool write(const std::string& slave_name, const std::string& data_name, const T& val)
    //{
    //    m_RegisteredSlaves.at(slave_name);
    //    
    //    return true;
    //}
    //
    ///**
    // * @brief 
    // * 
    // * @tparam T: any primitive data type 
    // * @param slave_name: Name of the slave to read from. 
    // * @param data_name: Name of the registered PDO entry to read.
    // * @return std::optional<T>, std::nullopt if any errors are occured, otherwise a T value. 
    // */
    //template<typename T>
    //std::optional<T> read(const std::string& slave_name, const std::string& data_name)
    //{
//
    //}

    void receive();

    void send();

    bool receiveDomainData(const std::string& domain_name);

    bool sendDomainData(const std::string& domain_name);

    private:

    std::string m_PathToConfigurationFile;

    /**
     * @brief Communication medium to use
     * 
     */

    CommunicationInterfacePtr m_CommunicationInterface;

    ec_master_t* m_MasterPtr;

    std::unordered_map<std::string, Domain> m_Domains;

    /**
     * @brief Update function object to call inside the update function.
     * 
     */
    UpdateFunction m_UpdateFunction;

    Slaves m_RegisteredSlaves;

    SharedData m_SharedData;

    ec::ProgramConfig m_ProgramConfiguration;
    
    std::unique_ptr<CyclicTaskTimer> m_TaskTimer;
    bool m_IsDistributedClockEnabled = false;

    /**
     * @brief Register a slave in the map
     * 
     * @param slave: ec::slave::Slave object instance
     * @return true if slave is added to the map successfully 
     * @return false if slave can't be added to the map
     */
    bool registerSlave(Slave& slave);

    /**
     * @brief Registers the slaves specified in the configuration file.
     * 
     * @return true If all slaves are added to the map.
     * @return false If a slave can't be added to the map.
     */
    bool registerSlaves();

    bool createDomains();

    bool initSlaves();

    bool registerDomainEntries();
    
};

#endif 
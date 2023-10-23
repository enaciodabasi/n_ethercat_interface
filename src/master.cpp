/**
 * @file master.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ethercat_interface/master.hpp"

using namespace ec;

Domain::Domain()
{
    domainPtr = nullptr;
    domainDataPtr = nullptr;
    domainEntries = nullptr;
}

Domain::~Domain()
{

}

bool Domain::registerPDOs()
{   
    if(!domainEntries){
        std::cout << "Domain entries is not initialized\n";
        return false;
    }

    if(!domainPtr){
        std::cout << "Domain pointer is not initialized\n";
        return false;
    }
    return ecrt_domain_reg_pdo_entry_list(domainPtr, domainEntries);
}

bool Domain::createDomainData()
{
    domainDataPtr = ecrt_domain_data(domainPtr);
    if(!domainDataPtr){
        return false;
    }
    return true;
}

Master::Master()
{

}

Master::Master(const std::string& config_file_path)
    : m_PathToConfigurationFile(config_file_path)
{

}


Master::~Master()
{
    ecrt_master_deactivate_slaves(m_MasterPtr);
    ecrt_master_deactivate(m_MasterPtr);
}

bool Master::init()
{
    bool initOK = true;

    m_MasterPtr = ecrt_request_master(0);
    if(!m_MasterPtr){
        return false;
    }
    //std::cout << "Requested master\n";

    if(m_PathToConfigurationFile.empty()){
        return false;
    }

    //std::cout << "Got config file\n";

    const auto programConfigOpt = ec::parser::parseConfigFile(m_PathToConfigurationFile);
    if(!programConfigOpt){
        return false;
    }

    //std::cout << "Parsed config file\n";

    m_ProgramConfiguration = programConfigOpt.value();


    initOK = registerSlaves();
    if(!initOK){
        return false;
    }
    
    if(m_RegisteredSlaves.empty()){
        return false;
    }

    //std::cout << "Registered slaves\n";

    bool isDcEnabledForAnyOfTheSlaves = [&slaves = m_ProgramConfiguration.slaveConfigurations]() -> bool {
        bool res = false;
        for(auto slaveIter = slaves.cbegin(); slaveIter != slaves.cend(); slaveIter++)
        {
            if((*slaveIter).distributedClockConfig != std::nullopt){
                res = true;
                break;
            }
        }
        return res;
    }();
    
    if(isDcEnabledForAnyOfTheSlaves){
        m_IsDistributedClockEnabled = true;
        m_TaskTimer = std::make_unique<CyclicTaskTimerDC>();
    }
    else if(m_ProgramConfiguration.cyclePeriod != 0){
        m_TaskTimer = std::make_unique<CyclicTaskTimer>();
    }


    initOK = createDomains();
    if(!initOK){
        return false;
    }

    //std::cout << "Created domains\n";

    initOK = initSlaves();
    if(!initOK){
        return false;
    }
    
    //std::cout << "Initialized slaves\n";

    initOK = registerDomainEntries();

    //std::cout << "Registered domain entries\n";

    bool isRegisteringPDOsOk = true;
    for(auto& [name, domain] : m_Domains)
    {   
        //std::cout << "Registering: " << name << std::endl;
        if(!domain.domainPtr){
            //std::cout << "Domain pointer is nullptr\n";
            break;
        }
        if(domain.registerPDOs()){
            isRegisteringPDOsOk = false;
            break;
        }
    }

    if(!isRegisteringPDOsOk){
        // LOG
        return isRegisteringPDOsOk;
    }

    //std::cout << "Registered PDOs\n";

    if(ecrt_master_activate(m_MasterPtr) < 0){
        // LOG
        return false;
    }

    bool creatingDomainDataOk = true;
    for(auto& [name, domain] : m_Domains)
    {
        if(!domain.createDomainData()){
            creatingDomainDataOk = false;
            break;
        }
    }

    if(!creatingDomainDataOk){
        // LOG
        return creatingDomainDataOk;
    }

    for(auto domainMapIter = m_Domains.begin(); domainMapIter != m_Domains.end(); domainMapIter++)
    {
        for(const auto& str : (*domainMapIter).second.domainSlaves)
        {   
            auto foundSlave = m_RegisteredSlaves.find(str);
            if(foundSlave == m_RegisteredSlaves.end()){
                continue;
            }

            (*foundSlave).second->setDomainDataPtr((*domainMapIter).second.domainDataPtr);
        }
    }

    //std::cout << "Created domain data\n";

    return initOK;

}

void Master::update()
{   

    if(m_UpdateFunction){
        
        if(m_CommunicationInterface){
            
        }

        m_UpdateFunction();

    }
    else{

        return;
    }


}

void Master::setUpdateFunction(UpdateFunction update_function)
{
    m_UpdateFunction = std::move(update_function);
}

void Master::setCommunicationInterface(CommunicationInterface* interface)
{
    m_CommunicationInterface = interface;
}

bool Master::registerSlave(Slave& slave)
{
    const std::string slaveName = slave.getSlaveInfo().slaveName;
    const auto slaveFound = m_RegisteredSlaves.find(slaveName);
    if(slaveFound != m_RegisteredSlaves.end()){
        return false;
    }
    m_RegisteredSlaves[slaveName] = new Slave(std::move(slave));
    return true;
}

bool Master::registerSlaves()
{

    const std::size_t numOfSlaves = m_ProgramConfiguration.slaveConfigurations.size();
    bool slavesRegistered = true;

    for(std::vector<SlaveInfo>::const_iterator slaveIter = m_ProgramConfiguration.slaveConfigurations.begin(); slaveIter < m_ProgramConfiguration.slaveConfigurations.cend(); slaveIter++)
    {
        const auto& slaveConfig = *slaveIter;
        const auto slaveType = slaveConfig.slaveType;
        switch (slaveType)
        {
        case SlaveType::Driver :
        {
            slave::Driver driverSlave(slaveConfig);
            slavesRegistered = registerSlave(driverSlave);
            break;
        }
        case SlaveType::IO :
        {
            slave::IO ioSlave(slaveConfig);
            slavesRegistered = registerSlave(ioSlave);
            break;
        }
        case SlaveType::PLC :
        {
            slave::PLC plcSlave(slaveConfig);
            slavesRegistered = registerSlave(plcSlave); 
            break;
        }
        case SlaveType::Coupler :
        {
            slave::Coupler couplerSlave(slaveConfig);
            slavesRegistered = registerSlave(couplerSlave);
            break;
        }
        default:
            slavesRegistered = false;
            break;
        }

    }

    return slavesRegistered;
}

bool Master::createDomains()
{
    bool domainsCreated = true;

    for(auto const & [name, slave] : m_RegisteredSlaves)
    {
        const auto domainNameOfSlave = slave->getSlaveInfo().domainName;
        if(domainNameOfSlave.empty()){
            domainsCreated = false;
            break;
        }
        
        // Check if domain exists
        if(m_Domains.find(domainNameOfSlave) == m_Domains.end()){
            // If not, create a new Domain struct and EtherCAT domain pointer.
            m_Domains[domainNameOfSlave] = Domain();
            auto& currentDomain = m_Domains.at(domainNameOfSlave);
            currentDomain.domainPtr = ecrt_master_create_domain(this->m_MasterPtr);
            // Check domain pointer
            currentDomain.domainSlaves.push_back(name);

            break;
        }
        // If domain already exists, just add the slave name to its domainSlaves vector:
        auto& currentDomain = m_Domains.at(domainNameOfSlave);
        currentDomain.domainSlaves.push_back(name);

    }

    return domainsCreated;
}

bool Master::initSlaves()
{

    bool slavesInitiliazed = true;

    for(auto& [name, slave] : m_RegisteredSlaves)
    {
        const auto slavesDomainName = slave->getSlaveInfo().domainName;
        auto& currentDomain = m_Domains.at(slavesDomainName);
        slavesInitiliazed = slave->init(this->m_MasterPtr, currentDomain.domainPtr);
    }

    return slavesInitiliazed;
}

bool Master::registerDomainEntries()
{   

    auto deduceDomainSize = [this](const Domain& domain) -> std::size_t{
        const auto& domainSlaves = domain.domainSlaves;
        std::size_t domainSize = 0;
        for(std::vector<std::string>::const_iterator slaveNameIter = domainSlaves.cbegin(); slaveNameIter < domainSlaves.cend(); slaveNameIter++){
            const auto currentSlaveInfo = this->m_RegisteredSlaves.at(*slaveNameIter)->getSlaveInfo();
            
            for(const auto& pdoMapping : currentSlaveInfo.rxPDOs)
            {
                domainSize += pdoMapping.entries.size();
            }

            for(const auto& pdoMapping : currentSlaveInfo.txPDOs)
            {
                domainSize += pdoMapping.entries.size();
            }
            //domainSize += (currentSlaveInfo.rxPDOs.size() + currentSlaveInfo.txPDOs.size());
        }

        return domainSize;
    };

    bool registerOK = true;

    for(auto & [name, domain] : m_Domains)
    {

        std::size_t currentDomainEntrySize = deduceDomainSize(domain);
        std::cout << "Number of PDOs to register for the domain: " << currentDomainEntrySize << std::endl;
        domain.domainEntries = new ec_pdo_entry_reg_t[currentDomainEntrySize + 1]; // Plus one is for the empty struct at the end of the pointer.

        std:size_t entryIteration = 0;
        for(const std::string slaveName : domain.domainSlaves)
        {
            auto& currentSlave = m_RegisteredSlaves.at(slaveName);
            const auto currentSlaveInfo = currentSlave->getSlaveInfo();
            for(const auto rxpdo : currentSlaveInfo.rxPDOs)
            {
                for(const auto& entry : rxpdo.entries)
                {

                    auto entryOffsetPtr = currentSlave->getOffsetPtr(entry.entryName);
                    if(!entryOffsetPtr){
                        registerOK = false;
                        // LOG:
                        std::cout << "Can't get offset\n";
                        break;
                    }
                    ec_pdo_entry_reg_t entryReg;
                    entryReg.alias = currentSlaveInfo.alias;
                    entryReg.position = currentSlaveInfo.position;
                    entryReg.vendor_id = currentSlaveInfo.vendorID;
                    entryReg.product_code = currentSlaveInfo.productCode;
                    entryReg.index = entry.index;
                    entryReg.subindex = entry.subindex;
                    entryReg.offset = entryOffsetPtr.value();
                    domain.domainEntries[entryIteration] = entryReg;
                    entryIteration += 1;
                    
                }
                if(!registerOK){
                    break;
                }
            }
            if(!registerOK){
                break;
            }

            for(const auto txpdo : currentSlaveInfo.txPDOs)
            {
                for(const auto entry : txpdo.entries)
                {   
                    auto entryOffsetPtr = currentSlave->getOffsetPtr(entry.entryName);
                    if(!entryOffsetPtr){
                        registerOK = false;
                        // LOG:
                        break;
                    }
                    ec_pdo_entry_reg_t entryReg;
                    entryReg.alias = currentSlaveInfo.alias;
                    entryReg.position = currentSlaveInfo.position;
                    entryReg.vendor_id = currentSlaveInfo.vendorID;
                    entryReg.product_code = currentSlaveInfo.productCode;
                    entryReg.index = entry.index;
                    entryReg.subindex = entry.subindex;
                    entryReg.offset = entryOffsetPtr.value();
                    domain.domainEntries[entryIteration] = entryReg;
                    entryIteration += 1;
                    
                }
                if(!registerOK){
                    break;
                }
            }
            if(!registerOK){
                break;
            }
        }
        if(!registerOK){
            break;
        }

        domain.domainEntries[currentDomainEntrySize] = {};
    }

    return registerOK;
}

void Master::receive()
{
    if(m_IsDistributedClockEnabled){
        CyclicTaskTimerDC* tempDcTimer = dynamic_cast<CyclicTaskTimerDC*>(m_TaskTimer.get());     
        tempDcTimer->writeAppTimeToMaster(m_MasterPtr);
        delete tempDcTimer;
    }

    ecrt_master_receive(m_MasterPtr);
    
}

void Master::send()
{
    if(m_IsDistributedClockEnabled){
        CyclicTaskTimerDC* tempDcTimer = dynamic_cast<CyclicTaskTimerDC*>(m_TaskTimer.get());
        tempDcTimer->syncReferenceClock(m_MasterPtr);
        tempDcTimer->syncSlaveClocks(m_MasterPtr);
        delete tempDcTimer;
    }

    ecrt_master_send(m_MasterPtr);
}

bool Master::receiveDomainData(const std::string& domain_name)
{
    auto domainFound = m_Domains.find(domain_name);
    if(domainFound == m_Domains.end()){
        // TODO: Log
        return false;
    }

    ecrt_domain_process(domainFound->second.domainPtr);

    return true;
}

bool Master::sendDomainData(const std::string& domain_name)
{
    auto domainFound = m_Domains.find(domain_name);
    if(domainFound == m_Domains.end()){
        // TODO: Log
        return false;
    }

    ecrt_domain_queue(domainFound->second.domainPtr);

    return true;
}


#include "ethercat_interface/ethercat_interface.hpp"
#include <gtest/gtest.h>
#include "ecrt.h"

#include <iostream>

namespace {
class SingleSlaveTest : public ::testing::Test
{
    protected:

    SingleSlaveTest(){
        auto user = std::getenv("USER");
        configFilePath = "/home/naci/ethercat_interface/test/single_slave_test/single_slave_test.yaml";
    }

    void SetUp() override{
        programConfig = ec::parser::parseConfigFile(configFilePath);
    }

    std::string configFilePath;
    std::optional<ec::ProgramConfig> programConfig;

    std::unique_ptr<Master> m_Master;
    
};

TEST_F(SingleSlaveTest, IsParseSuccessful)
{
    ASSERT_NE(programConfig, std::nullopt);

    const auto conf = programConfig.value();
    std::size_t numOfSlaves = conf.slaveConfigurations.size();
    EXPECT_EQ(numOfSlaves, 1);

    auto dcConfig = conf.slaveConfigurations.at(0).distributedClockConfig;
    EXPECT_NE(dcConfig, std::nullopt);

    std::size_t rxPdoCount = conf.slaveConfigurations.at(0).rxPDOs.size();

    EXPECT_EQ(rxPdoCount, 1);

    std::size_t txPdoCount = conf.slaveConfigurations.at(0).txPDOs.size();

    EXPECT_EQ(txPdoCount, 1);

    std::cout << conf.slaveConfigurations.at(0).toString() << std::endl;
    
}

//TEST_F(SingleSlaveTest, SlaveConfigurationFunctionsWork)
//{
//
//    ASSERT_NE(programConfig, std::nullopt);
//
//    ec_master_t* masterPtr = ecrt_request_master(0);
//    ASSERT_NE(masterPtr, nullptr);
//
//    ec_domain_t* domainPtr = ecrt_master_create_domain(masterPtr);
//    ASSERT_NE(domainPtr, nullptr);
//
//    const auto slave0Config = programConfig.value().slaveConfigurations.at(0);
//    ec::slave::Slave slave0(slave0Config);
//    bool slaveInitOk = slave0.init(masterPtr, domainPtr);
//    
//    ASSERT_EQ(slaveInitOk, true);
//
//    ecrt_release_master(masterPtr);
//    
//}

TEST_F(SingleSlaveTest, MasterWorks)
{

    m_Master = std::make_unique<Master>(configFilePath);

    bool masterInitOk = m_Master->init();

    ASSERT_EQ(masterInitOk, true);

}

}
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
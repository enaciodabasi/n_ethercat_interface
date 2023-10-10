
#include "ethercat_interface/ethercat_interface.hpp"
#include <gtest/gtest.h>

#include <iostream>

namespace {
class SingleSlaveTest : public ::testing::Test
{
    protected:

    SingleSlaveTest(){
        auto user = std::getenv("USER");
        configFilePath = "/home/" + std::string(user) + "/ethercat_interface/test/single_slave_test/single_slave_test.yaml";
    }

    void SetUp() override{
        programConfig = ec::parser::parseConfigFile(configFilePath);
    }

    std::string configFilePath;
    std::optional<ec::ProgramConfig> programConfig;
    
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

    EXPECT_EQ(rxPdoCount, 2);

    std::size_t txPdoCount = conf.slaveConfigurations.at(0).txPDOs.size();

    EXPECT_EQ(txPdoCount, 2);

    conf.slaveConfigurations.at(0).toString();

}
}
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
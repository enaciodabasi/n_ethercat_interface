#include "ethercat_interface/ethercat_interface.hpp"
#include <gtest/gtest.h>

#include <iostream>

namespace {
class MultipleIdenticalSlavesTest : public ::testing::Test
{
    protected:

    MultipleIdenticalSlavesTest(){
        auto user = std::getenv("USER");
        configFilePath = "/home/" + std::string(user) + "/ethercat_interface/test/multiple_identical_slaves_test/multiple_identical_slaves_test.yaml";
    }

    void SetUp() override{
        programConfig = ec::parser::parseConfigFile(configFilePath);
    }

    std::string configFilePath;
    std::optional<ec::ProgramConfig> programConfig;
    
};

TEST_F(MultipleIdenticalSlavesTest, IsParseSuccessful)
{
    
    ASSERT_NE(programConfig, std::nullopt);

    const auto conf = programConfig.value();
    std::size_t numOfSlaves = conf.slaveConfigurations.size();
    EXPECT_EQ(numOfSlaves, 3);

    const std::vector<std::string> names = {"sag_teker", "sol_teker", "lifter_motor"};
    
    EXPECT_EQ(conf.slaveConfigurations.at(0).slaveName, names.at(0));
    EXPECT_EQ(conf.slaveConfigurations.at(1).slaveName, names.at(1));
    EXPECT_EQ(conf.slaveConfigurations.at(2).slaveName, names.at(2));

    EXPECT_EQ(conf.slaveConfigurations.at(0).position, 1);
    EXPECT_EQ(conf.slaveConfigurations.at(1).position, 2);
    EXPECT_EQ(conf.slaveConfigurations.at(2).position, 3);

    for(const auto& currConf : conf.slaveConfigurations)
    {
        std::cout << currConf.toString() << std::endl;
    }

}
}
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
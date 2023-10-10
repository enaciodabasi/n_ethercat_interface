#include "ethercat_interface/ethercat_interface.hpp"
#include <gtest/gtest.h>

#include <iostream>

namespace {
class MultipleIdenticalSlavesTest : public ::testing::Test
{
    protected:

    MultipleIdenticalSlavesTest(){
        auto user = std::getenv("USER");
        configFilePath = "/home/" + std::string(user) + "/ethercat_interface/test/single_slave_test/single_slave_test.yaml";
    }

    void SetUp() override{
        programConfig = ec::parser::parseConfigFile(configFilePath);
    }

    std::string configFilePath;
    std::optional<ec::ProgramConfig> programConfig;
    
};

TEST_F(MultipleIdenticalSlavesTest, IsParseSuccessful)
{
    

}
}
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
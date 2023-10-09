/**
 * @file test.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <gtest/gtest.h>

#include "ethercat_interface/parser.hpp"

using namespace ec::parser;

const std::string configFilePath = "/home/naci/ethercat_interface/test/parser-test/test_config.yaml";

TEST(
    SingleSlaveConfigFileParserTest, SuccessfulParse 
){
    ASSERT_NE(std::nullopt, parseConfigFile(configFilePath));
    ASSERT_EQ(4, parseConfigFile(configFilePath)->slaveConfigurations.at(0).rxPDOs.at(0).entries.size());
    ASSERT_EQ(4, parseConfigFile(configFilePath)->slaveConfigurations.at(0).txPDOs.at(0).entries.size());
}

/* class SingleSlaveConfigFileParserTest : public ::testing::Test
{
    protected:

    virtual void SetUp()
    {

    }

    virtual void TearDown()
    {

    }
}; */


int main(int argc, char** argv)
{
    
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

}
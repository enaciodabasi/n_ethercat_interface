
#include "ethercat_interface/ethercat_interface.hpp"
#include <gtest/gtest.h>
#include <iostream>

using namespace ec;


namespace {
class SharedDataTest : public ::testing::Test
{
    protected:
    SharedDataTest(){
        
    }
    ~SharedDataTest(){}
    void SetUp() override{
        m_SharedDataMap = std::make_shared<data::DataMap>(pdos);
    }

    const std::vector<PDO_Entry> pdos = {
        {"control_word", 0x6040, 0x0, 16, DataType::UINT16},
        {"op_mode", 0x6060, 0x0, 8, DataType::UINT8},
        {"target_position", 0x607A, 0x0, 32, DataType::INT32},
        {"target_velocity", 0x60FF, 0x0, 32, DataType::INT32},
        {"status_word", 0x6041, 0x0, 16, DataType::UINT16},
        {"actual_position", 0x6064, 0x0, 32, DataType::INT32},
        {"actual_velocity", 0x606C, 0x0, 32, DataType::INT32},
    };  


    std::shared_ptr<data::DataMap> m_SharedDataMap;

    public:

};

TEST_F(SharedDataTest, SameThreadDataTransferTest)
{   

    std::cout << "Initializing the Shared Data Map" << std::endl;
    ASSERT_EQ(m_SharedDataMap->init(), true);

    uint16_t ctrlWord = 0xF;

    bool writeToControlWord = m_SharedDataMap->set("control_word", ctrlWord);
    EXPECT_EQ(writeToControlWord, true);

    std::cout << "Written control word: " << ctrlWord << std::endl;

    auto writtenControlWord = m_SharedDataMap->get<uint16_t>("control_word");
    EXPECT_NE(writtenControlWord, std::nullopt);
    
    if(writtenControlWord){
        EXPECT_EQ(writtenControlWord.value(), ctrlWord);
        std::cout << std::hex << "Written: " << ctrlWord << " | Read: " << writtenControlWord.value() << std::endl; 
    }

    /* // Check if the pop operation on the Data Queue is successful.
    auto optCtrlWord = m_SharedDataMap->get<uint16_t>("control_word");
    EXPECT_EQ(optCtrlWord, std::nullopt); */

}

TEST_F(SharedDataTest, MultipleThreadDataTransferTest)
{

}

}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
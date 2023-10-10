/**
 * @file comm_interface.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ethercat_interface/comm_interface.hpp"

CommunicationInterface::CommunicationInterface()
{

}

CommunicationInterface::~CommunicationInterface()
{
    
}

void CommunicationInterface::setSharedDataPtr(SharedData& shared_data_ptr)
{
    m_SharedDataPtr = shared_data_ptr;
}

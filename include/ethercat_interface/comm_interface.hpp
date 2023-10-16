/**
 * @file comm_medium.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef COMM_INTERFACE_HPP_
#define COMM_INTERFACE_HPP_

#include <memory>
#include "data.hpp"

typedef std::shared_ptr<std::map<std::string, std::shared_ptr<ec::data::DataMap>>> SharedData;

class CommunicationInterface
{
    public:

    CommunicationInterface();
    
    virtual ~CommunicationInterface();

    void setSharedDataPtr(SharedData& shared_data_ptr);

    virtual bool init() = 0; 

    virtual void run() = 0;

    protected:

    SharedData m_SharedDataPtr;

};


#endif // COMM_INTERFACE_HPP_
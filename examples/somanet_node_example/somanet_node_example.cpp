/**
 * @file somanet_node_example.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ethercat_interface/ethercat_interface.hpp"

class CommunicationInterfaceROS : public CommunicationInterface
{
    public:

    CommunicationInterfaceROS()
    {

    }

    ~CommunicationInterfaceROS()
    {

    }

    bool init() override;

    void run() override;

    private:

};

int main(int argc, char** argv)
{

    std::unique_ptr<Master> master = std::make_unique<Master>("");
    if(!master->init()){
        
        return -1;
    }
    auto updateFuncion = [&master](){

        auto optLeftMotor = master->getSlave<DriverPtr>("left_motor");
        auto optRightMotor = master->getSlave<DriverPtr>("right_motor");
        auto optLifterMotor = master->getSlave<DriverPtr>("lifter_motor");
        
        if(optLeftMotor || optRightMotor || optLifterMotor){
            return;
        }

        auto leftMotor = optLeftMotor.value();
        auto rightMotor = optRightMotor.value();
        auto lifterMotor = optLifterMotor.value();
        
        while(true)
        {

            // ******************************
                // EtherCAT loop logic:
            // ******************************

            auto leftMotorVel = leftMotor->read<int32_t>("actual_velocity");
            

            /* auto optTargetVel = master->getSharedData<int32_t>("left_motor", "target_velocity");
            if(optTargetVel){
                leftMotor->write("target_velocity", optTargetVel.value());
            } */
        }


    };

    master->setUpdateFunction(updateFuncion);
    master->update();

    return 0;

}
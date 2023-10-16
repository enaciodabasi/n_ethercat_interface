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
    bool masterInitOk = master->init();
    auto updateFuncion = [&master](){

        auto optLeftMotor = master->getSlave<Driver*>("left_motor");
        auto optRightMotor = master->getSlave<Driver*>("right_motor");
        auto optLifterMotor = master->getSlave<Driver*>("lifter_motor");
        
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

            

        }

    };

    master->setUpdateFunction(updateFuncion);
    master->update();

}
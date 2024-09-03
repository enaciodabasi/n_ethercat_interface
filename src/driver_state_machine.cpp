/**
 * @file driver_state_machine.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-11-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ethercat_interface/driver/driver_state_machine.hpp"

::StateMachine::StateMachine() {}

::StateMachine::~StateMachine() {}

namespace CIA402
{

    StateMachine::StateMachine()
        : ::StateMachine()
    {
    }

    StateMachine::~StateMachine()
    {
    }

    bool StateMachine::addTransition(
        const State first_state,
        const State second_state,
        const uint16_t set_bits,
        const uint16_t reset_bits)
    {
        return addTransition(
            std::make_pair(first_state, second_state),
            std::make_pair(set_bits, reset_bits));
    }

    bool StateMachine::addTransition(
        const Transition &transitions,
        const TransitionCommand &transition_command)
    {
        m_TransitionMap[transitions] = transition_command;

        return true;
    }

    bool StateMachine::init()
    {

        using namespace state_transition_commands;
        // 0:
        addTransition(
            State::Start, State::NotReadyToSwitchOn,
            autoSet, autoReset);
        // 1:
        addTransition(
            State::NotReadyToSwitchOn, State::SwitchOnDisabled,
            autoSet, autoReset);
        // 2:
        addTransition(
            State::SwitchedOn, State::ReadyToSwitchOn,
            shutdownSet, shutdownReset);
        // 3:
        addTransition(
            State::ReadyToSwitchOn, State::SwitchedOn,
            switchOnSet, switchOnReset);
        // 4:
        addTransition(
            State::SwitchedOn, State::OperationEnabled,
            enableOperationSet, enableOperationReset);
        // 5:
        addTransition(
            State::OperationEnabled, State::SwitchedOn,
            disableOperationSet, disableOperationReset);
        // 6:
        addTransition(
            State::SwitchedOn, State::ReadyToSwitchOn,
            shutdownSet, shutdownReset);
        // 7:
        addTransition(
            State::ReadyToSwitchOn, State::SwitchOnDisabled,
            disableVoltageSet, disableVoltageReset);
        // 8:
        addTransition(
            State::SwitchedOn, State::ReadyToSwitchOn,
            shutdownSet, shutdownReset);
        // 9:
        addTransition(
            State::SwitchedOn, State::ReadyToSwitchOn,
            disableVoltageSet, disableVoltageSet);
        // 10:
        addTransition(
            State::SwitchedOn, State::ReadyToSwitchOn,
            disableVoltageSet, disableVoltageReset);
        // 11:
        addTransition(
            State::OperationEnabled, State::QuickStopActive,
            quickStopSet, quickStopReset);
        // 12:
        addTransition(
            State::QuickStopActive, State::SwitchOnDisabled,
            disableVoltageSet, disableVoltageReset);
        // 13:
        addTransition(
            State::Unknown, State::FaultReactionActive,
            autoSet, autoReset);
        // 14:
        addTransition(
            State::FaultReactionActive, State::Fault,
            autoSet, autoReset);
        // 15:
        addTransition(
            State::Fault, State::SwitchOnDisabled,
            faultResetSet, faultResetReset);
        // 16:
        addTransition(
            State::QuickStopActive, State::OperationEnabled,
            enableOperationSet, enableOperationReset);
    }

    bool StateMachine::findCurrentState(const uint16_t &current_status_word)
    {
        const uint16_t cleanState = current_status_word & stateCheckBits;
        State currentState = State::Unknown;

        if (
            cleanState == (0 | 0 | 0 | 0 | 0 | 0) ||
            cleanState == (0 | importantStateBits[4] | 0 | 0 | 0 | 0))
        {
            currentState = State::NotReadyToSwitchOn;
        }
        else if (
            cleanState == (importantStateBits[5] | 0 | 0 | 0 | 0 | 0) ||
            cleanState == (importantStateBits[5] | importantStateBits[4] | 0 | 0 | 0 | 0))
        {
            currentState = State::SwitchOnDisabled;
        }
        else if (
            cleanState == (0 | importantStateBits[4] | 0 | 0 | 0 | importantStateBits[0]))
        {
            currentState = State::ReadyToSwitchOn;
        }
        else if (
            cleanState == (0 | importantStateBits[4] | 0 | 0 | importantStateBits[1] | importantStateBits[0]))
        {
            currentState = State::SwitchedOn;
        }
        else if (
            cleanState == (0 | importantStateBits[4] | 0 | importantStateBits[2] | importantStateBits[1] | importantStateBits[0]))
        {
            currentState = State::OperationEnabled;
        }
        else if (
            cleanState == (0 | 0 | 0 | importantStateBits[2] | importantStateBits[1] | importantStateBits[0]))
        {
            currentState = State::QuickStopActive;
        }
        else if (
            cleanState == (0 | 0 | importantStateBits[3] | importantStateBits[2] | importantStateBits[1] | importantStateBits[0]) ||
            cleanState == (0 | importantStateBits[4] | importantStateBits[3] | importantStateBits[2] | importantStateBits[1] | importantStateBits[0])

        )
        {
            currentState = State::FaultReactionActive;
        }
        else if(
            cleanState == (0 | 0 | importantStateBits[3] | 0 | 0 | 0) ||
            cleanState == (0 | importantStateBits[4] | importantStateBits[3] | 0 | 0 | 0)
        )
        {
            currentState = State::Fault;
        }
        else
        {
            currentState = State::Unknown;
            m_CurrentState = currentState;
            return false;    
        }

        m_CurrentState = currentState;

        return true;
    }

} // namespace CIA402

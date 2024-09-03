/**
 * @file driver_state_machine.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-11-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef DRIVER_STATE_MACHINE_HPP_
#define DRIVER_STATE_MACHINE_HPP_

#include "ethercat_interface/ec_utils.hpp"

#include <map>
#include <queue>

// enum class State;

class StateMachine
{
public:
    StateMachine();

    virtual ~StateMachine();

    virtual bool init() = 0;

protected:
};

namespace CIA402
{

    enum class State
    {
        Start,
        NotReadyToSwitchOn,
        SwitchOnDisabled,
        ReadyToSwitchOn,
        SwitchedOn,
        OperationEnabled,
        QuickStopActive,
        FaultReactionActive,
        Fault,
        Unknown
    };

    const enum StatusWordBits {
        ReadyToSwitchOn = 0,
        SwitchedOn = 1,
        OperationEnabled = 2,
        Fault = 3,
        VoltageEnabled = 4,
        QuickStop = 5,
        SwitchonDisabled = 6,
        Warning = 7,
        ManufacturerSpecific_0 = 8,
        Remote = 9,
        TargetReached = 10,
        InternalLimit = 11,
        OperationModeSpecific_0 = 12,
        OperationModeSpecific_1 = 13,
        ManufacturerSpecific_1 = 14,
        ManufacturerSpecific_2 = 15
    };

    const std::array<uint16_t, 6> importantStateBits = {
        (1 << StatusWordBits::ReadyToSwitchOn),
        (1 << StatusWordBits::SwitchedOn),
        (1 << StatusWordBits::OperationEnabled),
        (1 << StatusWordBits::Fault),
        (1 << StatusWordBits::QuickStop),
        (1 << StatusWordBits::SwitchonDisabled)
    };  

    const uint16_t stateCheckBits = ((1 << ReadyToSwitchOn) |
                                         (1 << SwitchedOn) |
                                         (1 << OperationEnabled) |
                                         (1 << Fault) | 
                                         (1 << QuickStop) | 
                                         (1 << ReadyToSwitchOn));

    enum class ControlWord : uint16_t
    {
        SwitchOn = 0,
        EnableVoltage = 1,
        QuickStop = 2,
        EnableOperation = 3,
        FaultReset = 7,
        Halt = 8
    };

    constexpr const uint16_t getControlWordBitIndex(ControlWord control_word)
    {
        return static_cast<typename std::underlying_type<ControlWord>::type>(control_word);
    }

    const std::map<State, State> StateTransitions = {
        {State::Start, State::NotReadyToSwitchOn},            // 0
        {State::NotReadyToSwitchOn, State::SwitchOnDisabled}, // 1
        {State::SwitchedOn, State::ReadyToSwitchOn},          // 2
        {State::ReadyToSwitchOn, State::SwitchedOn},          // 3
        {State::SwitchedOn, State::OperationEnabled},         // 4
        {State::OperationEnabled, State::SwitchedOn},         // 5
        {State::SwitchedOn, State::ReadyToSwitchOn},          // 6
        {State::ReadyToSwitchOn, State::SwitchOnDisabled},    // 7
        {State::OperationEnabled, State::ReadyToSwitchOn},    // 8
        {State::OperationEnabled, State::SwitchOnDisabled},   // 9
        {State::SwitchedOn, State::SwitchOnDisabled},         // 10
        {State::OperationEnabled, State::QuickStopActive},    // 11
        {State::QuickStopActive, State::SwitchOnDisabled},    // 12
        {State::Unknown, State::FaultReactionActive},         // 13
        {State::FaultReactionActive, State::Fault},           // 14
        {State::Fault, State::SwitchOnDisabled},              // 15
        {State::QuickStopActive, State::OperationEnabled}     // 16
    };

    namespace state_transition_commands
    {

        constexpr uint16_t autoSet = 0x0;
        constexpr uint16_t autoReset = 0x0;

        constexpr uint16_t shutdownSet = ((1 << getControlWordBitIndex(ControlWord::EnableVoltage)) | (1 << getControlWordBitIndex(ControlWord::QuickStop)));
        constexpr uint16_t shutdownReset = ((1 << getControlWordBitIndex(ControlWord::SwitchOn)) | (1 << getControlWordBitIndex(ControlWord::FaultReset)));

        constexpr uint16_t switchOnSet = ((1 << getControlWordBitIndex(ControlWord::SwitchOn)) | (1 << getControlWordBitIndex(ControlWord::EnableVoltage)) | (1 << (getControlWordBitIndex(ControlWord::QuickStop))));
        constexpr uint16_t switchOnReset = ((1 << getControlWordBitIndex(ControlWord::EnableVoltage)) | (1 << getControlWordBitIndex(ControlWord::FaultReset)));

        constexpr uint16_t disableOperationSet = ((1 << getControlWordBitIndex(ControlWord::SwitchOn)) | (1 << getControlWordBitIndex(ControlWord::EnableVoltage)) | (1 << (getControlWordBitIndex(ControlWord::QuickStop))));
        constexpr uint16_t disableOperationReset = ((1 << getControlWordBitIndex(ControlWord::EnableOperation)) | 1 << (getControlWordBitIndex(ControlWord::FaultReset)));

        constexpr uint16_t disableVoltageSet = 0x0;
        constexpr uint16_t disableVoltageReset = ((1 << getControlWordBitIndex(ControlWord::FaultReset)) | (1 << getControlWordBitIndex(ControlWord::EnableVoltage)));

        constexpr uint16_t quickStopSet = (1 << getControlWordBitIndex(ControlWord::EnableVoltage));
        constexpr uint16_t quickStopReset = ((1 << getControlWordBitIndex(ControlWord::FaultReset)) | (1 << getControlWordBitIndex(ControlWord::QuickStop)));

        constexpr uint16_t enableOperationSet = ((1 << getControlWordBitIndex(ControlWord::QuickStop)) | (1 << getControlWordBitIndex(ControlWord::EnableVoltage)) | (1 << getControlWordBitIndex(ControlWord::SwitchOn)) | (1 << getControlWordBitIndex(ControlWord::EnableOperation)));
        constexpr uint16_t enableOperationReset = (1 << getControlWordBitIndex(ControlWord::FaultReset));

        constexpr uint16_t faultResetSet = (1 << getControlWordBitIndex(ControlWord::FaultReset));
        constexpr uint16_t faultResetReset = 0x0;
    }

    using Transition = std::pair<State, State>;
    using TransitionCommand = std::pair<uint16_t, uint16_t>;

    class StateMachine : public ::StateMachine
    {
    public:
        StateMachine();

        ~StateMachine();

        bool init() override;

        bool addTransition(
            const State first_state,
            const State second_state,
            const uint16_t set_bits,
            const uint16_t reset_bits);

        bool addTransition(
            const Transition &transitions,
            const TransitionCommand &transition_command);

        const std::queue<TransitionCommand> findTransition(

        ) const;

    private:
        std::map<Transition, TransitionCommand> m_TransitionMap;

        State m_CurrentState;

        bool findCurrentState(const uint16_t &current_status_word);
    };

} // End of namespace CIA402

#endif // DRIVER_STATE_MACHINE_HPP_
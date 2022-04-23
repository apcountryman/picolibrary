/**
 * picolibrary
 *
 * Copyright 2020-2022, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
 * contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this
 * file except in compliance with the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * \file
 * \brief picolibrary::GPIO interface.
 */

#ifndef PICOLIBRARY_GPIO_H
#define PICOLIBRARY_GPIO_H

#include <cstdint>

/**
 * \brief General Purpose Input/Output (GPIO) facilities.
 */
namespace picolibrary::GPIO {

/**
 * \brief Initial internal pull-up resistor state.
 */
enum class Initial_Pull_Up_State : std::uint_fast8_t {
    DISABLED, ///< Disabled.
    ENABLED,  ///< Enabled.
};

/**
 * \brief Initial pin state.
 */
enum class Initial_Pin_State : std::uint_fast8_t {
    LOW,  ///< Low.
    HIGH, ///< High.
};

/**
 * \brief Input pin concept.
 *
 * \attention picolibrary assumes that the high pin/signal state is the active pin/signal
 *            state. All input pin implementations should use this assumption. If the high
 *            pin/signal state is not the active pin/signal state,
 *            picolibrary::GPIO::Active_Low_Input_Pin can be used to invert an input pin
 *            implementation's behavior.
 */
class Input_Pin_Concept {
  public:
    /**
     * \brief Constructor.
     */
    Input_Pin_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Input_Pin_Concept( Input_Pin_Concept && source ) noexcept;

    Input_Pin_Concept( Input_Pin_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Input_Pin_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Input_Pin_Concept && expression ) noexcept -> Input_Pin_Concept &;

    auto operator=( Input_Pin_Concept const & ) = delete;

    /**
     * \brief Initialize the pin's hardware.
     */
    void initialize() noexcept;

    /**
     * \brief Check if the pin is in the low state.
     *
     * \return true if the pin is in the low state.
     * \return false if the pin is not in the low state.
     */
    auto is_low() const noexcept -> bool;

    /**
     * \brief Check if the pin is in the high state.
     *
     * \return true if the pin is in the high state.
     * \return false if the pin is not in the high state.
     */
    auto is_high() const noexcept -> bool;
};

/**
 * \brief Internally pulled-up input pin concept.
 *
 * \attention picolibrary assumes that the high pin/signal state is the active pin/signal
 *            state. All input pin implementations should use this assumption. If the high
 *            pin/signal state is not the active pin/signal state,
 *            picolibrary::GPIO::Active_Low_Input_Pin can be used to invert an input pin
 *            implementation's behavior.
 */
class Internally_Pulled_Up_Input_Pin_Concept {
  public:
    /**
     * \brief Constructor.
     */
    Internally_Pulled_Up_Input_Pin_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Internally_Pulled_Up_Input_Pin_Concept( Internally_Pulled_Up_Input_Pin_Concept && source ) noexcept;

    Internally_Pulled_Up_Input_Pin_Concept( Internally_Pulled_Up_Input_Pin_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Internally_Pulled_Up_Input_Pin_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Internally_Pulled_Up_Input_Pin_Concept && expression ) noexcept
        -> Internally_Pulled_Up_Input_Pin_Concept &;

    auto operator=( Internally_Pulled_Up_Input_Pin_Concept const & ) = delete;

    /**
     * \brief Initialize the pin's hardware.
     *
     * \param[in] initial_pull_up_state The initial state of the pin's internal pull-up
     *            resistor.
     */
    void initialize( Initial_Pull_Up_State initial_pull_up_state = Initial_Pull_Up_State::DISABLED ) noexcept;

    /**
     * \brief Check if the pin's internal pull-up resistor is disabled.
     *
     * \return true if the pin's internal pull-up resistor is disabled.
     * \return false if the pin's internal pull-up resistor is not disabled.
     */
    auto pull_up_is_disabled() const noexcept -> bool;

    /**
     * \brief Check if the pin's internal pull-up resistor is enabled.
     *
     * \return true if the pin's internal pull-up resistor is enabled.
     * \return false if the pin's internal pull-up resistor is not enabled.
     */
    auto pull_up_is_enabled() const noexcept -> bool;

    /**
     * \brief Disable the pin's internal pull-up resistor.
     */
    void disable_pull_up() noexcept;

    /**
     * \brief Enable the pin's internal pull-up resistor.
     */
    void enable_pull_up() noexcept;

    /**
     * \brief Check if the pin is in the low state.
     *
     * \return true if the pin is in the low state.
     * \return false if the pin is not in the low state.
     */
    auto is_low() const noexcept -> bool;

    /**
     * \brief Check if the pin is in the high state.
     *
     * \return true if the pin is in the high state.
     * \return false if the pin is not in the high state.
     */
    auto is_high() const noexcept -> bool;
};

/**
 * \brief Output pin concept.
 *
 * \attention picolibrary assumes that the high pin/signal state is the active pin/signal
 *            state. All output pin implementations should use this assumption. If the
 *            high pin/signal state is not the active pin/signal state,
 *            picolibrary::GPIO::Active_Low_Output_Pin can be used to invert an output pin
 *            implementation's behavior.
 */
class Output_Pin_Concept {
  public:
    /**
     * \brief Constructor.
     */
    Output_Pin_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Output_Pin_Concept( Output_Pin_Concept && source ) noexcept;

    Output_Pin_Concept( Output_Pin_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Output_Pin_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Output_Pin_Concept && expression ) noexcept -> Output_Pin_Concept &;

    auto operator=( Output_Pin_Concept const & ) = delete;

    /**
     * \brief Initialize the pin's hardware.
     *
     * \param[in] initial_pin_state The initial state of the pin.
     */
    void initialize( Initial_Pin_State initial_pin_state = Initial_Pin_State::LOW ) noexcept;

    /**
     * \brief Transition the pin to the low state.
     */
    void transition_to_low() noexcept;

    /**
     * \brief Transition the pin to the high state.
     */
    void transition_to_high() noexcept;

    /**
     * \brief Toggle the pin state.
     */
    void toggle() noexcept;
};

/**
 * \brief Input/Output (I/O) pin concept.
 *
 * \attention picolibrary assumes that the high pin/signal state is the active pin/signal
 *            state. All I/O pin implementations should use this assumption. If the high
 *            pin/signal state is not the active pin/signal state,
 *            picolibrary::GPIO::Active_Low_IO_Pin can be used to invert an I/O pin
 *            implementation's behavior.
 */
class IO_Pin_Concept {
  public:
    /**
     * \brief Constructor.
     */
    IO_Pin_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    IO_Pin_Concept( IO_Pin_Concept && source ) noexcept;

    IO_Pin_Concept( IO_Pin_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~IO_Pin_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( IO_Pin_Concept && expression ) noexcept -> IO_Pin_Concept &;

    auto operator=( IO_Pin_Concept const & ) = delete;

    /**
     * \brief Initialize the pin's hardware.
     *
     * \param[in] initial_pin_state The initial state of the pin.
     */
    void initialize( Initial_Pin_State initial_pin_state = Initial_Pin_State::LOW ) noexcept;

    /**
     * \brief Check if the pin is in the low state.
     *
     * \return true if the pin is in the low state.
     * \return false if the pin is not in the low state.
     */
    auto is_low() const noexcept -> bool;

    /**
     * \brief Check if the pin is in the high state.
     *
     * \return true if the pin is in the high state.
     * \return false if the pin is not in the high state.
     */
    auto is_high() const noexcept -> bool;

    /**
     * \brief Transition the pin to the low state.
     */
    void transition_to_low() noexcept;

    /**
     * \brief Transition the pin to the high state.
     */
    void transition_to_high() noexcept;

    /**
     * \brief Toggle the pin state.
     */
    void toggle() noexcept;
};

/**
 * \brief Active low input pin adapter.
 *
 * \attention picolibrary assumes that the high pin/signal state is the active pin/signal
 *            state. All input pin implementations should use this assumption. If the high
 *            pin/signal state is not the active pin/signal state, this class can be used
 *            to invert an input pin implementation's behavior.
 *
 * \tparam Input_Pin The type of input pin being adapted.
 */
template<typename Input_Pin>
class Active_Low_Input_Pin : public Input_Pin {
  public:
    using Input_Pin::Input_Pin;

    /**
     * \brief Check if the pin is in the high state.
     *
     * \return true if the pin is in the high state.
     * \return false if the pin is not in the high state.
     */
    auto is_low() const noexcept -> bool
    {
        return Input_Pin::is_high();
    }

    /**
     * \brief Check if the pin is in the low state.
     *
     * \return true if the pin is in the low state.
     * \return false if the pin is not in the low state.
     */
    auto is_high() const noexcept -> bool
    {
        return Input_Pin::is_low();
    }
};

/**
 * \brief Active low output pin adapter.
 *
 * \attention picolibrary assumes that the high pin/signal state is the active pin/signal
 *            state. All output pin implementations should use this assumption. If the
 *            high pin/signal state is not the active pin/signal state, this class can be
 *            used to invert an output pin implementation's behavior.
 *
 * \tparam Output_Pin The type of output pin being adapted.
 */
template<typename Output_Pin>
class Active_Low_Output_Pin : public Output_Pin {
  public:
    using Output_Pin::Output_Pin;

    /**
     * \brief Initialize the pin's hardware.
     *
     * \param[in] initial_pin_state The initial state of the pin.
     */
    void initialize( Initial_Pin_State initial_pin_state = Initial_Pin_State::LOW ) noexcept
    {
        Output_Pin::initialize(
            initial_pin_state == Initial_Pin_State::HIGH ? Initial_Pin_State::LOW
                                                         : Initial_Pin_State::HIGH );
    }

    /**
     * \brief Transition the pin to the high state.
     */
    void transition_to_low() noexcept
    {
        Output_Pin::transition_to_high();
    }

    /**
     * \brief Transition the pin to the low state.
     */
    void transition_to_high() noexcept
    {
        Output_Pin::transition_to_low();
    }
};

/**
 * \brief Active low I/O pin adapter.
 *
 *
 * \attention picolibrary assumes that the high pin/signal state is the active pin/signal
 *            state. All I/O pin implementations should use this assumption. If the high
 *            pin/signal state is not the active pin/signal state, this class can be used
 *            to invert an I/O pin implementation's behavior.
 *
 * \tparam IO_Pin The type of I/O pin being adapted.
 */
template<typename IO_Pin>
class Active_Low_IO_Pin : public Active_Low_Output_Pin<Active_Low_Input_Pin<IO_Pin>> {
  public:
    using Active_Low_Output_Pin<Active_Low_Input_Pin<IO_Pin>>::Active_Low_Output_Pin;
};

} // namespace picolibrary::GPIO

#endif // PICOLIBRARY_GPIO_H

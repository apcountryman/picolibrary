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
 */
class Input_Pin_Concept {
  public:
    /**
     * \brief Constructor.
     */
    Input_Pin_Concept() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Input_Pin_Concept( Input_Pin_Concept && source ) noexcept = default;

    Input_Pin_Concept( Input_Pin_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Input_Pin_Concept() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Input_Pin_Concept && expression ) noexcept -> Input_Pin_Concept & = default;

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
 */
class Internally_Pulled_Up_Input_Pin_Concept {
  public:
    /**
     * \brief Constructor.
     */
    Internally_Pulled_Up_Input_Pin_Concept() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Internally_Pulled_Up_Input_Pin_Concept( Internally_Pulled_Up_Input_Pin_Concept && source ) noexcept = default;

    Internally_Pulled_Up_Input_Pin_Concept( Internally_Pulled_Up_Input_Pin_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Internally_Pulled_Up_Input_Pin_Concept() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Internally_Pulled_Up_Input_Pin_Concept && expression ) noexcept
        -> Internally_Pulled_Up_Input_Pin_Concept & = default;

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
 */
class Output_Pin_Concept {
  public:
    /**
     * \brief Constructor.
     */
    Output_Pin_Concept() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Output_Pin_Concept( Output_Pin_Concept && source ) noexcept = default;

    Output_Pin_Concept( Output_Pin_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Output_Pin_Concept() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Output_Pin_Concept && expression ) noexcept -> Output_Pin_Concept & = default;

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

} // namespace picolibrary::GPIO

#endif // PICOLIBRARY_GPIO_H

/**
 * picolibrary
 *
 * Copyright 2020-2021, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::Indicator interface.
 */

#ifndef PICOLIBRARY_INDICATOR_H
#define PICOLIBRARY_INDICATOR_H

#include "picolibrary/error.h"
#include "picolibrary/gpio.h"
#include "picolibrary/result.h"
#include "picolibrary/void.h"

/**
 * \brief Indicator facilities.
 */
namespace picolibrary::Indicator {

/**
 * \brief Initial indicator state.
 */
enum class Initial_Indicator_State {
    ILLUMINATED,  ///< Illuminated.
    EXTINGUISHED, ///< Extinguished.
};

/**
 * \brief Fixed intensity indicator concept.
 */
class Fixed_Intensity_Indicator_Concept {
  public:
    /**
     * \brief Constructor.
     */
    Fixed_Intensity_Indicator_Concept() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Fixed_Intensity_Indicator_Concept( Fixed_Intensity_Indicator_Concept && source ) noexcept = default;

    Fixed_Intensity_Indicator_Concept( Fixed_Intensity_Indicator_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Fixed_Intensity_Indicator_Concept() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Fixed_Intensity_Indicator_Concept && expression ) noexcept
        -> Fixed_Intensity_Indicator_Concept & = default;

    auto operator=( Fixed_Intensity_Indicator_Concept const & ) = delete;

    /**
     * \brief Initialize the indicator's hardware.
     *
     * \param[in] initial_indicator_state The initial state of the indicator.
     *
     * \return Nothing if indicator hardware initialization succeeded.
     * \return An error code if indicator hardware initialization failed. If indicator
     *         hardware initialization cannot fail, return
     *         picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto initialize( Initial_Indicator_State initial_indicator_state = Initial_Indicator_State::EXTINGUISHED ) noexcept
        -> Result<Void, Error_Code>;

    /**
     * \brief Illuminate the indicator.
     *
     * \return Nothing if illuminating the indicator succeeded.
     * \return An error code if illuminating the indicator failed. If illuminating the
     *         indicator cannot fail, return picolibrary::Result<picolibrary::Void,
     *         picolibrary::Void>.
     */
    auto illuminate() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Extinguish the indicator.
     *
     * \return Nothing if extinguishing the indicator succeeded.
     * \return An error code if extinguishing the indicator failed. If extinguishing the
     *         indicator cannot fail, return picolibrary::Result<picolibrary::Void,
     *         picolibrary::Void>.
     */
    auto extinguish() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Toggle the indicator.
     *
     * \return Nothing if toggling the indicator succeeded.
     * \return An error code if toggling the indicator failed. If toggling the indicator
     *         cannot fail, return picolibrary::Result<picolibrary::Void,
     *         picolibrary::Void>.
     */
    auto toggle() noexcept -> Result<Void, Error_Code>;
};

/**
 * \brief GPIO output pin based fixed intensity indicator
 *
 * \tparam Output_Pin The type of output pin being adapted.
 */
template<typename Output_Pin>
class GPIO_Output_Pin_Fixed_Intensity_Indicator : public Output_Pin {
  public:
    using Output_Pin::Output_Pin;

    /**
     * \brief Initialize the indicator's hardware.
     *
     * \param[in] initial_indicator_state The initial state of the indicator.
     *
     * \return Nothing if indicator hardware initialization succeeded.
     * \return The error reported by the underlying pin if pin hardware initialization
     *         failed.
     */
    auto initialize( Initial_Indicator_State initial_indicator_state = Initial_Indicator_State::EXTINGUISHED ) noexcept
    {
        return Output_Pin::initialize(
            initial_indicator_state == Initial_Indicator_State::ILLUMINATED
                ? GPIO::Initial_Pin_State::HIGH
                : GPIO::Initial_Pin_State::LOW );
    }

    /**
     * \brief Illuminate the indicator.
     *
     * \return Nothing if illuminating the indicator succeeded.
     * \return The error reported by the underlying pin if transitioning the pin to the
     *         high state failed.
     */
    auto illuminate() noexcept
    {
        return this->transition_to_high();
    }

    /**
     * \brief Extinguish the indicator.
     *
     * \return Nothing if extinguishing the indicator succeeded.
     * \return The error reported by the underlying pin if transitioning the pin to the
     *         low state failed.
     */
    auto extinguish() noexcept
    {
        return this->transition_to_low();
    }
};

} // namespace picolibrary::Indicator

#endif // PICOLIBRARY_INDICATOR_H

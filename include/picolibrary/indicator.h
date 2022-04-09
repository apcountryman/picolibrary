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
 * \brief picolibrary::Indicator interface.
 */

#ifndef PICOLIBRARY_INDICATOR_H
#define PICOLIBRARY_INDICATOR_H

#include <cstdint>

/**
 * \brief Indicator facilities.
 */
namespace picolibrary::Indicator {

/**
 * \brief Initial indicator state.
 */
enum class Initial_Indicator_State : std::uint_fast8_t {
    EXTINGUISHED, ///< Extinguished.
    ILLUMINATED,  ///< Illuminated.
};

/**
 * \brief Fixed intensity indicator concept.
 */
class Fixed_Intensity_Indicator_Concept {
  public:
    /**
     * \brief Constructor.
     */
    Fixed_Intensity_Indicator_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Fixed_Intensity_Indicator_Concept( Fixed_Intensity_Indicator_Concept && source ) noexcept;

    Fixed_Intensity_Indicator_Concept( Fixed_Intensity_Indicator_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Fixed_Intensity_Indicator_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Fixed_Intensity_Indicator_Concept && expression ) noexcept
        -> Fixed_Intensity_Indicator_Concept &;

    auto operator=( Fixed_Intensity_Indicator_Concept const & ) = delete;

    /**
     * \brief Initialize the indicator's hardware.
     *
     * \param[in] initial_indicator_state The initial state of the indicator.
     */
    void initialize( Initial_Indicator_State initial_indicator_state = Initial_Indicator_State::EXTINGUISHED ) noexcept;

    /**
     * \brief Extinguish the indicator.
     */
    void extinguish() noexcept;

    /**
     * \brief Illuminate the indicator.
     */
    void illuminate() noexcept;

    /**
     * \brief Toggle the indicator state.
     */
    void toggle() noexcept;
};

} // namespace picolibrary::Indicator

#endif // PICOLIBRARY_INDICATOR_H

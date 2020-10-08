/**
 * picolibrary
 *
 * Copyright 2020 Andrew Countryman <apcountryman@gmail.com>
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

/**
 * \brief General Purpose Input/Output (GPIO) facilities.
 */
namespace picolibrary::GPIO {

/**
 * \brief Pin state.
 */
class Pin_State {
  public:
    Pin_State() = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Pin_State( Pin_State && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Pin_State( Pin_State const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Pin_State() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Pin_State && expression ) noexcept -> Pin_State & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Pin_State const & expression ) noexcept -> Pin_State & = default;
};

} // namespace picolibrary::GPIO

#endif // PICOLIBRARY_GPIO_H

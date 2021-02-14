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
 * \brief picolibrary::I2C interface.
 */

#ifndef PICOLIBRARY_I2C_H
#define PICOLIBRARY_I2C_H

#include <cstdint>

/**
 * \brief Inter-Integrated Circuit (I2C) facilities.
 */
namespace picolibrary::I2C {

/**
 * \brief Device address.
 */
class Address {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Address() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Address( Address && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Address( Address const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Address() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Address && expression ) noexcept -> Address & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Address const & expression ) noexcept -> Address & = default;

    /**
     * \brief Get the device address in numeric form.
     *
     * \return The device address in numeric form.
     */
    constexpr auto numeric() const noexcept -> std::uint_fast8_t
    {
        return {};
    }

    /**
     * \brief Get the device address in transmitted form.
     *
     * \return The device address in transmitted form.
     */
    constexpr auto transmitted() const noexcept -> std::uint8_t
    {
        return {};
    }
};

} // namespace picolibrary::I2C

#endif // PICOLIBRARY_I2C_H

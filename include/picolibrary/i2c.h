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

#include "picolibrary/error.h"
#include "picolibrary/result.h"

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
     * \brief Device address numeric form tag.
     */
    struct Numeric {
        /**
         * \brief The minimum supported device address in numeric form.
         */
        static constexpr auto MIN = std::uint_fast8_t{ 0b000'0000 };

        /**
         * \brief The maximum supported device address in numeric form.
         */
        static constexpr auto MAX = std::uint_fast8_t{ 0b111'1111 };
    };

    /**
     * \brief Device address transmitted form tag.
     */
    struct Transmitted {
        /**
         * \brief The minimum supported device address in transmitted form.
         */
        static constexpr auto MIN = std::uint_fast8_t{ Numeric::MIN << 1 };

        /**
         * \brief The maximum supported device address in transmitted form.
         */
        static constexpr auto MAX = std::uint_fast8_t{ Numeric::MAX << 1 };
    };

    /**
     * \brief Device address numeric form tag.
     */
    static constexpr auto NUMERIC = Numeric{};

    /**
     * \brief Device address transmitted form tag.
     */
    static constexpr auto TRANSMITTED = Transmitted{};

    /**
     * \brief Constructor.
     */
    constexpr Address() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \warning This constructor does not validate its arguments. Use
     *          picolibrary::I2C::make_address( picolibrary::I2C::Address::Numeric,
     *          std::uint_fast8_t ) if argument validation is desired.
     *
     * \param[in] address The device address in numeric form.
     */
    constexpr Address( Numeric, std::uint_fast8_t address ) noexcept :
        m_address{ static_cast<std::uint8_t>( address << 1 ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \warning This constructor does not validate its arguments. Use
     *          picolibrary::I2C::make_address( picolibrary::I2C::Address::Transmitted,
     *          std::uint8_t ) if argument validation is desired.
     *
     * \param[in] address The device address in transmitted form.
     */
    constexpr Address( Transmitted, std::uint8_t address ) noexcept : m_address{ address }
    {
    }

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
        return m_address >> 1;
    }

    /**
     * \brief Get the device address in transmitted form.
     *
     * \return The device address in transmitted form.
     */
    constexpr auto transmitted() const noexcept
    {
        return m_address;
    }

  private:
    /**
     * \brief The device address in transmitted form.
     */
    std::uint8_t m_address{};
};

/**
 * \brief Construct a picolibrary::I2C::Address.
 *
 * \relatedalso picolibrary::I2C::Address
 *
 * \param[in] address The device address in numeric form.
 *
 * \return The device address if address is less than or equal to
 *         picolibrary::I2C::Address::Numeric::MAX.
 * \return picolibrary::Generic_Error::INVALID_ARGUMENT if address is greater than
 *         picolibrary::I2C::Address::Numeric::MAX.
 */
inline auto make_address( Address::Numeric, std::uint_fast8_t address ) noexcept
    -> Result<Address, Error_Code>
{
    if ( address > Address::Numeric::MAX ) {
        return Generic_Error::INVALID_ARGUMENT;
    } // if

    return Address{ Address::NUMERIC, address };
}

/**
 * \brief Construct a picolibrary::I2C::Address.
 *
 * \relatedalso picolibrary::I2C::Address
 *
 * \param[in] address The device address in transmitted form.
 *
 * \return The device address if the least significant bit of address is not set.
 * \return picolibrary::Generic_Error::INVALID_ARGUMENT if the least significant bit of
 *         address is set.
 */
inline auto make_address( Address::Transmitted, std::uint8_t address ) noexcept
    -> Result<Address, Error_Code>
{
    if ( address & 0x01 ) {
        return Generic_Error::INVALID_ARGUMENT;
    } // if

    return Address{ Address::TRANSMITTED, address };
}

} // namespace picolibrary::I2C

#endif // PICOLIBRARY_I2C_H

/**
 * picolibrary
 *
 * Copyright 2020-2024, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::IPv4 interface.
 */

#ifndef PICOLIBRARY_IPV4_H
#define PICOLIBRARY_IPV4_H

#include <cstddef>
#include <cstdint>
#include <limits>

#include "picolibrary/array.h"
#include "picolibrary/format.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"

/**
 * \brief IPv4 facilities.
 */
namespace picolibrary::IPv4 {

/**
 * \brief Address.
 */
class Address {
  public:
    /**
     * \brief Address byte array representation.
     */
    using Byte_Array = Array<std::uint8_t, 4>;

    /**
     * \brief Address unsigned integer representation.
     */
    using Unsigned_Integer = std::uint32_t;

    /**
     * \brief Get the minimum valid address.
     *
     * \return The minimum valid address.
     */
    static constexpr auto min() noexcept -> Address
    {
        return Address{ { 0, 0, 0, 0 } };
    }

    /**
     * \brief Get the maximum valid address.
     *
     * \return The maximum valid address.
     */
    static constexpr auto max() noexcept -> Address
    {
        return Address{ { 255, 255, 255, 255 } };
    }

    /**
     * \brief Get the address that is used to represent any address (0.0.0.0).
     *
     * \return The address that is used to represent any address.
     */
    static constexpr auto any() noexcept -> Address
    {
        return Address{ { 0, 0, 0, 0 } };
    }

    /**
     * \brief Get the typical loopback address (127.0.0.1).
     *
     * \return The typical loopback address.
     */
    static constexpr auto loopback() noexcept -> Address
    {
        return Address{ { 127, 0, 0, 1 } };
    }

    /**
     * \brief Get the local network broadcast address.
     *
     * \return The local network broadcast address.
     */
    static constexpr auto broadcast() noexcept -> Address
    {
        return Address{ { 255, 255, 255, 255 } };
    }

    /**
     * \brief Constructor.
     */
    constexpr Address() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] address The address.
     */
    constexpr Address( Byte_Array const & address ) noexcept : m_address{ address }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] address The address.
     */
    constexpr Address( Unsigned_Integer const & address ) noexcept :
        m_address{
            static_cast<std::uint8_t>( address >> 3 * std::numeric_limits<std::uint8_t>::digits ),
            static_cast<std::uint8_t>( address >> 2 * std::numeric_limits<std::uint8_t>::digits ),
            static_cast<std::uint8_t>( address >> 1 * std::numeric_limits<std::uint8_t>::digits ),
            static_cast<std::uint8_t>( address >> 0 * std::numeric_limits<std::uint8_t>::digits ),
        }
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
     * \brief Check if the address is the address that is used to represent any address
     *        (0.0.0.0).
     *
     * \return true if the address is the address that is used to represent any address.
     * \return false if the address is not the address that is used to represent any
     *         address.
     */
    constexpr auto is_any() const noexcept -> bool
    {
        return m_address == any().as_byte_array();
    }

    /**
     * \brief Check if the address is a loopback address (127.0.0.0-127.255.255.255).
     *
     * \return true if the address is a loopback address.
     * \return false if the address is not a loopback address.
     */
    constexpr auto is_loopback() const noexcept -> bool
    {
        return m_address[ 0 ] == 127;
    }

    /**
     * \brief Check if the address is the local network broadcast address
     *        (255.255.255.255).
     *
     * \return true if the address is the local network broadcast address.
     * \return false if the address is not the local network broadcast address.
     */
    constexpr auto is_broadcast() const noexcept -> bool
    {
        return m_address == broadcast().as_byte_array();
    }

    /**
     * \brief Check if the address is a multicast address (224.0.0.0-239.255.255.255).
     *
     * \return true if the address is a multicast address.
     * \return false if the address is not a multicast address.
     */
    constexpr auto is_multicast() const noexcept -> bool
    {
        return m_address[ 0 ] >= 224 and m_address[ 0 ] <= 239;
    }

    /**
     * \brief Get the address in its byte array representation.
     *
     * \return The address in its byte array representation.
     */
    constexpr auto as_byte_array() const noexcept -> Byte_Array
    {
        return m_address;
    }

    /**
     * \brief Get the address in its unsigned integer representation.
     *
     * \return The address in its unsigned integer representation.
     */
    constexpr auto as_unsigned_integer() const noexcept -> Unsigned_Integer
    {
        // clang-format off

        return static_cast<Unsigned_Integer>(
            ( static_cast<Unsigned_Integer>( m_address[ 0 ] ) << 3 * std::numeric_limits<std::uint8_t>::digits ) |
            ( static_cast<Unsigned_Integer>( m_address[ 1 ] ) << 2 * std::numeric_limits<std::uint8_t>::digits ) |
            ( static_cast<Unsigned_Integer>( m_address[ 2 ] ) << 1 * std::numeric_limits<std::uint8_t>::digits ) |
            ( static_cast<Unsigned_Integer>( m_address[ 3 ] ) << 0 * std::numeric_limits<std::uint8_t>::digits ) );

        // clang-format on
    }

  private:
    /**
     * \brief The address in its byte array representation.
     */
    Byte_Array m_address{};
};

/**
 * \brief Equality operator.
 *
 * \relatedalso picolibrary::IPv4::Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is equal to rhs.
 * \return false if lhs is not equal to rhs.
 */
constexpr auto operator==( Address const & lhs, Address const & rhs ) noexcept -> bool
{
    return lhs.as_byte_array() == rhs.as_byte_array();
}

/**
 * \brief Inequality operator.
 *
 * \relatedalso picolibrary::IPv4::Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is not equal to rhs.
 * \return false if lhs is equal to rhs.
 */
constexpr auto operator!=( Address const & lhs, Address const & rhs ) noexcept -> bool
{
    return not( lhs == rhs );
}

/**
 * \brief Less than operator.
 *
 * \relatedalso picolibrary::IPv4::Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than rhs.
 * \return false if lhs is not less than rhs.
 */
constexpr auto operator<( Address const & lhs, Address const & rhs ) noexcept -> bool
{
    return lhs.as_unsigned_integer() < rhs.as_unsigned_integer();
}

/**
 * \brief Greater than operator.
 *
 * \relatedalso picolibrary::IPv4::Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than rhs.
 * \return false if lhs is not greater than rhs.
 */
constexpr auto operator>( Address const & lhs, Address const & rhs ) noexcept -> bool
{
    return rhs < lhs;
}

/**
 * \brief Less than or equal to operator.
 *
 * \relatedalso picolibrary::IPv4::Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than or equal to rhs.
 * \return false if lhs is not less than or equal to rhs.
 */
constexpr auto operator<=( Address const & lhs, Address const & rhs ) noexcept -> bool
{
    return not( lhs > rhs );
}

/**
 * \brief Greater than or equal to operator.
 *
 * \relatedalso picolibrary::IPv4::Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than or equal to rhs.
 * \return false if lhs is not greater than or equal to rhs.
 */
constexpr auto operator>=( Address const & lhs, Address const & rhs ) noexcept -> bool
{
    return not( lhs < rhs );
}

} // namespace picolibrary::IPv4

namespace picolibrary {

/**
 * \brief picolibrary::IPv4::Address output formatter.
 */
template<>
class Output_Formatter<IPv4::Address> {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Output_Formatter() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Output_Formatter( Output_Formatter && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Output_Formatter( Output_Formatter const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Output_Formatter() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Output_Formatter && expression ) noexcept -> Output_Formatter & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator   =( Output_Formatter const & expression ) noexcept
        -> Output_Formatter & = default;

    /**
     * \brief Write the formatted picolibrary::IPv4::Address to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::IPv4::Address to.
     * \param[in] address The picolibrary::IPv4::Address to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, IPv4::Address const & address ) const noexcept
        -> Result<std::size_t>
    {
        return stream.print(
            Format::Dec{ address.as_byte_array()[ 0 ] },
            '.',
            Format::Dec{ address.as_byte_array()[ 1 ] },
            '.',
            Format::Dec{ address.as_byte_array()[ 2 ] },
            '.',
            Format::Dec{ address.as_byte_array()[ 3 ] } );
    }

    /**
     * \brief Write the formatted picolibrary::IPv4::Address to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::IPv4::Address to.
     * \param[in] address The picolibrary::IPv4::Address to format.
     *
     * \return The number of characters written to the stream.
     */
    auto print( Reliable_Output_Stream & stream, IPv4::Address const & address ) const noexcept
        -> std::size_t
    {
        return stream.print(
            Format::Dec{ address.as_byte_array()[ 0 ] },
            '.',
            Format::Dec{ address.as_byte_array()[ 1 ] },
            '.',
            Format::Dec{ address.as_byte_array()[ 2 ] },
            '.',
            Format::Dec{ address.as_byte_array()[ 3 ] } );
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_IPV4_H

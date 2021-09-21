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
 * \brief picolibrary::MAC_Address interface.
 */

#ifndef PICOLIBRARY_MAC_ADDRESS_H
#define PICOLIBRARY_MAC_ADDRESS_H

#include <cstdint>
#include <limits>

#include "picolibrary/bit_manipulation.h"
#include "picolibrary/error.h"
#include "picolibrary/fixed_size_array.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"
#include "picolibrary/void.h"

namespace picolibrary {

/**
 * \brief MAC address.
 */
class MAC_Address {
  public:
    /**
     * \brief Address byte array representation.
     */
    using Byte_Array = Fixed_Size_Array<std::uint8_t, 6>;

    /**
     * \brief Address unsigned integer representation.
     */
    using Unsigned_Integer = std::uint_least64_t;

    /**
     * \brief Get the minimum valid address.
     *
     * \return The minimum valid address.
     */
    static constexpr auto min() noexcept
    {
        return MAC_Address{ { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };
    }

    /**
     * \brief Get the maximum valid address.
     *
     * \return The maximum valid address.
     */
    static constexpr auto max() noexcept
    {
        return MAC_Address{ { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } };
    }

    /**
     * \brief Constructor.
     */
    constexpr MAC_Address() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] address The address in its byte array representation.
     */
    constexpr MAC_Address( Byte_Array const & address ) noexcept : m_address{ address }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] address The address in its unsigned integer representation.
     */
    constexpr MAC_Address( Unsigned_Integer const & address ) noexcept :
        m_address{
            static_cast<std::uint8_t>( address >> 5 * std::numeric_limits<std::uint8_t>::digits ),
            static_cast<std::uint8_t>( address >> 4 * std::numeric_limits<std::uint8_t>::digits ),
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
    constexpr MAC_Address( MAC_Address && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr MAC_Address( MAC_Address const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~MAC_Address() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( MAC_Address && expression ) noexcept -> MAC_Address & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( MAC_Address const & expression ) noexcept -> MAC_Address & = default;

    /**
     * \brief Check if the address is locally administered.
     *
     * \return true if the address is locally administered.
     * \return false if the address is not locally administered.
     */
    constexpr auto is_locally_administered() const noexcept
    {
        return static_cast<bool>( m_address[ 0 ] & Mask::UL );
    }

    /**
     * \brief Check if the address is universally administered.
     *
     * \return true if the address is universally administered.
     * \return false if the address is not universally administered.
     */
    constexpr auto is_universally_administered() const noexcept
    {
        return not is_locally_administered();
    }

    /**
     * \brief Check if the address is a multicast address.
     *
     * \return true if the address is a multicast address.
     * \return false if the address is not a multicast address.
     */
    constexpr auto is_multicast() const noexcept
    {
        return static_cast<bool>( m_address[ 0 ] & Mask::IG );
    }

    /**
     * \brief Check if the address is a unicast address.
     *
     * \return true if the address is a unicast address.
     * \return false if the address is not a unicast address.
     */
    constexpr auto is_unicast() const noexcept
    {
        return not is_multicast();
    }

    /**
     * \brief Get the address in its byte array representation.
     *
     * \return The address in its byte array representation.
     */
    constexpr auto as_byte_array() const noexcept
    {
        return m_address;
    }

    /**
     * \brief Get the address in its unsigned integer representation.
     *
     * \return The address in its unsigned integer representation.
     */
    constexpr auto as_unsigned_integer() const noexcept
    {
        return static_cast<Unsigned_Integer>(
            ( static_cast<Unsigned_Integer>( m_address[ 0 ] )
              << 5 * std::numeric_limits<std::uint8_t>::digits )
            | ( static_cast<Unsigned_Integer>( m_address[ 1 ] )
                << 4 * std::numeric_limits<std::uint8_t>::digits )
            | ( static_cast<Unsigned_Integer>( m_address[ 2 ] )
                << 3 * std::numeric_limits<std::uint8_t>::digits )
            | ( static_cast<Unsigned_Integer>( m_address[ 3 ] )
                << 2 * std::numeric_limits<std::uint8_t>::digits )
            | ( static_cast<Unsigned_Integer>( m_address[ 4 ] )
                << 1 * std::numeric_limits<std::uint8_t>::digits )
            | ( static_cast<Unsigned_Integer>( m_address[ 5 ] )
                << 0 * std::numeric_limits<std::uint8_t>::digits ) );
    }

  private:
    /**
     * \brief 1st octet field sizes.
     */
    struct Size {
        static constexpr auto IG = std::uint_fast8_t{ 1 }; ///< Individual/Group (I/G).
        static constexpr auto UL = std::uint_fast8_t{ 1 }; ///< Universal/Local (U/L).
    };

    /**
     * \brief 1st octet field bit positions.
     */
    struct Bit {
        static constexpr auto IG = std::uint_fast8_t{};                ///< I/G.
        static constexpr auto UL = std::uint_fast8_t{ IG + Size::IG }; ///< U/L.
    };

    /**
     * \brief 1st octet field bit masks.
     */
    struct Mask {
        static constexpr auto IG = mask<std::uint8_t>( Size::IG, Bit::IG ); ///< I/G.
        static constexpr auto UL = mask<std::uint8_t>( Size::UL, Bit::UL ); ///< U/L.
    };

    /**
     * \brief The address in its byte array representation.
     */
    Byte_Array m_address{};
};

/**
 * \brief Equality operator.
 *
 * \relatedalso picolibrary::MAC_Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is equal to rhs.
 * \return false if lhs is not equal to rhs.
 */
constexpr auto operator==( MAC_Address const & lhs, MAC_Address const & rhs ) noexcept
{
    return lhs.as_byte_array() == rhs.as_byte_array();
}

/**
 * \brief Inequality operator.
 *
 * \relatedalso picolibrary::MAC_Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is not equal to rhs.
 * \return false if lhs is equal to rhs.
 */
constexpr auto operator!=( MAC_Address const & lhs, MAC_Address const & rhs ) noexcept
{
    return not( lhs == rhs );
}

/**
 * \brief Less than operator.
 *
 * \relatedalso picolibrary::MAC_Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than rhs.
 * \return false if lhs is not less than rhs.
 */
constexpr auto operator<( MAC_Address const & lhs, MAC_Address const & rhs ) noexcept
{
    return lhs.as_unsigned_integer() < rhs.as_unsigned_integer();
}

/**
 * \brief Greater than operator.
 *
 * \relatedalso picolibrary::MAC_Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than rhs.
 * \return false if lhs is not greater than rhs.
 */
constexpr auto operator>( MAC_Address const & lhs, MAC_Address const & rhs ) noexcept
{
    return rhs < lhs;
}

/**
 * \brief Less than or equal to operator.
 *
 * \relatedalso picolibrary::MAC_Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than or equal to rhs.
 * \return false if lhs is not less than or equal to rhs.
 */
constexpr auto operator<=( MAC_Address const & lhs, MAC_Address const & rhs ) noexcept
{
    return not( lhs > rhs );
}

/**
 * \brief Greater than or equal to operator.
 *
 * \relatedalso picolibrary::MAC_Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than or equal to rhs.
 * \return false if lhs is not greater than or equal to rhs.
 */
constexpr auto operator>=( MAC_Address const & lhs, MAC_Address const & rhs ) noexcept
{
    return not( lhs < rhs );
}

/**
 * \brief picolibrary::MAC_Address output formatter.
 *
 * picolibrary::MAC_Address only supports the default format specification ("{}").
 */
template<>
class Output_Formatter<MAC_Address> {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Output_Formatter() noexcept = default;

    Output_Formatter( Output_Formatter && ) = delete;

    Output_Formatter( Output_Formatter const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Output_Formatter() noexcept = default;

    auto operator=( Output_Formatter && ) = delete;

    auto operator=( Output_Formatter const & ) = delete;

    /**
     * \brief Parse the format specification for the picolibrary::MAC_Address to be
     *        formatted.
     *
     * \param[in] format The format specification for the picolibrary::MAC_Address to be
     *            formatted.
     *
     * \return format.
     */
    constexpr auto parse( char const * format ) noexcept -> Result<char const *, Void>
    {
        return format;
    }

    /**
     * \brief Write the picolibrary::MAC_Address to the stream.
     *
     * \param[in] stream The stream to write the picolibrary::MAC_Address to.
     * \param[in] address The picolibrary::MAC_Address to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, MAC_Address const & address ) noexcept -> Result<Void, Error_Code>
    {
        // #lizard forgives the length

        constexpr auto address_bytes = fixed_size_array_size_v<MAC_Address::Byte_Array>;
        constexpr auto byte_nibbles = std::numeric_limits<MAC_Address::Byte_Array::Value>::digits / 4;
        constexpr auto address_nibbles = address_bytes * byte_nibbles;

        Fixed_Size_Array<char, address_nibbles + ( address_bytes - 1 )> formatted_address;

        auto i = formatted_address.begin();
        for ( auto const byte : address.as_byte_array() ) {
            Fixed_Size_Array<std::uint8_t, byte_nibbles> nibbles{
                static_cast<std::uint8_t>( byte >> 4 ),
                static_cast<std::uint8_t>( byte & 0xF ),
            };

            for ( auto const nibble : nibbles ) {
                *i = nibble < 0xA ? nibble + '0' : nibble - 0xA + 'A';

                ++i;
            } // for

            if ( i != formatted_address.end() ) {
                *i = '-';

                ++i;
            } // if
        }     // for

        return stream.put( formatted_address.begin(), formatted_address.end() );
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_MAC_ADDRESS_H
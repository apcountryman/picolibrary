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
 * \brief picolibrary::MAC_Address interface.
 */

#ifndef PICOLIBRARY_MAC_ADDRESS_H
#define PICOLIBRARY_MAC_ADDRESS_H

#include <cstddef>
#include <cstdint>
#include <limits>

#include "picolibrary/array.h"
#include "picolibrary/bit_manipulation.h"
#include "picolibrary/precondition.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"

namespace picolibrary {

/**
 * \brief MAC address.
 */
class MAC_Address {
  public:
    /**
     * \brief Address byte array representation.
     */
    using Byte_Array = Array<std::uint8_t, 6>;

    /**
     * \brief Address unsigned integer representation.
     */
    using Unsigned_Integer = std::uint_fast64_t;

    /**
     * \brief Get the minimum valid address.
     *
     * \return The minimum valid address.
     */
    static constexpr auto min() noexcept -> MAC_Address
    {
        return MAC_Address{ { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };
    }

    /**
     * \brief Get the maximum valid address.
     *
     * \return The maximum valid address.
     */
    static constexpr auto max() noexcept -> MAC_Address
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
     * \param[in] address The address.
     */
    constexpr MAC_Address( Byte_Array const & address ) noexcept : m_address{ address }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] address The address.
     *
     * \pre address <= picolibrary::MAC_Address::max().as_unsigned_integer()
     */
    constexpr MAC_Address( Unsigned_Integer const & address ) noexcept :
        MAC_Address{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, address }
    {
        PICOLIBRARY_EXPECT( address <= max().as_unsigned_integer(), Generic_Error::INVALID_ARGUMENT );
    }

    /**
     * \brief Constructor.
     *
     * \param[in] address The address.
     */
    constexpr MAC_Address( Bypass_Precondition_Expectation_Checks, Unsigned_Integer const & address ) noexcept :
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
    constexpr auto is_locally_administered() const noexcept -> bool
    {
        return m_address[ 0 ] & Mask::UL;
    }

    /**
     * \brief Check if the address is universally administered.
     *
     * \return true if the address is universally administered.
     * \return false if the address is not universally administered.
     */
    constexpr auto is_universally_administered() const noexcept -> bool
    {
        return not is_locally_administered();
    }

    /**
     * \brief Check if the address is a multicast address.
     *
     * \return true if the address is a multicast address.
     * \return false if the address is not a multicast address.
     */
    constexpr auto is_multicast() const noexcept -> bool
    {
        return m_address[ 0 ] & Mask::IG;
    }

    /**
     * \brief Check if the address is a unicast address.
     *
     * \return true if the address is a unicast address.
     * \return false if the address is not a unicast address.
     */
    constexpr auto is_unicast() const noexcept -> bool
    {
        return not is_multicast();
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
            ( static_cast<Unsigned_Integer>( m_address[ 0 ] ) << 5 * std::numeric_limits<std::uint8_t>::digits ) |
            ( static_cast<Unsigned_Integer>( m_address[ 1 ] ) << 4 * std::numeric_limits<std::uint8_t>::digits ) |
            ( static_cast<Unsigned_Integer>( m_address[ 2 ] ) << 3 * std::numeric_limits<std::uint8_t>::digits ) |
            ( static_cast<Unsigned_Integer>( m_address[ 3 ] ) << 2 * std::numeric_limits<std::uint8_t>::digits ) |
            ( static_cast<Unsigned_Integer>( m_address[ 4 ] ) << 1 * std::numeric_limits<std::uint8_t>::digits ) |
            ( static_cast<Unsigned_Integer>( m_address[ 5 ] ) << 0 * std::numeric_limits<std::uint8_t>::digits ) );

        // clang-format on
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
constexpr auto operator==( MAC_Address const & lhs, MAC_Address const & rhs ) noexcept -> bool
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
constexpr auto operator!=( MAC_Address const & lhs, MAC_Address const & rhs ) noexcept -> bool
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
constexpr auto operator<( MAC_Address const & lhs, MAC_Address const & rhs ) noexcept -> bool
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
constexpr auto operator>( MAC_Address const & lhs, MAC_Address const & rhs ) noexcept -> bool
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
constexpr auto operator<=( MAC_Address const & lhs, MAC_Address const & rhs ) noexcept -> bool
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
constexpr auto operator>=( MAC_Address const & lhs, MAC_Address const & rhs ) noexcept -> bool
{
    return not( lhs < rhs );
}

/**
 * \brief picolibrary::MAC_Address output formatter.
 */
template<>
class Output_Formatter<MAC_Address> {
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
     * \brief Write the formatted picolibrary::MAC_Address to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::MAC_Address to.
     * \param[in] address The picolibrary::MAC_Address to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, MAC_Address const & address ) const noexcept
        -> Result<std::size_t>
    {
        auto const formatted_address = format( address );

        auto result = stream.put( formatted_address.begin(), formatted_address.end() );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return formatted_address.size();
    }

    /**
     * \brief Write the formatted picolibrary::MAC_Address to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::MAC_Address to.
     * \param[in] address The picolibrary::MAC_Address to format.
     *
     * \return The number of characters written to the stream.
     */
    auto print( Reliable_Output_Stream & stream, MAC_Address const & address ) const noexcept
        -> std::size_t
    {
        auto const formatted_address = format( address );

        stream.put( formatted_address.begin(), formatted_address.end() );

        return formatted_address.size();
    }

  private:
    /**
     * \brief The number of bits in a nibble.
     */
    static constexpr auto NIBBLE_DIGITS = 4;

    /**
     * \brief The largest value a nibble can hold.
     */
    static constexpr auto NIBBLE_MAX = std::uint_fast8_t{ 0xF };

    /**
     * \brief The number of bits in a byte.
     */
    static constexpr auto BYTE_DIGITS = std::numeric_limits<std::uint8_t>::digits;

    /**
     * \brief The number of nibbles in a byte.
     */
    static constexpr auto BYTE_NIBBLES = BYTE_DIGITS / NIBBLE_DIGITS;

    /**
     * \brief The number of bytes in a MAC address.
     */
    static constexpr auto ADDRESS_BYTES = array_size_v<MAC_Address::Byte_Array>;

    /**
     * \brief The number of nibbles in a MAC address.
     */
    static constexpr auto ADDRESS_NIBBLES = ADDRESS_BYTES * BYTE_NIBBLES;

    /**
     * \brief Formatted MAC address.
     */
    using Formatted_Address = Array<char, ADDRESS_NIBBLES + ( ADDRESS_BYTES - 1 )>;

    /**
     * \brief Get the pair of nibbles that make up a byte (most significant nibble first).
     *
     * \param[in] byte The byte to get the pair of nibbles from.
     *
     * \return The pair of nibbles that make up the byte (most significant nibble first).
     */
    static constexpr auto get_byte_nibbles( std::uint8_t byte ) noexcept
    {
        return Array<std::uint_fast8_t, BYTE_NIBBLES>{
            static_cast<std::uint_fast8_t>( byte >> NIBBLE_DIGITS ),
            static_cast<std::uint_fast8_t>( byte & NIBBLE_MAX ),
        };
    }

    /**
     * \brief Format a picolibrary::MAC_Address.
     *
     * \param[in] address The picolibrary::MAC_Address to format.
     *
     * \return The formatted picolibrary::MAC_Address.
     */
    static auto format( MAC_Address const & address ) noexcept -> Formatted_Address
    {
        Formatted_Address formatted_address;

        auto i = formatted_address.begin();
        for ( auto const byte : address.as_byte_array() ) {
            auto const nibbles = get_byte_nibbles( byte );

            for ( auto const nibble : nibbles ) {
                *i = static_cast<char>( nibble < 0xA ? '0' + nibble : 'A' + ( nibble - 0xA ) );

                ++i;
            } // for

            if ( i != formatted_address.end() ) {
                *i = '-';

                ++i;
            } // if
        }     // for

        return formatted_address;
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_MAC_ADDRESS_H

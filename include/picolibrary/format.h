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
 * \brief picolibrary::Format interface.
 */

#ifndef PICOLIBRARY_FORMAT_H
#define PICOLIBRARY_FORMAT_H

#include <cstdlib>
#include <cstring>
#include <limits>
#include <type_traits>

#include "picolibrary/error.h"
#include "picolibrary/fixed_size_array.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"
#include "picolibrary/void.h"

/**
 * \brief Formatting facilities.
 */
namespace picolibrary::Format {

/**
 * \brief Integer binary output format specifier.
 *
 * \tparam Integer The type of integer to be printed.
 */
template<typename Integer>
class Binary {
  public:
    static_assert( std::is_integral_v<Integer> );

    Binary() = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] value The integer to be printed.
     */
    constexpr Binary( Integer value ) noexcept : m_value{ value }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Binary( Binary && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Binary( Binary const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Binary() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Binary && expression ) noexcept -> Binary & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Binary const & expression ) noexcept -> Binary & = default;

    /**
     * \brief Get the integer to be printed.
     *
     * \return The integer to be printed.
     */
    constexpr operator Integer() const noexcept
    {
        return m_value;
    }

  private:
    /**
     * \brief The integer to be printed.
     */
    Integer m_value{};
};

/**
 * \brief Integer decimal output format specifier.
 *
 * \tparam Integer The type of integer to be printed.
 */
template<typename Integer>
class Decimal {
  public:
    static_assert( std::is_integral_v<Integer> );

    Decimal() = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] value The integer to be printed.
     */
    constexpr Decimal( Integer value ) noexcept : m_value{ value }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Decimal( Decimal && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Decimal( Decimal const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Decimal() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Decimal && expression ) noexcept -> Decimal & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Decimal const & expression ) noexcept -> Decimal & = default;

    /**
     * \brief Get the integer to be printed.
     *
     * \return The integer to be printed.
     */
    constexpr operator Integer() const noexcept
    {
        return m_value;
    }

  private:
    /**
     * \brief The integer to be printed.
     */
    Integer m_value{};
};

/**
 * \brief Integer hexadecimal output format specifier.
 *
 * \tparam Integer The type of integer to be printed.
 */
template<typename Integer>
class Hexadecimal {
  public:
    static_assert( std::is_integral_v<Integer> );

    Hexadecimal() = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] value The integer to be printed.
     */
    constexpr Hexadecimal( Integer value ) noexcept : m_value{ value }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Hexadecimal( Hexadecimal && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Hexadecimal( Hexadecimal const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Hexadecimal() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Hexadecimal && expression ) noexcept -> Hexadecimal & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Hexadecimal const & expression ) noexcept -> Hexadecimal & = default;

    /**
     * \brief Get the integer to be printed.
     *
     * \return The integer to be printed.
     */
    constexpr operator Integer() const noexcept
    {
        return m_value;
    }

  private:
    /**
     * \brief The integer to be printed.
     */
    Integer m_value{};
};

} // namespace picolibrary::Format

namespace picolibrary {

/**
 * \brief Integer binary output formatter.
 *
 * picolibrary::Format::Binary only supports the default format specification ("{}").
 *
 * \tparam Integer The type of integer to be printed.
 */
template<typename Integer>
class Output_Formatter<Format::Binary<Integer>> {
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
     * \brief Parse the format specification for the integer to be formatted.
     *
     * \param[in] format The format specification for the integer to be formatted.
     *
     * \return format.
     */
    constexpr auto parse( char const * format ) noexcept -> Result<char const *, Void>
    {
        return format;
    }

    /**
     * \brief Write the integer to the stream.
     *
     * \param[in] stream The stream to write the integer to.
     * \param[in] value The integer to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, Integer value ) noexcept
    {
        using U = std::make_unsigned_t<Integer>;

        U u;
        static_assert( sizeof( u ) == sizeof( value ) );
        std::memcpy( &u, &value, sizeof( value ) );

        constexpr auto bits = std::numeric_limits<U>::digits;

        Fixed_Size_Array<char, 2 + bits> binary;

        auto i = binary.rbegin();
        for ( auto bit = 0; bit < bits; ++bit ) {
            *i = ( u & 0b1 ) + '0';

            ++i;
            u >>= 1;
        } // for
        *i = 'b';
        ++i;
        *i = '0';

        return stream.put( binary.begin(), binary.end() );
    }
};

/**
 * \brief Signed integer decimal output formatter.
 *
 * picolibrary::Format::Decimal only supports the default format specification ("{}").
 *
 * \tparam Integer The type of integer to be printed.
 */
template<typename Integer>
class Output_Formatter<Format::Decimal<Integer>, std::enable_if_t<std::is_signed_v<Integer>>> {
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
     * \brief Parse the format specification for the integer to be formatted.
     *
     * \param[in] format The format specification for the integer to be formatted.
     *
     * \return format.
     */
    constexpr auto parse( char const * format ) noexcept -> Result<char const *, Void>
    {
        return format;
    }

    /**
     * \brief Write the integer to the stream.
     *
     * \param[in] stream The stream to write the integer to.
     * \param[in] value The integer to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, Integer value ) noexcept -> Result<Void, Error_Code>
    {
        Fixed_Size_Array<char, 1 + std::numeric_limits<Integer>::digits10 + 1> decimal;

        auto is_negative = value < 0;

        auto i = decimal.rbegin();

        do {
            *i = std::abs( value % 10 ) + '0';

            ++i;
            value /= 10;
        } while ( value );

        if ( is_negative ) {
            *i = '-';

            ++i;
        } // if

        return stream.put( i.base(), decimal.end() );
    }
};

/**
 * \brief Unsigned integer decimal output formatter.
 *
 * picolibrary::Format::Decimal only supports the default format specification ("{}").
 *
 * \tparam Integer The type of integer to be printed.
 */
template<typename Integer>
class Output_Formatter<Format::Decimal<Integer>, std::enable_if_t<std::is_unsigned_v<Integer>>> {
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
     * \brief Parse the format specification for the integer to be formatted.
     *
     * \param[in] format The format specification for the integer to be formatted.
     *
     * \return format.
     */
    constexpr auto parse( char const * format ) noexcept -> Result<char const *, Void>
    {
        return format;
    }

    /**
     * \brief Write the integer to the stream.
     *
     * \param[in] stream The stream to write the integer to.
     * \param[in] value The integer to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, Integer value ) noexcept -> Result<Void, Error_Code>
    {
        Fixed_Size_Array<char, std::numeric_limits<Integer>::digits10 + 1> decimal;

        auto i = decimal.rbegin();
        do {
            *i = ( value % 10 ) + '0';

            ++i;
            value /= 10;
        } while ( value );

        return stream.put( i.base(), decimal.end() );
    }
};

/**
 * \brief Integer hexadecimal output formatter.
 *
 * picolibrary::Format::Hexadecimal only supports the default format specification ("{}").
 *
 * \tparam Integer The type of integer to be printed.
 */
template<typename Integer>
class Output_Formatter<Format::Hexadecimal<Integer>> {
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
     * \brief Parse the format specification for the integer to be formatted.
     *
     * \param[in] format The format specification for the integer to be formatted.
     *
     * \return format.
     */
    constexpr auto parse( char const * format ) noexcept -> Result<char const *, Void>
    {
        return format;
    }

    /**
     * \brief Write the integer to the stream.
     *
     * \param[in] stream The stream to write the integer to.
     * \param[in] value The integer to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, Integer value ) noexcept
    {
        // #lizard forgives the length

        using U = std::make_unsigned_t<Integer>;

        U u;
        static_assert( sizeof( u ) == sizeof( value ) );
        std::memcpy( &u, &value, sizeof( value ) );

        constexpr auto nibbles = std::numeric_limits<U>::digits / 4;

        Fixed_Size_Array<char, 2 + nibbles> hexadecimal;

        auto i = hexadecimal.rbegin();
        for ( auto nibble = 0; nibble < nibbles; ++nibble ) {
            auto const n = u & 0xF;

            *i = n < 0xA ? n + '0' : n - 0xA + 'A';

            ++i;
            u >>= 4;
        } // for
        *i = 'x';
        ++i;
        *i = '0';

        return stream.put( hexadecimal.begin(), hexadecimal.end() );
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_FORMAT_H

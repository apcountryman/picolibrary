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
 * \brief picolibrary::Format interface.
 */

#ifndef PICOLIBRARY_FORMAT_H
#define PICOLIBRARY_FORMAT_H

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <type_traits>

#include "picolibrary/array.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"

/**
 * \brief Formatting facilities.
 */
namespace picolibrary::Format {

/**
 * \brief Integer binary output format specifier.
 *
 * \tparam Integer The integer type to format.
 */
template<typename Integer>
class Binary {
  public:
    static_assert( std::is_integral_v<Integer> );

    Binary() = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] value The integer to be formatted.
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
     * \brief Get the integer to be formatted.
     *
     * \return The integer to be formatted.
     */
    constexpr operator Integer() const noexcept
    {
        return m_value;
    }

  private:
    /**
     * \brief The integer to be formatted.
     */
    Integer m_value{};
};

/**
 * \brief Integer decimal output format specifier.
 *
 * \tparam Integer The integer type to format.
 */
template<typename Integer>
class Decimal {
  public:
    static_assert( std::is_integral_v<Integer> );

    Decimal() = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] value The integer to be formatted.
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
     * \brief Get the integer to be formatted.
     *
     * \return The integer to be formatted.
     */
    constexpr operator Integer() const noexcept
    {
        return m_value;
    }

  private:
    /**
     * \brief The integer to be formatted.
     */
    Integer m_value{};
};

} // namespace picolibrary::Format

namespace picolibrary {

/**
 * \brief picolibrary::Format::Binary output formatter.
 *
 * picolibrary::Format::Binary only supports the default format specification ("{}").
 *
 * \tparam Integer The type of integer to print.
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
     * \brief Parse the format specification for the picolibrary::Format::Binary to be
     *        formatted.
     *
     * \param[in] format The format specification for the picolibrary::Format::Binary to
     *            be formatted.
     *
     * \return format
     */
    constexpr auto parse( char const * format ) noexcept
    {
        return format;
    }

    /**
     * \brief Write the formatted picolibrary::Format::Binary to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::Format::Binary to.
     * \param[in] value The integer to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, Integer value ) noexcept -> Result<std::size_t, Error_Code>
    {
        // #lizard forgives the length

        using U = std::make_unsigned_t<Integer>;

        constexpr auto bits = std::numeric_limits<U>::digits;

        U u;
        static_assert( sizeof( u ) == sizeof( value ) );
        std::memcpy( &u, &value, sizeof( value ) );

        Array<char, 2 + bits> binary;

        auto i = binary.rbegin();
        for ( auto bit = 0; bit < bits; ++bit ) {
            *i = '0' + ( u & 0b1 );

            ++i;
            u >>= 1;
        } // for
        *i = 'b';
        ++i;
        *i = '0';

        auto result = stream.put( binary.begin(), binary.end() );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return binary.size();
    }
};

/**
 * \brief picolibrary::Format::Decimal output formatter.
 *
 * picolibrary::Format::Decimal only supports the default format specification ("{}").
 *
 * \tparam Integer The type of integer to print.
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
     * \brief Parse the format specification for the picolibrary::Format::Decimal to be
     *        formatted.
     *
     * \param[in] format The format specification for the picolibrary::Format::Decimal to
     *            be formatted.
     *
     * \return format
     */
    constexpr auto parse( char const * format ) noexcept
    {
        return format;
    }

    /**
     * \brief Write the formatted picolibrary::Format::Decimal to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::Format::Decimal
     *            to.
     * \param[in] value The integer to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, Integer value ) noexcept -> Result<std::size_t, Error_Code>
    {
        Array<char, 1 + std::numeric_limits<Integer>::digits10 + 1> decimal;

        auto is_negative = value < 0;

        auto i = decimal.rbegin();

        do {
            *i = '0' + std::abs( value % 10 );

            ++i;
            value /= 10;
        } while ( value );

        if ( is_negative ) {
            *i = '-';

            ++i;
        } // if

        auto result = stream.put( i.base(), decimal.end() );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return static_cast<std::size_t>( decimal.end() - i.base() );
    }
};

/**
 * \brief picolibrary::Format::Decimal output formatter.
 *
 * picolibrary::Format::Decimal only supports the default format specification ("{}").
 *
 * \tparam Integer The type of integer to print.
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
     * \brief Parse the format specification for the picolibrary::Format::Decimal to be
     *        formatted.
     *
     * \param[in] format The format specification for the picolibrary::Format::Decimal to
     *            be formatted.
     *
     * \return format
     */
    constexpr auto parse( char const * format ) noexcept
    {
        return format;
    }

    /**
     * \brief Write the formatted picolibrary::Format::Decimal to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::Format::Decimal
     *            to.
     * \param[in] value The integer to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, Integer value ) noexcept -> Result<std::size_t, Error_Code>
    {
        Array<char, std::numeric_limits<Integer>::digits10 + 1> decimal;

        auto i = decimal.rbegin();
        do {
            *i = '0' + ( value % 10 );

            ++i;
            value /= 10;
        } while ( value );

        auto result = stream.put( i.base(), decimal.end() );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return static_cast<std::size_t>( decimal.end() - i.base() );
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_FORMAT_H

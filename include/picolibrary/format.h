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
#include <limits>
#include <type_traits>

#include "picolibrary/array.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"
#include "picolibrary/utility.h"

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
class Bin {
  public:
    static_assert( std::is_integral_v<Integer> );

    Bin() = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] value The integer to be formatted.
     */
    constexpr Bin( Integer value ) noexcept : m_value{ value }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Bin( Bin && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Bin( Bin const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Bin() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Bin && expression ) noexcept -> Bin & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Bin const & expression ) noexcept -> Bin & = default;

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
class Dec {
  public:
    static_assert( std::is_integral_v<Integer> );

    Dec() = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] value The integer to be formatted.
     */
    constexpr Dec( Integer value ) noexcept : m_value{ value }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Dec( Dec && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Dec( Dec const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Dec() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Dec && expression ) noexcept -> Dec & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Dec const & expression ) noexcept -> Dec & = default;

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
 * \brief Integer hexadecimal output format specifier.
 *
 * \tparam Integer The integer type to format.
 */
template<typename Integer>
class Hex {
  public:
    static_assert( std::is_integral_v<Integer> );

    Hex() = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] value The integer to be formatted.
     */
    constexpr Hex( Integer value ) noexcept : m_value{ value }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Hex( Hex && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Hex( Hex const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Hex() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Hex && expression ) noexcept -> Hex & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Hex const & expression ) noexcept -> Hex & = default;

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
 * \brief picolibrary::Format::Bin output formatter.
 *
 * \tparam Integer The type of integer to print.
 */
template<typename Integer>
class Output_Formatter<Format::Bin<Integer>> {
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
     * \brief Write the formatted picolibrary::Format::Bin to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::Format::Bin to.
     * \param[in] integer The integer to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, Integer integer ) const noexcept
        -> Result<std::size_t, Error_Code>
    {
        auto const formatted_integer = format( integer );

        auto result = stream.put( formatted_integer.begin(), formatted_integer.end() );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return formatted_integer.size();
    }

    /**
     * \brief Write the formatted picolibrary::Format::Bin to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::Format::Bin to.
     * \param[in] integer The integer to format.
     *
     * \return The number of characters written to the stream.
     */
    auto print( Reliable_Output_Stream & stream, Integer integer ) const noexcept
    {
        auto const formatted_integer = format( integer );

        stream.put( formatted_integer.begin(), formatted_integer.end() );

        return formatted_integer.size();
    }

  private:
    /**
     * \brief The number of bits in an unsigned integer.
     */
    static constexpr auto DIGITS = std::numeric_limits<std::make_unsigned_t<Integer>>::digits;

    /**
     * \brief Formatted integer.
     */
    using Formatted_Integer = Array<char, 2 + DIGITS>;

    /**
     * \brief Format an integer.
     *
     * \param[in] integer The integer to format.
     *
     * \return The formatted integer.
     */
    static constexpr auto format( Integer integer ) noexcept -> Formatted_Integer
    {
        auto unsigned_integer = to_unsigned( integer );

        Formatted_Integer formatted_integer;

        auto i = formatted_integer.rbegin();
        for ( auto bit = 0; bit < DIGITS; ++bit ) {
            *i = '0' + ( unsigned_integer & 0b1 );

            ++i;
            unsigned_integer >>= 1;
        } // for
        *i = 'b';
        ++i;
        *i = '0';

        return formatted_integer;
    }
};

/**
 * \brief picolibrary::Format::Dec output formatter.
 *
 * \tparam Integer The type of integer to print.
 */
template<typename Integer>
class Output_Formatter<Format::Dec<Integer>, std::enable_if_t<std::is_signed_v<Integer>>> {
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
     * \brief Write the formatted picolibrary::Format::Dec to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::Format::Dec to.
     * \param[in] integer The integer to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, Integer integer ) const noexcept
        -> Result<std::size_t, Error_Code>
    {
        Formatted_Integer formatted_integer;

        auto const begin = format( integer, formatted_integer );

        auto result = stream.put( begin, formatted_integer.end() );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return static_cast<std::size_t>( formatted_integer.end() - begin );
    }

    /**
     * \brief Write the formatted picolibrary::Format::Dec to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::Format::Dec to.
     * \param[in] integer The integer to format.
     *
     * \return The number of characters written to the stream.
     */
    auto print( Reliable_Output_Stream & stream, Integer integer ) const noexcept -> std::size_t
    {
        Formatted_Integer formatted_integer;

        auto const begin = format( integer, formatted_integer );

        stream.put( begin, formatted_integer.end() );

        return static_cast<std::size_t>( formatted_integer.end() - begin );
    }

  private:
    /**
     * \brief Formatted integer.
     */
    using Formatted_Integer = Array<char, 1 + std::numeric_limits<Integer>::digits10 + 1>;

    /**
     * \brief Format an integer.
     *
     * \param[in] integer The integer to format.
     * \param[out] formatted_integer The array to write the formatted integer to.
     *
     * \return An iterator to the beginning of the formatted integer.
     */
    static constexpr auto format( Integer integer, Formatted_Integer & formatted_integer ) noexcept
    {
        auto is_negative = integer < 0;

        auto i = formatted_integer.rbegin();

        do {
            *i = '0' + std::abs( integer % 10 );

            ++i;
            integer /= 10;
        } while ( integer );

        if ( is_negative ) {
            *i = '-';

            ++i;
        } // if

        return i.base();
    }
};

/**
 * \brief picolibrary::Format::Dec output formatter.
 *
 * \tparam Integer The type of integer to print.
 */
template<typename Integer>
class Output_Formatter<Format::Dec<Integer>, std::enable_if_t<std::is_unsigned_v<Integer>>> {
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
     * \brief Write the formatted picolibrary::Format::Dec to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::Format::Dec to.
     * \param[in] integer The integer to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, Integer integer ) const noexcept
        -> Result<std::size_t, Error_Code>
    {
        Formatted_Integer formatted_integer;

        auto const begin = format( integer, formatted_integer );

        auto result = stream.put( begin, formatted_integer.end() );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return static_cast<std::size_t>( formatted_integer.end() - begin );
    }

    /**
     * \brief Write the formatted picolibrary::Format::Dec to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::Format::Dec to.
     * \param[in] integer The integer to format.
     *
     * \return The number of characters written to the stream.
     */
    auto print( Reliable_Output_Stream & stream, Integer integer ) const noexcept -> std::size_t
    {
        Formatted_Integer formatted_integer;

        auto const begin = format( integer, formatted_integer );

        stream.put( begin, formatted_integer.end() );

        return static_cast<std::size_t>( formatted_integer.end() - begin );
    }

  private:
    /**
     * \brief Formatted integer.
     */
    using Formatted_Integer = Array<char, std::numeric_limits<Integer>::digits10 + 1>;

    /**
     * \brief Format an integer.
     *
     * \param[in] integer The integer to format.
     * \param[in] formatted_integer The array to write the formatted integer to.
     *
     * \return An iterator to the beginning of the formatted integer.
     */
    static constexpr auto format( Integer integer, Formatted_Integer & formatted_integer ) noexcept
    {
        auto i = formatted_integer.rbegin();
        do {
            *i = '0' + ( integer % 10 );

            ++i;
            integer /= 10;
        } while ( integer );
        return i.base();
    }
};

/**
 * \brief picolibrary::Format::Hex output formatter.
 *
 * \tparam Integer The type of integer to print.
 */
template<typename Integer>
class Output_Formatter<Format::Hex<Integer>> {
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
     * \brief Write the formatted picolibrary::Format::Hex to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::Format::Hex to.
     * \param[in] integer The integer to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, Integer integer ) const noexcept
        -> Result<std::size_t, Error_Code>
    {
        auto const formatted_integer = format( integer );

        auto result = stream.put( formatted_integer.begin(), formatted_integer.end() );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return formatted_integer.size();
    }

    /**
     * \brief Write the formatted picolibrary::Format::Hex to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::Format::Hex to.
     * \param[in] integer The integer to format.
     *
     * \return The number of characters written to the stream.
     */
    auto print( Reliable_Output_Stream & stream, Integer integer ) const noexcept -> std::size_t
    {
        auto const formatted_integer = format( integer );

        stream.put( formatted_integer.begin(), formatted_integer.end() );

        return formatted_integer.size();
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
     * \brief The number of nibbles in an unsigned integer.
     */
    static constexpr auto NIBBLES = std::numeric_limits<std::make_unsigned_t<Integer>>::digits / NIBBLE_DIGITS;

    /**
     * \brief Formatted integer.
     */
    using Formatted_Integer = Array<char, 2 + NIBBLES>;

    /**
     * \brief Format an integer.
     *
     * \param[in] integer The integer to format.
     *
     * \return The formatted integer.
     */
    static constexpr auto format( Integer integer ) noexcept -> Formatted_Integer
    {
        auto unsigned_integer = to_unsigned( integer );

        Formatted_Integer formatted_integer;

        auto i = formatted_integer.rbegin();
        for ( auto nibble = 0; nibble < NIBBLES; ++nibble ) {
            auto const n = unsigned_integer & NIBBLE_MAX;

            *i = n < 0xA ? '0' + n : 'A' + ( n - 0xA );

            ++i;
            unsigned_integer >>= NIBBLE_DIGITS;
        } // for
        *i = 'x';
        ++i;
        *i = '0';

        return formatted_integer;
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_FORMAT_H

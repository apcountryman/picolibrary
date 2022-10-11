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

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <type_traits>

#include "picolibrary/algorithm.h"
#include "picolibrary/array.h"
#include "picolibrary/bit_manipulation.h"
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

/**
 * \brief Hex dump output format specifier.
 */
class Hex_Dump {
  public:
    Hex_Dump() = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] begin The beginning of the block of memory to dump.
     * \param[in] end The end of the block of memory to dump.
     */
    constexpr Hex_Dump( void const * begin, void const * end ) noexcept :
        m_begin{ begin },
        m_end{ end }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Hex_Dump( Hex_Dump && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Hex_Dump( Hex_Dump const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Hex_Dump() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Hex_Dump && expression ) noexcept -> Hex_Dump & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Hex_Dump const & expression ) noexcept -> Hex_Dump & = default;

    /**
     * \brief Get the beginning of the block of memory to dump.
     *
     * \return The beginning of the block of memory to dump.
     */
    constexpr auto begin() const noexcept -> void const *
    {
        return m_begin;
    }

    /**
     * \brief Get the end of the block of memory to dump.
     *
     * \return The end of the block of memory to dump.
     */
    constexpr auto end() const noexcept -> void const *
    {
        return m_end;
    }

  private:
    /**
     * \brief The beginning of the block of memory to dump.
     */
    void const * m_begin{};

    /**
     * \brief The end of the block of memory to dump.
     */
    void const * m_end{};
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

/**
 * \brief picolibrary::Format::Hex_Dump output formatter.
 */
template<>
class Output_Formatter<Format::Hex_Dump> {
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
     * \brief Write the formatted picolibrary::Format::Hex_Dump to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::Format::Hex_Dump
     *            to.
     * \param[in] hex_dump The picolibrary::Format::Hex_Dump to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, Format::Hex_Dump const & hex_dump ) const noexcept
        -> Result<std::size_t, Error_Code>
    {
        Row_Buffer row_buffer;

        auto begin = static_cast<std::uint8_t const *>( hex_dump.begin() );
        auto end   = static_cast<std::uint8_t const *>( hex_dump.end() );
        auto n     = std::size_t{ 0 };

        for ( auto offset = std::uintptr_t{ 0 }; begin != end;
              offset += 16, n += row_buffer.size() ) {
            begin = generate_row( offset, begin, end, row_buffer );

            auto result = stream.put( row_buffer.begin(), row_buffer.end() );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }     // for

        return n;
    }

    /**
     * \brief Write the formatted picolibrary::Format::Hex_Dump to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::Format::Hex_Dump
     *            to.
     * \param[in] hex_dump The picolibrary::Format::Hex_Dump to format.
     *
     * \return The number of characters written to the stream.
     */
    auto print( Reliable_Output_Stream & stream, Format::Hex_Dump const & hex_dump ) const noexcept
        -> std::size_t
    {
        Row_Buffer row_buffer;

        auto begin = static_cast<std::uint8_t const *>( hex_dump.begin() );
        auto end   = static_cast<std::uint8_t const *>( hex_dump.end() );
        auto n     = std::size_t{ 0 };

        for ( auto offset = std::uintptr_t{ 0 }; begin != end;
              offset += 16, n += row_buffer.size() ) {
            begin = generate_row( offset, begin, end, row_buffer );

            stream.put( row_buffer.begin(), row_buffer.end() );
        } // for

        return n;
    }

  private:
    /**
     * \brief The number of bits in a nibble.
     */
    static constexpr auto NIBBLE_DIGITS = std::uint_fast8_t{ 4 };

    /**
     * \brief Nibble bit mask.
     */
    static constexpr auto NIBBLE_MASK = mask<std::uint_fast8_t>( NIBBLE_DIGITS, 0 );

    /**
     * \brief The number of nibbles in an offset.
     */
    static constexpr auto OFFSET_NIBBLES = std::uint_fast8_t{ std::numeric_limits<std::uintptr_t>::digits
                                                              / NIBBLE_DIGITS };

    /**
     * \brief The number of nibbles in a byte.
     */
    static constexpr auto BYTE_NIBBLES = std::uint_fast8_t{ std::numeric_limits<std::uint8_t>::digits
                                                            / NIBBLE_DIGITS };

    /**
     * \brief Group separation spaces.
     */
    static constexpr auto GROUP_SEPARATION = std::uint_fast8_t{ 2 };

    /**
     * \brief The number of bytes in a row.
     */
    static constexpr auto ROW_BYTES = std::uint_fast8_t{ 16 };

    /**
     * \brief Row buffer offset (hex) index.
     */
    static constexpr auto OFFSET_HEX_INDEX = std::uint_fast8_t{ 0 };

    /**
     * \brief Row buffer data (hex) index.
     */
    static constexpr auto DATA_HEX_INDEX = std::uint_fast8_t{ OFFSET_HEX_INDEX + OFFSET_NIBBLES
                                                              + GROUP_SEPARATION };

    /**
     * \brief Row buffer data (ASCII) index.
     */
    static constexpr auto DATA_ASCII_INDEX = std::uint_fast8_t{
        DATA_HEX_INDEX + ( ( ( BYTE_NIBBLES + 1 ) * ROW_BYTES ) - 1 ) + GROUP_SEPARATION + 1
    };

    /**
     * \brief Row buffer.
     */
    using Row_Buffer =
        Array<char, OFFSET_NIBBLES + GROUP_SEPARATION + ( ( ( BYTE_NIBBLES + 1 ) * ROW_BYTES ) - 1 ) + GROUP_SEPARATION + 1 + ROW_BYTES + 1 + 1>;

    /**
     * \brief Format an offset (hex).
     *
     * \param[in] offset The offset to format.
     * \param[out] row_buffer The row buffer location to write the formatted offset to.
     */
    static void format_hex( std::uintptr_t offset, char * row_buffer ) noexcept
    {
        auto i = Row_Buffer::Reverse_Iterator{ row_buffer + OFFSET_NIBBLES };
        for ( auto nibble = std::uint_fast8_t{ 0 }; nibble < OFFSET_NIBBLES; ++nibble ) {
            auto const n = offset & NIBBLE_MASK;

            *i = n < 0xA ? '0' + n : 'A' + ( n - 0xA );

            ++i;
            offset >>= NIBBLE_DIGITS;
        } // for
    }

    /**
     * \brief Format a byte (hex).
     *
     * \param[in] byte The byte of data to format.
     * \param[out] row_buffer The row buffer location to write the formatted byte to.
     */
    static void format_hex( std::uint8_t byte, char * row_buffer ) noexcept
    {
        auto i = Row_Buffer::Reverse_Iterator{ row_buffer + BYTE_NIBBLES };
        for ( auto nibble = std::uint_fast8_t{ 0 }; nibble < BYTE_NIBBLES; ++nibble ) {
            auto const n = byte & NIBBLE_MASK;

            *i = n < 0xA ? '0' + n : 'A' + ( n - 0xA );

            ++i;
            byte >>= NIBBLE_DIGITS;
        } // for
    }

    /**
     * \brief Format a byte (ASCII).
     *
     * \param[in] byte The byte of data to format.
     * \param[out] row_buffer The row buffer location to write the formatted byte to.
     */
    static void format_ascii( std::uint8_t byte, char * row_buffer ) noexcept
    {
        *row_buffer = std::isprint( byte ) ? static_cast<char>( byte ) : '.';
    }

    /**
     * \brief Generate a row.
     *
     * \param[in] offset The row's offset.
     * \param[in] begin The beginning of the block of memory.
     * \param[in] end The end of the block of memory.
     * \param[out] row_buffer The row buffer to write the generated row to.
     *
     * \return The beginning of the remaining block of memory.
     */
    // NOLINTNEXTLINE(readability-function-size)
    static auto generate_row( std::uintptr_t offset, std::uint8_t const * begin, std::uint8_t const * end, Row_Buffer & row_buffer ) noexcept
        -> std::uint8_t const *
    {
        fill( row_buffer.begin() + OFFSET_NIBBLES, row_buffer.end() - 1, ' ' );

        row_buffer.back() = '\n';

        row_buffer[ DATA_ASCII_INDEX - 1 ] = '|';

        format_hex( offset, &row_buffer[ OFFSET_HEX_INDEX ] );

        auto byte = std::uint_fast8_t{ 0 };
        for ( ; begin != end and byte < ROW_BYTES; ++begin, ++byte ) {
            format_hex( *begin, &row_buffer[ DATA_HEX_INDEX + ( ( BYTE_NIBBLES + 1 ) * byte ) ] );

            format_ascii( *begin, &row_buffer[ DATA_ASCII_INDEX + byte ] );
        } // for
        row_buffer[ DATA_ASCII_INDEX + byte ] = '|';

        return begin;
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_FORMAT_H

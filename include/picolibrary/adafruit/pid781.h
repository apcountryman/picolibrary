/**
 * picolibrary
 *
 * Copyright 2020-2023, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::Adafruit::PID781 interface.
 */

#ifndef PICOLIBRARY_ADAFRUIT_PID781_H
#define PICOLIBRARY_ADAFRUIT_PID781_H

#include <cstddef>
#include <cstdint>
#include <limits>

#include "picolibrary/array.h"
#include "picolibrary/stream.h"
#include "picolibrary/utility.h"

/**
 * \brief Adafruit PID781 facilities.
 */
namespace picolibrary::Adafruit::PID781 {

/**
 * \brief Bit rate.
 */
enum class Bit_Rate : std::uint8_t {
    _1200   = 0x53, ///< 1200 bits/second.
    _2400   = 0x29, ///< 2400 bits/second.
    _4800   = 0xCF, ///< 4800 bits/second.
    _9600   = 0x67, ///< 9600 bits/second.
    _19200  = 0x33, ///< 19200 bits/second.
    _28800  = 0x22, ///< 28800 bits/second.
    _38400  = 0x19, ///< 38400 bits/second.
    _57600  = 0x10, ///< 57600 bits/second.
    _115200 = 0x08, ///< 115200 bits/second.
};

/**
 * \brief LCD size.
 */
enum class LCD_Size : std::uint16_t {
    _16X2 = ( 16 << std::numeric_limits<std::uint8_t>::digits ) | 2, ///< 16 columns, 2 rows.
    _20X4 = ( 20 << std::numeric_limits<std::uint8_t>::digits ) | 4, ///< 20 columns, 4 rows.
};

/**
 * \brief Get the number of columns an LCD has.
 *
 * \param[in] lcd_size The size of the LCD.
 *
 * \return The number of columns the LCD has.
 */
constexpr auto columns( LCD_Size lcd_size ) noexcept -> std::uint8_t
{
    return to_underlying( lcd_size ) >> std::numeric_limits<std::uint8_t>::digits;
}

/**
 * \brief Get the number of rows an LCD has.
 *
 * \param[in] lcd_size The size of the LCD.
 *
 * \return The number of rows the LCD has.
 */
constexpr auto rows( LCD_Size lcd_size ) noexcept -> std::uint8_t
{
    return to_underlying( lcd_size );
}

/**
 * \brief Cursor position.
 *
 * \attention Cursor positions are 1-indexed (e.g. (1,1) is the top left position).
 */
struct Cursor_Position {
    /**
     * \brief The column component of the cursor position.
     */
    std::uint8_t column;

    /**
     * \brief The row component of the cursor position.
     */
    std::uint8_t row;
};

/**
 * \brief Custom character.
 */
struct Custom_Character {
    /**
     * \brief Bank.
     */
    enum class Bank : std::uint8_t {
        _0 = 0, ///< 0.
        _1 = 1, ///< 1.
        _2 = 2, ///< 2.
        _3 = 3, ///< 3.
        _4 = 4, ///< 4.
    };

    /**
     * \brief ID.
     */
    enum class ID : std::uint8_t {
        _0 = 0, ///< 0.
        _1 = 1, ///< 1.
        _2 = 2, ///< 2.
        _3 = 3, ///< 3.
        _4 = 4, ///< 4.
        _5 = 5, ///< 5.
        _6 = 6, ///< 6.
        _7 = 7, ///< 7.
    };

    /**
     * \brief Pixel matrix.
     */
    using Pixel_Matrix = Array<std::uint8_t, 8>;

    /**
     * \brief The custom character's bank.
     */
    Bank bank;

    /**
     * \brief The custom character's ID.
     */
    ID id;

    /**
     * \brief The custom character's pixel matrix.
     */
    Pixel_Matrix pixel_matrix;
};

} // namespace picolibrary::Adafruit::PID781

namespace picolibrary {

/**
 * \brief picolibrary::Adafruit::PID781::Custom_Character output formatter.
 */
template<>
class Output_Formatter<Adafruit::PID781::Custom_Character> {
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
     * \brief Write the formatted picolibrary::Adafruit::PID781::Custom_Character to the
     *        stream.
     *
     * \param[in] stream The stream to write the formatted
     *            picolibrary::Adafruit::PID781::Custom_Character to.
     * \param[in] custom_character The picolibrary::Adafruit::PID781::Custom_Character to
     *            format.
     *
     * \return The number of characters written to the stream.
     */
    auto print( Reliable_Output_Stream & stream, Adafruit::PID781::Custom_Character const & custom_character ) const noexcept
        -> std::size_t
    {
        stream.put( to_underlying( custom_character.id ) );

        return std::size_t{ 1 };
    }
};

/**
 * \brief picolibrary::Adafruit::PID781::Custom_Character::ID output formatter.
 */
template<>
class Output_Formatter<Adafruit::PID781::Custom_Character::ID> {
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
     * \brief Write the formatted picolibrary::Adafruit::PID781::Custom_Character::ID to
     *        the stream.
     *
     * \param[in] stream The stream to write the formatted
     *            picolibrary::Adafruit::PID781::Custom_Character::ID to.
     * \param[in] id The picolibrary::Adafruit::PID781::Custom_Character::ID to format.
     *
     * \return The number of characters written to the stream.
     */
    auto print( Reliable_Output_Stream & stream, Adafruit::PID781::Custom_Character::ID id ) const noexcept
        -> std::size_t
    {
        stream.put( to_underlying( id ) );

        return std::size_t{ 1 };
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_ADAFRUIT_PID781_H

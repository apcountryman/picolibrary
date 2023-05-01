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

#include <cstdint>
#include <limits>

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

} // namespace picolibrary::Adafruit::PID781

#endif // PICOLIBRARY_ADAFRUIT_PID781_H

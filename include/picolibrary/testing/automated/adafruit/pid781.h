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
 * \brief picolibrary::Testing::Automated::Adafruit::PID781 interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_ADAFRUIT_PID781_H
#define PICOLIBRARY_TESTING_AUTOMATED_ADAFRUIT_PID781_H

#include <cstdint>
#include <ostream>
#include <stdexcept>

#include "picolibrary/adafruit/pid781.h"
#include "picolibrary/gpio.h"
#include "picolibrary/rgb.h"
#include "picolibrary/testing/automated/stream.h"

namespace picolibrary::Adafruit::PID781 {

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::Adafruit::PID781::Bit_Rate to.
 * \param[in] bit_rate The picolibrary::Adafruit::PID781::Bit_Rate to write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Bit_Rate bit_rate ) -> std::ostream &
{
    switch ( bit_rate ) {
            // clang-format off

        case Bit_Rate::_1200:   return stream << "::picolibrary::Adafruit::PID781::Bit_Rate::_1200";
        case Bit_Rate::_2400:   return stream << "::picolibrary::Adafruit::PID781::Bit_Rate::_2400";
        case Bit_Rate::_4800:   return stream << "::picolibrary::Adafruit::PID781::Bit_Rate::_4800";
        case Bit_Rate::_9600:   return stream << "::picolibrary::Adafruit::PID781::Bit_Rate::_9600";
        case Bit_Rate::_19200:  return stream << "::picolibrary::Adafruit::PID781::Bit_Rate::_19200";
        case Bit_Rate::_28800:  return stream << "::picolibrary::Adafruit::PID781::Bit_Rate::_28800";
        case Bit_Rate::_38400:  return stream << "::picolibrary::Adafruit::PID781::Bit_Rate::_38400";
        case Bit_Rate::_57600:  return stream << "::picolibrary::Adafruit::PID781::Bit_Rate::_57600";
        case Bit_Rate::_115200: return stream << "::picolibrary::Adafruit::PID781::Bit_Rate::_115200";

            // clang-format on
    } // switch

    throw std::invalid_argument{
        "bit_rate is not a valid ::picolibrary::Adafruit::PID781::Bit_Rate"
    };
}

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::Adafruit::PID781::LCD_Size to.
 * \param[in] lcd_size The picolibrary::Adafruit::PID781::LCD_Size to write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, LCD_Size lcd_size ) -> std::ostream &
{
    switch ( lcd_size ) {
            // clang-format off

        case LCD_Size::_16X2: return stream << "::picolibrary::Adafruit::PID781::LCD_Size::_16X2";
        case LCD_Size::_20X4: return stream << "::picolibrary::Adafruit::PID781::LCD_Size::_20X4";

            // clang-format on
    } // switch

    throw std::invalid_argument{
        "lcd_size is not a valid ::picolibrary::Adafruit::PID781::LCD_Size"
    };
}

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the
 *            picolibrary::Adafruit::PID781::Custom_Character_Bank to.
 * \param[in] custom_character_bank The
 *            picolibrary::Adafruit::PID781::Custom_Character_Bank to write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Custom_Character_Bank custom_character_bank )
    -> std::ostream &
{
    switch ( custom_character_bank ) {
            // clang-format off

        case Custom_Character_Bank::_0: return stream << "::picolibrary::Adafruit::PID781::Custom_Character_Bank::_0";
        case Custom_Character_Bank::_1: return stream << "::picolibrary::Adafruit::PID781::Custom_Character_Bank::_1";
        case Custom_Character_Bank::_2: return stream << "::picolibrary::Adafruit::PID781::Custom_Character_Bank::_2";
        case Custom_Character_Bank::_3: return stream << "::picolibrary::Adafruit::PID781::Custom_Character_Bank::_3";
        case Custom_Character_Bank::_4: return stream << "::picolibrary::Adafruit::PID781::Custom_Character_Bank::_4";

            // clang-format on
    } // switch

    throw std::invalid_argument{
        "custom_character_bank is not a valid "
        "::picolibrary::Adafruit::PID781::Custom_Character_Bank"
    };
}

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the
 *            picolibrary::Adafruit::PID781::Custom_Character_ID to.
 * \param[in] custom_character_id The picolibrary::Adafruit::PID781::Custom_Character_ID
 *            to write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Custom_Character_ID custom_character_id )
    -> std::ostream &
{
    switch ( custom_character_id ) {
            // clang-format off

        case Custom_Character_ID::_0: return stream << "::picolibrary::Adafruit::PID781::Custom_Character_ID::_0";
        case Custom_Character_ID::_1: return stream << "::picolibrary::Adafruit::PID781::Custom_Character_ID::_1";
        case Custom_Character_ID::_2: return stream << "::picolibrary::Adafruit::PID781::Custom_Character_ID::_2";
        case Custom_Character_ID::_3: return stream << "::picolibrary::Adafruit::PID781::Custom_Character_ID::_3";
        case Custom_Character_ID::_4: return stream << "::picolibrary::Adafruit::PID781::Custom_Character_ID::_4";
        case Custom_Character_ID::_5: return stream << "::picolibrary::Adafruit::PID781::Custom_Character_ID::_5";
        case Custom_Character_ID::_6: return stream << "::picolibrary::Adafruit::PID781::Custom_Character_ID::_6";
        case Custom_Character_ID::_7: return stream << "::picolibrary::Adafruit::PID781::Custom_Character_ID::_7";

            // clang-format on
    } // switch

    throw std::invalid_argument{
        "custom_character_id is not a valid "
        "::picolibrary::Adafruit::PID781::Custom_Character_ID"
    };
}

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::Adafruit::PID781::Pin to.
 * \param[in] pin The picolibrary::Adafruit::PID781::Pin to write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Pin pin ) -> std::ostream &
{
    switch ( pin ) {
            // clang-format off

        case Pin::PB0: return stream << "::picolibrary::Adafruit::PID781::Pin::PB0";
        case Pin::PC2: return stream << "::picolibrary::Adafruit::PID781::Pin::PC2";
        case Pin::PC4: return stream << "::picolibrary::Adafruit::PID781::Pin::PC4";
        case Pin::PC7: return stream << "::picolibrary::Adafruit::PID781::Pin::PC7";

            // clang-format on
    } // switch

    throw std::invalid_argument{
        "pin is not a valid ::picolibrary::Adafruit::PID781::Pin"
    };
}

} // namespace picolibrary::Adafruit::PID781

/**
 * \brief Adafruit PID781 automated testing facilities.
 */
namespace picolibrary::Testing::Automated::Adafruit::PID781 {

/**
 * \brief Mock driver.
 */
class Mock_Driver : public Mock_Reliable_Output_Stream {
  public:
    Mock_Driver() = default;

    Mock_Driver( Mock_Driver && ) = delete;

    Mock_Driver( Mock_Driver const & ) = delete;

    ~Mock_Driver() noexcept = default;

    auto operator=( Mock_Driver && ) = delete;

    auto operator=( Mock_Driver const & ) = delete;

    MOCK_METHOD( void, set_and_save_bit_rate, ( ::picolibrary::Adafruit::PID781::Bit_Rate ) );

    MOCK_METHOD( void, report_bit_rate, () );

    MOCK_METHOD( void, set_and_save_lcd_size, ( ::picolibrary::Adafruit::PID781::LCD_Size ) );

    MOCK_METHOD( void, set_and_save_backlight_color, (::picolibrary::RGB::Color const &));

    MOCK_METHOD( void, set_and_save_backlight_brightness, ( std::uint8_t ) );

    MOCK_METHOD( void, set_and_save_lcd_contrast, ( std::uint8_t ) );

    MOCK_METHOD( void, set_and_save_splash_screen, () );

    MOCK_METHOD( void, illuminate_backlight, () );

    MOCK_METHOD( void, extinguish_backlight, () );

    MOCK_METHOD( void, enable_auto_scrolling, () );

    MOCK_METHOD( void, disable_auto_scrolling, () );

    MOCK_METHOD( void, enable_cursor_underline, () );

    MOCK_METHOD( void, disable_cursor_underline, () );

    MOCK_METHOD( void, enable_cursor_blink, () );

    MOCK_METHOD( void, disable_cursor_blink, () );

    MOCK_METHOD( void, set_cursor_position, ( ::picolibrary::Adafruit::PID781::Cursor_Position ) );

    MOCK_METHOD( void, home_cursor, () );

    MOCK_METHOD( void, retreat_cursor, () );

    MOCK_METHOD( void, advance_cursor, () );

    MOCK_METHOD( void, clear_screen, () );

    MOCK_METHOD(
        void,
        create_custom_character,
        (::picolibrary::Adafruit::PID781::Custom_Character_ID,
         ::picolibrary::Adafruit::PID781::Custom_Character_Pixel_Matrix const &));

    MOCK_METHOD(
        void,
        create_and_save_custom_character,
        (::picolibrary::Adafruit::PID781::Custom_Character_Bank,
         ::picolibrary::Adafruit::PID781::Custom_Character_ID,
         ::picolibrary::Adafruit::PID781::Custom_Character_Pixel_Matrix const &));

    MOCK_METHOD( void, load_custom_character_bank, ( ::picolibrary::Adafruit::PID781::Custom_Character_Bank ) );

    MOCK_METHOD(
        void,
        set_and_save_output_pin_initial_state,
        ( ::picolibrary::Adafruit::PID781::Pin, ::picolibrary::GPIO::Initial_Pin_State ) );

    MOCK_METHOD( void, transition_output_pin_to_high, ( ::picolibrary::Adafruit::PID781::Pin ) );

    MOCK_METHOD( void, transition_output_pin_to_low, ( ::picolibrary::Adafruit::PID781::Pin ) );
};

} // namespace picolibrary::Testing::Automated::Adafruit::PID781

#endif // PICOLIBRARY_TESTING_AUTOMATED_ADAFRUIT_PID781_H

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
 * \brief picolibrary::Adafruit::PID781::Driver automated test program.
 */

#include <cstdint>
#include <iomanip>
#include <ios>
#include <limits>
#include <ostream>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/adafruit/pid781.h"
#include "picolibrary/gpio.h"
#include "picolibrary/rgb.h"
#include "picolibrary/testing/automated/adafruit/pid781.h"
#include "picolibrary/testing/automated/gpio.h"
#include "picolibrary/testing/automated/stream.h"

namespace {

using ::picolibrary::Adafruit::PID781::Bit_Rate;
using ::picolibrary::Adafruit::PID781::Cursor_Position;
using ::picolibrary::Adafruit::PID781::Custom_Character_Bank;
using ::picolibrary::Adafruit::PID781::Custom_Character_ID;
using ::picolibrary::Adafruit::PID781::Custom_Character_Pixel_Matrix;
using ::picolibrary::Adafruit::PID781::LCD_Size;
using ::picolibrary::Adafruit::PID781::Pin;
using ::picolibrary::GPIO::Initial_Pin_State;
using ::picolibrary::RGB::Color;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

using Driver = ::picolibrary::Adafruit::PID781::Driver<::picolibrary::Testing::Automated::Reliable_Output_Vector_Stream<std::uint8_t>>;

} // namespace

/**
 * \brief picolibrary::Adafruit::PID781::Driver::set_and_save_bit_rate() test case.
 */
struct setAndSaveBitRate_Test_Case {
    /**
     * \brief The desired bit rate.
     */
    Bit_Rate bit_rate;

    /**
     * \brief The unsigned byte written to the stream.
     */
    std::uint8_t value;
};

auto operator<<( std::ostream & stream, setAndSaveBitRate_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".bit_rate = " << test_case.bit_rate
                  << ", "
                  << ".value = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( test_case.value )
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Adafruit::PID781::Driver::set_and_save_bit_rate() test fixture.
 */
class setAndSaveBitRate : public TestWithParam<setAndSaveBitRate_Test_Case> {
};

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::set_and_save_bit_rate() works
 *        properly.
 */
TEST_P( setAndSaveBitRate, worksProperly )
{
    auto const test_case = GetParam();

    auto pid781 = Driver{};

    pid781.set_and_save_bit_rate( test_case.bit_rate );

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0x39,
            test_case.value,
        } ) );
}

/**
 * \brief picolibrary::Adafruit::PID781::Driver::set_and_save_bit_rate() test cases.
 */
setAndSaveBitRate_Test_Case const setAndSaveBitRate_TEST_CASES[]{
    // clang-format off

    { Bit_Rate::_1200,   0x53 },
    { Bit_Rate::_2400,   0x29 },
    { Bit_Rate::_4800,   0xCF },
    { Bit_Rate::_9600,   0x67 },
    { Bit_Rate::_19200,  0x33 },
    { Bit_Rate::_28800,  0x22 },
    { Bit_Rate::_38400,  0x19 },
    { Bit_Rate::_57600,  0x10 },
    { Bit_Rate::_115200, 0x08 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, setAndSaveBitRate, ValuesIn( setAndSaveBitRate_TEST_CASES ) );

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::report_bit_rate() works properly.
 */
TEST( reportBitRate, worksProperly )
{
    auto pid781 = Driver{};

    pid781.report_bit_rate();

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0xD2,
        } ) );
}

/**
 * \brief picolibrary::Adafruit::PID781::Driver::set_and_save_lcd_size() test case.
 */
struct setAndSaveLCDSize_Test_Case {
    /**
     * \brief The desired LCD size.
     */
    LCD_Size lcd_size;

    /**
     * \brief The number of columns the LCD has.
     */
    std::uint8_t columns;

    /**
     * \brief The number of rows the LCD has.
     */
    std::uint8_t rows;
};

auto operator<<( std::ostream & stream, setAndSaveLCDSize_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".lcd_size = " << test_case.lcd_size
                  << ", "
                  << ".columns = " << static_cast<std::uint_fast16_t>( test_case.columns )
                  << ", "
                  << ".rows = " << static_cast<std::uint_fast16_t>( test_case.rows )
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Adafruit::PID781::Driver::set_and_save_lcd_size() test fixture.
 */
class setAndSaveLCDSize : public TestWithParam<setAndSaveLCDSize_Test_Case> {
};

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::set_and_save_lcd_size() works
 *        properly.
 */
TEST_P( setAndSaveLCDSize, worksProperly )
{
    auto const test_case = GetParam();

    auto pid781 = Driver{};

    pid781.set_and_save_lcd_size( test_case.lcd_size );

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0xD1,
            test_case.columns,
            test_case.rows,
        } ) );
}

/**
 * \brief picolibrary::Adafruit::PID781::Driver::set_and_save_lcd_size() test cases.
 */
setAndSaveLCDSize_Test_Case const setAndSaveLCDSize_TEST_CASES[]{
    // clang-format off

    { LCD_Size::_16X2, 16, 2 },
    { LCD_Size::_20X4, 20, 4 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, setAndSaveLCDSize, ValuesIn( setAndSaveLCDSize_TEST_CASES ) );

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::set_and_save_backlight_color()
 *        works properly.
 */
TEST( setAndSaveBacklightColor, worksProperly )
{
    auto pid781 = Driver{};

    auto const color = Color{ 0xB2, 0x27, 0x34 };

    pid781.set_and_save_backlight_color( color );

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0xD0,
            color.r,
            color.g,
            color.b,
        } ) );
}

/**
 * \brief Verify
 *        picolibrary::Adafruit::PID781::Driver::set_and_save_backlight_brightness() works
 *        properly.
 */
TEST( setAndSaveBacklightBrightness, worksProperly )
{
    auto pid781 = Driver{};

    auto const brightness = std::uint8_t{ 0xD6 };

    pid781.set_and_save_backlight_brightness( brightness );

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0x98,
            brightness,
        } ) );
}

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::set_and_save_lcd_contrast() works
 *        properly.
 */
TEST( setAndSaveLCDContrast, worksProperly )
{
    auto pid781 = Driver{};

    auto const contrast = std::uint8_t{ 0x37 };

    pid781.set_and_save_lcd_contrast( contrast );

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0x91,
            contrast,
        } ) );
}

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::set_and_save_splash_screen() works
 *        properly.
 */
TEST( setAndSaveSplashScreen, worksProperly )
{
    auto pid781 = Driver{};

    pid781.set_and_save_splash_screen();

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0x40,
        } ) );
}

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::illuminate_backlight() works
 *        properly.
 */
TEST( illuminateBacklight, worksProperly )
{
    auto pid781 = Driver{};

    pid781.illuminate_backlight();

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0x42,
            0x00,
        } ) );
}

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::extinguish_backlight() works
 *        properly.
 */
TEST( extinguishBacklight, worksProperly )
{
    auto pid781 = Driver{};

    pid781.extinguish_backlight();

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0x46,
        } ) );
}

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::enable_auto_scrolling() works
 *        properly.
 */
TEST( enableAutoScrolling, worksProperly )
{
    auto pid781 = Driver{};

    pid781.enable_auto_scrolling();

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0x51,
        } ) );
}

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::disable_auto_scrolling() works
 *        properly.
 */
TEST( disableAutoScrolling, worksProperly )
{
    auto pid781 = Driver{};

    pid781.disable_auto_scrolling();

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0x52,
        } ) );
}

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::enable_cursor_underline() works
 *        properly.
 */
TEST( enableCursorUnderline, worksProperly )
{
    auto pid781 = Driver{};

    pid781.enable_cursor_underline();

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0x4A,
        } ) );
}

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::disable_cursor_underline() works
 *        properly.
 */
TEST( disableCursorUnderline, worksProperly )
{
    auto pid781 = Driver{};

    pid781.disable_cursor_underline();

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0x4B,
        } ) );
}

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::enable_cursor_blink() works
 *        properly.
 */
TEST( enableCursorBlink, worksProperly )
{
    auto pid781 = Driver{};

    pid781.enable_cursor_blink();

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0x53,
        } ) );
}

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::disable_cursor_blink() works
 *        properly.
 */
TEST( disableCursorBlink, worksProperly )
{
    auto pid781 = Driver{};

    pid781.disable_cursor_blink();

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0x54,
        } ) );
}

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::set_cursor_position() works
 *        properly.
 */
TEST( setCursorPosition, worksProperly )
{
    auto pid781 = Driver{};

    auto const cursor_position = Cursor_Position{ 8, 1 };

    pid781.set_cursor_position( cursor_position );

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0x47,
            cursor_position.column,
            cursor_position.row,
        } ) );
}

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::home_cursor() works properly.
 */
TEST( homeCursor, worksProperly )
{
    auto pid781 = Driver{};

    pid781.home_cursor();

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0x48,
        } ) );
}

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::retreat_cursor() works properly.
 */
TEST( retreatCursor, worksProperly )
{
    auto pid781 = Driver{};

    pid781.retreat_cursor();

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0x4C,
        } ) );
}

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::advance_cursor() works properly.
 */
TEST( advanceCursor, worksProperly )
{
    auto pid781 = Driver{};

    pid781.advance_cursor();

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0x4D,
        } ) );
}

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::clear_screen() works properly.
 */
TEST( clearScreen, worksProperly )
{
    auto pid781 = Driver{};

    pid781.clear_screen();

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0x58,
        } ) );
}

/**
 * \brief picolibrary::Adafruit::PID781::Driver::create_custom_character() test case.
 */
struct createCustomCharacter_Test_Case {
    /**
     * \brief The custom character's ID.
     */
    Custom_Character_ID id;

    /**
     * \brief The unsigned byte written to the stream.
     */
    std::uint8_t value;
};

auto operator<<( std::ostream & stream, createCustomCharacter_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".id = " << test_case.id
                  << ", "
                  << ".value = " << static_cast<std::uint_fast16_t>( test_case.value )
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Adafruit::PID781::Driver::create_custom_character() test fixture.
 */
class createCustomCharacter : public TestWithParam<createCustomCharacter_Test_Case> {
};

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::create_custom_character() works
 *        properly.
 */
TEST_P( createCustomCharacter, worksProperly )
{
    auto const test_case = GetParam();

    auto pid781 = Driver{};

    auto const pixel_matrix = Custom_Character_Pixel_Matrix{
        // clang-format off

        0b01011,
        0b00000,
        0b10001,
        0b00111,
        0b10010,
        0b10010,
        0b01110,
        0b01011,

        // clang-format on
    };

    pid781.create_custom_character( test_case.id, pixel_matrix );

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0x4E,
            test_case.value,
            pixel_matrix[ 0 ],
            pixel_matrix[ 1 ],
            pixel_matrix[ 2 ],
            pixel_matrix[ 3 ],
            pixel_matrix[ 4 ],
            pixel_matrix[ 5 ],
            pixel_matrix[ 6 ],
            pixel_matrix[ 7 ],
        } ) );
}

/**
 * \brief picolibrary::Adafruit::PID781::Driver::create_custom_character() test cases.
 */
createCustomCharacter_Test_Case const createCustomCharacter_TEST_CASES[]{
    // clang-format off

    { Custom_Character_ID::_0, 0 },
    { Custom_Character_ID::_1, 1 },
    { Custom_Character_ID::_2, 2 },
    { Custom_Character_ID::_3, 3 },
    { Custom_Character_ID::_4, 4 },
    { Custom_Character_ID::_5, 5 },
    { Custom_Character_ID::_6, 6 },
    { Custom_Character_ID::_7, 7 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, createCustomCharacter, ValuesIn( createCustomCharacter_TEST_CASES ) );

/**
 * \brief picolibrary::Adafruit::PID781::Driver::create_and_save_custom_character() test
 *        case.
 */
struct createAndSaveCustomCharacter_Test_Case {
    /**
     * \brief The custom character bank to save the custom character to.
     */
    Custom_Character_Bank bank;

    /**
     * \brief The custom character's ID.
     */
    Custom_Character_ID id;

    /**
     * \brief The unsigned byte written to the stream (bank).
     */
    std::uint8_t value_bank;

    /**
     * \brief The unsigned byte written to the stream (id).
     */
    std::uint8_t value_id;
};

auto operator<<( std::ostream & stream, createAndSaveCustomCharacter_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".bank = " << test_case.bank
                  << ", "
                  << ".id = " << test_case.id
                  << ", "
                  << ".value_bank = " << static_cast<std::uint_fast16_t>( test_case.value_bank )
                  << ", "
                  << ".value_id = " << static_cast<std::uint_fast16_t>( test_case.value_id )
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Adafruit::PID781::Driver::create_and_save_custom_character() test
 *        fixture.
 */
class createAndSaveCustomCharacter : public TestWithParam<createAndSaveCustomCharacter_Test_Case> {
};

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::create_and_save_custom_character()
 *        works properly.
 */
TEST_P( createAndSaveCustomCharacter, worksProperly )
{
    auto const test_case = GetParam();

    auto pid781 = Driver{};

    auto const pixel_matrix = Custom_Character_Pixel_Matrix{
        // clang-format off

        0b11001,
        0b00110,
        0b01100,
        0b00000,
        0b11100,
        0b10000,
        0b11110,
        0b01010,

        // clang-format on
    };

    pid781.create_and_save_custom_character( test_case.bank, test_case.id, pixel_matrix );

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0xC1,
            test_case.value_bank,
            test_case.value_id,
            pixel_matrix[ 0 ],
            pixel_matrix[ 1 ],
            pixel_matrix[ 2 ],
            pixel_matrix[ 3 ],
            pixel_matrix[ 4 ],
            pixel_matrix[ 5 ],
            pixel_matrix[ 6 ],
            pixel_matrix[ 7 ],
        } ) );
}

/**
 * \brief picolibrary::Adafruit::PID781::Driver::create_and_save_custom_character() test
 *        cases.
 */
createAndSaveCustomCharacter_Test_Case const createAndSaveCustomCharacter_TEST_CASES[]{
    // clang-format off

    { Custom_Character_Bank::_0, Custom_Character_ID::_0, 0, 0 },
    { Custom_Character_Bank::_0, Custom_Character_ID::_1, 0, 1 },
    { Custom_Character_Bank::_0, Custom_Character_ID::_2, 0, 2 },
    { Custom_Character_Bank::_0, Custom_Character_ID::_3, 0, 3 },
    { Custom_Character_Bank::_0, Custom_Character_ID::_4, 0, 4 },
    { Custom_Character_Bank::_0, Custom_Character_ID::_5, 0, 5 },
    { Custom_Character_Bank::_0, Custom_Character_ID::_6, 0, 6 },
    { Custom_Character_Bank::_0, Custom_Character_ID::_7, 0, 7 },

    { Custom_Character_Bank::_1, Custom_Character_ID::_0, 1, 0 },
    { Custom_Character_Bank::_1, Custom_Character_ID::_1, 1, 1 },
    { Custom_Character_Bank::_1, Custom_Character_ID::_2, 1, 2 },
    { Custom_Character_Bank::_1, Custom_Character_ID::_3, 1, 3 },
    { Custom_Character_Bank::_1, Custom_Character_ID::_4, 1, 4 },
    { Custom_Character_Bank::_1, Custom_Character_ID::_5, 1, 5 },
    { Custom_Character_Bank::_1, Custom_Character_ID::_6, 1, 6 },
    { Custom_Character_Bank::_1, Custom_Character_ID::_7, 1, 7 },

    { Custom_Character_Bank::_2, Custom_Character_ID::_0, 2, 0 },
    { Custom_Character_Bank::_2, Custom_Character_ID::_1, 2, 1 },
    { Custom_Character_Bank::_2, Custom_Character_ID::_2, 2, 2 },
    { Custom_Character_Bank::_2, Custom_Character_ID::_3, 2, 3 },
    { Custom_Character_Bank::_2, Custom_Character_ID::_4, 2, 4 },
    { Custom_Character_Bank::_2, Custom_Character_ID::_5, 2, 5 },
    { Custom_Character_Bank::_2, Custom_Character_ID::_6, 2, 6 },
    { Custom_Character_Bank::_2, Custom_Character_ID::_7, 2, 7 },

    { Custom_Character_Bank::_3, Custom_Character_ID::_0, 3, 0 },
    { Custom_Character_Bank::_3, Custom_Character_ID::_1, 3, 1 },
    { Custom_Character_Bank::_3, Custom_Character_ID::_2, 3, 2 },
    { Custom_Character_Bank::_3, Custom_Character_ID::_3, 3, 3 },
    { Custom_Character_Bank::_3, Custom_Character_ID::_4, 3, 4 },
    { Custom_Character_Bank::_3, Custom_Character_ID::_5, 3, 5 },
    { Custom_Character_Bank::_3, Custom_Character_ID::_6, 3, 6 },
    { Custom_Character_Bank::_3, Custom_Character_ID::_7, 3, 7 },

    { Custom_Character_Bank::_4, Custom_Character_ID::_0, 4, 0 },
    { Custom_Character_Bank::_4, Custom_Character_ID::_1, 4, 1 },
    { Custom_Character_Bank::_4, Custom_Character_ID::_2, 4, 2 },
    { Custom_Character_Bank::_4, Custom_Character_ID::_3, 4, 3 },
    { Custom_Character_Bank::_4, Custom_Character_ID::_4, 4, 4 },
    { Custom_Character_Bank::_4, Custom_Character_ID::_5, 4, 5 },
    { Custom_Character_Bank::_4, Custom_Character_ID::_6, 4, 6 },
    { Custom_Character_Bank::_4, Custom_Character_ID::_7, 4, 7 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, createAndSaveCustomCharacter, ValuesIn( createAndSaveCustomCharacter_TEST_CASES ) );

/**
 * \brief picolibrary::Adafruit::PID781::Driver::load_custom_character_bank() test case.
 */
struct loadCustomCharacterBank_Test_Case {
    /**
     * \brief The custom character bank to load.
     */
    Custom_Character_Bank bank;

    /**
     * \brief The unsigned byte written to the stream.
     */
    std::uint8_t value;
};

auto operator<<( std::ostream & stream, loadCustomCharacterBank_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".bank = " << test_case.bank
                  << ", "
                  << ".value = " << static_cast<std::uint_fast16_t>( test_case.value )
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Adafruit::PID781::Driver::load_custom_character_bank() test
 *        fixture.
 */
class loadCustomCharacterBank : public TestWithParam<loadCustomCharacterBank_Test_Case> {
};

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::load_custom_character_bank() works
 *        properly.
 */
TEST_P( loadCustomCharacterBank, worksProperly )
{
    auto const test_case = GetParam();

    auto pid781 = Driver{};

    pid781.load_custom_character_bank( test_case.bank );

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0xC0,
            test_case.value,
        } ) );
}

/**
 * \brief picolibrary::Adafruit::PID781::Driver::load_custom_character_bank() test
 *        cases.
 */
loadCustomCharacterBank_Test_Case const loadCustomCharacterBank_TEST_CASES[]{
    // clang-format off

    { Custom_Character_Bank::_0, 0 },
    { Custom_Character_Bank::_1, 1 },
    { Custom_Character_Bank::_2, 2 },
    { Custom_Character_Bank::_3, 3 },
    { Custom_Character_Bank::_4, 4 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, loadCustomCharacterBank, ValuesIn( loadCustomCharacterBank_TEST_CASES ) );

/**
 * \brief picolibrary::Adafruit::PID781::Driver::set_and_save_output_pin_initial_state()
 *        test case.
 */
struct setAndSaveOutputPinInitialState_Test_Case {
    /**
     * \brief The pin whose initial pin state is to be set and saved.
     */
    Pin pin;

    /**
     * \brief The desired initial pin state.
     */
    Initial_Pin_State initial_pin_state;

    /**
     * \brief The unsigned byte written to the stream (pin).
     */
    std::uint8_t value_pin;

    /**
     * \brief The unsigned byte written to the stream (initial_pin_state).
     */
    std::uint8_t value_initial_pin_state;
};

auto operator<<( std::ostream & stream, setAndSaveOutputPinInitialState_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".pin = " << test_case.pin
                  << ", "
                  << ".initial_pin_state = " << test_case.initial_pin_state
                  << ", "
                  << ".value_pin = " << static_cast<std::uint_fast16_t>( test_case.value_pin )
                  << ", "
                  << ".value_initial_pin_state = " << static_cast<std::uint_fast16_t>( test_case.value_initial_pin_state )
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Adafruit::PID781::Driver::set_and_save_output_pin_initial_state()
 *        test fixture.
 */
class setAndSaveOutputPinInitialState :
    public TestWithParam<setAndSaveOutputPinInitialState_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::Adafruit::PID781::Driver::set_and_save_output_pin_initial_state()
 *        works properly.
 */
TEST_P( setAndSaveOutputPinInitialState, worksProperly )
{
    auto const test_case = GetParam();

    auto pid781 = Driver{};

    pid781.set_and_save_output_pin_initial_state( test_case.pin, test_case.initial_pin_state );

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0xC3,
            test_case.value_pin,
            test_case.value_initial_pin_state,
        } ) );
}

/**
 * \brief picolibrary::Adafruit::PID781::Driver::set_and_save_output_pin_initial_state()
 *        test cases.
 */
setAndSaveOutputPinInitialState_Test_Case const setAndSaveOutputPinInitialState_TEST_CASES[]{
    // clang-format off

    { Pin::PB0, Initial_Pin_State::LOW,  1, 0 },
    { Pin::PB0, Initial_Pin_State::HIGH, 1, 1 },

    { Pin::PC2, Initial_Pin_State::LOW,  2, 0 },
    { Pin::PC2, Initial_Pin_State::HIGH, 2, 1 },

    { Pin::PC4, Initial_Pin_State::LOW,  3, 0 },
    { Pin::PC4, Initial_Pin_State::HIGH, 3, 1 },

    { Pin::PC7, Initial_Pin_State::LOW,  4, 0 },
    { Pin::PC7, Initial_Pin_State::HIGH, 4, 1 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, setAndSaveOutputPinInitialState, ValuesIn( setAndSaveOutputPinInitialState_TEST_CASES ) );

/**
 * \brief picolibrary::Adafruit::PID781::Driver::transition_output_pin_to_high() and
 *        picolibrary::Adafruit::PID781::Driver::transition_output_pin_to_low() test case.
 */
struct transitionOutputPin_Test_Case {
    /**
     * \brief The pin whose state is to be transitioned.
     */
    Pin pin;

    /**
     * \brief The unsigned byte written to the stream.
     */
    std::uint8_t value;
};

auto operator<<( std::ostream & stream, transitionOutputPin_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".pin = " << test_case.pin
                  << ", "
                  << ".value = " << static_cast<std::uint_fast16_t>( test_case.value )
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Adafruit::PID781::Driver::transition_output_pin_to_high() and
 *        picolibrary::Adafruit::PID781::Driver::transition_output_pin_to_low() test
 *        cases.
 */
transitionOutputPin_Test_Case const transitionOutputPin_TEST_CASES[]{
    // clang-format off

    { Pin::PB0, 1 },
    { Pin::PC2, 2 },
    { Pin::PC4, 3 },
    { Pin::PC7, 4 },

    // clang-format on
};

/**
 * \brief picolibrary::Adafruit::PID781::Driver::transition_output_pin_to_high() test
 *        fixture.
 */
class transitionOutputPinToHigh : public TestWithParam<transitionOutputPin_Test_Case> {
};

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::transition_output_pin_to_high()
 *        works properly.
 */
TEST_P( transitionOutputPinToHigh, worksProperly )
{
    auto const test_case = GetParam();

    auto pid781 = Driver{};

    pid781.transition_output_pin_to_high( test_case.pin );

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0x57,
            test_case.value,
        } ) );
}

INSTANTIATE_TEST_SUITE_P( testCases, transitionOutputPinToHigh, ValuesIn( transitionOutputPin_TEST_CASES ) );

/**
 * \brief picolibrary::Adafruit::PID781::Driver::transition_output_pin_to_low() test
 *        fixture.
 */
class transitionOutputPinToLow : public TestWithParam<transitionOutputPin_Test_Case> {
};

/**
 * \brief Verify picolibrary::Adafruit::PID781::Driver::transition_output_pin_to_low()
 *        works properly.
 */
TEST_P( transitionOutputPinToLow, worksProperly )
{
    auto const test_case = GetParam();

    auto pid781 = Driver{};

    pid781.transition_output_pin_to_low( test_case.pin );

    EXPECT_EQ(
        pid781.vector(),
        ( std::vector<std::uint8_t>{
            0xFE,
            0x56,
            test_case.value,
        } ) );
}

INSTANTIATE_TEST_SUITE_P( testCases, transitionOutputPinToLow, ValuesIn( transitionOutputPin_TEST_CASES ) );

/**
 * \brief Execute the picolibrary::Adafruit::PID781::Driver automated tests.
 *
 * \param[in] argc The number of arguments to pass to testing::InitGoogleMock().
 * \param[in] argc The array of arguments to pass to testing::InitGoogleMock().
 *
 * \return See Google Test's RUN_ALL_TESTS().
 */
int main( int argc, char * argv[] )
{
    ::testing::InitGoogleMock( &argc, argv );

    return RUN_ALL_TESTS();
}

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
 * \brief picolibrary::Adafruit::PID781::LCD_Size automated test program.
 */

#include <cstdint>
#include <ostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/adafruit/pid781.h"
#include "picolibrary/testing/automated/adafruit/pid781.h"

namespace {

using ::picolibrary::Adafruit::PID781::columns;
using ::picolibrary::Adafruit::PID781::LCD_Size;
using ::picolibrary::Adafruit::PID781::rows;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

} // namespace

/**
 * \brief picolibrary::Adafruit::PID781::LCD_Size test case.
 */
struct lcdSize_Test_Case {
    /**
     * \brief The LCD size.
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

auto operator<<( std::ostream & stream, lcdSize_Test_Case const & test_case ) -> std::ostream &
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
 * \brief picolibrary::Adafruit::PID781::LCD_Size test fixture.
 */
class lcdSize : public TestWithParam<lcdSize_Test_Case> {
};

/**
 * \brief Verify picolibrary::Adafruit::PID781::columns() and
 *        picolibrary::Adafruit::PID781::rows() work properly.
 */
TEST_P( lcdSize, worksProperly )
{
    auto const test_case = GetParam();

    EXPECT_EQ( columns( test_case.lcd_size ), test_case.columns );
    EXPECT_EQ( rows( test_case.lcd_size ), test_case.rows );
}

/**
 * \brief picolibrary::Adafruit::PID781::LCD_Size test cases.
 */
lcdSize_Test_Case const lcdSize_TEST_CASES[]{
    // clang-format off

    { LCD_Size::_16X2, 16, 2 },
    { LCD_Size::_20X4, 20, 4 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, lcdSize, ValuesIn( lcdSize_TEST_CASES ) );

/**
 * \brief Execute the picolibrary::Adafruit::PID781::LCD_Size automated tests.
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

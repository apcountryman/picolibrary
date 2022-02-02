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
 * \brief picolibrary::GPIO::Active_Low_Input_Pin unit test program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/gpio.h"
#include "picolibrary/testing/unit/gpio.h"

namespace {

using ::testing::Return;

using Pin = ::picolibrary::GPIO::Active_Low_Input_Pin<::picolibrary::Testing::Unit::GPIO::Mock_Input_Pin>;

} // namespace

/**
 * \brief Verify picolibrary::GPIO::Active_Low_Input_Pin::is_low() works properly.
 */
TEST( isLow, worksProperly )
{
    struct {
        bool is_high;
    } const test_cases[]{
        // clang-format off

        { true  },
        { false },

        //clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const pin = Pin{};

        EXPECT_CALL( pin, is_high() ).WillOnce( Return( test_case.is_high ) );

        EXPECT_EQ( pin.is_low(), test_case.is_high );
    } // for
}

/**
 * \brief Verify picolibrary::GPIO::Active_Low_Input_Pin::is_high() works properly.
 */
TEST( isHigh, worksProperly )
{
    struct {
        bool is_low;
    } const test_cases[]{
        // clang-format off

        { true  },
        { false },

        //clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const pin = Pin{};

        EXPECT_CALL( pin, is_low() ).WillOnce( Return( test_case.is_low ) );

        EXPECT_EQ( pin.is_high(), test_case.is_low );
    } // for
}

/**
 * \brief Execute the picolibrary::GPIO::Active_Low_Input_Pin unit tests.
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

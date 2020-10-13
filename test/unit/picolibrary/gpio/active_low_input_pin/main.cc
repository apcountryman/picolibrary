/**
 * picolibrary
 *
 * Copyright 2020 Andrew Countryman <apcountryman@gmail.com>
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
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/gpio.h"
#include "picolibrary/testing/unit/random.h"

namespace {

using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::testing::Return;

using Pin = ::picolibrary::GPIO::Active_Low_Input_Pin<::picolibrary::Testing::Unit::GPIO::Mock_Input_Pin>;

} // namespace

/**
 * \brief Verify picolibrary::GPIO::Active_Low_Input_Pin::state() works properly when
 *        getting the state of the underlying pin succeeds.
 */
TEST( state, success )
{
    struct {
        bool is_high;
    } const test_cases[]{
        { true },
        { false },
    };

    for ( auto const test_case : test_cases ) {
        auto const pin = Pin{};

        EXPECT_CALL( pin, state() ).WillOnce( Return( test_case.is_high ) );

        auto const result = pin.state();

        EXPECT_FALSE( result.is_error() );
        EXPECT_EQ( result.value().is_high(), not test_case.is_high );
    } // for
}

/**
 * \brief Verify picolibrary::GPIO::Active_Low_Input_Pin::state() works properly when
 *        getting the state of the underlying pin fails.
 */
TEST( state, failure )
{
    auto const pin = Pin{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( pin, state() ).WillOnce( Return( error ) );

    auto const result = pin.state();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Execute the picolibrary::GPIO::Active_Low_Input_Pin unit tests.
 *
 * \param[in] argc The number of arguments to pass to testing::InitGoogleMock().
 * \param[in] argv The array  of arguments to pass to testing::InitGoogleMock().
 *
 * \return See Google Test's RUN_ALL_TESTS().
 */
int main( int argc, char * argv[] )
{
    ::testing::InitGoogleMock( &argc, argv );

    return RUN_ALL_TESTS();
}

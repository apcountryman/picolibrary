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
 * \brief picolibrary::GPIO::Active_Low_Input_Pin automated test program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/gpio.h"
#include "picolibrary/testing/automated/gpio.h"

namespace {

using ::testing::Return;
using ::testing::TestWithParam;
using ::testing::Values;

using Pin = ::picolibrary::GPIO::Active_Low_Input_Pin<::picolibrary::Testing::Automated::GPIO::Mock_Input_Pin>;

} // namespace

/**
 * \brief picolibrary::GPIO::Active_Low_Input_Pin::is_low() test fixture.
 */
class isLow : public TestWithParam<bool> {
};

INSTANTIATE_TEST_SUITE_P( testCases, isLow, Values( true, false ) );

/**
 * \brief Verify picolibrary::GPIO::Active_Low_Input_Pin::is_low() works properly.
 */
TEST_P( isLow, worksProperly )
{
    auto const is_high = GetParam();

    auto const pin = Pin{};

    EXPECT_CALL( pin, is_high() ).WillOnce( Return( is_high ) );

    EXPECT_EQ( pin.is_low(), is_high );
}

/**
 * \brief picolibrary::GPIO::Active_Low_Input_Pin::is_high() test fixture.
 */
class isHigh : public TestWithParam<bool> {
};

INSTANTIATE_TEST_SUITE_P( testCases, isHigh, Values( true, false ) );

/**
 * \brief Verify picolibrary::GPIO::Active_Low_Input_Pin::is_high() works properly.
 */
TEST_P( isHigh, worksProperly )
{
    auto const is_low = GetParam();

    auto const pin = Pin{};

    EXPECT_CALL( pin, is_low() ).WillOnce( Return( is_low ) );

    EXPECT_EQ( pin.is_high(), is_low );
}

/**
 * \brief Execute the picolibrary::GPIO::Active_Low_Input_Pin automated tests.
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

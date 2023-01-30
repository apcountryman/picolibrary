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
 * \brief picolibrary::GPIO::Active_Low_Output_Pin automated test program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/gpio.h"
#include "picolibrary/testing/automated/gpio.h"

namespace {

using ::picolibrary::GPIO::Initial_Pin_State;

using Pin = ::picolibrary::GPIO::Active_Low_Output_Pin<::picolibrary::Testing::Automated::GPIO::Mock_Output_Pin>;

} // namespace

/**
 * \brief Verify picolibrary::GPIO::Active_Low_Output_Pin::initialize() works properly.
 */
TEST( initialize, worksProperly )
{
    struct {
        Initial_Pin_State requested_state;
        Initial_Pin_State actual_state;
    } const test_cases[]{
        // clang-format off

        { Initial_Pin_State::LOW,  Initial_Pin_State::HIGH },
        { Initial_Pin_State::HIGH, Initial_Pin_State::LOW  },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto pin = Pin{};

        EXPECT_CALL( pin, initialize( test_case.actual_state ) );

        pin.initialize( test_case.requested_state );
    } // for
}

/**
 * \brief Verify picolibrary::GPIO::Active_Low_Output_Pin::transition_to_low() works
 *        properly.
 */
TEST( transitionToLow, worksProperly )
{
    auto pin = Pin{};

    EXPECT_CALL( pin, transition_to_high() );

    pin.transition_to_low();
}

/**
 * \brief Verify picolibrary::GPIO::Active_Low_Output_Pin::transition_to_high() works
 *        properly.
 */
TEST( transitionToHigh, worksProperly )
{
    auto pin = Pin{};

    EXPECT_CALL( pin, transition_to_low() );

    pin.transition_to_high();
}

/**
 * \brief Execute the picolibrary::GPIO::Active_Low_Output_Pin automated tests.
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

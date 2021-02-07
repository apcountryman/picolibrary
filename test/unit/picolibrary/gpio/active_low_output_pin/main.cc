/**
 * picolibrary
 *
 * Copyright 2020-2021, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::GPIO::Active_Low_Output_Pin unit test program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/gpio.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/gpio.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/utility.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::GPIO::Initial_Pin_State;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::testing::_;
using ::testing::Return;

using Pin = ::picolibrary::GPIO::Active_Low_Output_Pin<::picolibrary::Testing::Unit::GPIO::Mock_Output_Pin>;

} // namespace

/**
 * \brief Verify picolibrary::GPIO::Active_Low_Output_Pin::initialize() properly handles
 *        an initialization error.
 */
TEST( initialize, initializationError )
{
    auto pin = Pin{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( pin, initialize( _ ) ).WillOnce( Return( error ) );

    auto const result = pin.initialize( random<Initial_Pin_State>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::GPIO::Active_Low_Output_Pin::initialize() works properly.
 */
TEST( initialize, worksProperly )
{
    struct {
        Initial_Pin_State requested_state;
        Initial_Pin_State actual_state;
    } const test_cases[]{
        { Initial_Pin_State::HIGH, Initial_Pin_State::LOW },
        { Initial_Pin_State::LOW, Initial_Pin_State::HIGH },
    };

    for ( auto const test_case : test_cases ) {
        auto pin = Pin{};

        EXPECT_CALL( pin, initialize( test_case.actual_state ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

        EXPECT_FALSE( pin.initialize( test_case.requested_state ).is_error() );
    } // for
}

/**
 * \brief Verify picolibrary::GPIO::Active_Low_Output_Pin::transition_to_high() properly
 *        handles a state transition error.
 */
TEST( transitionToHigh, stateTransitionError )
{
    auto pin = Pin{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( pin, transition_to_low() ).WillOnce( Return( error ) );

    auto const result = pin.transition_to_high();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::GPIO::Active_Low_Output_Pin::transition_to_high() works
 *        properly.
 */
TEST( transitionToHigh, worksProperly )
{
    auto pin = Pin{};

    EXPECT_CALL( pin, transition_to_low() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( pin.transition_to_high().is_error() );
}

/**
 * \brief Verify picolibrary::GPIO::Active_Low_Output_Pin::transition_to_low() properly
 *        handles a state transition error.
 */
TEST( transitionToLow, stateTransitionError )
{
    auto pin = Pin{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( pin, transition_to_high() ).WillOnce( Return( error ) );

    auto const result = pin.transition_to_low();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::GPIO::Active_Low_Output_Pin::transition_to_low() works
 *        properly.
 */
TEST( transitionToLow, worksProperly )
{
    auto pin = Pin{};

    EXPECT_CALL( pin, transition_to_high() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( pin.transition_to_low().is_error() );
}

/**
 * \brief Execute the picolibrary::GPIO::Active_Low_Output_Pin unit tests.
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

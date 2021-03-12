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
 * \brief picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator unit test
 *        program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/gpio.h"
#include "picolibrary/indicator.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/gpio.h"
#include "picolibrary/testing/unit/indicator.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/void.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::GPIO::Initial_Pin_State;
using ::picolibrary::Indicator::Initial_Indicator_State;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::testing::_;
using ::testing::Return;

using Indicator =
    ::picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator<::picolibrary::Testing::Unit::GPIO::Mock_Output_Pin>;

} // namespace

/**
 * \brief Verify
 *        picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator::initialize()
 *        properly handles an initialization error.
 */
TEST( initialize, initializationError )
{
    auto indicator = Indicator{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( indicator, initialize( _ ) ).WillOnce( Return( error ) );

    auto const result = indicator.initialize( random<Initial_Indicator_State>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator::initialize()
 *        works properly.
 */
TEST( initialize, worksProperly )
{
    struct {
        Initial_Indicator_State indicator_state;
        Initial_Pin_State       pin_state;
    } const test_cases[]{
        { Initial_Indicator_State::ILLUMINATED, Initial_Pin_State::HIGH },
        { Initial_Indicator_State::EXTINGUISHED, Initial_Pin_State::LOW },
    };

    for ( auto const test_case : test_cases ) {
        auto indicator = Indicator{};

        EXPECT_CALL( indicator, initialize( test_case.pin_state ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

        EXPECT_FALSE( indicator.initialize( test_case.indicator_state ).is_error() );
    } // for
}

/**
 * \brief Verify
 *        picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator::illuminate()
 *        properly handles a state transition error.
 */
TEST( illuminate, stateTransitionError )
{
    auto indicator = Indicator{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( indicator, transition_to_high() ).WillOnce( Return( error ) );

    auto const result = indicator.illuminate();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator::illuminate()
 *        works properly.
 */
TEST( illuminate, worksProperly )
{
    auto indicator = Indicator{};

    EXPECT_CALL( indicator, transition_to_high() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( indicator.illuminate().is_error() );
}

/**
 * \brief Verify
 *        picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator::extinguish()
 *        properly handles a state transition error.
 */
TEST( extinguish, stateTransitionError )
{
    auto indicator = Indicator{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( indicator, transition_to_low() ).WillOnce( Return( error ) );

    auto const result = indicator.extinguish();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator::extinguish()
 *        works properly.
 */
TEST( extinguish, worksProperly )
{
    auto indicator = Indicator{};

    EXPECT_CALL( indicator, transition_to_low() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( indicator.extinguish().is_error() );
}

/**
 * \brief Execute the picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator
 *        unit tests.
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

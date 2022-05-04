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
 * \brief picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator automated test
 *        program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/gpio.h"
#include "picolibrary/indicator.h"
#include "picolibrary/testing/automated/gpio.h"

namespace {

using ::picolibrary::GPIO::Initial_Pin_State;
using ::picolibrary::Indicator::Initial_Indicator_State;

using Indicator =
    ::picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator<::picolibrary::Testing::Automated::GPIO::Mock_Output_Pin>;

} // namespace

/**
 * \brief Verify
 *        picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator::initialize()
 *        works properly.
 */
TEST( initialize, worksProperly )
{
    {
        auto indicator = Indicator{};

        EXPECT_CALL( indicator, initialize( Initial_Pin_State::LOW ) );

        indicator.initialize();
    }

    {
        struct {
            Initial_Indicator_State initial_indicator_state;
            Initial_Pin_State       initial_pin_state;
        } const test_cases[]{
            // clang-format off

            { Initial_Indicator_State::EXTINGUISHED, Initial_Pin_State::LOW  },
            { Initial_Indicator_State::ILLUMINATED,  Initial_Pin_State::HIGH },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto indicator = Indicator{};

            EXPECT_CALL( indicator, initialize( test_case.initial_pin_state ) );

            indicator.initialize( test_case.initial_indicator_state );
        } // for
    }
}

/**
 * \brief Verify
 *        picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator::extinguish()
 *        works properly.
 */
TEST( extinguish, worksProperly )
{
    auto indicator = Indicator{};

    EXPECT_CALL( indicator, transition_to_low() );

    indicator.extinguish();
}

/**
 * \brief Verify
 *        picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator::illuminate()
 *        works properly.
 */
TEST( illuminate, worksProperly )
{
    auto indicator = Indicator{};

    EXPECT_CALL( indicator, transition_to_high() );

    indicator.illuminate();
}

/**
 * \brief Execute the picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator
 *        automated tests.
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

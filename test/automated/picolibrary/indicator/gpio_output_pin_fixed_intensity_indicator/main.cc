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
 * \brief picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator automated test
 *        program.
 */

#include <ostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/gpio.h"
#include "picolibrary/indicator.h"
#include "picolibrary/testing/automated/gpio.h"
#include "picolibrary/testing/automated/indicator.h"

namespace {

using ::picolibrary::GPIO::Initial_Pin_State;
using ::picolibrary::Indicator::Initial_Indicator_State;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

using Indicator =
    ::picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator<::picolibrary::Testing::Automated::GPIO::Mock_Output_Pin>;

} // namespace

/**
 * \brief Verify
 *        picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator::initialize()
 *        works properly when the default argument is used.
 */
TEST( initializeDefaultArgument, worksProperly )
{
    auto indicator = Indicator{};

    EXPECT_CALL( indicator, initialize( Initial_Pin_State::LOW ) );

    indicator.initialize();
}

/**
 * \brief picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator::initialize()
 *        non-default argument test case.
 */
struct initializeNonDefaultArgument_Test_Case {
    /**
     * \brief The initial indicator state.
     */
    Initial_Indicator_State initial_indicator_state;

    /**
     * \brief The initial pin state.
     */
    Initial_Pin_State initial_pin_state;
};

auto operator<<( std::ostream & stream, initializeNonDefaultArgument_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".initial_indicator_state = " << test_case.initial_indicator_state
                  << ", "
                  << ".initial_pin_state = " << test_case.initial_pin_state
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator::initialize()
 *        non-default argument test case.
 */
class initializeNonDefaultArgument : public TestWithParam<initializeNonDefaultArgument_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator::initialize()
 *        works properly when a non-default argument is used.
 */
TEST_P( initializeNonDefaultArgument, worksProperly )
{
    auto const test_case = GetParam();

    auto indicator = Indicator{};

    EXPECT_CALL( indicator, initialize( test_case.initial_pin_state ) );

    indicator.initialize( test_case.initial_indicator_state );
}

/**
 * \brief picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator::initialize()
 *        non-default argument test cases.
 */
initializeNonDefaultArgument_Test_Case const initializeNonDefaultArgument_TEST_CASES[]{
    // clang-format off

    { Initial_Indicator_State::EXTINGUISHED, Initial_Pin_State::LOW  },
    { Initial_Indicator_State::ILLUMINATED,  Initial_Pin_State::HIGH },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, initializeNonDefaultArgument, ValuesIn( initializeNonDefaultArgument_TEST_CASES ) );

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

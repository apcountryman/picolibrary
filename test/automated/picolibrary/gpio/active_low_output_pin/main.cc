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

#include <ostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/gpio.h"
#include "picolibrary/testing/automated/gpio.h"

namespace {

using ::picolibrary::GPIO::Initial_Pin_State;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

using Pin = ::picolibrary::GPIO::Active_Low_Output_Pin<::picolibrary::Testing::Automated::GPIO::Mock_Output_Pin>;

} // namespace

/**
 * \brief picolibrary::GPIO::Active_Low_Output_Pin::initialize() test case.
 */
struct initialize_Test_Case {
    /**
     * \brief The requested initial pin state.
     */
    Initial_Pin_State requested_state;

    /**
     * \brief The actual initial pin state.
     */
    Initial_Pin_State actual_state;
};

auto operator<<( std::ostream & stream, initialize_Test_Case const & test_case ) -> std::ostream &
{
    auto const as_string = []( Initial_Pin_State initial_pin_state ) -> char const * {
        switch ( initial_pin_state ) {
            case Initial_Pin_State::LOW: return "Initial_Pin_State::LOW";
            case Initial_Pin_State::HIGH: return "Initial_Pin_State::HIGH";
            default: return "UNKNOWN";
        } // switch
    };

    // clang-format off

    return stream << "{ "
                  << ".requested_state = " << as_string( test_case.requested_state )
                  << ", "
                  << ".actual_state = " << as_string( test_case.actual_state )
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::GPIO::Active_Low_Output_Pin::initialize() test fixture.
 */
class initialize : public TestWithParam<initialize_Test_Case> {
};

/**
 * \brief Verify picolibrary::GPIO::Active_Low_Output_Pin::initialize() works properly.
 */
TEST_P( initialize, worksProperly )
{
    auto const test_case = GetParam();

    auto pin = Pin{};

    EXPECT_CALL( pin, initialize( test_case.actual_state ) );

    pin.initialize( test_case.requested_state );
}

/**
 * \brief picolibrary::GPIO::Active_Low_Output_Pin::initialize() test cases.
 */
initialize_Test_Case const initialize_TEST_CASES[]{
    // clang-format off

    { Initial_Pin_State::LOW,  Initial_Pin_State::HIGH },
    { Initial_Pin_State::HIGH, Initial_Pin_State::LOW  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, initialize, ValuesIn( initialize_TEST_CASES ) );

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

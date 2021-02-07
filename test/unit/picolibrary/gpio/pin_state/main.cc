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
 * \brief picolibrary::GPIO::Pin_State unit test program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/gpio.h"

namespace {

using ::picolibrary::GPIO::Pin_State;

} // namespace

/**
 * \brief Verify picolibrary::GPIO::Pin_State::Pin_State() works properly.
 */
TEST( constructor, worksProperly )
{
    struct {
        bool is_high;
    } const test_cases[]{
        { true },
        { false },
    };

    for ( auto const test_case : test_cases ) {
        auto const pin_state = Pin_State{ test_case.is_high };

        EXPECT_EQ( pin_state.is_high(), test_case.is_high );
        EXPECT_EQ( pin_state.is_low(), not test_case.is_high );
    } // for
}

/**
 * \brief Execute the picolibrary::GPIO::Pin_State unit tests.
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

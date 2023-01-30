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
 * \brief picolibrary::I2C::Bus_Control_Guard automated test program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/i2c.h"
#include "picolibrary/testing/automated/i2c.h"

namespace {

using ::picolibrary::I2C::Bus_Control_Guard;
using ::picolibrary::Testing::Automated::I2C::Mock_Controller;

} // namespace

/**
 * \brief Verify picolibrary::I2C::Bus_Control_Guard works properly.
 */
TEST( busControlGuard, worksProperly )
{
    auto controller = Mock_Controller{};

    EXPECT_CALL( controller, start() );

    auto const guard = Bus_Control_Guard{ controller };

    EXPECT_CALL( controller, stop() );
}

/**
 * \brief Execute the picolibrary::I2C::Bus_Control_Guard automated tests.
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

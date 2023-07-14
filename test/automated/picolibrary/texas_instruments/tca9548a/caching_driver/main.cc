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
 * \brief picolibrary::Texas_Instruments::TCA9548A::Caching_Driver automated test program.
 */

#include <cstdint>
#include <functional>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/testing/automated/i2c.h"
#include "picolibrary/testing/automated/texas_instruments/tca9548a.h"
#include "picolibrary/texas_instruments/tca9548a.h"

namespace {

using ::picolibrary::Testing::Automated::I2C::Mock_Controller;
using ::picolibrary::Testing::Automated::Texas_Instruments::TCA9548A::Mock_Driver;

using Caching_Driver =
    ::picolibrary::Texas_Instruments::TCA9548A::Caching_Driver<std::function<void()>, Mock_Controller, Mock_Driver>;

} // namespace

/**
 * \brief Verify picolibrary::Texas_Instruments::TCA9548A::Caching_Driver::write_control()
 *        and picolibrary::Texas_Instruments::TCA9548A::Caching_Driver::control() work
 *        properly.
 */
TEST( control, worksProperly )
{
    auto tca9548a = Caching_Driver{};

    EXPECT_EQ( tca9548a.control(), 0b00000000 );

    auto const data = std::uint8_t{ 0b11011000 };

    EXPECT_CALL( tca9548a, write_control( data ) );

    tca9548a.write_control( data );

    EXPECT_EQ( tca9548a.control(), data );

    tca9548a.reset_cache();

    EXPECT_EQ( tca9548a.control(), 0b00000000 );
}

/**
 * \brief Execute the picolibrary::Texas_Instruments::TCA9548A::Caching_Driver automated
 *        tests.
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

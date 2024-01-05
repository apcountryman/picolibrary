/**
 * picolibrary
 *
 * Copyright 2020-2024, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::Texas_Instruments::TCA9548A::Driver automated test program.
 */

#include <cstdint>
#include <functional>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/i2c.h"
#include "picolibrary/texas_instruments/tca9548a.h"

namespace {

using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::I2C::Mock_Controller;
using ::picolibrary::Testing::Automated::I2C::Mock_Device;
using ::picolibrary::Texas_Instruments::TCA9548A::Address_Numeric;
using ::picolibrary::Texas_Instruments::TCA9548A::Address_Transmitted;
using ::testing::Return;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

using Driver = ::picolibrary::Texas_Instruments::TCA9548A::Driver<std::function<void()>, Mock_Controller, Mock_Device>;

} // namespace

/**
 * \brief picolibrary::Texas_Instruments::TCA9548A::Driver::Driver(
 *        Bus_Multiplexer_Aligner, Controller &,
 *        picolibrary::Texas_Instruments::TCA9548A::Address_Transmitted,
 *        picolibrary::Error_Code const & ) test fixture.
 */
class constructor : public TestWithParam<Address_Transmitted> {
};

/**
 * \brief Verify picolibrary::Texas_Instruments::TCA9548A::Driver::Driver(
 *        Bus_Multiplexer_Aligner, Controller &,
 *        picolibrary::Texas_Instruments::TCA9548A::Address_Transmitted,
 *        picolibrary::Error_Code const & ) works properly.
 */
TEST_P( constructor, worksProperly )
{
    auto       controller                 = Mock_Controller{};
    auto const address                    = GetParam();
    auto const nonresponsive_device_error = Mock_Error{ 29 };

    auto const tca9548a = ::picolibrary::Texas_Instruments::TCA9548A::Driver{
        std::function<void()>{}, controller, address, nonresponsive_device_error
    };

    EXPECT_EQ( tca9548a.address(), address );
    EXPECT_EQ( tca9548a.nonresponsive_device_error(), nonresponsive_device_error );
}

/**
 * \brief picolibrary::Texas_Instruments::TCA9548A::Driver::Driver(
 *        Bus_Multiplexer_Aligner, Controller &,
 *        picolibrary::Texas_Instruments::TCA9548A::Address_Transmitted,
 *        picolibrary::Error_Code const & ) test cases.
 */
Address_Transmitted const constructor_TEST_CASES[]{
    // clang-format off

    { Address_Numeric{ 0b1110'000 } },
    { Address_Numeric{ 0b1110'001 } },
    { Address_Numeric{ 0b1110'010 } },
    { Address_Numeric{ 0b1110'011 } },
    { Address_Numeric{ 0b1110'100 } },
    { Address_Numeric{ 0b1110'101 } },
    { Address_Numeric{ 0b1110'110 } },
    { Address_Numeric{ 0b1110'111 } },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, constructor, ValuesIn( constructor_TEST_CASES ) );

/**
 * \brief Verify picolibrary::Texas_Instruments::TCA9548A::Driver::read_control() works
 *        properly.
 */
TEST( readControl, worksProperly )
{
    auto const tca9548a = Driver{};

    auto const data = std::uint8_t{ 0xD6 };

    EXPECT_CALL( tca9548a, read() ).WillOnce( Return( data ) );

    EXPECT_EQ( tca9548a.read_control(), data );
}

/**
 * \brief Verify picolibrary::Texas_Instruments::TCA9548A::Driver::write_control() works
 *        properly.
 */
TEST( writeControl, worksProperly )
{
    auto tca9548a = Driver{};

    auto const data = std::uint8_t{ 0xFD };

    EXPECT_CALL( tca9548a, write( data ) );

    tca9548a.write_control( data );
}

/**
 * \brief Execute the picolibrary::Texas_Instruments::TCA9548A::Driver automated tests.
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

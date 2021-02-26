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
 * \brief picolibrary::Microchip::MCP23008::Driver unit test program.
 */

#include <functional>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/i2c.h"
#include "picolibrary/microchip/mcp23008.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/i2c.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/void.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Generic_Error;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::I2C::Address;
using ::picolibrary::Microchip::MCP23008::make_driver;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::I2C::Mock_Controller;

} // namespace

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver( Bus_Multiplexer_Aligner,
 *        Controller, picolibrary::I2C::Address, picolibrary::Error_Code const & ) works
 *        properly.
 */
TEST( constructor, worksProperly )
{
    auto       controller                 = Mock_Controller{};
    auto const address                    = random<Address>();
    auto const nonresponsive_device_error = random<Mock_Error>();

    auto const mcp23008 = ::picolibrary::Microchip::MCP23008::Driver{
        std::function<Result<Void, Error_Code>()>{}, controller, address, nonresponsive_device_error
    };

    EXPECT_EQ( mcp23008.address(), address );
    EXPECT_EQ( mcp23008.nonresponsive_device_error(), nonresponsive_device_error );
}

/**
 * \brief Verify picolibrary::Micorchip::MCP23008::make_driver() properly handles an
 *        invalid device address.
 */
TEST( makeDriver, invalidAddress )
{
    {
        auto controller = Mock_Controller{};

        auto const result = make_driver(
            std::function<Result<Void, Error_Code>()>{},
            controller,
            random<Address>(
                Address{},
                Address{ Address::NUMERIC,
                         ::picolibrary::Microchip::MCP23008::Address::MIN.numeric() - 1 } ),
            random<Mock_Error>() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::INVALID_ARGUMENT );
    }

    {
        auto controller = Mock_Controller{};

        auto const result = make_driver(
            std::function<Result<Void, Error_Code>()>{},
            controller,
            random<Address>( Address{
                Address::NUMERIC, ::picolibrary::Microchip::MCP23008::Address::MAX.numeric() + 1 } ),
            random<Mock_Error>() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::INVALID_ARGUMENT );
    }
}

/**
 * \brief Verify picolibrary::Micorchip::MCP23008::make_driver() works properly.
 */
TEST( makeDriver, worksProperly )
{
    auto       controller = Mock_Controller{};
    auto const address    = random<Address>(
        ::picolibrary::Microchip::MCP23008::Address::MIN,
        ::picolibrary::Microchip::MCP23008::Address::MAX );
    auto const nonresponsive_device_error = random<Mock_Error>();

    auto const result = make_driver(
        std::function<Result<Void, Error_Code>()>{}, controller, address, nonresponsive_device_error );

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value().address(), address );
    EXPECT_EQ( result.value().nonresponsive_device_error(), nonresponsive_device_error );
}

/**
 * \brief Execute the picolibrary::Microchip::MCP23008::Driver unit tests.
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

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

#include <cstdint>
#include <functional>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/i2c.h"
#include "picolibrary/microchip/mcp23008.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/i2c.h"
#include "picolibrary/testing/unit/microchip/mcp23008.h"
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
using ::picolibrary::Testing::Unit::I2C::Mock_Device;
using ::picolibrary::Testing::Unit::Microchip::MCP23008::Mock_Register_Cache;
using ::testing::_;
using ::testing::A;
using ::testing::InSequence;
using ::testing::Return;

using Driver =
    ::picolibrary::Microchip::MCP23008::Driver<std::function<Result<Void, Error_Code>()>, Mock_Controller, Mock_Register_Cache, Mock_Device<std::uint8_t>>;

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
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_iodir() properly handles a
 *        read error.
 */
TEST( readIODIR, readError )
{
    auto const mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, read( _ ) ).WillOnce( Return( error ) );

    auto const result = mcp23008.read_iodir();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_iodir() works properly.
 */
TEST( readIODIR, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x00 ) ).WillOnce( Return( data ) );

    auto const result = mcp23008.read_iodir();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_iodir() properly handles
 *        a write error.
 */
TEST( writeIODIR, writeError )
{
    auto mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( mcp23008, cache_iodir( _ ) ).Times( 0 );

    auto const result = mcp23008.write_iodir( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_iodir() works properly.
 */
TEST( writeIODIR, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, write( 0x00, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( mcp23008, cache_iodir( data ) );

    EXPECT_FALSE( mcp23008.write_iodir( data ).is_error() );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_ipol() properly handles a
 *        read error.
 */
TEST( readIPOL, readError )
{
    auto const mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, read( _ ) ).WillOnce( Return( error ) );

    auto const result = mcp23008.read_ipol();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_ipol() works properly.
 */
TEST( readIPOL, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x01 ) ).WillOnce( Return( data ) );

    auto const result = mcp23008.read_ipol();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_ipol() properly handles a
 *        write error.
 */
TEST( writeIPOL, writeError )
{
    auto mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( mcp23008, cache_ipol( _ ) ).Times( 0 );

    auto const result = mcp23008.write_ipol( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_ipol() works properly.
 */
TEST( writeIPOL, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, write( 0x01, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( mcp23008, cache_ipol( data ) );

    EXPECT_FALSE( mcp23008.write_ipol( data ).is_error() );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_gpinten() properly handles
 *        a read error.
 */
TEST( readGPINTEN, readError )
{
    auto const mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, read( _ ) ).WillOnce( Return( error ) );

    auto const result = mcp23008.read_gpinten();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_gpinten() works properly.
 */
TEST( readGPINTEN, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x02 ) ).WillOnce( Return( data ) );

    auto const result = mcp23008.read_gpinten();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_gpinten() properly
 *        handles a write error.
 */
TEST( writeGPINTEN, writeError )
{
    auto mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( mcp23008, cache_gpinten( _ ) ).Times( 0 );

    auto const result = mcp23008.write_gpinten( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_gpinten() works properly.
 */
TEST( writeGPINTEN, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, write( 0x02, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( mcp23008, cache_gpinten( data ) );

    EXPECT_FALSE( mcp23008.write_gpinten( data ).is_error() );
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

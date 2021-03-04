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
#include <vector>

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
using ::picolibrary::Microchip::MCP23008::Interrupt_Mode;
using ::picolibrary::Microchip::MCP23008::make_driver;
using ::picolibrary::Microchip::MCP23008::SDA_Slew_Rate_Control_Configuration;
using ::picolibrary::Microchip::MCP23008::Sequential_Operation_Mode;
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
 *        Controller &, picolibrary::I2C::Address, picolibrary::Error_Code const & ) works
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
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_defval() properly handles
 *        a read error.
 */
TEST( readDEFVAL, readError )
{
    auto const mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, read( _ ) ).WillOnce( Return( error ) );

    auto const result = mcp23008.read_defval();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_defval() works properly.
 */
TEST( readDEFVAL, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x03 ) ).WillOnce( Return( data ) );

    auto const result = mcp23008.read_defval();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_defval() properly handles
 *        a write error.
 */
TEST( writeDEFVAL, writeError )
{
    auto mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( mcp23008, cache_defval( _ ) ).Times( 0 );

    auto const result = mcp23008.write_defval( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_defval() works properly.
 */
TEST( writeDEFVAL, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, write( 0x03, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( mcp23008, cache_defval( data ) );

    EXPECT_FALSE( mcp23008.write_defval( data ).is_error() );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_intcon() properly handles
 *        a read error.
 */
TEST( readINTCON, readError )
{
    auto const mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, read( _ ) ).WillOnce( Return( error ) );

    auto const result = mcp23008.read_intcon();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_intcon() works properly.
 */
TEST( readINTCON, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x04 ) ).WillOnce( Return( data ) );

    auto const result = mcp23008.read_intcon();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_intcon() properly handles
 *        a write error.
 */
TEST( writeINTCON, writeError )
{
    auto mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( mcp23008, cache_intcon( _ ) ).Times( 0 );

    auto const result = mcp23008.write_intcon( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_intcon() works properly.
 */
TEST( writeINTCON, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, write( 0x04, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( mcp23008, cache_intcon( data ) );

    EXPECT_FALSE( mcp23008.write_intcon( data ).is_error() );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_iocon() properly handles a
 *        read error.
 */
TEST( readIOCON, readError )
{
    auto const mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, read( _ ) ).WillOnce( Return( error ) );

    auto const result = mcp23008.read_iocon();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_iocon() works properly.
 */
TEST( readIOCON, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x05 ) ).WillOnce( Return( data ) );

    auto const result = mcp23008.read_iocon();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_iocon() properly handles
 *        a write error.
 */
TEST( writeIOCON, writeError )
{
    auto mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( mcp23008, cache_iocon( _ ) ).Times( 0 );

    auto const result = mcp23008.write_iocon( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_iocon() works properly.
 */
TEST( writeIOCON, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, write( 0x05, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( mcp23008, cache_iocon( data ) );

    EXPECT_FALSE( mcp23008.write_iocon( data ).is_error() );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_gppu() properly handles a
 *        read error.
 */
TEST( readGPPU, readError )
{
    auto const mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, read( _ ) ).WillOnce( Return( error ) );

    auto const result = mcp23008.read_gppu();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_gppu() works properly.
 */
TEST( readGPPU, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x06 ) ).WillOnce( Return( data ) );

    auto const result = mcp23008.read_gppu();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_gppu() properly handles a
 *        write error.
 */
TEST( writeGPPU, writeError )
{
    auto mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( mcp23008, cache_gppu( _ ) ).Times( 0 );

    auto const result = mcp23008.write_gppu( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_gppu() works properly.
 */
TEST( writeGPPU, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, write( 0x06, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( mcp23008, cache_gppu( data ) );

    EXPECT_FALSE( mcp23008.write_gppu( data ).is_error() );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_intf() properly handles a
 *        read error.
 */
TEST( readINTF, readError )
{
    auto const mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, read( _ ) ).WillOnce( Return( error ) );

    auto const result = mcp23008.read_intf();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_intf() works properly.
 */
TEST( readINTF, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x07 ) ).WillOnce( Return( data ) );

    auto const result = mcp23008.read_intf();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_intcap() properly handles
 *        a read error.
 */
TEST( readINTCAP, readError )
{
    auto const mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, read( _ ) ).WillOnce( Return( error ) );

    auto const result = mcp23008.read_intcap();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_intcap() works properly.
 */
TEST( readINTCAP, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x08 ) ).WillOnce( Return( data ) );

    auto const result = mcp23008.read_intcap();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_gpio() properly handles a
 *        read error.
 */
TEST( readGPIO, readError )
{
    auto const mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, read( _ ) ).WillOnce( Return( error ) );

    auto const result = mcp23008.read_gpio();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_gpio() works properly.
 */
TEST( readGPIO, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x09 ) ).WillOnce( Return( data ) );

    auto const result = mcp23008.read_gpio();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_gpio() properly handles a
 *        write error.
 */
TEST( writeGPIO, writeError )
{
    auto mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( mcp23008, cache_gpio( _ ) ).Times( 0 );

    auto const result = mcp23008.write_gpio( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_gpio() works properly.
 */
TEST( writeGPIO, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, write( 0x09, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( mcp23008, cache_gpio( data ) );

    EXPECT_FALSE( mcp23008.write_gpio( data ).is_error() );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_olat() properly handles a
 *        read error.
 */
TEST( readOLAT, readError )
{
    auto const mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, read( _ ) ).WillOnce( Return( error ) );

    auto const result = mcp23008.read_olat();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_olat() works properly.
 */
TEST( readOLAT, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x0A ) ).WillOnce( Return( data ) );

    auto const result = mcp23008.read_olat();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_olat() properly handles a
 *        write error.
 */
TEST( writeOLAT, writeError )
{
    auto mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( mcp23008, cache_olat( _ ) ).Times( 0 );

    auto const result = mcp23008.write_olat( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_olat() works properly.
 */
TEST( writeOLAT, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, write( 0x0A, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( mcp23008, cache_olat( data ) );

    EXPECT_FALSE( mcp23008.write_olat( data ).is_error() );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_interrupt_context()
 *        properly handles a read error.
 */
TEST( readInterruptContext, readError )
{
    auto const mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, read( _, _ ) ).WillOnce( Return( error ) );

    auto const result = mcp23008.read_interrupt_context();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_interrupt_context() works
 *        properly.
 */
TEST( readInterruptContext, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const intf   = random<std::uint8_t>();
    auto const intcap = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x07, _ ) ).WillOnce( Return( std::vector<std::uint8_t>{ intf, intcap } ) );

    auto const result = mcp23008.read_interrupt_context();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value().intf, intf );
    EXPECT_EQ( result.value().intcap, intcap );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::configure() properly handles a
 *        write error.
 */
TEST( configure, writeError )
{
    auto mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( mcp23008, cache_iocon( _ ) ).Times( 0 );

    auto const result = mcp23008.configure(
        random<Sequential_Operation_Mode>(),
        random<SDA_Slew_Rate_Control_Configuration>(),
        random<Interrupt_Mode>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::configure() works properly.
 */
TEST( configure, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23008 = Driver{};

    auto const sequential_operation_mode           = random<Sequential_Operation_Mode>();
    auto const sda_slew_rate_control_configuration = random<SDA_Slew_Rate_Control_Configuration>();
    auto const interrupt_mode                      = random<Interrupt_Mode>();

    auto const data = static_cast<std::uint8_t>(
        static_cast<std::uint8_t>( sequential_operation_mode )
        | static_cast<std::uint8_t>( sda_slew_rate_control_configuration )
        | static_cast<std::uint8_t>( interrupt_mode ) );

    EXPECT_CALL( mcp23008, write( 0x05, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( mcp23008, cache_iocon( data ) );

    EXPECT_FALSE( mcp23008
                      .configure( sequential_operation_mode, sda_slew_rate_control_configuration, interrupt_mode )
                      .is_error() );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23008::Driver::configure_pin_as_internally_pulled_up_input()
 *        properly handles a write error.
 */
TEST( configurePinAsInternallyPulledUpInput, writeError )
{
    auto mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( mcp23008, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( mcp23008, cache_iodir( _ ) ).Times( 0 );

    auto const result = mcp23008.configure_pin_as_internally_pulled_up_input( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23008::Driver::configure_pin_as_internally_pulled_up_input()
 *        works properly.
 */
TEST( configurePinAsInternallyPulledUpInput, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23008 = Driver{};

    auto const iodir = random<std::uint8_t>();
    auto const mask  = random<std::uint8_t>();
    auto const data  = static_cast<std::uint8_t>( iodir | mask );

    EXPECT_CALL( mcp23008, iodir() ).WillOnce( Return( iodir ) );
    EXPECT_CALL( mcp23008, write( 0x00, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( mcp23008, cache_iodir( data ) );

    EXPECT_FALSE( mcp23008.configure_pin_as_internally_pulled_up_input( mask ).is_error() );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23008::Driver::configure_pin_as_open_drain_output()
 *        properly handles a write error.
 */
TEST( configurePinAsOpenDrainOutput, writeError )
{
    auto mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, gpio() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( mcp23008, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( mcp23008, cache_gpio( _ ) ).Times( 0 );

    auto const result = mcp23008.configure_pin_as_open_drain_output( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23008::Driver::configure_pin_as_open_drain_output()
 *        works properly.
 */
TEST( configurePinAsOpenDrainOutput, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23008 = Driver{};

    auto const gpio = random<std::uint8_t>();
    auto const mask = random<std::uint8_t>();
    auto const data = static_cast<std::uint8_t>( gpio & ~mask );

    EXPECT_CALL( mcp23008, gpio() ).WillOnce( Return( gpio ) );
    EXPECT_CALL( mcp23008, write( 0x09, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( mcp23008, cache_gpio( data ) );

    EXPECT_FALSE( mcp23008.configure_pin_as_open_drain_output( mask ).is_error() );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23008::Driver::configure_pin_as_push_pull_output()
 *        properly handles a write error.
 */
TEST( configurePinAsPushPullOutput, writeError )
{
    auto mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( mcp23008, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( mcp23008, cache_iodir( _ ) ).Times( 0 );

    auto const result = mcp23008.configure_pin_as_push_pull_output( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23008::Driver::configure_pin_as_push_pull_output()
 *        works properly.
 */
TEST( configurePinAsPushPullOutput, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23008 = Driver{};

    auto const iodir = random<std::uint8_t>();
    auto const mask  = random<std::uint8_t>();
    auto const data  = static_cast<std::uint8_t>( iodir & ~mask );

    EXPECT_CALL( mcp23008, iodir() ).WillOnce( Return( iodir ) );
    EXPECT_CALL( mcp23008, write( 0x00, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( mcp23008, cache_iodir( data ) );

    EXPECT_FALSE( mcp23008.configure_pin_as_push_pull_output( mask ).is_error() );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::enable_pull_up() properly
 *        handles a write error.
 */
TEST( enablePullUp, writeError )
{
    auto mcp23008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp23008, gppu() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( mcp23008, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( mcp23008, cache_gppu( _ ) ).Times( 0 );

    auto const result = mcp23008.enable_pull_up( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::enable_pull_up() works
 *        properly.
 */
TEST( enablePullUp, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23008 = Driver{};

    auto const gppu = random<std::uint8_t>();
    auto const mask = random<std::uint8_t>();
    auto const data = static_cast<std::uint8_t>( gppu | mask );

    EXPECT_CALL( mcp23008, gppu() ).WillOnce( Return( gppu ) );
    EXPECT_CALL( mcp23008, write( 0x06, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( mcp23008, cache_gppu( data ) );

    EXPECT_FALSE( mcp23008.enable_pull_up( mask ).is_error() );
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

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
 * \brief picolibrary::Microchip::MCP23008::Driver automated test program.
 */

#include <cstdint>
#include <functional>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/microchip/mcp23008.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/i2c.h"

namespace {

using ::picolibrary::Microchip::MCP23008::Address_Numeric;
using ::picolibrary::Microchip::MCP23008::Address_Transmitted;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::I2C::Mock_Controller;
using ::picolibrary::Testing::Automated::I2C::Mock_Device;
using ::testing::Eq;
using ::testing::Return;
using ::testing::SafeMatcherCast;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

using Driver = ::picolibrary::Microchip::MCP23008::Driver<std::function<void()>, Mock_Controller, Mock_Device>;

} // namespace

/**
 * \brief picolibrary::Microchip::MCP23008::Driver::Driver( Bus_Multiplexer_Aligner,
 *        Controller &, picolibrary::Microchip::MCP23008::Address_Transmitted,
 *        picolibrary::Error_Code const & ) test fixture.
 */
class constructor : public TestWithParam<Address_Transmitted> {
};

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::Driver(
 *        Bus_Multiplexer_Aligner, Controller &,
 *        picolibrary::Microchip::MCP23008::Address_Transmitted, picolibrary::Error_Code
 *        const & ) works properly.
 */
TEST_P( constructor, worksProperly )
{
    auto       controller                 = Mock_Controller{};
    auto const address                    = GetParam();
    auto const nonresponsive_device_error = Mock_Error{ 68 };

    auto const mcp23008 = ::picolibrary::Microchip::MCP23008::Driver{
        std::function<void()>{}, controller, address, nonresponsive_device_error
    };

    EXPECT_EQ( mcp23008.address(), address );
    EXPECT_EQ( mcp23008.nonresponsive_device_error(), nonresponsive_device_error );
}

/**
 * \brief picolibrary::Microchip::MCP23008::Driver::Driver( Bus_Multiplexer_Aligner,
 *        Controller &, picolibrary::Microchip::MCP23008::Address_Transmitted,
 *        picolibrary::Error_Code const & ) test cases.
 */
Address_Transmitted const constructor_TEST_CASES[]{
    // clang-format off

    { Address_Numeric{ 0b0100'000 } },
    { Address_Numeric{ 0b0100'001 } },
    { Address_Numeric{ 0b0100'010 } },
    { Address_Numeric{ 0b0100'011 } },
    { Address_Numeric{ 0b0100'100 } },
    { Address_Numeric{ 0b0100'101 } },
    { Address_Numeric{ 0b0100'110 } },
    { Address_Numeric{ 0b0100'111 } },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, constructor, ValuesIn( constructor_TEST_CASES ) );

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_iodir() works properly.
 */
TEST( readIODIR, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = std::uint8_t{ 0x14 };

    EXPECT_CALL( mcp23008, read( SafeMatcherCast<std::uint8_t>( Eq( 0x00 ) ) ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_iodir(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_iodir() works properly.
 */
TEST( writeIODIR, worksProperly )
{
    auto mcp23008 = Driver{};

    auto const data = std::uint8_t{ 0x1E };

    EXPECT_CALL( mcp23008, write( SafeMatcherCast<std::uint8_t>( Eq( 0x00 ) ), data ) );

    mcp23008.write_iodir( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_ipol() works properly.
 */
TEST( readIPOL, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = std::uint8_t{ 0x66 };

    EXPECT_CALL( mcp23008, read( SafeMatcherCast<std::uint8_t>( Eq( 0x01 ) ) ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_ipol(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_ipol() works properly.
 */
TEST( writeIPOL, worksProperly )
{
    auto mcp23008 = Driver{};

    auto const data = std::uint8_t{ 0xFB };

    EXPECT_CALL( mcp23008, write( SafeMatcherCast<std::uint8_t>( Eq( 0x01 ) ), data ) );

    mcp23008.write_ipol( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_gpinten() works properly.
 */
TEST( readGPINTEN, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = std::uint8_t{ 0x14 };

    EXPECT_CALL( mcp23008, read( SafeMatcherCast<std::uint8_t>( Eq( 0x02 ) ) ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_gpinten(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_gpinten() works properly.
 */
TEST( writeGPINTEN, worksProperly )
{
    auto mcp23008 = Driver{};

    auto const data = std::uint8_t{ 0x01 };

    EXPECT_CALL( mcp23008, write( SafeMatcherCast<std::uint8_t>( Eq( 0x02 ) ), data ) );

    mcp23008.write_gpinten( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_defval() works properly.
 */
TEST( readDEFVAL, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = std::uint8_t{ 0x6F };

    EXPECT_CALL( mcp23008, read( SafeMatcherCast<std::uint8_t>( Eq( 0x03 ) ) ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_defval(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_defval() works properly.
 */
TEST( writeDEFVAL, worksProperly )
{
    auto mcp23008 = Driver{};

    auto const data = std::uint8_t{ 0xC5 };

    EXPECT_CALL( mcp23008, write( SafeMatcherCast<std::uint8_t>( Eq( 0x03 ) ), data ) );

    mcp23008.write_defval( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_intcon() works properly.
 */
TEST( readINTCON, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = std::uint8_t{ 0x37 };

    EXPECT_CALL( mcp23008, read( SafeMatcherCast<std::uint8_t>( Eq( 0x04 ) ) ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_intcon(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_intcon() works properly.
 */
TEST( writeINTCON, worksProperly )
{
    auto mcp23008 = Driver{};

    auto const data = std::uint8_t{ 0x2B };

    EXPECT_CALL( mcp23008, write( SafeMatcherCast<std::uint8_t>( Eq( 0x04 ) ), data ) );

    mcp23008.write_intcon( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_iocon() works properly.
 */
TEST( readIOCON, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = std::uint8_t{ 0x06 };

    EXPECT_CALL( mcp23008, read( SafeMatcherCast<std::uint8_t>( Eq( 0x05 ) ) ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_iocon(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_iocon() works properly.
 */
TEST( writeIOCON, worksProperly )
{
    auto mcp23008 = Driver{};

    auto const data = std::uint8_t{ 0x85 };

    EXPECT_CALL( mcp23008, write( SafeMatcherCast<std::uint8_t>( Eq( 0x05 ) ), data ) );

    mcp23008.write_iocon( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_gppu() works properly.
 */
TEST( readGPPU, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = std::uint8_t{ 0xD5 };

    EXPECT_CALL( mcp23008, read( SafeMatcherCast<std::uint8_t>( Eq( 0x06 ) ) ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_gppu(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_gppu() works properly.
 */
TEST( writeGPPU, worksProperly )
{
    auto mcp23008 = Driver{};

    auto const data = std::uint8_t{ 0x25 };

    EXPECT_CALL( mcp23008, write( SafeMatcherCast<std::uint8_t>( Eq( 0x06 ) ), data ) );

    mcp23008.write_gppu( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_intf() works properly.
 */
TEST( readINTF, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = std::uint8_t{ 0x94 };

    EXPECT_CALL( mcp23008, read( SafeMatcherCast<std::uint8_t>( Eq( 0x07 ) ) ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_intf(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_intcap() works properly.
 */
TEST( readINTCAP, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = std::uint8_t{ 0x2D };

    EXPECT_CALL( mcp23008, read( SafeMatcherCast<std::uint8_t>( Eq( 0x08 ) ) ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_intcap(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_gpio() works properly.
 */
TEST( readGPIO, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = std::uint8_t{ 0x9C };

    EXPECT_CALL( mcp23008, read( SafeMatcherCast<std::uint8_t>( Eq( 0x09 ) ) ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_gpio(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_gpio() works properly.
 */
TEST( writeGPIO, worksProperly )
{
    auto mcp23008 = Driver{};

    auto const data = std::uint8_t{ 0x04 };

    EXPECT_CALL( mcp23008, write( SafeMatcherCast<std::uint8_t>( Eq( 0x09 ) ), data ) );

    mcp23008.write_gpio( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_olat() works properly.
 */
TEST( readOLAT, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = std::uint8_t{ 0x38 };

    EXPECT_CALL( mcp23008, read( SafeMatcherCast<std::uint8_t>( Eq( 0x0A ) ) ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_olat(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_olat() works properly.
 */
TEST( writeOLAT, worksProperly )
{
    auto mcp23008 = Driver{};

    auto const data = std::uint8_t{ 0xF5 };

    EXPECT_CALL( mcp23008, write( SafeMatcherCast<std::uint8_t>( Eq( 0x0A ) ), data ) );

    mcp23008.write_olat( data );
}

/**
 * \brief Execute the picolibrary::Microchip::MCP23008::Driver automated tests.
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

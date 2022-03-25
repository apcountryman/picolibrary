/**
 * picolibrary
 *
 * Copyright 2020-2022, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
#include "picolibrary/i2c.h"
#include "picolibrary/microchip/mcp23008.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/i2c.h"
#include "picolibrary/testing/unit/random.h"

namespace {

using ::picolibrary::I2C::Address_Numeric;
using ::picolibrary::I2C::Address_Transmitted;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::I2C::Mock_Controller;
using ::picolibrary::Testing::Unit::I2C::Mock_Device;
using ::testing::Return;

using Driver = ::picolibrary::Microchip::MCP23008::Driver<std::function<void()>, Mock_Controller, Mock_Device>;

} // namespace

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::Driver(
 *        Bus_Multiplexer_Aligner, Controller &, picolibrary::I2C::Address_Transmitted,
 *        picolibrary::Error_Code const & ) works properly.
 */
TEST( constructor, worksProperly )
{
    struct {
        Address_Transmitted address;
    } const test_cases[]{
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

    for ( auto const test_case : test_cases ) {
        auto       controller                 = Mock_Controller{};
        auto const nonresponsive_device_error = random<Mock_Error>();

        auto const mcp23008 = ::picolibrary::Microchip::MCP23008::Driver{
            std::function<void()>{}, controller, test_case.address, nonresponsive_device_error
        };

        EXPECT_EQ( mcp23008.address(), test_case.address );
        EXPECT_EQ( mcp23008.nonresponsive_device_error(), nonresponsive_device_error );
    } // for
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_iodir() works properly.
 */
TEST( readIODIR, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x00 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_iodir(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_iodir() works properly.
 */
TEST( writeIODIR, worksProperly )
{
    auto mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, write( 0x00, data ) );

    mcp23008.write_iodir( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_ipol() works properly.
 */
TEST( readIPOL, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x01 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_ipol(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_ipol() works properly.
 */
TEST( writeIPOL, worksProperly )
{
    auto mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, write( 0x01, data ) );

    mcp23008.write_ipol( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_gpinten() works properly.
 */
TEST( readGPINTEN, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x02 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_gpinten(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_gpinten() works properly.
 */
TEST( writeGPINTEN, worksProperly )
{
    auto mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, write( 0x02, data ) );

    mcp23008.write_gpinten( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_defval() works properly.
 */
TEST( readDEFVAL, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x03 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_defval(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_defval() works properly.
 */
TEST( writeDEFVAL, worksProperly )
{
    auto mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, write( 0x03, data ) );

    mcp23008.write_defval( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_intcon() works properly.
 */
TEST( readINTCON, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x04 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_intcon(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_intcon() works properly.
 */
TEST( writeINTCON, worksProperly )
{
    auto mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, write( 0x04, data ) );

    mcp23008.write_intcon( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_iocon() works properly.
 */
TEST( readIOCON, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x05 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_iocon(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_iocon() works properly.
 */
TEST( writeIOCON, worksProperly )
{
    auto mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, write( 0x05, data ) );

    mcp23008.write_iocon( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_gppu() works properly.
 */
TEST( readGPPU, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x06 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_gppu(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_gppu() works properly.
 */
TEST( writeGPPU, worksProperly )
{
    auto mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, write( 0x06, data ) );

    mcp23008.write_gppu( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_intf() works properly.
 */
TEST( readINTF, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x07 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_intf(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_intcap() works properly.
 */
TEST( readINTCAP, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x08 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_intcap(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_gpio() works properly.
 */
TEST( readGPIO, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x09 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_gpio(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_gpio() works properly.
 */
TEST( writeGPIO, worksProperly )
{
    auto mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, write( 0x09, data ) );

    mcp23008.write_gpio( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::read_olat() works properly.
 */
TEST( readOLAT, worksProperly )
{
    auto const mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, read( 0x0A ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23008.read_olat(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Driver::write_olat() works properly.
 */
TEST( writeOLAT, worksProperly )
{
    auto mcp23008 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23008, write( 0x0A, data ) );

    mcp23008.write_olat( data );
}

/**
 * \brief Execute the picolibrary::Microchip::MCP23008::Driver unit tests.
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

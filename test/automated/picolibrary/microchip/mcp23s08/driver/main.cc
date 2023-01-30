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
 * \brief picolibrary::Microchip::MCP23S08::Driver automated test program.
 */

#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/microchip/mcp23s08.h"
#include "picolibrary/testing/automated/microchip/mcp23s08.h"
#include "picolibrary/testing/automated/random.h"
#include "picolibrary/testing/automated/spi.h"

namespace {

using ::picolibrary::Microchip::MCP23S08::Address_Numeric;
using ::picolibrary::Microchip::MCP23S08::Address_Transmitted;
using ::picolibrary::Testing::Automated::random;
using ::picolibrary::Testing::Automated::Microchip::MCP23S08::Mock_Communication_Controller;
using ::picolibrary::Testing::Automated::SPI::Mock_Controller;
using ::picolibrary::Testing::Automated::SPI::Mock_Device_Selector;
using ::testing::Return;

using Driver = ::picolibrary::Microchip::MCP23S08::Driver<Mock_Controller, Mock_Device_Selector, Mock_Communication_Controller>;

} // namespace

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::Driver( Controller &,
 *        Controller::Configuration const &, Device_Selector,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) works properly.
 */
TEST( constructor, worksProperly )
{
    struct {
        Address_Transmitted address;
    } const test_cases[]{
        // clang-format off

        { Address_Numeric{ 0b01000'00 } },
        { Address_Numeric{ 0b01000'01 } },
        { Address_Numeric{ 0b01000'10 } },
        { Address_Numeric{ 0b01000'11 } },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto controller      = Mock_Controller{};
        auto device_selector = Mock_Device_Selector{};

        auto const driver = ::picolibrary::Microchip::MCP23S08::Driver{
            controller,
            random<Mock_Controller::Configuration>(),
            device_selector.handle(),
            test_case.address
        };

        EXPECT_EQ( driver.address(), test_case.address );
    } // for
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::read_iodir() works properly.
 */
TEST( readIODIR, worksProperly )
{
    auto const mcp23s08 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23s08, read( 0x00 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23s08.read_iodir(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::write_iodir() works properly.
 */
TEST( writeIODIR, worksProperly )
{
    auto mcp23s08 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23s08, write( 0x00, data ) );

    mcp23s08.write_iodir( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::read_ipol() works properly.
 */
TEST( readIPOL, worksProperly )
{
    auto const mcp23s08 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23s08, read( 0x01 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23s08.read_ipol(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::write_ipol() works properly.
 */
TEST( writeIPOL, worksProperly )
{
    auto mcp23s08 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23s08, write( 0x01, data ) );

    mcp23s08.write_ipol( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::read_gpinten() works properly.
 */
TEST( readGPINTEN, worksProperly )
{
    auto const mcp23s08 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23s08, read( 0x02 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23s08.read_gpinten(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::write_gpinten() works properly.
 */
TEST( writeGPINTEN, worksProperly )
{
    auto mcp23s08 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23s08, write( 0x02, data ) );

    mcp23s08.write_gpinten( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::read_defval() works properly.
 */
TEST( readDEFVAL, worksProperly )
{
    auto const mcp23s08 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23s08, read( 0x03 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23s08.read_defval(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::write_defval() works properly.
 */
TEST( writeDEFVAL, worksProperly )
{
    auto mcp23s08 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23s08, write( 0x03, data ) );

    mcp23s08.write_defval( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::read_intcon() works properly.
 */
TEST( readINTCON, worksProperly )
{
    auto const mcp23s08 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23s08, read( 0x04 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23s08.read_intcon(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::write_intcon() works properly.
 */
TEST( writeINTCON, worksProperly )
{
    auto mcp23s08 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23s08, write( 0x04, data ) );

    mcp23s08.write_intcon( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::read_iocon() works properly.
 */
TEST( readIOCON, worksProperly )
{
    auto const mcp23s08 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23s08, read( 0x05 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23s08.read_iocon(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::write_iocon() works properly.
 */
TEST( writeIOCON, worksProperly )
{
    auto mcp23s08 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23s08, write( 0x05, data ) );

    mcp23s08.write_iocon( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::read_gppu() works properly.
 */
TEST( readGPPU, worksProperly )
{
    auto const mcp23s08 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23s08, read( 0x06 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23s08.read_gppu(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::write_gppu() works properly.
 */
TEST( writeGPPU, worksProperly )
{
    auto mcp23s08 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23s08, write( 0x06, data ) );

    mcp23s08.write_gppu( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::read_intf() works properly.
 */
TEST( readINTF, worksProperly )
{
    auto const mcp23s08 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23s08, read( 0x07 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23s08.read_intf(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::read_intcap() works properly.
 */
TEST( readINTCAP, worksProperly )
{
    auto const mcp23s08 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23s08, read( 0x08 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23s08.read_intcap(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::read_gpio() works properly.
 */
TEST( readGPIO, worksProperly )
{
    auto const mcp23s08 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23s08, read( 0x09 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23s08.read_gpio(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::write_gpio() works properly.
 */
TEST( writeGPIO, worksProperly )
{
    auto mcp23s08 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23s08, write( 0x09, data ) );

    mcp23s08.write_gpio( data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::read_olat() works properly.
 */
TEST( readOLAT, worksProperly )
{
    auto const mcp23s08 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23s08, read( 0x0A ) ).WillOnce( Return( data ) );

    EXPECT_EQ( mcp23s08.read_olat(), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Driver::write_olat() works properly.
 */
TEST( writeOLAT, worksProperly )
{
    auto mcp23s08 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( mcp23s08, write( 0x0A, data ) );

    mcp23s08.write_olat( data );
}

/**
 * \brief Execute the picolibrary::Microchip::MCP23S08::Driver automated tests.
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

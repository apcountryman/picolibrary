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
 * \brief picolibrary::Microchip::MCP23X08::Caching_Driver automated test program.
 */

#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/microchip/mcp23x08.h"
#include "picolibrary/testing/automated/microchip/mcp23x08.h"

namespace {

using ::picolibrary::Testing::Automated::Microchip::MCP23X08::Mock_Driver;

using Caching_Driver = ::picolibrary::Microchip::MCP23X08::Caching_Driver<Mock_Driver>;

} // namespace

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Caching_Driver::write_iodir() and
 *        picolibrary::Microchip::MCP23X08::Caching_Driver::iodir() work properly.
 */
TEST( iodir, worksProperly )
{
    auto mcp23x08 = Caching_Driver{};

    EXPECT_EQ( mcp23x08.iodir(), 0xFF );

    auto const data = std::uint8_t{ 0xC9 };

    EXPECT_CALL( mcp23x08, write_iodir( data ) );

    mcp23x08.write_iodir( data );

    EXPECT_EQ( mcp23x08.iodir(), data );

    mcp23x08.reset_cache();

    EXPECT_EQ( mcp23x08.iodir(), 0xFF );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Caching_Driver::write_ipol() and
 *        picolibrary::Microchip::MCP23X08::Caching_Driver::ipol() work properly.
 */
TEST( ipol, worksProperly )
{
    auto mcp23x08 = Caching_Driver{};

    EXPECT_EQ( mcp23x08.ipol(), 0x00 );

    auto const data = std::uint8_t{ 0xA0 };

    EXPECT_CALL( mcp23x08, write_ipol( data ) );

    mcp23x08.write_ipol( data );

    EXPECT_EQ( mcp23x08.ipol(), data );

    mcp23x08.reset_cache();

    EXPECT_EQ( mcp23x08.ipol(), 0x00 );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Caching_Driver::write_gpinten() and
 *        picolibrary::Microchip::MCP23X08::Caching_Driver::gpinten() work properly.
 */
TEST( gpinten, worksProperly )
{
    auto mcp23x08 = Caching_Driver{};

    EXPECT_EQ( mcp23x08.gpinten(), 0x00 );

    auto const data = std::uint8_t{ 0xF7 };

    EXPECT_CALL( mcp23x08, write_gpinten( data ) );

    mcp23x08.write_gpinten( data );

    EXPECT_EQ( mcp23x08.gpinten(), data );

    mcp23x08.reset_cache();

    EXPECT_EQ( mcp23x08.gpinten(), 0x00 );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Caching_Driver::write_defval() and
 *        picolibrary::Microchip::MCP23X08::Caching_Driver::defval() work properly.
 */
TEST( defval, worksProperly )
{
    auto mcp23x08 = Caching_Driver{};

    EXPECT_EQ( mcp23x08.defval(), 0x00 );

    auto const data = std::uint8_t{ 0xE4 };

    EXPECT_CALL( mcp23x08, write_defval( data ) );

    mcp23x08.write_defval( data );

    EXPECT_EQ( mcp23x08.defval(), data );

    mcp23x08.reset_cache();

    EXPECT_EQ( mcp23x08.defval(), 0x00 );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Caching_Driver::write_intcon() and
 *        picolibrary::Microchip::MCP23X08::Caching_Driver::intcon() work properly.
 */
TEST( intcon, worksProperly )
{
    auto mcp23x08 = Caching_Driver{};

    EXPECT_EQ( mcp23x08.intcon(), 0x00 );

    auto const data = std::uint8_t{ 0x95 };

    EXPECT_CALL( mcp23x08, write_intcon( data ) );

    mcp23x08.write_intcon( data );

    EXPECT_EQ( mcp23x08.intcon(), data );

    mcp23x08.reset_cache();

    EXPECT_EQ( mcp23x08.intcon(), 0x00 );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Caching_Driver::write_iocon() and
 *        picolibrary::Microchip::MCP23X08::Caching_Driver::iocon() work properly.
 */
TEST( iocon, worksProperly )
{
    auto mcp23x08 = Caching_Driver{};

    EXPECT_EQ( mcp23x08.iocon(), 0x00 );

    auto const data = std::uint8_t{ 0x22 };

    EXPECT_CALL( mcp23x08, write_iocon( data ) );

    mcp23x08.write_iocon( data );

    EXPECT_EQ( mcp23x08.iocon(), data );

    mcp23x08.reset_cache();

    EXPECT_EQ( mcp23x08.iocon(), 0x00 );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Caching_Driver::write_gppu() and
 *        picolibrary::Microchip::MCP23X08::Caching_Driver::gppu() work properly.
 */
TEST( gppu, worksProperly )
{
    auto mcp23x08 = Caching_Driver{};

    EXPECT_EQ( mcp23x08.gppu(), 0x00 );

    auto const data = std::uint8_t{ 0x6C };

    EXPECT_CALL( mcp23x08, write_gppu( data ) );

    mcp23x08.write_gppu( data );

    EXPECT_EQ( mcp23x08.gppu(), data );

    mcp23x08.reset_cache();

    EXPECT_EQ( mcp23x08.gppu(), 0x00 );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Caching_Driver::write_gpio() and
 *        picolibrary::Microchip::MCP23X08::Caching_Driver::olat() work properly.
 */
TEST( gpio, worksProperly )
{
    auto mcp23x08 = Caching_Driver{};

    EXPECT_EQ( mcp23x08.olat(), 0x00 );

    auto const data = std::uint8_t{ 0x58 };

    EXPECT_CALL( mcp23x08, write_gpio( data ) );

    mcp23x08.write_gpio( data );

    EXPECT_EQ( mcp23x08.olat(), data );

    mcp23x08.reset_cache();

    EXPECT_EQ( mcp23x08.olat(), 0x00 );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Caching_Driver::write_olat() and
 *        picolibrary::Microchip::MCP23X08::Caching_Driver::olat() work properly.
 */
TEST( olat, worksProperly )
{
    auto mcp23x08 = Caching_Driver{};

    EXPECT_EQ( mcp23x08.olat(), 0x00 );

    auto const data = std::uint8_t{ 0x1C };

    EXPECT_CALL( mcp23x08, write_olat( data ) );

    mcp23x08.write_olat( data );

    EXPECT_EQ( mcp23x08.olat(), data );

    mcp23x08.reset_cache();

    EXPECT_EQ( mcp23x08.olat(), 0x00 );
}

/**
 * \brief Execute the picolibrary::Microchip::MCP23X08::Caching_Driver automated tests.
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

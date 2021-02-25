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
 * \brief picolibrary::Microchip::MCP23008::Register_Cache unit test program.
 */

#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/microchip/mcp23008.h"
#include "picolibrary/testing/unit/random.h"

namespace {

using ::picolibrary::Testing::Unit::random;

class Cache : public ::picolibrary::Microchip::MCP23008::Register_Cache {
  public:
    constexpr Cache() noexcept
    {
    }

    using ::picolibrary::Microchip::MCP23008::Register_Cache::cache_defval;
    using ::picolibrary::Microchip::MCP23008::Register_Cache::cache_gpinten;
    using ::picolibrary::Microchip::MCP23008::Register_Cache::cache_gpio;
    using ::picolibrary::Microchip::MCP23008::Register_Cache::cache_gppu;
    using ::picolibrary::Microchip::MCP23008::Register_Cache::cache_intcon;
    using ::picolibrary::Microchip::MCP23008::Register_Cache::cache_iocon;
    using ::picolibrary::Microchip::MCP23008::Register_Cache::cache_iodir;
    using ::picolibrary::Microchip::MCP23008::Register_Cache::cache_ipol;
    using ::picolibrary::Microchip::MCP23008::Register_Cache::cache_olat;
    using ::picolibrary::Microchip::MCP23008::Register_Cache::initialize;
};

} // namespace

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Register_Cache::Register_Cache(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::initialize(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::iodir(), and
 *        picolibrary::Microchip::MCP23008::Register_Cache::cache_iodir() work properly.
 */
TEST( iodir, worksProperly )
{
    auto cache = Cache{};

    EXPECT_EQ( cache.iodir(), 0xFF );

    auto const value = random<std::uint8_t>();

    cache.cache_iodir( value );

    EXPECT_EQ( cache.iodir(), value );

    cache.initialize();

    EXPECT_EQ( cache.iodir(), 0xFF );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Register_Cache::Register_Cache(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::initialize(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::ipol(), and
 *        picolibrary::Microchip::MCP23008::Register_Cache::cache_ipol() work properly.
 */
TEST( ipol, worksProperly )
{
    auto cache = Cache{};

    EXPECT_EQ( cache.ipol(), 0x00 );

    auto const value = random<std::uint8_t>();

    cache.cache_ipol( value );

    EXPECT_EQ( cache.ipol(), value );

    cache.initialize();

    EXPECT_EQ( cache.ipol(), 0x00 );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Register_Cache::Register_Cache(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::initialize(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::gpinten(), and
 *        picolibrary::Microchip::MCP23008::Register_Cache::cache_gpinten() work properly.
 */
TEST( gpinten, worksProperly )
{
    auto cache = Cache{};

    EXPECT_EQ( cache.gpinten(), 0x00 );

    auto const value = random<std::uint8_t>();

    cache.cache_gpinten( value );

    EXPECT_EQ( cache.gpinten(), value );

    cache.initialize();

    EXPECT_EQ( cache.gpinten(), 0x00 );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Register_Cache::Register_Cache(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::initialize(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::defval(), and
 *        picolibrary::Microchip::MCP23008::Register_Cache::cache_defval() work properly.
 */
TEST( defval, worksProperly )
{
    auto cache = Cache{};

    EXPECT_EQ( cache.defval(), 0x00 );

    auto const value = random<std::uint8_t>();

    cache.cache_defval( value );

    EXPECT_EQ( cache.defval(), value );

    cache.initialize();

    EXPECT_EQ( cache.defval(), 0x00 );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Register_Cache::Register_Cache(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::initialize(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::intcon(), and
 *        picolibrary::Microchip::MCP23008::Register_Cache::cache_intcon() work properly.
 */
TEST( intcon, worksProperly )
{
    auto cache = Cache{};

    EXPECT_EQ( cache.intcon(), 0x00 );

    auto const value = random<std::uint8_t>();

    cache.cache_intcon( value );

    EXPECT_EQ( cache.intcon(), value );

    cache.initialize();

    EXPECT_EQ( cache.intcon(), 0x00 );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Register_Cache::Register_Cache(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::initialize(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::iocon(), and
 *        picolibrary::Microchip::MCP23008::Register_Cache::cache_iocon() work properly.
 */
TEST( iocon, worksProperly )
{
    auto cache = Cache{};

    EXPECT_EQ( cache.iocon(), 0x00 );

    auto const value = random<std::uint8_t>();

    cache.cache_iocon( value );

    EXPECT_EQ( cache.iocon(), value );

    cache.initialize();

    EXPECT_EQ( cache.iocon(), 0x00 );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Register_Cache::Register_Cache(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::initialize(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::gppu(), and
 *        picolibrary::Microchip::MCP23008::Register_Cache::cache_gppu() work properly.
 */
TEST( gppu, worksProperly )
{
    auto cache = Cache{};

    EXPECT_EQ( cache.gppu(), 0x00 );

    auto const value = random<std::uint8_t>();

    cache.cache_gppu( value );

    EXPECT_EQ( cache.gppu(), value );

    cache.initialize();

    EXPECT_EQ( cache.gppu(), 0x00 );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Register_Cache::Register_Cache(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::initialize(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::gpio(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::olat(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::cache_gpio() work properly.
 */
TEST( gpio, worksProperly )
{
    auto cache = Cache{};

    EXPECT_EQ( cache.gpio(), 0x00 );
    EXPECT_EQ( cache.olat(), 0x00 );

    auto const value = random<std::uint8_t>();

    cache.cache_gpio( value );

    EXPECT_EQ( cache.gpio(), value );
    EXPECT_EQ( cache.olat(), value );

    cache.initialize();

    EXPECT_EQ( cache.gpio(), 0x00 );
    EXPECT_EQ( cache.olat(), 0x00 );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Register_Cache::Register_Cache(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::initialize(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::gpio(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::olat(),
 *        picolibrary::Microchip::MCP23008::Register_Cache::cache_olat() work properly.
 */
TEST( olat, worksProperly )
{
    auto cache = Cache{};

    EXPECT_EQ( cache.gpio(), 0x00 );
    EXPECT_EQ( cache.olat(), 0x00 );

    auto const value = random<std::uint8_t>();

    cache.cache_olat( value );

    EXPECT_EQ( cache.gpio(), value );
    EXPECT_EQ( cache.olat(), value );

    cache.initialize();

    EXPECT_EQ( cache.gpio(), 0x00 );
    EXPECT_EQ( cache.olat(), 0x00 );
}

/**
 * \brief Execute the picolibrary::Microchip::MCP23008::Register_Cache unit tests.
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

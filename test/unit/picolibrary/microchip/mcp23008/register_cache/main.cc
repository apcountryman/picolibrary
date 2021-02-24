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

    using ::picolibrary::Microchip::MCP23008::Register_Cache::cache_iodir;
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

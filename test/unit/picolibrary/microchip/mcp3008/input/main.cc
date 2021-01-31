/**
 * picolibrary
 *
 * Copyright 2021 Andrew Countryman <apcountryman@gmail.com>
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
 * \brief picolibrary::Microchip::MCP3008::Input unit test program.
 */

#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/microchip/mcp3008.h"
#include "picolibrary/testing/unit/microchip/mcp3008.h"
#include "picolibrary/testing/unit/random.h"

namespace {

using ::picolibrary::Microchip::MCP3008::Channel;
using ::picolibrary::Microchip::MCP3008::Channel_Pair;
using ::picolibrary::Microchip::MCP3008::Input;
using ::picolibrary::Testing::Unit::random;

} // namespace

/**
 * \brief Verify picolibrary::Microchip::MCP3008::Input(
 *        picolibrary::Microchip::MCP3008::Channel ) works properly.
 */
TEST( constructorChannel, worksProperly )
{
    auto const channel = random<Channel>();

    EXPECT_EQ( static_cast<std::uint8_t>( Input{ channel } ), static_cast<std::uint8_t>( channel ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP3008::Input(
 *        picolibrary::Microchip::MCP3008::Channel_Pair ) works properly.
 */
TEST( constructorChannelPair, worksProperly )
{
    auto const channel_pair = random<Channel_Pair>();

    EXPECT_EQ( static_cast<std::uint8_t>( Input{ channel_pair } ), static_cast<std::uint8_t>( channel_pair ) );
}

/**
 * \brief Execute the picolibrary::Microchip::MCP3008::Input unit tests.
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

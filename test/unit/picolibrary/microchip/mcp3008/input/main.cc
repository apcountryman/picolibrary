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
#include <utility>

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

auto random_unique_channels()
{
    auto const a = random<std::uint8_t>( 0b000, 0b111 );
    auto const b = random<std::uint8_t>( 0b000, 0b111 );

    auto const channel_a = static_cast<Channel>( ( 0b1'000 | a ) << 4 );
    auto const channel_b = static_cast<Channel>(
        ( 0b1'000 | ( b != a ? b : b ^ random<std::uint8_t>( 0b001, 0b111 ) ) ) << 4 );

    return std::pair<Channel, Channel>{ channel_a, channel_b };
}

auto random_unique_channel_pairs()
{
    auto const a = random<std::uint8_t>( 0b000, 0b111 );
    auto const b = random<std::uint8_t>( 0b000, 0b111 );

    auto const channel_pair_a = static_cast<Channel_Pair>( a << 4 );
    auto const channel_pair_b = static_cast<Channel_Pair>(
        ( b != a ? b : b ^ random<std::uint8_t>( 0b001, 0b111 ) ) << 4 );

    return std::pair<Channel_Pair, Channel_Pair>{ channel_pair_a, channel_pair_b };
}

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
 * \brief Verify picolibrary::Microchip::MCP3008::operator==(
 *        picolibrary::Microchip::MCP3008::Input, picolibrary::Microchip::MCP3008::Input )
 *        works properly.
 */
TEST( equalityOperator, worksProperly )
{
    {
        auto const channel = random<Channel>();

        EXPECT_TRUE( Input{ channel } == Input{ channel } );
    }

    {
        auto const [ lhs_channel, rhs_channel ] = random_unique_channels();

        EXPECT_FALSE( Input{ lhs_channel } == Input{ rhs_channel } );
    }

    {
        auto const channel_pair = random<Channel_Pair>();

        EXPECT_TRUE( Input{ channel_pair } == Input{ channel_pair } );
    }

    {
        auto const [ lhs_channel_pair, rhs_channel_pair ] = random_unique_channel_pairs();

        EXPECT_FALSE( Input{ lhs_channel_pair } == Input{ rhs_channel_pair } );
    }

    {
        EXPECT_FALSE( Input{ random<Channel>() } == Input{ random<Channel_Pair>() } );
    }
}

/**
 * \brief Verify picolibrary::Microchip::MCP3008::operator!=(
 *        picolibrary::Microchip::MCP3008::Input, picolibrary::Microchip::MCP3008::Input )
 *        works properly.
 */
TEST( inequalityOperator, worksProperly )
{
    {
        auto const channel = random<Channel>();

        EXPECT_FALSE( Input{ channel } != Input{ channel } );
    }

    {
        auto const [ lhs_channel, rhs_channel ] = random_unique_channels();

        EXPECT_TRUE( Input{ lhs_channel } != Input{ rhs_channel } );
    }

    {
        auto const channel_pair = random<Channel_Pair>();

        EXPECT_FALSE( Input{ channel_pair } != Input{ channel_pair } );
    }

    {
        auto const [ lhs_channel_pair, rhs_channel_pair ] = random_unique_channel_pairs();

        EXPECT_TRUE( Input{ lhs_channel_pair } != Input{ rhs_channel_pair } );
    }

    {
        EXPECT_TRUE( Input{ random<Channel>() } != Input{ random<Channel_Pair>() } );
    }
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

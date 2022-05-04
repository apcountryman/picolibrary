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
 * \brief picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin automated test program.
 */

#include <cstdint>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/gpio.h"
#include "picolibrary/microchip/mcp23x08.h"
#include "picolibrary/testing/automated/microchip/mcp23x08.h"
#include "picolibrary/testing/automated/random.h"

namespace {

using ::picolibrary::GPIO::Initial_Pin_State;
using ::picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin;
using ::picolibrary::Testing::Automated::random;
using ::picolibrary::Testing::Automated::Microchip::MCP23X08::Mock_Caching_Driver;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

} // namespace

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::Push_Pull_IO_Pin()
 *        works properly.
 */
TEST( constructorDefault, worksProperly )
{
    Push_Pull_IO_Pin<Mock_Caching_Driver>{};
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::Push_Pull_IO_Pin(
 *        Caching_Driver &, std::uint8_t ) works properly.
 */
TEST( constructorCachingDriverMask, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto const pin = Push_Pull_IO_Pin{ mcp23x08, mask };

    auto const iodir = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( iodir ) );
    EXPECT_CALL( mcp23x08, write_iodir( iodir | mask ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::Push_Pull_IO_Pin(
 *        picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin && ) works properly.
 */
TEST( constructorMove, worksProperly )
{
    {
        Push_Pull_IO_Pin<Mock_Caching_Driver>{ Push_Pull_IO_Pin<Mock_Caching_Driver>{} };
    }

    {
        auto const in_sequence = InSequence{};

        auto       mcp23x08 = Mock_Caching_Driver{};
        auto const mask     = random<std::uint8_t>();

        auto source = Push_Pull_IO_Pin{ mcp23x08, mask };

        EXPECT_CALL( mcp23x08, iodir() ).Times( 0 );
        EXPECT_CALL( mcp23x08, write_iodir( _ ) ).Times( 0 );

        auto const pin = Push_Pull_IO_Pin{ std::move( source ) };

        auto const iodir = random<std::uint8_t>();

        EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( iodir ) );
        EXPECT_CALL( mcp23x08, write_iodir( iodir | mask ) );
    }
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::operator=(
 *        picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin && ) works properly.
 */
TEST( assignmentOperatorMove, worksProperly )
{
    {
        auto expression = Push_Pull_IO_Pin<Mock_Caching_Driver>{};
        auto object     = Push_Pull_IO_Pin<Mock_Caching_Driver>{};

        object = std::move( expression );
    }

    {
        auto const in_sequence = InSequence{};

        auto       mcp23x08 = Mock_Caching_Driver{};
        auto const mask     = random<std::uint8_t>();

        auto expression = Push_Pull_IO_Pin{ mcp23x08, mask };
        auto object     = Push_Pull_IO_Pin<Mock_Caching_Driver>{};

        EXPECT_CALL( mcp23x08, iodir() ).Times( 0 );
        EXPECT_CALL( mcp23x08, write_iodir( _ ) ).Times( 0 );

        object = std::move( expression );

        auto const iodir = random<std::uint8_t>();

        EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( iodir ) );
        EXPECT_CALL( mcp23x08, write_iodir( iodir | mask ) );
    }

    {
        auto const in_sequence = InSequence{};

        auto       mcp23x08 = Mock_Caching_Driver{};
        auto const mask     = random<std::uint8_t>();

        auto expression = Push_Pull_IO_Pin<Mock_Caching_Driver>{};
        auto object     = Push_Pull_IO_Pin{ mcp23x08, mask };

        auto const iodir = random<std::uint8_t>();

        EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( iodir ) );
        EXPECT_CALL( mcp23x08, write_iodir( iodir | mask ) );

        object = std::move( expression );

        EXPECT_CALL( mcp23x08, iodir() ).Times( 0 );
        EXPECT_CALL( mcp23x08, write_iodir( _ ) ).Times( 0 );
    }

    {
        auto const in_sequence = InSequence{};

        auto       expression_mcp23x08 = Mock_Caching_Driver{};
        auto const expression_mask     = random<std::uint8_t>();
        auto       object_mcp23x08     = Mock_Caching_Driver{};
        auto const object_mask         = random<std::uint8_t>();

        auto expression = Push_Pull_IO_Pin{ expression_mcp23x08, expression_mask };
        auto object     = Push_Pull_IO_Pin{ object_mcp23x08, object_mask };

        auto const object_iodir = random<std::uint8_t>();

        EXPECT_CALL( expression_mcp23x08, iodir() ).Times( 0 );
        EXPECT_CALL( expression_mcp23x08, write_iodir( _ ) ).Times( 0 );
        EXPECT_CALL( object_mcp23x08, iodir() ).WillOnce( Return( object_iodir ) );
        EXPECT_CALL( object_mcp23x08, write_iodir( object_iodir | object_mask ) );

        object = std::move( expression );

        auto const expression_iodir = random<std::uint8_t>();

        EXPECT_CALL( expression_mcp23x08, iodir() ).WillOnce( Return( expression_iodir ) );
        EXPECT_CALL( expression_mcp23x08, write_iodir( expression_iodir | expression_mask ) );
        EXPECT_CALL( object_mcp23x08, iodir() ).Times( 0 );
        EXPECT_CALL( object_mcp23x08, write_iodir( _ ) ).Times( 0 );
    }

    {
        auto pin = Push_Pull_IO_Pin<Mock_Caching_Driver>{};

        pin = std::move( pin );
    }

    {
        auto const in_sequence = InSequence{};

        auto       mcp23x08 = Mock_Caching_Driver{};
        auto const mask     = random<std::uint8_t>();

        auto pin = Push_Pull_IO_Pin{ mcp23x08, mask };

        EXPECT_CALL( mcp23x08, iodir() ).Times( 0 );
        EXPECT_CALL( mcp23x08, write_iodir( _ ) ).Times( 0 );

        pin = std::move( pin );

        auto const iodir = random<std::uint8_t>();

        EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( iodir ) );
        EXPECT_CALL( mcp23x08, write_iodir( iodir | mask ) );
    }
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::initialize() works
 *        properly.
 */
TEST( initialize, worksProperly )
{
    {
        auto const in_sequence = InSequence{};

        auto       mcp23x08 = Mock_Caching_Driver{};
        auto const mask     = random<std::uint8_t>();

        auto pin = Push_Pull_IO_Pin{ mcp23x08, mask };

        auto const olat  = random<std::uint8_t>();
        auto const iodir = random<std::uint8_t>();

        EXPECT_CALL( mcp23x08, olat() ).WillOnce( Return( olat ) );
        EXPECT_CALL( mcp23x08, write_olat( olat & ~mask ) );
        EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( iodir ) );
        EXPECT_CALL( mcp23x08, write_iodir( iodir & ~mask ) );

        pin.initialize();

        EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
        EXPECT_CALL( mcp23x08, write_iodir( _ ) );
    }

    {
        auto const in_sequence = InSequence{};

        auto       mcp23x08 = Mock_Caching_Driver{};
        auto const mask     = random<std::uint8_t>();

        auto pin = Push_Pull_IO_Pin{ mcp23x08, mask };

        auto const olat  = random<std::uint8_t>();
        auto const iodir = random<std::uint8_t>();

        EXPECT_CALL( mcp23x08, olat() ).WillOnce( Return( olat ) );
        EXPECT_CALL( mcp23x08, write_olat( olat & ~mask ) );
        EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( iodir ) );
        EXPECT_CALL( mcp23x08, write_iodir( iodir & ~mask ) );

        pin.initialize( Initial_Pin_State::LOW );

        EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
        EXPECT_CALL( mcp23x08, write_iodir( _ ) );
    }

    {
        auto const in_sequence = InSequence{};

        auto       mcp23x08 = Mock_Caching_Driver{};
        auto const mask     = random<std::uint8_t>();

        auto pin = Push_Pull_IO_Pin{ mcp23x08, mask };

        auto const olat  = random<std::uint8_t>();
        auto const iodir = random<std::uint8_t>();

        EXPECT_CALL( mcp23x08, olat() ).WillOnce( Return( olat ) );
        EXPECT_CALL( mcp23x08, write_olat( olat | mask ) );
        EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( iodir ) );
        EXPECT_CALL( mcp23x08, write_iodir( iodir & ~mask ) );

        pin.initialize( Initial_Pin_State::HIGH );

        EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
        EXPECT_CALL( mcp23x08, write_iodir( _ ) );
    }
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::is_low() works
 *        properly.
 */
TEST( isLow, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto const pin = Push_Pull_IO_Pin{ mcp23x08, mask };

    auto const gpio = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, read_gpio() ).WillOnce( Return( gpio ) );

    EXPECT_EQ( pin.is_low(), not( gpio & mask ) );

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( mcp23x08, write_iodir( _ ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::is_high() works
 *        properly.
 */
TEST( isHigh, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto const pin = Push_Pull_IO_Pin{ mcp23x08, mask };

    auto const gpio = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, read_gpio() ).WillOnce( Return( gpio ) );

    EXPECT_EQ( pin.is_high(), static_cast<bool>( gpio & mask ) );

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( mcp23x08, write_iodir( _ ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::transition_to_low()
 *        works properly.
 */
TEST( transitionToLow, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto pin = Push_Pull_IO_Pin{ mcp23x08, mask };

    auto const olat = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, olat() ).WillOnce( Return( olat ) );
    EXPECT_CALL( mcp23x08, write_olat( olat & ~mask ) );

    pin.transition_to_low();

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( mcp23x08, write_iodir( _ ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::transition_to_high()
 *        works properly.
 */
TEST( transitionToHigh, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto pin = Push_Pull_IO_Pin{ mcp23x08, mask };

    auto const olat = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, olat() ).WillOnce( Return( olat ) );
    EXPECT_CALL( mcp23x08, write_olat( olat | mask ) );

    pin.transition_to_high();

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( mcp23x08, write_iodir( _ ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::toggle() works
 *        properly.
 */
TEST( toggle, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto pin = Push_Pull_IO_Pin{ mcp23x08, mask };

    auto const olat = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, olat() ).WillOnce( Return( olat ) );
    EXPECT_CALL( mcp23x08, write_olat( olat ^ mask ) );

    pin.toggle();

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( mcp23x08, write_iodir( _ ) );
}

/**
 * \brief Execute the picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin automated tests.
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

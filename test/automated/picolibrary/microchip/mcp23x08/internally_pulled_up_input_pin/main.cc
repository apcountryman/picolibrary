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
 * \brief picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin automated test
 *        program.
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

using ::picolibrary::GPIO::Initial_Pull_Up_State;
using ::picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin;
using ::picolibrary::Testing::Automated::random;
using ::picolibrary::Testing::Automated::Microchip::MCP23X08::Mock_Caching_Driver;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

} // namespace

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::Internally_Pulled_Up_Input_Pin()
 *        works properly.
 */
TEST( constructorDefault, worksProperly )
{
    Internally_Pulled_Up_Input_Pin<Mock_Caching_Driver>{};
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::Internally_Pulled_Up_Input_Pin(
 *        Caching_Driver &, std::uint8_t ) works properly.
 */
TEST( constructorCachingDriverMask, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto const pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, mask };

    auto const gppu = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( gppu ) );
    EXPECT_CALL( mcp23x08, write_gppu( gppu & ~mask ) );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::Internally_Pulled_Up_Input_Pin(
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin && ) works
 *        properly.
 */
TEST( constructorMove, worksProperly )
{
    {
        Internally_Pulled_Up_Input_Pin<Mock_Caching_Driver>{
            Internally_Pulled_Up_Input_Pin<Mock_Caching_Driver>{}
        };
    }

    {
        auto const in_sequence = InSequence{};

        auto       mcp23x08 = Mock_Caching_Driver{};
        auto const mask     = random<std::uint8_t>();

        auto source = Internally_Pulled_Up_Input_Pin{ mcp23x08, mask };

        EXPECT_CALL( mcp23x08, gppu() ).Times( 0 );
        EXPECT_CALL( mcp23x08, write_gppu( _ ) ).Times( 0 );

        auto const pin = Internally_Pulled_Up_Input_Pin{ std::move( source ) };

        auto const gppu = random<std::uint8_t>();

        EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( gppu ) );
        EXPECT_CALL( mcp23x08, write_gppu( gppu & ~mask ) );
    }
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::operator=(
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin && ) works
 *        properly.
 */
TEST( assignmentOperatorMove, worksProperly )
{
    {
        auto expression = Internally_Pulled_Up_Input_Pin<Mock_Caching_Driver>{};
        auto object     = Internally_Pulled_Up_Input_Pin<Mock_Caching_Driver>{};

        object = std::move( expression );
    }

    {
        auto const in_sequence = InSequence{};

        auto       mcp23x08 = Mock_Caching_Driver{};
        auto const mask     = random<std::uint8_t>();

        auto expression = Internally_Pulled_Up_Input_Pin{ mcp23x08, mask };
        auto object     = Internally_Pulled_Up_Input_Pin<Mock_Caching_Driver>{};

        EXPECT_CALL( mcp23x08, gppu() ).Times( 0 );
        EXPECT_CALL( mcp23x08, write_gppu( _ ) ).Times( 0 );

        object = std::move( expression );

        auto const gppu = random<std::uint8_t>();

        EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( gppu ) );
        EXPECT_CALL( mcp23x08, write_gppu( gppu & ~mask ) );
    }

    {
        auto const in_sequence = InSequence{};

        auto       mcp23x08 = Mock_Caching_Driver{};
        auto const mask     = random<std::uint8_t>();

        auto expression = Internally_Pulled_Up_Input_Pin<Mock_Caching_Driver>{};
        auto object     = Internally_Pulled_Up_Input_Pin{ mcp23x08, mask };

        auto const gppu = random<std::uint8_t>();

        EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( gppu ) );
        EXPECT_CALL( mcp23x08, write_gppu( gppu & ~mask ) );

        object = std::move( expression );

        EXPECT_CALL( mcp23x08, gppu() ).Times( 0 );
        EXPECT_CALL( mcp23x08, write_gppu( _ ) ).Times( 0 );
    }

    {
        auto const in_sequence = InSequence{};

        auto       expression_mcp23x08 = Mock_Caching_Driver{};
        auto const expression_mask     = random<std::uint8_t>();
        auto       object_mcp23x08     = Mock_Caching_Driver{};
        auto const object_mask         = random<std::uint8_t>();

        auto expression = Internally_Pulled_Up_Input_Pin{ expression_mcp23x08, expression_mask };
        auto object = Internally_Pulled_Up_Input_Pin{ object_mcp23x08, object_mask };

        auto const object_gppu = random<std::uint8_t>();

        EXPECT_CALL( expression_mcp23x08, gppu() ).Times( 0 );
        EXPECT_CALL( expression_mcp23x08, write_gppu( _ ) ).Times( 0 );
        EXPECT_CALL( object_mcp23x08, gppu() ).WillOnce( Return( object_gppu ) );
        EXPECT_CALL( object_mcp23x08, write_gppu( object_gppu & ~object_mask ) );

        object = std::move( expression );

        auto const expression_gppu = random<std::uint8_t>();

        EXPECT_CALL( expression_mcp23x08, gppu() ).WillOnce( Return( expression_gppu ) );
        EXPECT_CALL( expression_mcp23x08, write_gppu( expression_gppu & ~expression_mask ) );
        EXPECT_CALL( object_mcp23x08, gppu() ).Times( 0 );
        EXPECT_CALL( object_mcp23x08, write_gppu( _ ) ).Times( 0 );
    }

    {
        auto pin = Internally_Pulled_Up_Input_Pin<Mock_Caching_Driver>{};

        pin = std::move( pin );
    }

    {
        auto const in_sequence = InSequence{};

        auto       mcp23x08 = Mock_Caching_Driver{};
        auto const mask     = random<std::uint8_t>();

        auto pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, mask };

        EXPECT_CALL( mcp23x08, gppu() ).Times( 0 );
        EXPECT_CALL( mcp23x08, write_gppu( _ ) ).Times( 0 );

        pin = std::move( pin );

        auto const gppu = random<std::uint8_t>();

        EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( gppu ) );
        EXPECT_CALL( mcp23x08, write_gppu( gppu & ~mask ) );
    }
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::initialize()
 *        works properly.
 */
TEST( initialize, worksProperly )
{
    {
        auto const in_sequence = InSequence{};

        auto       mcp23x08 = Mock_Caching_Driver{};
        auto const mask     = random<std::uint8_t>();

        auto pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, mask };

        auto const iodir = random<std::uint8_t>();
        auto const gppu  = random<std::uint8_t>();

        EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( iodir ) );
        EXPECT_CALL( mcp23x08, write_iodir( iodir | mask ) );
        EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( gppu ) );
        EXPECT_CALL( mcp23x08, write_gppu( gppu & ~mask ) );

        pin.initialize();

        EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( random<std::uint8_t>() ) );
        EXPECT_CALL( mcp23x08, write_gppu( _ ) );
    }

    {
        auto const in_sequence = InSequence{};

        auto       mcp23x08 = Mock_Caching_Driver{};
        auto const mask     = random<std::uint8_t>();

        auto pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, mask };

        auto const iodir = random<std::uint8_t>();
        auto const gppu  = random<std::uint8_t>();

        EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( iodir ) );
        EXPECT_CALL( mcp23x08, write_iodir( iodir | mask ) );
        EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( gppu ) );
        EXPECT_CALL( mcp23x08, write_gppu( gppu & ~mask ) );

        pin.initialize( Initial_Pull_Up_State::DISABLED );

        EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( random<std::uint8_t>() ) );
        EXPECT_CALL( mcp23x08, write_gppu( _ ) );
    }

    {
        auto const in_sequence = InSequence{};

        auto       mcp23x08 = Mock_Caching_Driver{};
        auto const mask     = random<std::uint8_t>();

        auto pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, mask };

        auto const iodir = random<std::uint8_t>();
        auto const gppu  = random<std::uint8_t>();

        EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( iodir ) );
        EXPECT_CALL( mcp23x08, write_iodir( iodir | mask ) );
        EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( gppu ) );
        EXPECT_CALL( mcp23x08, write_gppu( gppu | mask ) );

        pin.initialize( Initial_Pull_Up_State::ENABLED );

        EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( random<std::uint8_t>() ) );
        EXPECT_CALL( mcp23x08, write_gppu( _ ) );
    }
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::pull_up_is_disabled()
 *        works properly.
 */
TEST( pullUpIsDisabled, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto const pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, mask };

    auto const gppu = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( gppu ) );

    EXPECT_EQ( pin.pull_up_is_disabled(), not( gppu & mask ) );

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( mcp23x08, write_gppu( _ ) );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::pull_up_is_enabled()
 *        works properly.
 */
TEST( pullUpIsEnabled, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto const pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, mask };

    auto const gppu = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( gppu ) );

    EXPECT_EQ( pin.pull_up_is_enabled(), static_cast<bool>( gppu & mask ) );

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( mcp23x08, write_gppu( _ ) );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::disable_pull_up()
 *        works properly.
 */
TEST( disablePullUp, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, mask };

    auto const gppu = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( gppu ) );
    EXPECT_CALL( mcp23x08, write_gppu( gppu & ~mask ) );

    pin.disable_pull_up();

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( mcp23x08, write_gppu( _ ) );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::enable_pull_up()
 *        works properly.
 */
TEST( enablePullUp, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, mask };

    auto const gppu = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( gppu ) );
    EXPECT_CALL( mcp23x08, write_gppu( gppu | mask ) );

    pin.enable_pull_up();

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( mcp23x08, write_gppu( _ ) );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::is_low() works
 *        properly.
 */
TEST( isLow, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto const pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, mask };

    auto const gpio = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, read_gpio() ).WillOnce( Return( gpio ) );

    EXPECT_EQ( pin.is_low(), not( gpio & mask ) );

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( mcp23x08, write_gppu( _ ) );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::is_high()
 *        works properly.
 */
TEST( isHigh, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto const pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, mask };

    auto const gpio = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, read_gpio() ).WillOnce( Return( gpio ) );

    EXPECT_EQ( pin.is_high(), static_cast<bool>( gpio & mask ) );

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( mcp23x08, write_gppu( _ ) );
}

/**
 * \brief Execute the picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin
 *        automated tests.
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

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
 * \brief picolibrary::Microchip::MCP23008::Open_Drain_IO_Pin unit test program.
 */

#include <cstdint>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/microchip/mcp23008.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/microchip/mcp23008.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/void.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::Microchip::MCP23008::Mock_Driver;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

using Open_Drain_IO_Pin = ::picolibrary::Microchip::MCP23008::Open_Drain_IO_Pin<Mock_Driver>;

} // namespace

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Open_Drain_IO_Pin::Open_Drain_IO_Pin()
 *        works properly.
 */
TEST( constructorDefault, worksProperly )
{
    Open_Drain_IO_Pin{};
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Open_Drain_IO_Pin::Open_Drain_IO_Pin(
 *        picolibrary::Microchip::MCP23008::Open_Drain_IO_Pin && ) works properly.
 */
TEST( constructorMove, worksProperly )
{
    {
        Open_Drain_IO_Pin{ Open_Drain_IO_Pin{} };
    }

    {
        auto const in_sequence = InSequence{};

        auto       driver = Mock_Driver{};
        auto const mask   = random<std::uint8_t>();

        auto source = Open_Drain_IO_Pin{ driver, mask };

        EXPECT_CALL( driver, iodir() ).Times( 0 );
        EXPECT_CALL( driver, write_iodir( _ ) ).Times( 0 );

        auto const pin = Open_Drain_IO_Pin{ std::move( source ) };

        auto const iodir = random<std::uint8_t>();

        EXPECT_CALL( driver, iodir() ).WillOnce( Return( iodir ) );
        EXPECT_CALL( driver, write_iodir( iodir | mask ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    }
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Open_Drain_IO_Pin::~Open_Drain_IO_Pin()
 *        properly handles an IODIR register write error.
 */
TEST( destructor, writeIODIRError )
{
    auto driver = Mock_Driver{};

    auto const pin = Open_Drain_IO_Pin{ driver, random<std::uint8_t>() };

    EXPECT_CALL( driver, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_iodir( _ ) ).WillOnce( Return( random<Mock_Error>() ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Open_Drain_IO_Pin::operator=(
 *        picolibrary::Microchip::MCP23008::Open_Drain_IO_Pin && ) properly handles an
 *        IODIR register write error.
 */
TEST( assignmentOperatorMove, writeIODIRError )
{
    {
        auto driver = Mock_Driver{};

        auto expression = Open_Drain_IO_Pin{};
        auto object     = Open_Drain_IO_Pin{ driver, random<std::uint8_t>() };

        EXPECT_CALL( driver, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
        EXPECT_CALL( driver, write_iodir( _ ) ).WillOnce( Return( random<Mock_Error>() ) );

        object = std::move( expression );

        EXPECT_CALL( driver, iodir() ).Times( 0 );
        EXPECT_CALL( driver, write_iodir( _ ) ).Times( 0 );
    }

    {
        auto driver_expression = Mock_Driver{};
        auto driver_object     = Mock_Driver{};

        auto expression = Open_Drain_IO_Pin{ driver_expression, random<std::uint8_t>() };
        auto object     = Open_Drain_IO_Pin{ driver_object, random<std::uint8_t>() };

        EXPECT_CALL( driver_expression, iodir() ).Times( 0 );
        EXPECT_CALL( driver_object, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
        EXPECT_CALL( driver_expression, write_iodir( _ ) ).Times( 0 );
        EXPECT_CALL( driver_object, write_iodir( _ ) ).WillOnce( Return( random<Mock_Error>() ) );

        object = std::move( expression );

        EXPECT_CALL( driver_object, iodir() ).Times( 0 );
        EXPECT_CALL( driver_expression, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
        EXPECT_CALL( driver_object, write_iodir( _ ) ).Times( 0 );
        EXPECT_CALL( driver_expression, write_iodir( _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    }
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Open_Drain_IO_Pin::operator=(
 *        picolibrary::Microchip::MCP23008::Open_Drain_IO_Pin && ) works properly.
 */
TEST( assignmentOperatorMove, worksProperly )
{
    {
        auto expression = Open_Drain_IO_Pin{};
        auto object     = Open_Drain_IO_Pin{};

        object = std::move( expression );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver = Mock_Driver{};
        auto mask   = random<std::uint8_t>();

        auto expression = Open_Drain_IO_Pin{ driver, mask };
        auto object     = Open_Drain_IO_Pin{};

        EXPECT_CALL( driver, iodir() ).Times( 0 );
        EXPECT_CALL( driver, write_iodir( _ ) ).Times( 0 );

        object = std::move( expression );

        auto const iodir = random<std::uint8_t>();

        EXPECT_CALL( driver, iodir() ).WillOnce( Return( iodir ) );
        EXPECT_CALL( driver, write_iodir( iodir | mask ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver = Mock_Driver{};
        auto mask   = random<std::uint8_t>();

        auto expression = Open_Drain_IO_Pin{};
        auto object     = Open_Drain_IO_Pin{ driver, mask };

        auto const iodir = random<std::uint8_t>();

        EXPECT_CALL( driver, iodir() ).WillOnce( Return( iodir ) );
        EXPECT_CALL( driver, write_iodir( iodir | mask ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

        object = std::move( expression );

        EXPECT_CALL( driver, iodir() ).Times( 0 );
        EXPECT_CALL( driver, write_iodir( _ ) ).Times( 0 );
    }

    {
        auto const in_sequence = InSequence{};

        auto       driver_expression = Mock_Driver{};
        auto const mask_expression   = random<std::uint8_t>();
        auto       driver_object     = Mock_Driver{};
        auto const mask_object       = random<std::uint8_t>();

        auto expression = Open_Drain_IO_Pin{ driver_expression, mask_expression };
        auto object     = Open_Drain_IO_Pin{ driver_object, mask_object };

        auto const iodir_object = random<std::uint8_t>();

        EXPECT_CALL( driver_expression, iodir() ).Times( 0 );
        EXPECT_CALL( driver_object, iodir() ).WillOnce( Return( iodir_object ) );
        EXPECT_CALL( driver_expression, write_iodir( _ ) ).Times( 0 );
        EXPECT_CALL( driver_object, write_iodir( iodir_object | mask_object ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );

        object = std::move( expression );

        auto const iodir_expression = random<std::uint8_t>();

        EXPECT_CALL( driver_object, iodir() ).Times( 0 );
        EXPECT_CALL( driver_expression, iodir() ).WillOnce( Return( iodir_expression ) );
        EXPECT_CALL( driver_object, write_iodir( _ ) ).Times( 0 );
        EXPECT_CALL( driver_expression, write_iodir( iodir_expression | mask_expression ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
    }

    {
        auto pin = Open_Drain_IO_Pin{};

        pin = std::move( pin );
    }

    {
        auto const in_sequence = InSequence{};

        auto       driver = Mock_Driver{};
        auto const mask   = random<std::uint8_t>();

        auto pin = Open_Drain_IO_Pin{ driver, mask };

        EXPECT_CALL( driver, iodir() ).Times( 0 );
        EXPECT_CALL( driver, write_iodir( _ ) ).Times( 0 );

        pin = std::move( pin );

        auto const iodir = random<std::uint8_t>();

        EXPECT_CALL( driver, iodir() ).WillOnce( Return( iodir ) );
        EXPECT_CALL( driver, write_iodir( iodir | mask ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    }
}

/**
 * \brief Verify picolibrary::Microchip::Open_Drain_IO_Pin::state() properly handles a
 *        GPIO register read error.
 */
TEST( state, readGPIOError )
{
    auto driver = Mock_Driver{};

    auto const pin = Open_Drain_IO_Pin{ driver, random<std::uint8_t>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_gpio() ).WillOnce( Return( error ) );

    auto const result = pin.state();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( driver, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_iodir( _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Verify picolibrary::Microchip::Open_Drain_IO_Pin::state() works properly.
 */
TEST( state, worksProperly )
{
    auto       driver = Mock_Driver{};
    auto const mask   = random<std::uint8_t>();

    auto const pin = Open_Drain_IO_Pin{ driver, mask };

    auto const gpio = random<std::uint8_t>();

    EXPECT_CALL( driver, read_gpio() ).WillOnce( Return( gpio ) );

    auto const result = pin.state();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value().is_high(), static_cast<bool>( gpio & mask ) );

    EXPECT_CALL( driver, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_iodir( _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Open_Drain_IO_Pin::transition_to_high()
 *        properly handles an IODIR register write error.
 */
TEST( transitionToHigh, writeIODIRError )
{
    auto driver = Mock_Driver{};

    auto pin = Open_Drain_IO_Pin{ driver, random<std::uint8_t>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_iodir( _ ) ).WillOnce( Return( error ) );

    auto const result = pin.transition_to_high();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( driver, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_iodir( _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Open_Drain_IO_Pin::transition_to_high()
 *        works properly.
 */
TEST( transitionToHigh, worksProperly )
{
    auto       driver = Mock_Driver{};
    auto const mask   = random<std::uint8_t>();

    auto pin = Open_Drain_IO_Pin{ driver, mask };

    auto const iodir = random<std::uint8_t>();

    EXPECT_CALL( driver, iodir() ).WillOnce( Return( iodir ) );
    EXPECT_CALL( driver, write_iodir( iodir | mask ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( pin.transition_to_high().is_error() );

    EXPECT_CALL( driver, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_iodir( _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Open_Drain_IO_Pin::transition_to_low()
 *        properly handles an IODIR register write error.
 */
TEST( transitionToLow, writeIODIRError )
{
    auto driver = Mock_Driver{};

    auto pin = Open_Drain_IO_Pin{ driver, random<std::uint8_t>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_iodir( _ ) ).WillOnce( Return( error ) );

    auto const result = pin.transition_to_low();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( driver, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_iodir( _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Open_Drain_IO_Pin::transition_to_low()
 *        works properly.
 */
TEST( transitionToLow, worksProperly )
{
    auto       driver = Mock_Driver{};
    auto const mask   = random<std::uint8_t>();

    auto pin = Open_Drain_IO_Pin{ driver, mask };

    auto const iodir = random<std::uint8_t>();

    EXPECT_CALL( driver, iodir() ).WillOnce( Return( iodir ) );
    EXPECT_CALL( driver, write_iodir( iodir & ~mask ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( pin.transition_to_low().is_error() );

    EXPECT_CALL( driver, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_iodir( _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Open_Drain_IO_Pin::toggle() properly
 *        handles an IODIR register write error.
 */
TEST( toggle, writeIODIRError )
{
    auto driver = Mock_Driver{};

    auto pin = Open_Drain_IO_Pin{ driver, random<std::uint8_t>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_iodir( _ ) ).WillOnce( Return( error ) );

    auto const result = pin.toggle();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( driver, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_iodir( _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Open_Drain_IO_Pin::toggle() works
 *        properly.
 */
TEST( toggle, worksProperly )
{
    auto       driver = Mock_Driver{};
    auto const mask   = random<std::uint8_t>();

    auto pin = Open_Drain_IO_Pin{ driver, mask };

    auto const iodir = random<std::uint8_t>();

    EXPECT_CALL( driver, iodir() ).WillOnce( Return( iodir ) );
    EXPECT_CALL( driver, write_iodir( iodir ^ mask ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( pin.toggle().is_error() );

    EXPECT_CALL( driver, iodir() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_iodir( _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Execute the picolibrary::Microchip::MCP23008::Open_Drain_IO_Pin unit tests.
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

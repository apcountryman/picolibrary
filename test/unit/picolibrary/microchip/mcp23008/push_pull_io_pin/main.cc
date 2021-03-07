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
 * \brief picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin unit test program.
 */

#include <cstdint>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/gpio.h"
#include "picolibrary/microchip/mcp23008.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/gpio.h"
#include "picolibrary/testing/unit/microchip/mcp23008.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/void.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::GPIO::Initial_Pin_State;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::Microchip::MCP23008::Mock_Driver;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

using Push_Pull_IO_Pin = ::picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin<Mock_Driver>;

} // namespace

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin::Push_Pull_IO_Pin()
 *        works properly.
 */
TEST( constructorDefault, worksProperly )
{
    Push_Pull_IO_Pin{};
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin::Push_Pull_IO_Pin(
 *        picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin && ) works properly.
 */
TEST( constructorMove, worksProperly )
{
    {
        Push_Pull_IO_Pin{ Push_Pull_IO_Pin{} };
    }

    {
        auto       driver = Mock_Driver{};
        auto const mask   = random<std::uint8_t>();

        auto source = Push_Pull_IO_Pin{ driver, mask };

        EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( _ ) ).Times( 0 );

        auto const pin = Push_Pull_IO_Pin{ std::move( source ) };

        EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( mask ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
    }
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin::~Push_Pull_IO_Pin()
 *        properly handles a configuration error.
 */
TEST( destructor, configurationError )
{
    auto driver = Mock_Driver{};

    auto const pin = Push_Pull_IO_Pin{ driver, random<std::uint8_t>() };

    EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( _ ) )
        .WillOnce( Return( random<Mock_Error>() ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin::operator=(
 *        picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin && ) properly handles a
 *        configuration error.
 */
TEST( assignmentOperatorMove, configurationError )
{
    {
        auto driver = Mock_Driver{};

        auto expression = Push_Pull_IO_Pin{};
        auto object     = Push_Pull_IO_Pin{ driver, random<std::uint8_t>() };

        EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( _ ) )
            .WillOnce( Return( random<Mock_Error>() ) );

        object = std::move( expression );
    }

    {
        auto driver_expression = Mock_Driver{};
        auto driver_object     = Mock_Driver{};

        auto expression = Push_Pull_IO_Pin{ driver_expression, random<std::uint8_t>() };
        auto object     = Push_Pull_IO_Pin{ driver_object, random<std::uint8_t>() };

        EXPECT_CALL( driver_object, configure_pin_as_internally_pulled_up_input( _ ) )
            .WillOnce( Return( random<Mock_Error>() ) );

        object = std::move( expression );

        EXPECT_CALL( driver_expression, configure_pin_as_internally_pulled_up_input( _ ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
    }
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin::operator=(
 *        picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin && ) works properly.
 */
TEST( assignmentOperatorMove, worksProperly )
{
    {
        auto expression = Push_Pull_IO_Pin{};
        auto object     = Push_Pull_IO_Pin{};

        object = std::move( expression );
    }

    {
        auto driver = Mock_Driver{};
        auto mask   = random<std::uint8_t>();

        auto expression = Push_Pull_IO_Pin{ driver, mask };
        auto object     = Push_Pull_IO_Pin{};

        EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( _ ) ).Times( 0 );

        object = std::move( expression );

        EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( mask ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
    }

    {
        auto driver = Mock_Driver{};
        auto mask   = random<std::uint8_t>();

        auto expression = Push_Pull_IO_Pin{};
        auto object     = Push_Pull_IO_Pin{ driver, mask };

        EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( mask ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );

        object = std::move( expression );

        EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( _ ) ).Times( 0 );
    }

    {
        auto       driver_expression = Mock_Driver{};
        auto const mask_expression   = random<std::uint8_t>();
        auto       driver_object     = Mock_Driver{};
        auto const mask_object       = random<std::uint8_t>();

        auto expression = Push_Pull_IO_Pin{ driver_expression, mask_expression };
        auto object     = Push_Pull_IO_Pin{ driver_object, mask_object };

        EXPECT_CALL( driver_expression, configure_pin_as_internally_pulled_up_input( _ ) ).Times( 0 );
        EXPECT_CALL( driver_object, configure_pin_as_internally_pulled_up_input( mask_object ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );

        object = std::move( expression );

        EXPECT_CALL( driver_object, configure_pin_as_internally_pulled_up_input( _ ) ).Times( 0 );
        EXPECT_CALL( driver_expression, configure_pin_as_internally_pulled_up_input( mask_expression ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
    }

    {
        auto pin = Push_Pull_IO_Pin{};

        pin = std::move( pin );
    }

    {
        auto       driver = Mock_Driver{};
        auto const mask   = random<std::uint8_t>();

        auto pin = Push_Pull_IO_Pin{ driver, mask };

        EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( _ ) ).Times( 0 );

        pin = std::move( pin );

        EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( mask ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
    }
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin::initialize() properly
 *        handles a low state transition error.
 */
TEST( initialize, transitionToLowError )
{
    auto driver = Mock_Driver{};

    auto pin = Push_Pull_IO_Pin{ driver, random<std::uint8_t>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, transition_push_pull_output_to_low( _ ) ).WillOnce( Return( error ) );

    auto const result = pin.initialize( Initial_Pin_State::LOW );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( _ ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin::initialize() properly
 *        handles a high state transition error.
 */
TEST( initialize, transitionToHighError )
{
    auto driver = Mock_Driver{};

    auto pin = Push_Pull_IO_Pin{ driver, random<std::uint8_t>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, transition_push_pull_output_to_high( _ ) ).WillOnce( Return( error ) );

    auto const result = pin.initialize( Initial_Pin_State::HIGH );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( _ ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin::initialize() properly
 *        handles a configuration error.
 */
TEST( initialize, configurationError )
{
    auto driver = Mock_Driver{};

    auto pin = Push_Pull_IO_Pin{ driver, random<std::uint8_t>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, transition_push_pull_output_to_low( _ ) )
        .WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, transition_push_pull_output_to_high( _ ) )
        .WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, configure_pin_as_push_pull_output( _ ) ).WillOnce( Return( error ) );

    auto const result = pin.initialize( random<Initial_Pin_State>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( _ ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin::initialize() works
 *        properly.
 */
TEST( initialize, worksProperly )
{
    {
        auto const in_sequence = InSequence{};

        auto       driver = Mock_Driver{};
        auto const mask   = random<std::uint8_t>();

        auto pin = Push_Pull_IO_Pin{ driver, mask };

        EXPECT_CALL( driver, transition_push_pull_output_to_low( mask ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, configure_pin_as_push_pull_output( mask ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );

        EXPECT_FALSE( pin.initialize().is_error() );

        EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( _ ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
    }

    {
        auto const in_sequence = InSequence{};

        auto       driver = Mock_Driver{};
        auto const mask   = random<std::uint8_t>();

        auto pin = Push_Pull_IO_Pin{ driver, mask };

        EXPECT_CALL( driver, transition_push_pull_output_to_low( mask ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, configure_pin_as_push_pull_output( mask ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );

        EXPECT_FALSE( pin.initialize( Initial_Pin_State::LOW ).is_error() );

        EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( _ ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
    }

    {
        auto const in_sequence = InSequence{};

        auto       driver = Mock_Driver{};
        auto const mask   = random<std::uint8_t>();

        auto pin = Push_Pull_IO_Pin{ driver, mask };

        EXPECT_CALL( driver, transition_push_pull_output_to_high( mask ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, configure_pin_as_push_pull_output( mask ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );

        EXPECT_FALSE( pin.initialize( Initial_Pin_State::HIGH ).is_error() );

        EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( _ ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
    }
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin::state() properly
 *        handles state get error.
 */
TEST( state, getStateError )
{
    auto driver = Mock_Driver{};

    auto const pin = Push_Pull_IO_Pin{ driver, random<std::uint8_t>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, state( _ ) ).WillOnce( Return( error ) );

    auto const result = pin.state();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( _ ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin::state() works
 *        properly.
 */
TEST( state, worksProperly )
{
    auto       driver = Mock_Driver{};
    auto const mask   = random<std::uint8_t>();

    auto const pin = Push_Pull_IO_Pin{ driver, mask };

    auto const state = random<std::uint8_t>();

    EXPECT_CALL( driver, state( mask ) ).WillOnce( Return( state ) );

    auto const result = pin.state();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value().is_high(), static_cast<bool>( state ) );

    EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( _ ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin::transition_to_high()
 *        properly handles a state transition error.
 */
TEST( transitionToHigh, transitionError )
{
    auto driver = Mock_Driver{};

    auto pin = Push_Pull_IO_Pin{ driver, random<std::uint8_t>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, transition_push_pull_output_to_high( _ ) ).WillOnce( Return( error ) );

    auto const result = pin.transition_to_high();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( _ ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin::transition_to_high()
 *        works properly.
 */
TEST( transitionToHigh, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       driver = Mock_Driver{};
    auto const mask   = random<std::uint8_t>();

    auto pin = Push_Pull_IO_Pin{ driver, mask };

    EXPECT_CALL( driver, transition_push_pull_output_to_high( mask ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( pin.transition_to_high().is_error() );

    EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( _ ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin::transition_to_low()
 *        properly handles a state transition error.
 */
TEST( transitionToLow, transitionError )
{
    auto driver = Mock_Driver{};

    auto pin = Push_Pull_IO_Pin{ driver, random<std::uint8_t>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, transition_push_pull_output_to_low( _ ) ).WillOnce( Return( error ) );

    auto const result = pin.transition_to_low();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( _ ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin::transition_to_low()
 *        works properly.
 */
TEST( transitionToLow, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       driver = Mock_Driver{};
    auto const mask   = random<std::uint8_t>();

    auto pin = Push_Pull_IO_Pin{ driver, mask };

    EXPECT_CALL( driver, transition_push_pull_output_to_low( mask ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( pin.transition_to_low().is_error() );

    EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( _ ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin::toggle() properly
 *        handles a state transition error.
 */
TEST( toggle, transitionError )
{
    auto driver = Mock_Driver{};

    auto pin = Push_Pull_IO_Pin{ driver, random<std::uint8_t>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, toggle_push_pull_output( _ ) ).WillOnce( Return( error ) );

    auto const result = pin.toggle();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( _ ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin::toggle() works
 *        properly.
 */
TEST( toggle, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       driver = Mock_Driver{};
    auto const mask   = random<std::uint8_t>();

    auto pin = Push_Pull_IO_Pin{ driver, mask };

    EXPECT_CALL( driver, toggle_push_pull_output( mask ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( pin.toggle().is_error() );

    EXPECT_CALL( driver, configure_pin_as_internally_pulled_up_input( _ ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Execute the picolibrary::Microchip::MCP23008::Push_Pull_IO_Pin unit tests.
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

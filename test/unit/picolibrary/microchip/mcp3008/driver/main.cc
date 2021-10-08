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
 * \brief picolibrary::Microchip::MCP3008::Driver unit test program.
 */

#include <cstdint>
#include <limits>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/microchip/mcp3008.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/microchip/mcp3008.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/spi.h"
#include "picolibrary/void.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::Microchip::MCP3008::Channel;
using ::picolibrary::Microchip::MCP3008::Channel_Pair;
using ::picolibrary::Microchip::MCP3008::Input;
using ::picolibrary::Microchip::MCP3008::Sample;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::SPI::Mock_Controller;
using ::picolibrary::Testing::Unit::SPI::Mock_Device_Selector;
using ::testing::A;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::ReturnRef;

using Driver =
    ::picolibrary::Microchip::MCP3008::Driver<Mock_Controller, Mock_Device_Selector::Handle, ::picolibrary::Testing::Unit::SPI::Mock_Device>;

} // namespace

/**
 * \brief Verify picolibrary::Microchip::MCP3008::Driver::Driver( Controller,
 *        Controller::Configuration, Device_Selector, picolibrary::Error_Code ) works
 *        properly.
 */
TEST( constructor, worksProperly )
{
    auto controller = Mock_Controller{};

    auto const nonresponsive_device_error = random<Mock_Error>();

    auto const mcp3008 = Driver{ controller, {}, {}, nonresponsive_device_error };

    EXPECT_EQ( mcp3008.nonresponsive_device_error(), nonresponsive_device_error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP3008::Driver::sample() properly handles a
 *        configuration error.
 */
TEST( sample, configurationError )
{
    auto mcp3008 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp3008, configure() ).WillOnce( Return( error ) );

    auto const result = mcp3008.sample( {} );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP3008::Driver::sample() properly handles a
 *        selection error.
 */
TEST( sample, selectionError )
{
    auto mcp3008 = Driver{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    EXPECT_CALL( mcp3008, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_CALL( mcp3008, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );

    auto const error = random<Mock_Error>();

    EXPECT_CALL( device_selector, select() ).WillOnce( Return( error ) );

    auto const result = mcp3008.sample( {} );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP3008::Driver::sample() properly handles a data
 *        exchange error.
 */
TEST( sample, dataExchangeError )
{
    auto mcp3008 = Driver{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    EXPECT_CALL( mcp3008, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_CALL( mcp3008, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );

    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp3008, exchange( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );

    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = mcp3008.sample( {} );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP3008::Driver::sample() properly handles a
 *        nonresponsive device error.
 */
TEST( sample, nonresponsiveDeviceError )
{
    auto controller = Mock_Controller{};

    auto const nonresponsive_device_error = random<Mock_Error>();

    auto mcp3008 = Driver{ controller, {}, {}, nonresponsive_device_error };

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    EXPECT_CALL( mcp3008, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_CALL( mcp3008, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );

    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const rx = std::vector<std::uint8_t>{
        random<std::uint8_t>(),
        static_cast<std::uint8_t>( random<std::uint8_t>() | 0b100 ),
        random<std::uint8_t>(),
    };

    EXPECT_CALL( mcp3008, exchange( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( rx ) );

    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = mcp3008.sample( {} );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), nonresponsive_device_error );
}

/**
 * \brief Verify picolibrary::Microchip::MCP3008::Driver::sample() works properly.
 */
TEST( sample, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp3008 = Driver{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    EXPECT_CALL( mcp3008, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_CALL( mcp3008, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );

    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const input  = random<Input>();
    auto const sample = random<Sample::Value>( Sample::MIN, Sample::MAX );

    auto const tx = std::vector<std::uint8_t>{
        0x01,
        static_cast<std::uint8_t>( input ),
        0x00,
    };
    auto const rx = std::vector<std::uint8_t>{
        random<std::uint8_t>(),
        static_cast<std::uint8_t>(
            ( random<std::uint8_t>( 0, 0x1F ) << 3 )
            | ( sample >> std::numeric_limits<std::uint8_t>::digits ) ),
        static_cast<std::uint8_t>( sample ),
    };
    EXPECT_CALL( mcp3008, exchange( tx ) ).WillOnce( Return( rx ) );

    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = mcp3008.sample( input );

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value().value(), sample );
}

/**
 * \brief Execute the picolibrary::Microchip::MCP3008::Driver unit tests.
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

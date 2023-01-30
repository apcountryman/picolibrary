/**
 * picolibrary
 *
 * Copyright 2020-2023, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::Microchip::MCP23X08::Pin automated test program.
 */

#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/microchip/mcp23x08.h"
#include "picolibrary/testing/automated/microchip/mcp23x08.h"
#include "picolibrary/testing/automated/random.h"

namespace {

using ::picolibrary::Microchip::MCP23X08::Pin;
using ::picolibrary::Testing::Automated::random;
using ::picolibrary::Testing::Automated::Microchip::MCP23X08::Mock_Caching_Driver;
using ::testing::Return;

} // namespace

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Pin::configure_pin_as_internally_pulled_up_input()
 *        works properly.
 */
TEST( configurePinAsInternallyPulledUpInput, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto pin = Pin{ mcp23x08, mask };

    auto const iodir = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( iodir ) );
    EXPECT_CALL( mcp23x08, write_iodir( iodir | mask ) );

    pin.configure_pin_as_internally_pulled_up_input();
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::configure_pin_as_open_drain_io()
 *        works properly.
 */
TEST( configurePinAsOpenDrainIO, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto pin = Pin{ mcp23x08, mask };

    auto const olat = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, olat() ).WillOnce( Return( olat ) );
    EXPECT_CALL( mcp23x08, write_olat( olat & ~mask ) );

    pin.configure_pin_as_open_drain_io();
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::configure_pin_as_push_pull_io()
 *        works properly.
 */
TEST( configurePinAsPushPullIO, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto pin = Pin{ mcp23x08, mask };

    auto const iodir = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( iodir ) );
    EXPECT_CALL( mcp23x08, write_iodir( iodir & ~mask ) );

    pin.configure_pin_as_push_pull_io();
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::pull_up_is_disabled() works
 *        properly.
 */
TEST( pullUpIsDisabled, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto const pin = Pin{ mcp23x08, mask };

    auto const gppu = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( gppu ) );

    EXPECT_EQ( pin.pull_up_is_disabled(), not( gppu & mask ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::pull_up_is_enabled() works
 *        properly.
 */
TEST( pullUpIsEnabled, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto const pin = Pin{ mcp23x08, mask };

    auto const gppu = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( gppu ) );

    EXPECT_EQ( pin.pull_up_is_enabled(), static_cast<bool>( gppu & mask ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::disable_pull_up() works properly.
 */
TEST( disablePullUp, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto pin = Pin{ mcp23x08, mask };

    auto const gppu = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( gppu ) );
    EXPECT_CALL( mcp23x08, write_gppu( gppu & ~mask ) );

    pin.disable_pull_up();
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::enable_pull_up() works properly.
 */
TEST( enablePullUp, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto pin = Pin{ mcp23x08, mask };

    auto const gppu = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( gppu ) );
    EXPECT_CALL( mcp23x08, write_gppu( gppu | mask ) );

    pin.enable_pull_up();
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::is_low() works properly.
 */
TEST( isLow, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto const pin = Pin{ mcp23x08, mask };

    auto const gpio = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, read_gpio() ).WillOnce( Return( gpio ) );

    EXPECT_EQ( pin.is_low(), not( gpio & mask ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::is_high() works properly.
 */
TEST( isHigh, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto const pin = Pin{ mcp23x08, mask };

    auto const gpio = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, read_gpio() ).WillOnce( Return( gpio ) );

    EXPECT_EQ( pin.is_high(), static_cast<bool>( gpio & mask ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::transition_open_drain_io_to_low()
 *        works properly.
 */
TEST( transitionOpenDrainIOToLow, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto pin = Pin{ mcp23x08, mask };

    auto const iodir = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( iodir ) );
    EXPECT_CALL( mcp23x08, write_iodir( iodir & ~mask ) );

    pin.transition_open_drain_io_to_low();
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::transition_push_pull_io_to_low()
 *        works properly.
 */
TEST( transitionPushPullIOToLow, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto pin = Pin{ mcp23x08, mask };

    auto const olat = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, olat() ).WillOnce( Return( olat ) );
    EXPECT_CALL( mcp23x08, write_olat( olat & ~mask ) );

    pin.transition_push_pull_io_to_low();
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::transition_open_drain_io_to_high()
 *        works properly.
 */
TEST( transitionOpenDrainIOToHigh, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto pin = Pin{ mcp23x08, mask };

    auto const iodir = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( iodir ) );
    EXPECT_CALL( mcp23x08, write_iodir( iodir | mask ) );

    pin.transition_open_drain_io_to_high();
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::transition_push_pull_io_to_high()
 *        works properly.
 */
TEST( transitionPushPullIOToHigh, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto pin = Pin{ mcp23x08, mask };

    auto const olat = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, olat() ).WillOnce( Return( olat ) );
    EXPECT_CALL( mcp23x08, write_olat( olat | mask ) );

    pin.transition_push_pull_io_to_high();
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::toggle_open_drain_io() works
 *        properly.
 */
TEST( toggleOpenDrainIO, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto pin = Pin{ mcp23x08, mask };

    auto const iodir = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( iodir ) );
    EXPECT_CALL( mcp23x08, write_iodir( iodir ^ mask ) );

    pin.toggle_open_drain_io();
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::toggle_push_pull_io() works
 *        properly.
 */
TEST( togglePushPullIO, worksProperly )
{
    auto       mcp23x08 = Mock_Caching_Driver{};
    auto const mask     = random<std::uint8_t>();

    auto pin = Pin{ mcp23x08, mask };

    auto const olat = random<std::uint8_t>();

    EXPECT_CALL( mcp23x08, olat() ).WillOnce( Return( olat ) );
    EXPECT_CALL( mcp23x08, write_olat( olat ^ mask ) );

    pin.toggle_push_pull_io();
}

/**
 * \brief Execute the picolibrary::Microchip::MCP23X08::Pin automated tests.
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

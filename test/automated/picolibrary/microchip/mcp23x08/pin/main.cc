/**
 * picolibrary
 *
 * Copyright 2020-2024, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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

#include <bitset>
#include <cstdint>
#include <ios>
#include <limits>
#include <ostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/microchip/mcp23x08.h"
#include "picolibrary/testing/automated/microchip/mcp23x08.h"

namespace {

using ::picolibrary::Microchip::MCP23X08::Pin;
using ::picolibrary::Testing::Automated::Microchip::MCP23X08::Mock_Caching_Driver;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

} // namespace

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Pin::configure_pin_as_internally_pulled_up_input()
 *        works properly.
 */
TEST( configurePinAsInternallyPulledUpInput, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Pin{ mcp23x08, 0b0101'0001 };

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( 0b0011'1001 ) );
    EXPECT_CALL( mcp23x08, write_iodir( 0b0111'1001 ) );

    pin.configure_pin_as_internally_pulled_up_input();
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::configure_pin_as_open_drain_io()
 *        works properly.
 */
TEST( configurePinAsOpenDrainIO, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Pin{ mcp23x08, 0b0101'1011 };

    EXPECT_CALL( mcp23x08, olat() ).WillOnce( Return( 0b0011'0111 ) );
    EXPECT_CALL( mcp23x08, write_olat( 0b0010'0100 ) );

    pin.configure_pin_as_open_drain_io();
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::configure_pin_as_push_pull_io()
 *        works properly.
 */
TEST( configurePinAsPushPullIO, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Pin{ mcp23x08, 0b0101'0111 };

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( 0b0011'1101 ) );
    EXPECT_CALL( mcp23x08, write_iodir( 0b0010'1000 ) );

    pin.configure_pin_as_push_pull_io();
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Pin::pull_up_is_disabled() test case.
 */
struct pullUpIsDisabled_Test_Case {
    /**
     * \brief The GPPU register value.
     */
    std::uint8_t gppu;

    /**
     * \brief The pin's internal pull-up resistor is disabled.
     */
    bool pull_up_is_disabled;
};

auto operator<<( std::ostream & stream, pullUpIsDisabled_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".gppu = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.gppu }
                  << ", "
                  << ".pull_up_is_disabled = " << std::boolalpha << test_case.pull_up_is_disabled
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Pin::pull_up_is_disabled() test fixture.
 */
class pullUpIsDisabled : public TestWithParam<pullUpIsDisabled_Test_Case> {
};

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::pull_up_is_disabled() works
 *        properly.
 */
TEST_P( pullUpIsDisabled, worksProperly )
{
    auto const test_case = GetParam();

    auto mcp23x08 = Mock_Caching_Driver{};

    auto const pin = Pin{ mcp23x08, 0b0000'1000 };

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( test_case.gppu ) );

    EXPECT_EQ( pin.pull_up_is_disabled(), test_case.pull_up_is_disabled );
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Pin::pull_up_is_disabled() test cases.
 */
pullUpIsDisabled_Test_Case const pullUpIsDisabled_TEST_CASES[]{
    // clang-format off

    { 0b0011'0110, true  },
    { 0b0011'1110, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, pullUpIsDisabled, ValuesIn( pullUpIsDisabled_TEST_CASES ) );

/**
 * \brief picolibrary::Microchip::MCP23X08::Pin::pull_up_is_enabled() test case.
 */
struct pullUpIsEnabled_Test_Case {
    /**
     * \brief The GPPU register value.
     */
    std::uint8_t gppu;

    /**
     * \brief The pin's internal pull-up resistor is enabled.
     */
    bool pull_up_is_enabled;
};

auto operator<<( std::ostream & stream, pullUpIsEnabled_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".gppu = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.gppu }
                  << ", "
                  << ".pull_up_is_enabled = " << std::boolalpha << test_case.pull_up_is_enabled
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Pin::pull_up_is_enabled() test fixture.
 */
class pullUpIsEnabled : public TestWithParam<pullUpIsEnabled_Test_Case> {
};

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::pull_up_is_enabled() works
 *        properly.
 */
TEST_P( pullUpIsEnabled, worksProperly )
{
    auto const test_case = GetParam();

    auto mcp23x08 = Mock_Caching_Driver{};

    auto const pin = Pin{ mcp23x08, 0b0001'0000 };

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( test_case.gppu ) );

    EXPECT_EQ( pin.pull_up_is_enabled(), test_case.pull_up_is_enabled );
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Pin::pull_up_is_enabled() test cases.
 */
pullUpIsEnabled_Test_Case const pullUpIsEnabled_TEST_CASES[]{
    // clang-format off

    { 0b0110'1000, false },
    { 0b0111'1000, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, pullUpIsEnabled, ValuesIn( pullUpIsEnabled_TEST_CASES ) );

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::disable_pull_up() works properly.
 */
TEST( disablePullUp, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Pin{ mcp23x08, 0b0101'1011 };

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( 0b0011'0110 ) );
    EXPECT_CALL( mcp23x08, write_gppu( 0b0010'0100 ) );

    pin.disable_pull_up();
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::enable_pull_up() works properly.
 */
TEST( enablePullUp, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Pin{ mcp23x08, 0b0101'1011 };

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( 0b0011'0110 ) );
    EXPECT_CALL( mcp23x08, write_gppu( 0b0111'1111 ) );

    pin.enable_pull_up();
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Pin::is_low() test case.
 */
struct isLow_Test_Case {
    /**
     * \brief The GPIO register value.
     */
    std::uint8_t gpio;

    /**
     * \brief The pin is in the low state.
     */
    bool is_low;
};

auto operator<<( std::ostream & stream, isLow_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".gpio = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.gpio }
                  << ", "
                  << ".is_low = " << std::boolalpha << test_case.is_low
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Pin::is_low() test fixture.
 */
class isLow : public TestWithParam<isLow_Test_Case> {
};

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::is_low() works properly.
 */
TEST_P( isLow, worksProperly )
{
    auto const test_case = GetParam();

    auto mcp23x08 = Mock_Caching_Driver{};

    auto const pin = Pin{ mcp23x08, 0b0100'0000 };

    EXPECT_CALL( mcp23x08, read_gpio() ).WillOnce( Return( test_case.gpio ) );

    EXPECT_EQ( pin.is_low(), test_case.is_low );
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Pin::is_low() test cases.
 */
isLow_Test_Case const isLow_TEST_CASES[]{
    // clang-format off

    { 0b1000'0101, true  },
    { 0b1100'0101, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, isLow, ValuesIn( isLow_TEST_CASES ) );

/**
 * \brief picolibrary::Microchip::MCP23X08::Pin::is_high() test case.
 */
struct isHigh_Test_Case {
    /**
     * \brief The GPIO register value.
     */
    std::uint8_t gpio;

    /**
     * \brief The pin is in the high state.
     */
    bool is_high;
};

auto operator<<( std::ostream & stream, isHigh_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".gpio = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.gpio }
                  << ", "
                  << ".is_high = " << std::boolalpha << test_case.is_high
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Pin::is_high() test fixture.
 */
class isHigh : public TestWithParam<isHigh_Test_Case> {
};

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::is_high() works properly.
 */
TEST_P( isHigh, worksProperly )
{
    auto const test_case = GetParam();

    auto mcp23x08 = Mock_Caching_Driver{};

    auto const pin = Pin{ mcp23x08, 0b0001'0000 };

    EXPECT_CALL( mcp23x08, read_gpio() ).WillOnce( Return( test_case.gpio ) );

    EXPECT_EQ( pin.is_high(), test_case.is_high );
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Pin::is_high() test cases.
 */
isHigh_Test_Case const isHigh_TEST_CASES[]{
    // clang-format off

    { 0b1100'1010, false },
    { 0b1101'1010, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, isHigh, ValuesIn( isHigh_TEST_CASES ) );

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::transition_open_drain_io_to_low()
 *        works properly.
 */
TEST( transitionOpenDrainIOToLow, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Pin{ mcp23x08, 0b0101'1010 };

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( 0b0011'1110 ) );
    EXPECT_CALL( mcp23x08, write_iodir( 0b0010'0100 ) );

    pin.transition_open_drain_io_to_low();
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::transition_push_pull_io_to_low()
 *        works properly.
 */
TEST( transitionPushPullIOToLow, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Pin{ mcp23x08, 0b0101'0111 };

    EXPECT_CALL( mcp23x08, olat() ).WillOnce( Return( 0b0011'0000 ) );
    EXPECT_CALL( mcp23x08, write_olat( 0b0010'0000 ) );

    pin.transition_push_pull_io_to_low();
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::transition_open_drain_io_to_high()
 *        works properly.
 */
TEST( transitionOpenDrainIOToHigh, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Pin{ mcp23x08, 0b0101'1101 };

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( 0b0011'0010 ) );
    EXPECT_CALL( mcp23x08, write_iodir( 0b0111'1111 ) );

    pin.transition_open_drain_io_to_high();
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::transition_push_pull_io_to_high()
 *        works properly.
 */
TEST( transitionPushPullIOToHigh, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Pin{ mcp23x08, 0b0101'1101 };

    EXPECT_CALL( mcp23x08, olat() ).WillOnce( Return( 0b0011'0111 ) );
    EXPECT_CALL( mcp23x08, write_olat( 0b0111'1111 ) );

    pin.transition_push_pull_io_to_high();
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::toggle_open_drain_io() works
 *        properly.
 */
TEST( toggleOpenDrainIO, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Pin{ mcp23x08, 0b0101'1010 };

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( 0b0011'0101 ) );
    EXPECT_CALL( mcp23x08, write_iodir( 0b0110'1111 ) );

    pin.toggle_open_drain_io();
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Pin::toggle_push_pull_io() works
 *        properly.
 */
TEST( togglePushPullIO, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Pin{ mcp23x08, 0b0101'1010 };

    EXPECT_CALL( mcp23x08, olat() ).WillOnce( Return( 0b0011'1000 ) );
    EXPECT_CALL( mcp23x08, write_olat( 0b0110'0010 ) );

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

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
 * \brief picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin automated test
 *        program.
 */

#include <bitset>
#include <cstdint>
#include <ios>
#include <limits>
#include <ostream>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/gpio.h"
#include "picolibrary/microchip/mcp23x08.h"
#include "picolibrary/testing/automated/gpio.h"
#include "picolibrary/testing/automated/microchip/mcp23x08.h"

namespace {

using ::picolibrary::GPIO::Initial_Pull_Up_State;
using ::picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin;
using ::picolibrary::Testing::Automated::Microchip::MCP23X08::Mock_Caching_Driver;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

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

    auto mcp23x08 = Mock_Caching_Driver{};

    auto const pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, 0b0101'1100 };

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( 0b0011'1111 ) );
    EXPECT_CALL( mcp23x08, write_gppu( 0b0010'0011 ) );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::Internally_Pulled_Up_Input_Pin(
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin && ) works
 *        properly when the source of the move was default constructed.
 */
TEST( constructorMove, worksProperlySourceDefaultConstructed )
{
    auto source = Internally_Pulled_Up_Input_Pin<Mock_Caching_Driver>{};

    auto const pin = Internally_Pulled_Up_Input_Pin{ std::move( source ) };
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::Internally_Pulled_Up_Input_Pin(
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin && ) works
 *        properly when the source of the move was constructed from a caching driver and a
 *        mask.
 */
TEST( constructorMove, worksProperlySourceCachingDriverMask )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto source = Internally_Pulled_Up_Input_Pin{ mcp23x08, 0b0101'1110 };

    EXPECT_CALL( mcp23x08, gppu() ).Times( 0 );
    EXPECT_CALL( mcp23x08, write_gppu( _ ) ).Times( 0 );

    auto const pin = Internally_Pulled_Up_Input_Pin{ std::move( source ) };

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( 0b0011'1101 ) );
    EXPECT_CALL( mcp23x08, write_gppu( 0b0010'0001 ) );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::operator=(
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin && ) works
 *        properly when the expression to be assigned was default constructed, and the
 *        assigned to object was default constructed.
 */
TEST( assignmentOperatorMove, worksProperlyExpressionDefaultConstructedObjectDefaultConstructed )
{
    auto expression = Internally_Pulled_Up_Input_Pin<Mock_Caching_Driver>{};
    auto object     = Internally_Pulled_Up_Input_Pin<Mock_Caching_Driver>{};

    object = std::move( expression );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::operator=(
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin && ) works
 *        properly when the expression to be assigned was constructed from a caching
 *        driver and a mask, and the assigned to object was default constructed.
 */
TEST( assignmentOperatorMove, worksProperlyExpressionCachingDriverMaskObjectDefaultConstructed )
{
    auto const in_sequence = InSequence{};

    auto expression_mcp23x08 = Mock_Caching_Driver{};

    auto expression = Internally_Pulled_Up_Input_Pin{ expression_mcp23x08, 0b0101'1100 };
    auto object     = Internally_Pulled_Up_Input_Pin<Mock_Caching_Driver>{};

    EXPECT_CALL( expression_mcp23x08, gppu() ).Times( 0 );
    EXPECT_CALL( expression_mcp23x08, write_gppu( _ ) ).Times( 0 );

    object = std::move( expression );

    EXPECT_CALL( expression_mcp23x08, gppu() ).WillOnce( Return( 0b0011'1001 ) );
    EXPECT_CALL( expression_mcp23x08, write_gppu( 0b0010'0001 ) );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::operator=(
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin && ) works
 *        properly when the expression to be assigned was default constructed, and the
 *        assigned to object was constructed from a caching driver and a mask.
 */
TEST( assignmentOperatorMove, worksProperlyExpressionDefaultConstructedObjectCachingDriverMask )
{
    auto const in_sequence = InSequence{};

    auto object_mcp23x08 = Mock_Caching_Driver{};

    auto expression = Internally_Pulled_Up_Input_Pin<Mock_Caching_Driver>{};
    auto object     = Internally_Pulled_Up_Input_Pin{ object_mcp23x08, 0b0101'1001 };

    EXPECT_CALL( object_mcp23x08, gppu() ).WillOnce( Return( 0b0011'1111 ) );
    EXPECT_CALL( object_mcp23x08, write_gppu( 0b0010'0110 ) );

    object = std::move( expression );

    EXPECT_CALL( object_mcp23x08, gppu() ).Times( 0 );
    EXPECT_CALL( object_mcp23x08, write_gppu( _ ) ).Times( 0 );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::operator=(
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin && ) works
 *        properly when the expression to be assigned was constructed from a caching
 *        driver and a mask, and the assigned to object was constructed from a caching
 *        driver and a mask.
 */
TEST( assignmentOperatorMove, worksProperlyExpressionCachingDriverMaskObjectCachingDriverMask )
{
    auto const in_sequence = InSequence{};

    auto expression_mcp23x08 = Mock_Caching_Driver{};
    auto object_mcp23x08     = Mock_Caching_Driver{};

    auto expression = Internally_Pulled_Up_Input_Pin{ expression_mcp23x08, 0b0101'0011 };
    auto object     = Internally_Pulled_Up_Input_Pin{ object_mcp23x08, 0b0011'0101 };

    EXPECT_CALL( expression_mcp23x08, gppu() ).Times( 0 );
    EXPECT_CALL( expression_mcp23x08, write_gppu( _ ) ).Times( 0 );
    EXPECT_CALL( object_mcp23x08, gppu() ).WillOnce( Return( 0b1111'0011 ) );
    EXPECT_CALL( object_mcp23x08, write_gppu( 0b1100'0010 ) );

    object = std::move( expression );

    EXPECT_CALL( expression_mcp23x08, gppu() ).WillOnce( Return( 0b0011'1111 ) );
    EXPECT_CALL( expression_mcp23x08, write_gppu( 0b0010'1100 ) );
    EXPECT_CALL( object_mcp23x08, gppu() ).Times( 0 );
    EXPECT_CALL( object_mcp23x08, write_gppu( _ ) ).Times( 0 );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::operator=(
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin && ) works
 *        properly when self assignment occurs, and the pin was default constructed.
 */
TEST( assignmentOperatorMove, worksProperlySelfAssignmentDefaultConstructed )
{
    auto pin = Internally_Pulled_Up_Input_Pin<Mock_Caching_Driver>{};

    pin = std::move( pin );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::operator=(
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin && ) works
 *        properly when self assignment occurs, and the pin was constructed from a caching
 *        driver and a mask.
 */
TEST( assignmentOperatorMove, worksProperlySelfAssignmentCachingDriverMask )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, 0b0101'0100 };

    EXPECT_CALL( mcp23x08, gppu() ).Times( 0 );
    EXPECT_CALL( mcp23x08, write_gppu( _ ) ).Times( 0 );

    pin = std::move( pin );

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( 0b0011'0001 ) );
    EXPECT_CALL( mcp23x08, write_gppu( 0b0010'0001 ) );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::initialize()
 *        works properly when the default initial internal pull-up resistor state is used.
 */
TEST( initializeDefaultInitialPullUpState, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, 0b0101'0001 };

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( 0b0011'1000 ) );
    EXPECT_CALL( mcp23x08, write_iodir( 0b0111'1001 ) );
    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( 0b0011'1011 ) );
    EXPECT_CALL( mcp23x08, write_gppu( 0b0010'1010 ) );

    pin.initialize();

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( 0b0010'0001 ) );
    EXPECT_CALL( mcp23x08, write_gppu( _ ) );
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::initialize()
 *        non-default initial internal pull-up resistor state test case.
 */
struct initializeNonDefaultInitialPullUpState_Test_Case {
    /**
     * \brief The initial state of the pin's internal pull-up resistor.
     */
    Initial_Pull_Up_State initial_pull_up_state;

    /**
     * \brief The GPPU register value.
     */
    std::uint8_t gppu;
};

auto operator<<( std::ostream & stream, initializeNonDefaultInitialPullUpState_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".initial_pull_up_state = " << test_case.initial_pull_up_state
                  << ", "
                  << ".gppu = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.gppu }
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::initialize()
 *        non-default initial internal pull-up resistor state test fixture.
 */
class initializeNonDefaultInitialPullUpState :
    public TestWithParam<initializeNonDefaultInitialPullUpState_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::initialize()
 *        works properly when a non-default initial internal pull-up resistor state is
 *        used.
 */
TEST_P( initializeNonDefaultInitialPullUpState, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, 0b0101'0001 };

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( 0b0011'1000 ) );
    EXPECT_CALL( mcp23x08, write_iodir( 0b0111'1001 ) );
    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( 0b0011'1011 ) );
    EXPECT_CALL( mcp23x08, write_gppu( test_case.gppu ) );

    pin.initialize( test_case.initial_pull_up_state );

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( 0b0010'0001 ) );
    EXPECT_CALL( mcp23x08, write_gppu( _ ) );
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::initialize()
 *        non-default initial internal pull-up resistor state test cases.
 */
initializeNonDefaultInitialPullUpState_Test_Case const initializeNonDefaultInitialPullUpState_TEST_CASES[]{
    // clang-format off

    { Initial_Pull_Up_State::DISABLED, 0b0010'1010 },
    { Initial_Pull_Up_State::ENABLED,  0b0111'1011 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, initializeNonDefaultInitialPullUpState, ValuesIn( initializeNonDefaultInitialPullUpState_TEST_CASES ) );

/**
 * \brief picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::pull_up_is_disabled()
 *        test case.
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
 * \brief picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::pull_up_is_disabled()
 *        test fixture.
 */
class pullUpIsDisabled : public TestWithParam<pullUpIsDisabled_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::pull_up_is_disabled()
 *        works properly.
 */
TEST_P( pullUpIsDisabled, worksProperly )
{
    auto const test_case = GetParam();

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, 0b0000'1000 };

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( test_case.gppu ) );

    ASSERT_EQ( pin.pull_up_is_disabled(), test_case.pull_up_is_disabled );

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( 0b0010'0001 ) );
    EXPECT_CALL( mcp23x08, write_gppu( _ ) );
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::pull_up_is_disabled()
 *        test cases.
 */
pullUpIsDisabled_Test_Case const pullUpIsDisabled_TEST_CASES[]{
    // clang-format off

    { 0b0011'0110, true  },
    { 0b0011'1110, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, pullUpIsDisabled, ValuesIn( pullUpIsDisabled_TEST_CASES ) );

/**
 * \brief picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::pull_up_is_enabled()
 *        test case.
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
 * \brief picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::pull_up_is_enabled()
 *        test fixture.
 */
class pullUpIsEnabled : public TestWithParam<pullUpIsEnabled_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::pull_up_is_enabled()
 *        works properly.
 */
TEST_P( pullUpIsEnabled, worksProperly )
{
    auto const test_case = GetParam();

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, 0b0001'0000 };

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( test_case.gppu ) );

    ASSERT_EQ( pin.pull_up_is_enabled(), test_case.pull_up_is_enabled );

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( 0b0010'0001 ) );
    EXPECT_CALL( mcp23x08, write_gppu( _ ) );
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::pull_up_is_enabled()
 *        test cases.
 */
pullUpIsEnabled_Test_Case const pullUpIsEnabled_TEST_CASES[]{
    // clang-format off

    { 0b0110'1000, false },
    { 0b0111'1000, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, pullUpIsEnabled, ValuesIn( pullUpIsEnabled_TEST_CASES ) );

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::disable_pull_up()
 *        works properly.
 */
TEST( disablePullUp, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, 0b0101'1011 };

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( 0b0011'0110 ) );
    EXPECT_CALL( mcp23x08, write_gppu( 0b0010'0100 ) );

    pin.disable_pull_up();

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( 0b0010'0001 ) );
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

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, 0b0101'1011 };

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( 0b0011'0110 ) );
    EXPECT_CALL( mcp23x08, write_gppu( 0b0111'1111 ) );

    pin.enable_pull_up();

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( 0b0010'0001 ) );
    EXPECT_CALL( mcp23x08, write_gppu( _ ) );
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::is_low() test
 *        case.
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
 * \brief picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::is_low() test
 *        fixture.
 */
class isLow : public TestWithParam<isLow_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::is_low() works
 *        properly.
 */
TEST_P( isLow, worksProperly )
{
    auto const test_case = GetParam();

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, 0b0100'0000 };

    EXPECT_CALL( mcp23x08, read_gpio() ).WillOnce( Return( test_case.gpio ) );

    ASSERT_EQ( pin.is_low(), test_case.is_low );

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( 0b0010'0001 ) );
    EXPECT_CALL( mcp23x08, write_gppu( _ ) );
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::is_low() test
 *        cases.
 */
isLow_Test_Case const isLow_TEST_CASES[]{
    // clang-format off

    { 0b1000'0101, true  },
    { 0b1100'0101, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, isLow, ValuesIn( isLow_TEST_CASES ) );

/**
 * \brief picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::is_high() test
 *        case.
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
 * \brief picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::is_high() test
 *        fixture.
 */
class isHigh : public TestWithParam<isHigh_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::is_high()
 *        works properly.
 */
TEST_P( isHigh, worksProperly )
{
    auto const test_case = GetParam();

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Internally_Pulled_Up_Input_Pin{ mcp23x08, 0b0001'0000 };

    EXPECT_CALL( mcp23x08, read_gpio() ).WillOnce( Return( test_case.gpio ) );

    ASSERT_EQ( pin.is_high(), test_case.is_high );

    EXPECT_CALL( mcp23x08, gppu() ).WillOnce( Return( 0b0010'0001 ) );
    EXPECT_CALL( mcp23x08, write_gppu( _ ) );
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin::is_high() test
 *        cases.
 */
isHigh_Test_Case const isHigh_TEST_CASES[]{
    // clang-format off

    { 0b1100'1010, false },
    { 0b1101'1010, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, isHigh, ValuesIn( isHigh_TEST_CASES ) );

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

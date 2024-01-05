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
 * \brief picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin automated test program.
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

using ::picolibrary::GPIO::Initial_Pin_State;
using ::picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin;
using ::picolibrary::Testing::Automated::Microchip::MCP23X08::Mock_Caching_Driver;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

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

    auto mcp23x08 = Mock_Caching_Driver{};

    auto const pin = Push_Pull_IO_Pin{ mcp23x08, 0b0101'1110 };

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( 0b0011'1001 ) );
    EXPECT_CALL( mcp23x08, write_iodir( 0b0111'1111 ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::Push_Pull_IO_Pin(
 *        picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin && ) works properly when the
 *        source of the move was default constructed.
 */
TEST( constructorMove, worksProperlySourceDefaultConstructed )
{
    auto source = Push_Pull_IO_Pin<Mock_Caching_Driver>{};

    auto const pin = Push_Pull_IO_Pin{ std::move( source ) };
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::Push_Pull_IO_Pin(
 *        picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin && ) works properly when the
 *        source of the move was constructed from a caching driver and a mask.
 */
TEST( constructorMove, worksProperlySourceCachingDriverMask )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto source = Push_Pull_IO_Pin{ mcp23x08, 0b0101'0000 };

    EXPECT_CALL( mcp23x08, iodir() ).Times( 0 );
    EXPECT_CALL( mcp23x08, write_iodir( _ ) ).Times( 0 );

    auto const pin = Push_Pull_IO_Pin{ std::move( source ) };

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( 0b0011'1001 ) );
    EXPECT_CALL( mcp23x08, write_iodir( 0b0111'1001 ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::operator=(
 *        picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin && ) works properly when the
 *        expression to be assigned was default constructed, and the assigned to object
 *        was default constructed.
 */
TEST( assignmentOperatorMove, worksProperlyExpressionDefaultConstructedObjectDefaultConstructed )
{
    auto expression = Push_Pull_IO_Pin<Mock_Caching_Driver>{};
    auto object     = Push_Pull_IO_Pin<Mock_Caching_Driver>{};

    object = std::move( expression );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::operator=(
 *        picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin && ) works properly when the
 *        expression to be assigned was constructed from a caching driver and a mask, and
 *        the assigned to object was default constructed.
 */
TEST( assignmentOperatorMove, worksProperlyExpressionCachingDriverMaskObjectDefaultConstructed )
{
    auto const in_sequence = InSequence{};

    auto expression_mcp23x08 = Mock_Caching_Driver{};

    auto expression = Push_Pull_IO_Pin{ expression_mcp23x08, 0b0101'0110 };
    auto object     = Push_Pull_IO_Pin<Mock_Caching_Driver>{};

    EXPECT_CALL( expression_mcp23x08, iodir() ).Times( 0 );
    EXPECT_CALL( expression_mcp23x08, write_iodir( _ ) ).Times( 0 );

    object = std::move( expression );

    EXPECT_CALL( expression_mcp23x08, iodir() ).WillOnce( Return( 0b0011'1111 ) );
    EXPECT_CALL( expression_mcp23x08, write_iodir( 0b0111'1111 ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::operator=(
 *        picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin && ) works properly when the
 *        expression to be assigned was default constructed, and the assigned to object
 *        was constructed from a caching driver and a mask.
 */
TEST( assignmentOperatorMove, worksProperlyExpressionDefaultConstructedObjectCachingDriverMask )
{
    auto const in_sequence = InSequence{};

    auto object_mcp23x08 = Mock_Caching_Driver{};

    auto expression = Push_Pull_IO_Pin<Mock_Caching_Driver>{};
    auto object     = Push_Pull_IO_Pin{ object_mcp23x08, 0b0101'1101 };

    EXPECT_CALL( object_mcp23x08, iodir() ).WillOnce( Return( 0b0011'0001 ) );
    EXPECT_CALL( object_mcp23x08, write_iodir( 0b0111'1101 ) );

    object = std::move( expression );

    EXPECT_CALL( object_mcp23x08, iodir() ).Times( 0 );
    EXPECT_CALL( object_mcp23x08, write_iodir( _ ) ).Times( 0 );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::operator=(
 *        picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin && ) works properly when the
 *        expression to be assigned was constructed from a caching driver and a mask, and
 *        the assigned to object was constructed from a caching driver and a mask.
 */
TEST( assignmentOperatorMove, worksProperlyExpressionCachingDriverMaskObjectCachingDriverMask )
{
    auto const in_sequence = InSequence{};

    auto expression_mcp23x08 = Mock_Caching_Driver{};
    auto object_mcp23x08     = Mock_Caching_Driver{};

    auto expression = Push_Pull_IO_Pin{ expression_mcp23x08, 0b0101'0011 };
    auto object     = Push_Pull_IO_Pin{ object_mcp23x08, 0b0011'0101 };

    EXPECT_CALL( expression_mcp23x08, iodir() ).Times( 0 );
    EXPECT_CALL( expression_mcp23x08, write_iodir( _ ) ).Times( 0 );
    EXPECT_CALL( object_mcp23x08, iodir() ).WillOnce( Return( 0b1100'0011 ) );
    EXPECT_CALL( object_mcp23x08, write_iodir( 0b1111'0111 ) );

    object = std::move( expression );

    EXPECT_CALL( expression_mcp23x08, iodir() ).WillOnce( Return( 0b0011'1100 ) );
    EXPECT_CALL( expression_mcp23x08, write_iodir( 0b0111'1111 ) );
    EXPECT_CALL( object_mcp23x08, iodir() ).Times( 0 );
    EXPECT_CALL( object_mcp23x08, write_iodir( _ ) ).Times( 0 );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::operator=(
 *        picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin && ) works properly when self
 *        assignment occurs, and the pin was default constructed.
 */
TEST( assignmentOperatorMove, worksProperlySelfAssignmentDefaultConstructed )
{
    auto pin = Push_Pull_IO_Pin<Mock_Caching_Driver>{};

    pin = std::move( pin );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::operator=(
 *        picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin && ) works properly when self
 *        assignment occurs, and the pin was constructed from a caching driver and a mask.
 */
TEST( assignmentOperatorMove, worksProperlySelfAssignmentCachingDriverMask )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Push_Pull_IO_Pin{ mcp23x08, 0b0101'0011 };

    EXPECT_CALL( mcp23x08, iodir() ).Times( 0 );
    EXPECT_CALL( mcp23x08, write_iodir( _ ) ).Times( 0 );

    pin = std::move( pin );

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( 0b0011'0111 ) );
    EXPECT_CALL( mcp23x08, write_iodir( 0b0111'0111 ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::initialize() works
 *        properly when the default initial pin state is used.
 */
TEST( initializeDefaultInitialPinState, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Push_Pull_IO_Pin{ mcp23x08, 0b0101'1000 };

    EXPECT_CALL( mcp23x08, olat() ).WillOnce( Return( 0b0011'1101 ) );
    EXPECT_CALL( mcp23x08, write_olat( 0b0010'0101 ) );
    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( 0b0011'1010 ) );
    EXPECT_CALL( mcp23x08, write_iodir( 0b0010'0010 ) );

    pin.initialize();

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( 0b1000'0110 ) );
    EXPECT_CALL( mcp23x08, write_iodir( _ ) );
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::initialize() non-default
 *        initial pin state test case.
 */
struct initializeNonDefaultInitialPinState_Test_Case {
    /**
     * \brief The initial state of the pin.
     */
    Initial_Pin_State initial_pin_state;

    /**
     * \brief The OLAT register value.
     */
    std::uint8_t olat;
};

auto operator<<( std::ostream & stream, initializeNonDefaultInitialPinState_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".initial_pin_state = " << test_case.initial_pin_state
                  << ", "
                  << ".olat = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.olat }
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::initialize() non-default
 *        initial pin state test fixture.
 */
class initializeNonDefaultInitialPinState :
    public TestWithParam<initializeNonDefaultInitialPinState_Test_Case> {
};

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::initialize() works
 *        properly when a non-default initial pin state is used.
 */
TEST_P( initializeNonDefaultInitialPinState, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Push_Pull_IO_Pin{ mcp23x08, 0b0101'1000 };

    EXPECT_CALL( mcp23x08, olat() ).WillOnce( Return( 0b0011'1010 ) );
    EXPECT_CALL( mcp23x08, write_olat( test_case.olat ) );
    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( 0b0011'1101 ) );
    EXPECT_CALL( mcp23x08, write_iodir( 0b0010'0101 ) );

    pin.initialize( test_case.initial_pin_state );

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( 0b1000'0110 ) );
    EXPECT_CALL( mcp23x08, write_iodir( _ ) );
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::initialize() non-default
 *        initial pin state test cases.
 */
initializeNonDefaultInitialPinState_Test_Case const initializeNonDefaultInitialPinState_TEST_CASES[]{
    // clang-format off

    { Initial_Pin_State::LOW,  0b0010'0010 },
    { Initial_Pin_State::HIGH, 0b0111'1010 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, initializeNonDefaultInitialPinState, ValuesIn( initializeNonDefaultInitialPinState_TEST_CASES ) );

/**
 * \brief picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::is_low() test case.
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
 * \brief picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::is_low() test fixture.
 */
class isLow : public TestWithParam<isLow_Test_Case> {
};

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::is_low() works
 *        properly.
 */
TEST_P( isLow, worksProperly )
{
    auto const test_case = GetParam();

    auto mcp23x08 = Mock_Caching_Driver{};

    auto const pin = Push_Pull_IO_Pin{ mcp23x08, 0b0100'0000 };

    EXPECT_CALL( mcp23x08, read_gpio() ).WillOnce( Return( test_case.gpio ) );

    EXPECT_EQ( pin.is_low(), test_case.is_low );

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( 0b1000'0110 ) );
    EXPECT_CALL( mcp23x08, write_iodir( _ ) );
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::is_low() test cases.
 */
isLow_Test_Case const isLow_TEST_CASES[]{
    // clang-format off

    { 0b1000'0101, true  },
    { 0b1100'0101, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, isLow, ValuesIn( isLow_TEST_CASES ) );

/**
 * \brief picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::is_high() test case.
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
 * \brief picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::is_high() test fixture.
 */
class isHigh : public TestWithParam<isHigh_Test_Case> {
};

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::is_high() works
 *        properly.
 */
TEST_P( isHigh, worksProperly )
{
    auto const test_case = GetParam();

    auto mcp23x08 = Mock_Caching_Driver{};

    auto const pin = Push_Pull_IO_Pin{ mcp23x08, 0b0001'0000 };

    EXPECT_CALL( mcp23x08, read_gpio() ).WillOnce( Return( test_case.gpio ) );

    EXPECT_EQ( pin.is_high(), test_case.is_high );

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( 0b1000'0110 ) );
    EXPECT_CALL( mcp23x08, write_iodir( _ ) );
}

/**
 * \brief picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::is_high() test cases.
 */
isHigh_Test_Case const isHigh_TEST_CASES[]{
    // clang-format off

    { 0b1100'1010, false },
    { 0b1101'1010, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, isHigh, ValuesIn( isHigh_TEST_CASES ) );

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::transition_to_low()
 *        works properly.
 */
TEST( transitionToLow, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Push_Pull_IO_Pin{ mcp23x08, 0b0101'0110 };

    EXPECT_CALL( mcp23x08, olat() ).WillOnce( Return( 0b0011'1011 ) );
    EXPECT_CALL( mcp23x08, write_olat( 0b0010'1001 ) );

    pin.transition_to_low();

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( 0b1000'0110 ) );
    EXPECT_CALL( mcp23x08, write_iodir( _ ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::transition_to_high()
 *        works properly.
 */
TEST( transitionToHigh, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Push_Pull_IO_Pin{ mcp23x08, 0b0101'1011 };

    EXPECT_CALL( mcp23x08, olat() ).WillOnce( Return( 0b0011'1000 ) );
    EXPECT_CALL( mcp23x08, write_olat( 0b0111'1011 ) );

    pin.transition_to_high();

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( 0b1000'0110 ) );
    EXPECT_CALL( mcp23x08, write_iodir( _ ) );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin::toggle() works
 *        properly.
 */
TEST( toggle, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto mcp23x08 = Mock_Caching_Driver{};

    auto pin = Push_Pull_IO_Pin{ mcp23x08, 0b0101'0011 };

    EXPECT_CALL( mcp23x08, olat() ).WillOnce( Return( 0b0011'1011 ) );
    EXPECT_CALL( mcp23x08, write_olat( 0b0110'1000 ) );

    pin.toggle();

    EXPECT_CALL( mcp23x08, iodir() ).WillOnce( Return( 0b1000'0110 ) );
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

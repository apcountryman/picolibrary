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
 * \brief picolibrary::Microchip::MCP23S08::Address_Transmitted unit test program.
 */

#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/microchip/mcp23s08.h"
#include "picolibrary/precondition.h"
#include "picolibrary/testing/unit/microchip/mcp23s08.h"
#include "picolibrary/testing/unit/random.h"

namespace {

using ::picolibrary::BYPASS_PRECONDITION_EXPECTATION_CHECKS;
using ::picolibrary::Microchip::MCP23S08::Address_Numeric;
using ::picolibrary::Microchip::MCP23S08::Address_Transmitted;
using ::picolibrary::Testing::Unit::random;

auto random_address( Address_Transmitted::Unsigned_Integer min = 0b01000'00'0, Address_Transmitted::Unsigned_Integer max = 0b01000'11'0 )
{
    return static_cast<Address_Transmitted::Unsigned_Integer>(
        random<Address_Transmitted::Unsigned_Integer>( min, max ) & 0b11111'11'0 );
}

auto random_unique_address_pair()
{
    auto const a = random_address();
    auto const b = random_address();

    return std::pair<Address_Transmitted::Unsigned_Integer, Address_Transmitted::Unsigned_Integer>{
        a, b != a ? b : b ^ ( random<Address_Transmitted::Unsigned_Integer>( 0b01'0, 0b11'0 ) & 0b11'0 )
    };
}

} // namespace

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted::Address_Transmitted()
 *        works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const address_transmitted = Address_Transmitted{};

    EXPECT_EQ( address_transmitted.as_unsigned_integer(), 0b01000'00'0 );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted::Address_Transmitted(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted::Unsigned_Integer ) works
 *        properly.
 */
TEST( constructorUnsignedInteger, worksProperly )
{
    struct {
        Address_Transmitted::Unsigned_Integer address;
    } const test_cases[]{
        // clang-format off

        { 0b01000'00'0 },
        { 0b01000'01'0 },
        { 0b01000'10'0 },
        { 0b01000'11'0 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const address_numeric = Address_Transmitted{ test_case.address };

        EXPECT_EQ( address_numeric.as_unsigned_integer(), test_case.address );
    } // for
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted::Address_Transmitted(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted::Unsigned_Integer ) works
 *        properly.
 */
TEST( constructorBypassPreconditionExpectationChecksUnsignedInteger, worksProperly )
{
    struct {
        Address_Transmitted::Unsigned_Integer address;
    } const test_cases[]{
        // clang-format off

        { 0b01000'00'0 },
        { 0b01000'01'0 },
        { 0b01000'10'0 },
        { 0b01000'11'0 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const address_numeric = Address_Transmitted{ BYPASS_PRECONDITION_EXPECTATION_CHECKS,
                                                          test_case.address };

        EXPECT_EQ( address_numeric.as_unsigned_integer(), test_case.address );
    } // for
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted::Address_Transmitted(
 *        picolibrary::Microchip::MCP23S08::Address_Numeric ) works properly.
 */
TEST( constructorAddressNumeric, worksProperly )
{
    auto const address = random<Address_Numeric>();

    auto const address_transmitted = Address_Transmitted{ address };

    EXPECT_EQ( address_transmitted.as_unsigned_integer(), address.as_unsigned_integer() << 1 );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::operator==(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) works properly.
 */
TEST( equalityOperator, worksProperly )
{
    {
        auto const lhs = random_address();
        auto const rhs = lhs;

        EXPECT_TRUE( Address_Transmitted{ lhs } == Address_Transmitted{ rhs } );
    }

    {
        auto const [ lhs, rhs ] = random_unique_address_pair();

        EXPECT_FALSE( Address_Transmitted{ lhs } == Address_Transmitted{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::operator!=(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) works properly.
 */
TEST( inequalityOperator, worksProperly )
{
    {
        auto const lhs = random_address();
        auto const rhs = lhs;

        EXPECT_FALSE( Address_Transmitted{ lhs } != Address_Transmitted{ rhs } );
    }

    {
        auto const [ lhs, rhs ] = random_unique_address_pair();

        EXPECT_TRUE( Address_Transmitted{ lhs } != Address_Transmitted{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::operator<(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) works properly.
 */
TEST( lessThanOperator, worksProperly )
{
    {
        auto const rhs = random_address( 0b01000'00'0 + 2 );
        auto const lhs = random_address( 0b01000'00'0, rhs - 2 );

        EXPECT_TRUE( Address_Transmitted{ lhs } < Address_Transmitted{ rhs } );
    }

    {
        auto const rhs = random_address();
        auto const lhs = random_address( rhs );

        EXPECT_FALSE( Address_Transmitted{ lhs } < Address_Transmitted{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::operator>(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) works properly.
 */
TEST( greaterThanOperator, worksProperly )
{
    {
        auto const lhs = random_address( 0b01000'00'0 + 2 );
        auto const rhs = random_address( 0b01000'00'0, lhs - 2 );

        EXPECT_TRUE( Address_Transmitted{ lhs } > Address_Transmitted{ rhs } );
    }

    {
        auto const lhs = random_address();
        auto const rhs = random_address( lhs );

        EXPECT_FALSE( Address_Transmitted{ lhs } > Address_Transmitted{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::operator<=(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) works properly.
 */
TEST( lessThanOrEqualToOperator, worksProperly )
{
    {
        auto const lhs = random_address();
        auto const rhs = random_address( lhs );

        EXPECT_TRUE( Address_Transmitted{ lhs } <= Address_Transmitted{ rhs } );
    }

    {
        auto const lhs = random_address( 0b01000'00'0 + 2 );
        auto const rhs = random_address( 0b01000'00'0, lhs - 2 );

        EXPECT_FALSE( Address_Transmitted{ lhs } <= Address_Transmitted{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::operator>=(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) works properly.
 */
TEST( greaterThanOrEqualToOperator, worksProperly )
{
    {
        auto const rhs = random_address();
        auto const lhs = random_address( rhs );

        EXPECT_TRUE( Address_Transmitted{ lhs } >= Address_Transmitted{ rhs } );
    }

    {
        auto const rhs = random_address( 0b01000'00'0 + 2 );
        auto const lhs = random_address( 0b01000'00'0, rhs - 2 );

        EXPECT_FALSE( Address_Transmitted{ lhs } >= Address_Transmitted{ rhs } );
    }
}

/**
 * \brief Execute the picolibrary::Microchip::MCP23S08::Address_Transmitted unit tests.
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

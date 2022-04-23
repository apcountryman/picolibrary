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
 * \brief picolibrary::Microchip::MCP23S08::Address_Numeric unit test program.
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

auto random_address( Address_Numeric::Unsigned_Integer min = 0b01000'00, Address_Numeric::Unsigned_Integer max = 0b01000'11 )
    -> Address_Numeric::Unsigned_Integer
{
    return random<Address_Numeric::Unsigned_Integer>( min, max );
}

auto random_unique_address_pair()
    -> std::pair<Address_Numeric::Unsigned_Integer, Address_Numeric::Unsigned_Integer>
{
    auto const a = random_address();
    auto const b = random_address();

    return std::pair<Address_Numeric::Unsigned_Integer, Address_Numeric::Unsigned_Integer>{
        a, b != a ? b : b ^ random<Address_Numeric::Unsigned_Integer>( 0b01, 0b11 )
    };
}

} // namespace

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Address_Numeric::Address_Numeric()
 *        works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const address_numeric = Address_Numeric{};

    EXPECT_EQ( address_numeric.as_unsigned_integer(), 0b01000'00 );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Address_Numeric::Address_Numeric(
 *        picolibrary::Microchip::MCP23S08::Address_Numeric::Unsigned_Integer ) works
 *        properly.
 */
TEST( constructorUnsignedInteger, worksProperly )
{
    struct {
        Address_Numeric::Unsigned_Integer address;
    } const test_cases[]{
        // clang-format off

        { 0b01000'00 },
        { 0b01000'01 },
        { 0b01000'10 },
        { 0b01000'11 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const address_numeric = Address_Numeric{ test_case.address };

        EXPECT_EQ( address_numeric.as_unsigned_integer(), test_case.address );
    } // for
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Address_Numeric::Address_Numeric(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::Microchip::MCP23S08::Address_Numeric::Unsigned_Integer ) works
 *        properly.
 */
TEST( constructorBypassPreconditionExpectationChecksUnsignedInteger, worksProperly )
{
    struct {
        Address_Numeric::Unsigned_Integer address;
    } const test_cases[]{
        // clang-format off

        { 0b01000'00 },
        { 0b01000'01 },
        { 0b01000'10 },
        { 0b01000'11 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const address_numeric = Address_Numeric{ BYPASS_PRECONDITION_EXPECTATION_CHECKS,
                                                      test_case.address };

        EXPECT_EQ( address_numeric.as_unsigned_integer(), test_case.address );
    } // for
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Address_Numeric::Address_Numeric(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) works properly.
 */
TEST( constructorAddressTransmitted, worksProperly )
{
    auto const address = random<Address_Transmitted>();

    auto const address_numeric = Address_Numeric{ address };

    EXPECT_EQ( address_numeric.as_unsigned_integer(), address.as_unsigned_integer() >> 1 );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::operator==(
 *        picolibrary::Microchip::MCP23S08::Address_Numeric,
 *        picolibrary::Microchip::MCP23S08::Address_Numeric ) works properly.
 */
TEST( equalityOperator, worksProperly )
{
    {
        auto const lhs = random_address();
        auto const rhs = lhs;

        EXPECT_TRUE( Address_Numeric{ lhs } == Address_Numeric{ rhs } );
    }

    {
        auto const [ lhs, rhs ] = random_unique_address_pair();

        EXPECT_FALSE( Address_Numeric{ lhs } == Address_Numeric{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::operator!=(
 *        picolibrary::Microchip::MCP23S08::Address_Numeric,
 *        picolibrary::Microchip::MCP23S08::Address_Numeric ) works properly.
 */
TEST( inequalityOperator, worksProperly )
{
    {
        auto const lhs = random_address();
        auto const rhs = lhs;

        EXPECT_FALSE( Address_Numeric{ lhs } != Address_Numeric{ rhs } );
    }

    {
        auto const [ lhs, rhs ] = random_unique_address_pair();

        EXPECT_TRUE( Address_Numeric{ lhs } != Address_Numeric{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::operator<(
 *        picolibrary::Microchip::MCP23S08::Address_Numeric,
 *        picolibrary::Microchip::MCP23S08::Address_Numeric ) works properly.
 */
TEST( lessThanOperator, worksProperly )
{
    {
        auto const rhs = random_address( 0b01000'00 + 1 );
        auto const lhs = random_address( 0b01000'00, rhs - 1 );

        EXPECT_TRUE( Address_Numeric{ lhs } < Address_Numeric{ rhs } );
    }

    {
        auto const rhs = random_address();
        auto const lhs = random_address( rhs );

        EXPECT_FALSE( Address_Numeric{ lhs } < Address_Numeric{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::operator>(
 *        picolibrary::Microchip::MCP23S08::Address_Numeric,
 *        picolibrary::Microchip::MCP23S08::Address_Numeric ) works properly.
 */
TEST( greaterThanOperator, worksProperly )
{
    {
        auto const lhs = random_address( 0b01000'00 + 1 );
        auto const rhs = random_address( 0b01000'00, lhs - 1 );

        EXPECT_TRUE( Address_Numeric{ lhs } > Address_Numeric{ rhs } );
    }

    {
        auto const lhs = random_address();
        auto const rhs = random_address( lhs );

        EXPECT_FALSE( Address_Numeric{ lhs } > Address_Numeric{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::operator<=(
 *        picolibrary::Microchip::MCP23S08::Address_Numeric,
 *        picolibrary::Microchip::MCP23S08::Address_Numeric ) works properly.
 */
TEST( lessThanOrEqualToOperator, worksProperly )
{
    {
        auto const lhs = random_address();
        auto const rhs = random_address( lhs );

        EXPECT_TRUE( Address_Numeric{ lhs } <= Address_Numeric{ rhs } );
    }

    {
        auto const lhs = random_address( 0b01000'00 + 1 );
        auto const rhs = random_address( 0b01000'00, lhs - 1 );

        EXPECT_FALSE( Address_Numeric{ lhs } <= Address_Numeric{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::operator>=(
 *        picolibrary::Microchip::MCP23S08::Address_Numeric,
 *        picolibrary::Microchip::MCP23S08::Address_Numeric ) works properly.
 */
TEST( greaterThanOrEqualToOperator, worksProperly )
{
    {
        auto const rhs = random_address();
        auto const lhs = random_address( rhs );

        EXPECT_TRUE( Address_Numeric{ lhs } >= Address_Numeric{ rhs } );
    }

    {
        auto const rhs = random_address( 0b01000'00 + 1 );
        auto const lhs = random_address( 0b01000'00, rhs - 1 );

        EXPECT_FALSE( Address_Numeric{ lhs } >= Address_Numeric{ rhs } );
    }
}

/**
 * \brief Execute the picolibrary::Microchip::MCP23S08::Address_Numeric unit tests.
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

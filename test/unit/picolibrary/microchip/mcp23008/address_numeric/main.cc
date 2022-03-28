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
 * \brief picolibrary::Microchip::MCP23008::Address_Numeric unit test program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/microchip/mcp23008.h"
#include "picolibrary/precondition.h"
#include "picolibrary/testing/unit/microchip/mcp23008.h"
#include "picolibrary/testing/unit/random.h"

namespace {

using ::picolibrary::BYPASS_PRECONDITION_EXPECTATION_CHECKS;
using ::picolibrary::Microchip::MCP23008::Address_Numeric;
using ::picolibrary::Microchip::MCP23008::Address_Transmitted;
using ::picolibrary::Testing::Unit::random;

} // namespace

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Address_Numeric::Address_Numeric()
 *        works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const address_numeric = Address_Numeric{};

    EXPECT_EQ( address_numeric.as_unsigned_integer(), 0b0100'000 );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Address_Numeric::Address_Numeric(
 *        picolibrary::Microchip::MCP23008::Address_Numeric::Unsigned_Integer ) works
 *        properly.
 */
TEST( constructorUnsignedInteger, worksProperly )
{
    struct {
        Address_Numeric::Unsigned_Integer address;
    } const test_cases[]{
        // clang-format off

        { 0b0100'000 },
        { 0b0100'001 },
        { 0b0100'010 },
        { 0b0100'011 },
        { 0b0100'100 },
        { 0b0100'101 },
        { 0b0100'110 },
        { 0b0100'111 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const address_numeric = Address_Numeric{ test_case.address };

        EXPECT_EQ( address_numeric.as_unsigned_integer(), test_case.address );
    } // for
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Address_Numeric::Address_Numeric(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::Microchip::MCP23008::Address_Numeric::Unsigned_Integer ) works
 *        properly.
 */
TEST( constructorBypassPreconditionExpectationChecksUnsignedInteger, worksProperly )
{
    struct {
        Address_Numeric::Unsigned_Integer address;
    } const test_cases[]{
        // clang-format off

        { 0b0100'000 },
        { 0b0100'001 },
        { 0b0100'010 },
        { 0b0100'011 },
        { 0b0100'100 },
        { 0b0100'101 },
        { 0b0100'110 },
        { 0b0100'111 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const address_numeric = Address_Numeric{ BYPASS_PRECONDITION_EXPECTATION_CHECKS,
                                                      test_case.address };

        EXPECT_EQ( address_numeric.as_unsigned_integer(), test_case.address );
    } // for
}

/**
 * \brief Verify picolibrary::Microchip::MCP23008::Address_Numeric::Address_Numeric(
 *        picolibrary::Microchip::MCP23008::Address_Transmitted ) works properly.
 */
TEST( constructorAddressTransmitted, worksProperly )
{
    auto const address = random<Address_Transmitted>();

    auto const address_numeric = Address_Numeric{ address };

    EXPECT_EQ( address_numeric.as_unsigned_integer(), address.as_unsigned_integer() >> 1 );
}

/**
 * \brief Execute the picolibrary::Microchip::MCP23008::Address_Numeric unit tests.
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

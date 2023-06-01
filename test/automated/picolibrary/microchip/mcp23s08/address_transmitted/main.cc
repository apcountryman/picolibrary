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
 * \brief picolibrary::Microchip::MCP23S08::Address_Transmitted automated test program.
 */

#include <ios>
#include <ostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/microchip/mcp23s08.h"
#include "picolibrary/precondition.h"
#include "picolibrary/testing/automated/microchip/mcp23s08.h"

namespace {

using ::picolibrary::BYPASS_PRECONDITION_EXPECTATION_CHECKS;
using ::picolibrary::Microchip::MCP23S08::Address_Numeric;
using ::picolibrary::Microchip::MCP23S08::Address_Transmitted;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

} // namespace

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted::Address_Transmitted()
 *        works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const address = Address_Transmitted{};

    ASSERT_EQ( address.as_unsigned_integer(), 0b01000'00'0 );
}

/**
 * \brief picolibrary::Microchip::MCP23S08::Address_Transmitted::Address_Transmitted(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted::Unsigned_Integer ) and
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted::Address_Transmitted(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted::Unsigned_Integer ) test
 *        cases.
 */
Address_Transmitted::Unsigned_Integer const constructorUnsignedInteger_TEST_CASES[]{
    // clang-format off

    0b01000'00'0,
    0b01000'01'0,
    0b01000'10'0,
    0b01000'11'0,

    // clang-format on
};

/**
 * \brief picolibrary::Microchip::MCP23S08::Address_Transmitted::Address_Transmitted(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted::Unsigned_Integer ) test
 *        fixture.
 */
class constructorUnsignedInteger : public TestWithParam<Address_Transmitted::Unsigned_Integer> {
};

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted::Address_Transmitted(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted::Unsigned_Integer ) works
 *        properly.
 */
TEST_P( constructorUnsignedInteger, worksProperly )
{
    auto const address = GetParam();

    auto const address_transmitted = Address_Transmitted{ address };

    ASSERT_EQ( address_transmitted.as_unsigned_integer(), address );
}

INSTANTIATE_TEST_SUITE_P( testCases, constructorUnsignedInteger, ValuesIn( constructorUnsignedInteger_TEST_CASES ) );

/**
 * \brief picolibrary::Microchip::MCP23S08::Address_Transmitted::Address_Transmitted(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted::Unsigned_Integer ) test
 *        fixture.
 */
class constructorBypassPreconditionExpectationChecksUnsignedInteger :
    public TestWithParam<Address_Transmitted::Unsigned_Integer> {
};

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted::Address_Transmitted(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted::Unsigned_Integer ) works
 *        properly.
 */
TEST_P( constructorBypassPreconditionExpectationChecksUnsignedInteger, worksProperly )
{
    auto const address = GetParam();

    auto const address_transmitted = Address_Transmitted{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, address };

    ASSERT_EQ( address_transmitted.as_unsigned_integer(), address );
}

INSTANTIATE_TEST_SUITE_P(
    testCases,
    constructorBypassPreconditionExpectationChecksUnsignedInteger,
    ValuesIn( constructorUnsignedInteger_TEST_CASES ) );

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Address_Transmitted::Address_Transmitted(
 *        picolibrary::Microchip::MCP23S08::Address_Numeric ) works properly.
 */
TEST( constructorAddressNumeric, worksProperly )
{
    auto const address_transmitted = Address_Transmitted{ Address_Numeric{ 0b01000'01 } };

    ASSERT_EQ( address_transmitted.as_unsigned_integer(), 0b01000'01'0 );
}

/**
 * \brief picolibrary::Microchip::MCP23S08::operator==(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ),
 *        picolibrary::Microchip::MCP23S08::operator!=(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ),
 *        picolibrary::Microchip::MCP23S08::operator<(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ),
 *        picolibrary::Microchip::MCP23S08::operator>(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ),
 *        picolibrary::Microchip::MCP23S08::operator<=(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ), and
 *        picolibrary::Microchip::MCP23S08::operator>=(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) test case.
 */
struct comparisonOperator_Test_Case {
    /**
     * \brief The left hand side of the comparison.
     */
    Address_Transmitted lhs;

    /**
     * \brief The right hand side of the comparison.
     */
    Address_Transmitted rhs;

    /**
     * \brief The result of the comparison.
     */
    bool comparison_result;
};

auto operator<<( std::ostream & stream, comparisonOperator_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".lhs = " << test_case.lhs
                  << ", "
                  << ".rhs = " << test_case.rhs
                  << ", "
                  << ".comparison_result = " << std::boolalpha << test_case.comparison_result
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Microchip::MCP23S08::operator==(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) test fixture.
 */
class equalityOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::operator==(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) works properly.
 */
TEST_P( equalityOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs == test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::Microchip::MCP23S08::operator==(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) test cases.
 */
comparisonOperator_Test_Case const equalityOperator_TEST_CASES[]{
    // clang-format off

    { 0b01000'00'0, 0b01000'00'0, true  },
    { 0b01000'00'0, 0b01000'01'0, false },
    { 0b01000'00'0, 0b01000'10'0, false },
    { 0b01000'00'0, 0b01000'11'0, false },

    { 0b01000'01'0, 0b01000'00'0, false },
    { 0b01000'01'0, 0b01000'01'0, true  },
    { 0b01000'01'0, 0b01000'10'0, false },
    { 0b01000'01'0, 0b01000'11'0, false },

    { 0b01000'10'0, 0b01000'00'0, false },
    { 0b01000'10'0, 0b01000'01'0, false },
    { 0b01000'10'0, 0b01000'10'0, true  },
    { 0b01000'10'0, 0b01000'11'0, false },

    { 0b01000'11'0, 0b01000'00'0, false },
    { 0b01000'11'0, 0b01000'01'0, false },
    { 0b01000'11'0, 0b01000'10'0, false },
    { 0b01000'11'0, 0b01000'11'0, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, equalityOperator, ValuesIn( equalityOperator_TEST_CASES ) );

/**
 * \brief picolibrary::Microchip::MCP23S08::operator!=(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) test fixture.
 */
class inequalityOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::operator!=(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) works properly.
 */
TEST_P( inequalityOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs != test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::Microchip::MCP23S08::operator!=(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) test cases.
 */
comparisonOperator_Test_Case const inequalityOperator_TEST_CASES[]{
    // clang-format off

    { 0b01000'00'0, 0b01000'00'0, false },
    { 0b01000'00'0, 0b01000'01'0, true  },
    { 0b01000'00'0, 0b01000'10'0, true  },
    { 0b01000'00'0, 0b01000'11'0, true  },

    { 0b01000'01'0, 0b01000'00'0, true  },
    { 0b01000'01'0, 0b01000'01'0, false },
    { 0b01000'01'0, 0b01000'10'0, true  },
    { 0b01000'01'0, 0b01000'11'0, true  },

    { 0b01000'10'0, 0b01000'00'0, true  },
    { 0b01000'10'0, 0b01000'01'0, true  },
    { 0b01000'10'0, 0b01000'10'0, false },
    { 0b01000'10'0, 0b01000'11'0, true  },

    { 0b01000'11'0, 0b01000'00'0, true  },
    { 0b01000'11'0, 0b01000'01'0, true  },
    { 0b01000'11'0, 0b01000'10'0, true  },
    { 0b01000'11'0, 0b01000'11'0, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, inequalityOperator, ValuesIn( inequalityOperator_TEST_CASES ) );

/**
 * \brief picolibrary::Microchip::MCP23S08::operator<(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) test fixture.
 */
class lessThanOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::operator<(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) works properly.
 */
TEST_P( lessThanOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs < test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::Microchip::MCP23S08::operator<(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) test cases.
 */
comparisonOperator_Test_Case const lessThanOperator_TEST_CASES[]{
    // clang-format off

    { 0b01000'00'0, 0b01000'00'0, false },
    { 0b01000'00'0, 0b01000'01'0, true  },
    { 0b01000'00'0, 0b01000'10'0, true  },
    { 0b01000'00'0, 0b01000'11'0, true  },

    { 0b01000'01'0, 0b01000'00'0, false },
    { 0b01000'01'0, 0b01000'01'0, false },
    { 0b01000'01'0, 0b01000'10'0, true  },
    { 0b01000'01'0, 0b01000'11'0, true  },

    { 0b01000'10'0, 0b01000'00'0, false },
    { 0b01000'10'0, 0b01000'01'0, false },
    { 0b01000'10'0, 0b01000'10'0, false },
    { 0b01000'10'0, 0b01000'11'0, true  },

    { 0b01000'11'0, 0b01000'00'0, false },
    { 0b01000'11'0, 0b01000'01'0, false },
    { 0b01000'11'0, 0b01000'10'0, false },
    { 0b01000'11'0, 0b01000'11'0, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, lessThanOperator, ValuesIn( lessThanOperator_TEST_CASES ) );

/**
 * \brief picolibrary::Microchip::MCP23S08::operator>(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) test fixture.
 */
class greaterThanOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::operator>(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) works properly.
 */
TEST_P( greaterThanOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs > test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::Microchip::MCP23S08::operator>(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) test cases.
 */
comparisonOperator_Test_Case const greaterThanOperator_TEST_CASES[]{
    // clang-format off

    { 0b01000'00'0, 0b01000'00'0, false },
    { 0b01000'00'0, 0b01000'01'0, false },
    { 0b01000'00'0, 0b01000'10'0, false },
    { 0b01000'00'0, 0b01000'11'0, false },

    { 0b01000'01'0, 0b01000'00'0, true  },
    { 0b01000'01'0, 0b01000'01'0, false },
    { 0b01000'01'0, 0b01000'10'0, false },
    { 0b01000'01'0, 0b01000'11'0, false },

    { 0b01000'10'0, 0b01000'00'0, true  },
    { 0b01000'10'0, 0b01000'01'0, true  },
    { 0b01000'10'0, 0b01000'10'0, false },
    { 0b01000'10'0, 0b01000'11'0, false },

    { 0b01000'11'0, 0b01000'00'0, true  },
    { 0b01000'11'0, 0b01000'01'0, true  },
    { 0b01000'11'0, 0b01000'10'0, true  },
    { 0b01000'11'0, 0b01000'11'0, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, greaterThanOperator, ValuesIn( greaterThanOperator_TEST_CASES ) );

/**
 * \brief picolibrary::Microchip::MCP23S08::operator<=(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) test fixture.
 */
class lessThanOrEqualToOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::operator<=(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) works properly.
 */
TEST_P( lessThanOrEqualToOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs <= test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::Microchip::MCP23S08::operator<=(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) test cases.
 */
comparisonOperator_Test_Case const lessThanOrEqualToOperator_TEST_CASES[]{
    // clang-format off

    { 0b01000'00'0, 0b01000'00'0, true },
    { 0b01000'00'0, 0b01000'01'0, true },
    { 0b01000'00'0, 0b01000'10'0, true },
    { 0b01000'00'0, 0b01000'11'0, true },

    { 0b01000'01'0, 0b01000'00'0, false },
    { 0b01000'01'0, 0b01000'01'0, true  },
    { 0b01000'01'0, 0b01000'10'0, true  },
    { 0b01000'01'0, 0b01000'11'0, true  },

    { 0b01000'10'0, 0b01000'00'0, false },
    { 0b01000'10'0, 0b01000'01'0, false },
    { 0b01000'10'0, 0b01000'10'0, true  },
    { 0b01000'10'0, 0b01000'11'0, true  },

    { 0b01000'11'0, 0b01000'00'0, false },
    { 0b01000'11'0, 0b01000'01'0, false },
    { 0b01000'11'0, 0b01000'10'0, false },
    { 0b01000'11'0, 0b01000'11'0, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, lessThanOrEqualToOperator, ValuesIn( lessThanOrEqualToOperator_TEST_CASES ) );

/**
 * \brief picolibrary::Microchip::MCP23S08::operator>=(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) test fixture.
 */
class greaterThanOrEqualToOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::operator>=(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) works properly.
 */
TEST_P( greaterThanOrEqualToOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs >= test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::Microchip::MCP23S08::operator>=(
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) test cases.
 */
comparisonOperator_Test_Case const greaterThanOrEqualToOperator_TEST_CASES[]{
    // clang-format off

    { 0b01000'00'0, 0b01000'00'0, true  },
    { 0b01000'00'0, 0b01000'01'0, false },
    { 0b01000'00'0, 0b01000'10'0, false },
    { 0b01000'00'0, 0b01000'11'0, false },

    { 0b01000'01'0, 0b01000'00'0, true  },
    { 0b01000'01'0, 0b01000'01'0, true  },
    { 0b01000'01'0, 0b01000'10'0, false },
    { 0b01000'01'0, 0b01000'11'0, false },

    { 0b01000'10'0, 0b01000'00'0, true  },
    { 0b01000'10'0, 0b01000'01'0, true  },
    { 0b01000'10'0, 0b01000'10'0, true  },
    { 0b01000'10'0, 0b01000'11'0, false },

    { 0b01000'11'0, 0b01000'00'0, true },
    { 0b01000'11'0, 0b01000'01'0, true },
    { 0b01000'11'0, 0b01000'10'0, true },
    { 0b01000'11'0, 0b01000'11'0, true },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, greaterThanOrEqualToOperator, ValuesIn( greaterThanOrEqualToOperator_TEST_CASES ) );

/**
 * \brief Execute the picolibrary::Microchip::MCP23S08::Address_Transmitted automated
 *        tests.
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

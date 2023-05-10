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
 * \brief picolibrary::I2C::Address_Numeric automated test program.
 */

#include <cstdint>
#include <ios>
#include <ostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/i2c.h"
#include "picolibrary/precondition.h"

namespace {

using ::picolibrary::BYPASS_PRECONDITION_EXPECTATION_CHECKS;
using ::picolibrary::I2C::Address_Numeric;
using ::picolibrary::I2C::Address_Transmitted;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

} // namespace

/**
 * \brief Verify picolibrary::I2C::Address_Numeric::Address_Numeric() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const address_numeric = Address_Numeric{};

    ASSERT_EQ( address_numeric.as_unsigned_integer(), 0b0000000 );
}

/**
 * \brief picolibrary::I2C::Address_Numeric::Address_Numeric(
 *        picolibrary::I2C::Address_Numeric::Unsigned_Integer ) and
 *        picolibrary::I2C::Address_Numeric::Address_Numeric(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::I2C::Address_Numeric::Unsigned_Integer ) test cases.
 */
Address_Numeric::Unsigned_Integer const constructorUnsignedInteger_TEST_CASES[]{
    // clang-format off

    0b0000000,
    0b1111011,
    0b1111111,

    // clang-format on
};

/**
 * \brief picolibrary::I2C::Address_Numeric::Address_Numeric(
 *        picolibrary::I2C::Address_Numeric::Unsigned_Integer ) test fixture.
 */
class constructorUnsignedInteger : public ::testing::TestWithParam<Address_Numeric::Unsigned_Integer> {
};

INSTANTIATE_TEST_SUITE_P( testCases, constructorUnsignedInteger, ValuesIn( constructorUnsignedInteger_TEST_CASES ) );

/**
 * \brief Verify picolibrary::I2C::Address_Numeric::Address_Numeric(
 *        picolibrary::I2C::Address_Numeric::Unsigned_Integer ) works properly.
 */
TEST_P( constructorUnsignedInteger, worksProperly )
{
    auto const address = GetParam();

    auto const address_numeric = Address_Numeric{ address };

    ASSERT_EQ( address_numeric.as_unsigned_integer(), address );
}

/**
 * \brief picolibrary::I2C::Address_Numeric::Address_Numeric(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::I2C::Address_Numeric::Unsigned_Integer ) test fixture.
 */
class constructorBypassPreconditionExpectationChecksUnsignedInteger :
    public ::testing::TestWithParam<Address_Numeric::Unsigned_Integer> {
};

INSTANTIATE_TEST_SUITE_P(
    testCases,
    constructorBypassPreconditionExpectationChecksUnsignedInteger,
    ValuesIn( constructorUnsignedInteger_TEST_CASES ) );

/**
 * \brief Verify picolibrary::I2C::Address_Numeric::Address_Numeric(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::I2C::Address_Numeric::Unsigned_Integer ) works properly.
 */
TEST_P( constructorBypassPreconditionExpectationChecksUnsignedInteger, worksProperly )
{
    auto const address = GetParam();

    auto const address_numeric = Address_Numeric{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, address };

    ASSERT_EQ( address_numeric.as_unsigned_integer(), address );
}

/**
 * \brief Verify picolibrary::I2C::Address_Numeric::Address_Numeric(
 *        picolibrary::I2C::Address_Transmitted ) works properly.
 */
TEST( constructorAddressTransmitted, worksProperly )
{
    auto const address_numeric = Address_Numeric{ Address_Transmitted{ 0b1011100'0 } };

    ASSERT_EQ( address_numeric.as_unsigned_integer(), 0b1011100 );
}

/**
 * \brief picolibrary::I2C::operator==( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ), picolibrary::I2C::operator!=(
 *        picolibrary::I2C::Address_Numeric, picolibrary::I2C::Address_Numeric ),
 *        picolibrary::I2C::operator<( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ), picolibrary::I2C::operator>(
 *        picolibrary::I2C::Address_Numeric, picolibrary::I2C::Address_Numeric ),
 *        picolibrary::I2C::operator<=( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ), and picolibrary::I2C::operator>=(
 *        picolibrary::I2C::Address_Numeric, picolibrary::I2C::Address_Numeric ) test
 *        case.
 */
struct comparisonOperator_Test_Case {
    /**
     * \brief The left hand side of the comparison.
     */
    Address_Numeric lhs;

    /**
     * \brief The right hand side of the comparison.
     */
    Address_Numeric rhs;

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
                  << ".lhs = " << static_cast<std::uint_fast16_t>( test_case.lhs.as_unsigned_integer() )
                  << ", "
                  << ".rhs = " << static_cast<std::uint_fast16_t>( test_case.lhs.as_unsigned_integer() )
                  << ", "
                  << ".comparison_result = " << std::boolalpha << test_case.comparison_result
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::I2C::operator==( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) test cases.
 */
comparisonOperator_Test_Case const equalityOperator_TEST_CASES[]{
    // clang-format off

    { 33, 40, false },
    { 67, 68, false },
    { 68, 68, true  },
    { 69, 68, false },
    { 72, 79, false },

    // clang-format on
};

/**
 * \brief picolibrary::I2C::operator==( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) test fixture.
 */
class equalityOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

INSTANTIATE_TEST_SUITE_P( testCases, equalityOperator, ValuesIn( equalityOperator_TEST_CASES ) );

/**
 * \brief Verify picolibrary::I2C::operator==( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) works properly.
 */
TEST_P( equalityOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs == test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::I2C::operator!=( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) test cases.
 */
comparisonOperator_Test_Case const inequalityOperator_TEST_CASES[]{
    // clang-format off

    { 33, 40, true  },
    { 67, 68, true  },
    { 68, 68, false },
    { 69, 68, true  },
    { 72, 79, true  },

    // clang-format on
};

/**
 * \brief picolibrary::I2C::operator!=( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) test fixture.
 */
class inequalityOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

INSTANTIATE_TEST_SUITE_P( testCases, inequalityOperator, ValuesIn( inequalityOperator_TEST_CASES ) );

/**
 * \brief Verify picolibrary::I2C::operator!=( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) works properly.
 */
TEST_P( inequalityOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs != test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::I2C::operator<( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) test cases.
 */
comparisonOperator_Test_Case const lessThanOperator_TEST_CASES[]{
    // clang-format off

    { 33, 68, true  },
    { 67, 68, true  },
    { 68, 68, false },
    { 69, 68, false },
    { 72, 68, false },

    // clang-format on
};

/**
 * \brief picolibrary::I2C::operator<( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) test fixture.
 */
class lessThanOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

INSTANTIATE_TEST_SUITE_P( testCases, lessThanOperator, ValuesIn( lessThanOperator_TEST_CASES ) );

/**
 * \brief Verify picolibrary::I2C::operator<( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) works properly.
 */
TEST_P( lessThanOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs < test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::I2C::operator>( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) test cases.
 */
comparisonOperator_Test_Case const greaterThanOperator_TEST_CASES[]{
    // clang-format off

    { 33, 68, false },
    { 67, 68, false },
    { 68, 68, false },
    { 69, 68, true  },
    { 72, 68, true  },

    // clang-format on
};

/**
 * \brief picolibrary::I2C::operator>( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) test fixture.
 */
class greaterThanOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

INSTANTIATE_TEST_SUITE_P( testCases, greaterThanOperator, ValuesIn( greaterThanOperator_TEST_CASES ) );

/**
 * \brief Verify picolibrary::I2C::operator>( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) works properly.
 */
TEST_P( greaterThanOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs > test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::I2C::operator<=( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) test cases.
 */
comparisonOperator_Test_Case const lessThanOrEqualToOperator_TEST_CASES[]{
    // clang-format off

    { 33, 68, true  },
    { 67, 68, true  },
    { 68, 68, true  },
    { 69, 68, false },
    { 72, 68, false },

    // clang-format on
};

/**
 * \brief picolibrary::I2C::operator<=( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) test fixture.
 */
class lessThanOrEqualToOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

INSTANTIATE_TEST_SUITE_P( testCases, lessThanOrEqualToOperator, ValuesIn( lessThanOrEqualToOperator_TEST_CASES ) );

/**
 * \brief Verify picolibrary::I2C::operator<=( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) works properly.
 */
TEST_P( lessThanOrEqualToOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs <= test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::I2C::operator>=( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) test cases.
 */
comparisonOperator_Test_Case const greaterThanOrEqualToOperator_TEST_CASES[]{
    // clang-format off

    { 33, 68, false },
    { 67, 68, false },
    { 68, 68, true  },
    { 69, 68, true  },
    { 72, 68, true  },

    // clang-format on
};

/**
 * \brief picolibrary::I2C::operator>=( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) test fixture.
 */
class greaterThanOrEqualToOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

INSTANTIATE_TEST_SUITE_P( testCases, greaterThanOrEqualToOperator, ValuesIn( greaterThanOrEqualToOperator_TEST_CASES ) );

/**
 * \brief Verify picolibrary::I2C::operator>=( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) works properly.
 */
TEST_P( greaterThanOrEqualToOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs >= test_case.rhs, test_case.comparison_result );
}

/**
 * \brief Execute the picolibrary::I2C::Address_Numeric automated tests.
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

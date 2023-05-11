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
 * \brief picolibrary::ADC::Sample automated test program.
 */

#include <cstdint>
#include <ios>
#include <ostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/adc.h"
#include "picolibrary/precondition.h"

namespace {

using ::picolibrary::BYPASS_PRECONDITION_EXPECTATION_CHECKS;
using ::picolibrary::ADC::Sample;
using ::testing::TestWithParam;
using ::testing::Types;
using ::testing::ValuesIn;

} // namespace

/**
 * \brief Verify picolibrary::ADC::Sample::Sample() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const sample = Sample<std::uint_fast16_t, 10>{};

    ASSERT_EQ( sample.as_unsigned_integer(), 0 );
}

/**
 * \brief picolibrary::ADC::Sample::Sample( picolibrary::ADC::Sample::Unsigned_Integer )
 *        and picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) std::uint_fast8_t, 8 test cases.
 */
std::uint_fast8_t const constructorUnsignedInteger8_TEST_CASES[]{
    0,
    255,
    51,
};

/**
 * \brief picolibrary::ADC::Sample::Sample( picolibrary::ADC::Sample::Unsigned_Integer )
 *        std::uint_fast8_t, 8 test fixture.
 */
class constructorUnsignedInteger8 : public TestWithParam<std::uint_fast8_t> {
};

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly.
 */
TEST_P( constructorUnsignedInteger8, worksProperly )
{
    auto const unsigned_integer = GetParam();

    auto const sample = Sample<std::uint_fast8_t, 8>{ unsigned_integer };

    ASSERT_EQ( sample.as_unsigned_integer(), unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P( testCases, constructorUnsignedInteger8, ValuesIn( constructorUnsignedInteger8_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) std::uint_fast8_t, 8 test fixture.
 */
class constructorBypassPreconditionExpectationChecksUnsignedInteger8 :
    public TestWithParam<std::uint_fast8_t> {
};

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly.
 */
TEST_P( constructorBypassPreconditionExpectationChecksUnsignedInteger8, worksProperly )
{
    auto const unsigned_integer = GetParam();

    auto const sample = Sample<std::uint_fast8_t, 8>{ BYPASS_PRECONDITION_EXPECTATION_CHECKS,
                                                      unsigned_integer };

    ASSERT_EQ( sample.as_unsigned_integer(), unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P(
    testCases,
    constructorBypassPreconditionExpectationChecksUnsignedInteger8,
    ValuesIn( constructorUnsignedInteger8_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::Sample::Sample( picolibrary::ADC::Sample::Unsigned_Integer )
 *        and picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) std::uint_fast16_t, 10 test cases.
 */
std::uint_fast16_t const constructorUnsignedInteger10_TEST_CASES[]{
    0,
    1023,
    167,
};

/**
 * \brief picolibrary::ADC::Sample::Sample( picolibrary::ADC::Sample::Unsigned_Integer )
 *        std::uint_fast16_t, 10 test fixture.
 */
class constructorUnsignedInteger10 : public TestWithParam<std::uint_fast16_t> {
};

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly.
 */
TEST_P( constructorUnsignedInteger10, worksProperly )
{
    auto const unsigned_integer = GetParam();

    auto const sample = Sample<std::uint_fast16_t, 10>{ unsigned_integer };

    ASSERT_EQ( sample.as_unsigned_integer(), unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P( testCases, constructorUnsignedInteger10, ValuesIn( constructorUnsignedInteger10_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) std::uint_fast16_t, 10 test
 *        fixture.
 */
class constructorBypassPreconditionExpectationChecksUnsignedInteger10 :
    public TestWithParam<std::uint_fast16_t> {
};

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly.
 */
TEST_P( constructorBypassPreconditionExpectationChecksUnsignedInteger10, worksProperly )
{
    auto const unsigned_integer = GetParam();

    auto const sample = Sample<std::uint_fast16_t, 10>{ BYPASS_PRECONDITION_EXPECTATION_CHECKS,
                                                        unsigned_integer };

    ASSERT_EQ( sample.as_unsigned_integer(), unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P(
    testCases,
    constructorBypassPreconditionExpectationChecksUnsignedInteger10,
    ValuesIn( constructorUnsignedInteger10_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::Sample::Sample( picolibrary::ADC::Sample::Unsigned_Integer )
 *        and picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) std::uint_fast16_t, 12 test cases.
 */
std::uint_fast16_t const constructorUnsignedInteger12_TEST_CASES[]{
    0,
    4095,
    3700,
};

/**
 * \brief picolibrary::ADC::Sample::Sample( picolibrary::ADC::Sample::Unsigned_Integer )
 *        std::uint_fast16_t, 12 test fixture.
 */
class constructorUnsignedInteger12 : public TestWithParam<std::uint_fast16_t> {
};

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly.
 */
TEST_P( constructorUnsignedInteger12, worksProperly )
{
    auto const unsigned_integer = GetParam();

    auto const sample = Sample<std::uint_fast16_t, 12>{ unsigned_integer };

    ASSERT_EQ( sample.as_unsigned_integer(), unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P( testCases, constructorUnsignedInteger12, ValuesIn( constructorUnsignedInteger12_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) std::uint_fast16_t, 12 test
 *        fixture.
 */
class constructorBypassPreconditionExpectationChecksUnsignedInteger12 :
    public TestWithParam<std::uint_fast16_t> {
};

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly.
 */
TEST_P( constructorBypassPreconditionExpectationChecksUnsignedInteger12, worksProperly )
{
    auto const unsigned_integer = GetParam();

    auto const sample = Sample<std::uint_fast16_t, 12>{ BYPASS_PRECONDITION_EXPECTATION_CHECKS,
                                                        unsigned_integer };

    ASSERT_EQ( sample.as_unsigned_integer(), unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P(
    testCases,
    constructorBypassPreconditionExpectationChecksUnsignedInteger12,
    ValuesIn( constructorUnsignedInteger12_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::Sample::Sample( picolibrary::ADC::Sample::Unsigned_Integer )
 *        and picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) std::uint_fast16_t, 14 test cases.
 */
std::uint_fast16_t const constructorUnsignedInteger14_TEST_CASES[]{
    0,
    16383,
    8533,
};

/**
 * \brief picolibrary::ADC::Sample::Sample( picolibrary::ADC::Sample::Unsigned_Integer )
 *        std::uint_fast16_t, 14 test fixture.
 */
class constructorUnsignedInteger14 : public TestWithParam<std::uint_fast16_t> {
};

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly.
 */
TEST_P( constructorUnsignedInteger14, worksProperly )
{
    auto const unsigned_integer = GetParam();

    auto const sample = Sample<std::uint_fast16_t, 14>{ unsigned_integer };

    ASSERT_EQ( sample.as_unsigned_integer(), unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P( testCases, constructorUnsignedInteger14, ValuesIn( constructorUnsignedInteger14_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) std::uint_fast16_t, 14 test
 *        fixture.
 */
class constructorBypassPreconditionExpectationChecksUnsignedInteger14 :
    public TestWithParam<std::uint_fast16_t> {
};

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly.
 */
TEST_P( constructorBypassPreconditionExpectationChecksUnsignedInteger14, worksProperly )
{
    auto const unsigned_integer = GetParam();

    auto const sample = Sample<std::uint_fast16_t, 14>{ BYPASS_PRECONDITION_EXPECTATION_CHECKS,
                                                        unsigned_integer };

    ASSERT_EQ( sample.as_unsigned_integer(), unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P(
    testCases,
    constructorBypassPreconditionExpectationChecksUnsignedInteger14,
    ValuesIn( constructorUnsignedInteger14_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::Sample::Sample( picolibrary::ADC::Sample::Unsigned_Integer )
 *        and picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) std::uint_fast16_t, 16 test cases.
 */
std::uint_fast16_t const constructorUnsignedInteger16_TEST_CASES[]{
    0,
    65535,
    52593,
};

/**
 * \brief picolibrary::ADC::Sample::Sample( picolibrary::ADC::Sample::Unsigned_Integer )
 *        std::uint_fast16_t, 16 test fixture.
 */
class constructorUnsignedInteger16 : public TestWithParam<std::uint_fast16_t> {
};

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly.
 */
TEST_P( constructorUnsignedInteger16, worksProperly )
{
    auto const unsigned_integer = GetParam();

    auto const sample = Sample<std::uint_fast16_t, 16>{ unsigned_integer };

    ASSERT_EQ( sample.as_unsigned_integer(), unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P( testCases, constructorUnsignedInteger16, ValuesIn( constructorUnsignedInteger16_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) std::uint_fast16_t, 16 test
 *        fixture.
 */
class constructorBypassPreconditionExpectationChecksUnsignedInteger16 :
    public TestWithParam<std::uint_fast16_t> {
};

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly.
 */
TEST_P( constructorBypassPreconditionExpectationChecksUnsignedInteger16, worksProperly )
{
    auto const unsigned_integer = GetParam();

    auto const sample = Sample<std::uint_fast16_t, 16>{ BYPASS_PRECONDITION_EXPECTATION_CHECKS,
                                                        unsigned_integer };

    ASSERT_EQ( sample.as_unsigned_integer(), unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P(
    testCases,
    constructorBypassPreconditionExpectationChecksUnsignedInteger16,
    ValuesIn( constructorUnsignedInteger16_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::Sample::Sample( picolibrary::ADC::Sample::Unsigned_Integer )
 *        and picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) std::uint_fast32_t, 18 test cases.
 */
std::uint_fast32_t const constructorUnsignedInteger18_TEST_CASES[]{
    0,
    262143,
    102396,
};

/**
 * \brief picolibrary::ADC::Sample::Sample( picolibrary::ADC::Sample::Unsigned_Integer )
 *        std::uint_fast32_t, 18 test fixture.
 */
class constructorUnsignedInteger18 : public TestWithParam<std::uint_fast32_t> {
};

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly.
 */
TEST_P( constructorUnsignedInteger18, worksProperly )
{
    auto const unsigned_integer = GetParam();

    auto const sample = Sample<std::uint_fast32_t, 18>{ unsigned_integer };

    ASSERT_EQ( sample.as_unsigned_integer(), unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P( testCases, constructorUnsignedInteger18, ValuesIn( constructorUnsignedInteger18_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) std::uint_fast32_t, 18 test
 *        fixture.
 */
class constructorBypassPreconditionExpectationChecksUnsignedInteger18 :
    public TestWithParam<std::uint_fast32_t> {
};

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly.
 */
TEST_P( constructorBypassPreconditionExpectationChecksUnsignedInteger18, worksProperly )
{
    auto const unsigned_integer = GetParam();

    auto const sample = Sample<std::uint_fast32_t, 18>{ BYPASS_PRECONDITION_EXPECTATION_CHECKS,
                                                        unsigned_integer };

    ASSERT_EQ( sample.as_unsigned_integer(), unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P(
    testCases,
    constructorBypassPreconditionExpectationChecksUnsignedInteger18,
    ValuesIn( constructorUnsignedInteger18_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::Sample::Sample( picolibrary::ADC::Sample::Unsigned_Integer )
 *        and picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) std::uint_fast32_t, 20 test cases.
 */
std::uint_fast32_t const constructorUnsignedInteger20_TEST_CASES[]{
    0,
    1048575,
    407638,
};

/**
 * \brief picolibrary::ADC::Sample::Sample( picolibrary::ADC::Sample::Unsigned_Integer )
 *        std::uint_fast32_t, 20 test fixture.
 */
class constructorUnsignedInteger20 : public TestWithParam<std::uint_fast32_t> {
};

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly.
 */
TEST_P( constructorUnsignedInteger20, worksProperly )
{
    auto const unsigned_integer = GetParam();

    auto const sample = Sample<std::uint_fast32_t, 20>{ unsigned_integer };

    ASSERT_EQ( sample.as_unsigned_integer(), unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P( testCases, constructorUnsignedInteger20, ValuesIn( constructorUnsignedInteger20_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) std::uint_fast32_t, 20 test
 *        fixture.
 */
class constructorBypassPreconditionExpectationChecksUnsignedInteger20 :
    public TestWithParam<std::uint_fast32_t> {
};

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly.
 */
TEST_P( constructorBypassPreconditionExpectationChecksUnsignedInteger20, worksProperly )
{
    auto const unsigned_integer = GetParam();

    auto const sample = Sample<std::uint_fast32_t, 20>{ BYPASS_PRECONDITION_EXPECTATION_CHECKS,
                                                        unsigned_integer };

    ASSERT_EQ( sample.as_unsigned_integer(), unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P(
    testCases,
    constructorBypassPreconditionExpectationChecksUnsignedInteger20,
    ValuesIn( constructorUnsignedInteger20_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::Sample::Sample( picolibrary::ADC::Sample::Unsigned_Integer )
 *        and picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) std::uint_fast32_t, 24 test cases.
 */
std::uint_fast32_t const constructorUnsignedInteger24_TEST_CASES[]{
    0,
    16777215,
    12395235,
};

/**
 * \brief picolibrary::ADC::Sample::Sample( picolibrary::ADC::Sample::Unsigned_Integer )
 *        std::uint_fast32_t, 24 test fixture.
 */
class constructorUnsignedInteger24 : public TestWithParam<std::uint_fast32_t> {
};

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly.
 */
TEST_P( constructorUnsignedInteger24, worksProperly )
{
    auto const unsigned_integer = GetParam();

    auto const sample = Sample<std::uint_fast32_t, 24>{ unsigned_integer };

    ASSERT_EQ( sample.as_unsigned_integer(), unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P( testCases, constructorUnsignedInteger24, ValuesIn( constructorUnsignedInteger24_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) std::uint_fast32_t, 24 test
 *        fixture.
 */
class constructorBypassPreconditionExpectationChecksUnsignedInteger24 :
    public TestWithParam<std::uint_fast32_t> {
};

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly.
 */
TEST_P( constructorBypassPreconditionExpectationChecksUnsignedInteger24, worksProperly )
{
    auto const unsigned_integer = GetParam();

    auto const sample = Sample<std::uint_fast32_t, 24>{ BYPASS_PRECONDITION_EXPECTATION_CHECKS,
                                                        unsigned_integer };

    ASSERT_EQ( sample.as_unsigned_integer(), unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P(
    testCases,
    constructorBypassPreconditionExpectationChecksUnsignedInteger24,
    ValuesIn( constructorUnsignedInteger24_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::operator==( picolibrary::ADC::Sample, picolibrary::ADC::Sample
 *        ), picolibrary::ADC::operator!=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ), picolibrary::ADC::operator<(
 *        picolibrary::ADC::Sample, picolibrary::ADC::Sample ),
 *        picolibrary::ADC::operator>( picolibrary::ADC::Sample, picolibrary::ADC::Sample
 *        ), picolibrary::ADC::operator<=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ), and picolibrary::ADC::operator>=(
 *        picolibrary::ADC::Sample, picolibrary::ADC::Sample ) test case.
 */
struct comparisonOperator_Test_Case {
    /**
     * \brief The left hand size of the comparison.
     */
    Sample<std::uint_fast16_t, 10> lhs;

    /**
     * \brief The right hand size of the comparison.
     */
    Sample<std::uint_fast16_t, 10> rhs;

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
                  << ".lhs = " << test_case.lhs.as_unsigned_integer()
                  << ", "
                  << ".rhs = " << test_case.rhs.as_unsigned_integer()
                  << ", "
                  << ".comparison_result = " << std::boolalpha << test_case.comparison_result
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::ADC::operator==( picolibrary::ADC::Sample, picolibrary::ADC::Sample
 *        ) test fixture.
 */
class equalityOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::ADC::operator==( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample) works properly.
 */
TEST_P( equalityOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs == test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::ADC::operator==( picolibrary::ADC::Sample, picolibrary::ADC::Sample
 *        ) test cases.
 */
comparisonOperator_Test_Case const equalityOperator_TEST_CASES[]{
    // clang-format off

    {  21, 448, false },
    { 448, 449, false },
    { 449, 449, true  },
    { 449, 450, false },
    { 450, 944, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, equalityOperator, ValuesIn( equalityOperator_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::operator!=( picolibrary::ADC::Sample, picolibrary::ADC::Sample
 *        ) test fixture.
 */
class inequalityOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::ADC::operator!=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample) works properly.
 */
TEST_P( inequalityOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs != test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::ADC::operator!=( picolibrary::ADC::Sample, picolibrary::ADC::Sample
 *        ) test cases.
 */
comparisonOperator_Test_Case const inequalityOperator_TEST_CASES[]{
    // clang-format off

    {  21, 448, true  },
    { 448, 449, true  },
    { 449, 449, false },
    { 449, 450, true  },
    { 450, 944, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, inequalityOperator, ValuesIn( inequalityOperator_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::operator<( picolibrary::ADC::Sample, picolibrary::ADC::Sample)
 *        test fixture.
 */
class lessThanOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::ADC::operator<( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample) works properly.
 */
TEST_P( lessThanOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs < test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::ADC::operator<( picolibrary::ADC::Sample, picolibrary::ADC::Sample)
 *        test cases.
 */
comparisonOperator_Test_Case const lessThanOperator_TEST_CASES[]{
    // clang-format off

    {  21, 448, true  },
    { 447, 448, true  },
    { 448, 448, false },
    { 449, 448, false },
    { 829, 448, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, lessThanOperator, ValuesIn( lessThanOperator_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::operator>( picolibrary::ADC::Sample, picolibrary::ADC::Sample)
 *        test fixture.
 */
class greaterThanOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::ADC::operator>( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample) works properly.
 */
TEST_P( greaterThanOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs > test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::ADC::operator>( picolibrary::ADC::Sample, picolibrary::ADC::Sample)
 *        test cases.
 */
comparisonOperator_Test_Case const greaterThanOperator_TEST_CASES[]{
    // clang-format off

    {  21, 448, false },
    { 447, 448, false },
    { 448, 448, false },
    { 449, 448, true  },
    { 829, 448, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, greaterThanOperator, ValuesIn( greaterThanOperator_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::operator<=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample) test fixture.
 */
class lessThanOrEqualToOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::ADC::operator<=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample) works properly.
 */
TEST_P( lessThanOrEqualToOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs <= test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::ADC::operator<=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample) test cases.
 */
comparisonOperator_Test_Case const lessThanOrEqualToOperator_TEST_CASES[]{
    // clang-format off

    {  21, 448, true  },
    { 447, 448, true  },
    { 448, 448, true  },
    { 449, 448, false },
    { 829, 448, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, lessThanOrEqualToOperator, ValuesIn( lessThanOrEqualToOperator_TEST_CASES ) );

/**
 * \brief picolibrary::ADC::operator>=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample) test fixture.
 */
class greaterThanOrEqualToOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::ADC::operator>=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample) works properly.
 */
TEST_P( greaterThanOrEqualToOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs >= test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::ADC::operator>=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample) test cases.
 */
comparisonOperator_Test_Case const greaterThanOrEqualToOperator_TEST_CASES[]{
    // clang-format off

    {  21, 448, false },
    { 447, 448, false },
    { 448, 448, true  },
    { 449, 448, true  },
    { 829, 448, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, greaterThanOrEqualToOperator, ValuesIn( greaterThanOrEqualToOperator_TEST_CASES ) );

/**
 * \brief Execute the picolibrary::ADC::Sample automated tests.
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

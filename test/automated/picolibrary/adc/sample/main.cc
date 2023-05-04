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
using ::testing::Test;
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
 *        picolibrary::ADC::Sample::Unsigned_Integer ) test case.
 *
 * \tparam T The sample unsigned integer representation.
 * \tparam N The number of bits in the sample.
 * \tparam MIN_VALUE The minimum valid sample.
 * \tparam MAX_VALUE The maximum valid sample.
 * \tparam MAX_ADJACENT_VALUE The valid sample that is adjacent to the maximum valid
 *         sample.
 * \tparam OTHER_VALUE An arbitrary valid sample.
 */
template<typename T, std::uint_fast8_t N, T MIN_VALUE, T MAX_VALUE, T MAX_ADJACENT_VALUE, T OTHER_VALUE>
struct constructorUnsignedInteger_Test_Case {
    /**
     * \brief The sample type.
     */
    using Sample = ::picolibrary::ADC::Sample<T, N>;

    /**
     * \brief The minimum valid sample value.
     */
    static constexpr auto MIN = MIN_VALUE;

    /**
     * \brief The maximum valid sample value.
     */
    static constexpr auto MAX = MAX_VALUE;

    /**
     * \brief The valid sample that is adjacent to the maximum valid sample.
     */
    static constexpr auto MAX_ADJACENT = MAX_ADJACENT_VALUE;

    /**
     * \brief An arbitrary valid sample.
     */
    static constexpr auto OTHER = OTHER_VALUE;
};

/**
 * \brief picolibrary::ADC::Sample::Sample( picolibrary::ADC::Sample::Unsigned_Integer )
 *        and picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) test cases.
 */
using constructorUnsignedInteger_Test_Cases = Types<
    // clang-format off

    constructorUnsignedInteger_Test_Case<std::uint_fast8_t,   8, 0,      255,      254,       51>,
    constructorUnsignedInteger_Test_Case<std::uint_fast16_t, 10, 0,     1023,     1022,      167>,
    constructorUnsignedInteger_Test_Case<std::uint_fast16_t, 12, 0,     4095,     4094,     3700>,
    constructorUnsignedInteger_Test_Case<std::uint_fast16_t, 14, 0,    16383,    16382,     8533>,
    constructorUnsignedInteger_Test_Case<std::uint_fast16_t, 16, 0,    65535,    65534,    52593>,
    constructorUnsignedInteger_Test_Case<std::uint_fast32_t, 18, 0,   262143,   262142,   102396>,
    constructorUnsignedInteger_Test_Case<std::uint_fast32_t, 20, 0,  1048575,  1048574,   407638>,
    constructorUnsignedInteger_Test_Case<std::uint_fast32_t, 24, 0, 16777215, 16777214, 12395235>

    // clang-format on
    >;

/**
 * \brief picolibrary::ADC::Sample::Sample( picolibrary::ADC::Sample::Unsigned_Integer )
 *        test fixture.
 *
 * \tparam Test_Case The test case.
 */
template<typename Test_Case>
class constructorUnsignedInteger : public Test {
};

TYPED_TEST_SUITE( constructorUnsignedInteger, constructorUnsignedInteger_Test_Cases );

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly with the minimum
 *        valid sample.
 */
TYPED_TEST( constructorUnsignedInteger, worksProperlyMin )
{
    using Test_Case = TypeParam;

    using Sample = typename Test_Case::Sample;

    auto const sample = Sample{ Test_Case::MIN };

    ASSERT_EQ( sample.as_unsigned_integer(), Test_Case::MIN );
}

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly with the maximum
 *        valid sample.
 */
TYPED_TEST( constructorUnsignedInteger, worksProperlyMax )
{
    using Test_Case = TypeParam;

    using Sample = typename Test_Case::Sample;

    auto const sample = Sample{ Test_Case::MAX };

    ASSERT_EQ( sample.as_unsigned_integer(), Test_Case::MAX );
}

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly with the valid
 *        sample that is adjacent to the maximum valid sample.
 */
TYPED_TEST( constructorUnsignedInteger, worksProperlyMaxAdjacent )
{
    using Test_Case = TypeParam;

    using Sample = typename Test_Case::Sample;

    auto const sample = Sample{ Test_Case::MAX_ADJACENT };

    ASSERT_EQ( sample.as_unsigned_integer(), Test_Case::MAX_ADJACENT );
}

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly with an arbitrary
 *        valid sample.
 */
TYPED_TEST( constructorUnsignedInteger, worksProperlyOther )
{
    using Test_Case = TypeParam;

    using Sample = typename Test_Case::Sample;

    auto const sample = Sample{ Test_Case::OTHER };

    ASSERT_EQ( sample.as_unsigned_integer(), Test_Case::OTHER );
}

/**
 * \brief picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) test fixture.
 *
 * \tparam Test_Case The test case.
 */
template<typename Test_Case>
class constructorBypassPreconditionExpectationChecksUnsignedInteger : public Test {
};

TYPED_TEST_SUITE( constructorBypassPreconditionExpectationChecksUnsignedInteger, constructorUnsignedInteger_Test_Cases );

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly with the minimum
 *        valid sample.
 */
TYPED_TEST( constructorBypassPreconditionExpectationChecksUnsignedInteger, worksProperlyMin )
{
    using Test_Case = TypeParam;

    using Sample = typename Test_Case::Sample;

    auto const sample = Sample{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, Test_Case::MIN };

    ASSERT_EQ( sample.as_unsigned_integer(), Test_Case::MIN );
}

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly with the maximum
 *        valid sample.
 */
TYPED_TEST( constructorBypassPreconditionExpectationChecksUnsignedInteger, worksProperlyMax )
{
    using Test_Case = TypeParam;

    using Sample = typename Test_Case::Sample;

    auto const sample = Sample{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, Test_Case::MAX };

    ASSERT_EQ( sample.as_unsigned_integer(), Test_Case::MAX );
}

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly with the valid
 *        sample that is adjacent to the maximum valid sample.
 */
TYPED_TEST( constructorBypassPreconditionExpectationChecksUnsignedInteger, worksProperlyMaxAdjacent )
{
    using Test_Case = TypeParam;

    using Sample = typename Test_Case::Sample;

    auto const sample = Sample{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, Test_Case::MAX_ADJACENT };

    ASSERT_EQ( sample.as_unsigned_integer(), Test_Case::MAX_ADJACENT );
}

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly with an arbitrary
 *        valid sample.
 */
TYPED_TEST( constructorBypassPreconditionExpectationChecksUnsignedInteger, worksProperlyOther )
{
    using Test_Case = TypeParam;

    using Sample = typename Test_Case::Sample;

    auto const sample = Sample{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, Test_Case::OTHER };

    ASSERT_EQ( sample.as_unsigned_integer(), Test_Case::OTHER );
}

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

/**
 * \brief picolibrary::ADC::operator==( picolibrary::ADC::Sample, picolibrary::ADC::Sample
 *        ) test fixture.
 */
class equalityOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

INSTANTIATE_TEST_SUITE_P( testCases, equalityOperator, ValuesIn( equalityOperator_TEST_CASES ) );

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

/**
 * \brief picolibrary::ADC::operator!=( picolibrary::ADC::Sample, picolibrary::ADC::Sample
 *        ) test fixture.
 */
class inequalityOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

INSTANTIATE_TEST_SUITE_P( testCases, inequalityOperator, ValuesIn( inequalityOperator_TEST_CASES ) );

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

/**
 * \brief picolibrary::ADC::operator<( picolibrary::ADC::Sample, picolibrary::ADC::Sample)
 *        test fixture.
 */
class lessThanOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

INSTANTIATE_TEST_SUITE_P( testCases, lessThanOperator, ValuesIn( lessThanOperator_TEST_CASES ) );

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

/**
 * \brief picolibrary::ADC::operator>( picolibrary::ADC::Sample, picolibrary::ADC::Sample)
 *        test fixture.
 */
class greaterThanOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

INSTANTIATE_TEST_SUITE_P( testCases, greaterThanOperator, ValuesIn( greaterThanOperator_TEST_CASES ) );

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

/**
 * \brief picolibrary::ADC::operator<=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample) test fixture.
 */
class lessThanOrEqualToOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

INSTANTIATE_TEST_SUITE_P( testCases, lessThanOrEqualToOperator, ValuesIn( lessThanOrEqualToOperator_TEST_CASES ) );

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

/**
 * \brief picolibrary::ADC::operator>=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample) test fixture.
 */
class greaterThanOrEqualToOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

INSTANTIATE_TEST_SUITE_P( testCases, greaterThanOrEqualToOperator, ValuesIn( greaterThanOrEqualToOperator_TEST_CASES ) );

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

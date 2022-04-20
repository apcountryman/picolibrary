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
 * \brief picolibrary::ADC::Sample unit test program.
 */

#include <cstdint>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/adc.h"
#include "picolibrary/precondition.h"
#include "picolibrary/testing/unit/random.h"

namespace {

using ::picolibrary::BYPASS_PRECONDITION_EXPECTATION_CHECKS;
using ::picolibrary::ADC::Sample;
using ::picolibrary::Testing::Unit::random;
using ::testing::Test;

template<typename Sample>
auto random_unsigned_integer(
    typename Sample::Unsigned_Integer min = Sample::min().as_unsigned_integer(),
    typename Sample::Unsigned_Integer max = Sample::max().as_unsigned_integer() )
{
    return random<typename Sample::Unsigned_Integer>( min, max );
}

template<typename Sample>
auto random_unique_unsigned_integer_pair()
{
    auto const a = random_unsigned_integer<Sample>();
    auto const b = random_unsigned_integer<Sample>();

    return std::pair<typename Sample::Unsigned_Integer, typename Sample::Unsigned_Integer>{ a, b != a ? b : b ^ random_unsigned_integer<Sample>( 1 ) };
}

} // namespace

/**
 * \brief picolibrary::ADC::Sample unit test sample types.
 */
using Samples = ::testing::Types<
    Sample<std::uint_fast8_t, 8>,
    Sample<std::uint_fast16_t, 10>,
    Sample<std::uint_fast16_t, 12>,
    Sample<std::uint_fast16_t, 14>,
    Sample<std::uint_fast16_t, 16>,
    Sample<std::uint_fast32_t, 18>,
    Sample<std::uint_fast32_t, 20>,
    Sample<std::uint_fast32_t, 24>>;

/**
 * \brief picolibrary::ADC::Sample::Sample() unit test fixture.
 *
 * \tparam Sample The picolibrary::ADC::Sample instantiation to be tested.
 */
template<typename Sample>
class constructorDefault : public Test {
};

/**
 * \brief picolibrary::ADC::Sample::Sample() unit test fixture.
 */
TYPED_TEST_SUITE( constructorDefault, Samples );

/**
 * \brief Verify picolibrary::ADC::Sample::Sample() works properly.
 */
TYPED_TEST( constructorDefault, worksProperly )
{
    using Sample = TypeParam;

    auto const sample = Sample{};

    EXPECT_EQ( sample.as_unsigned_integer(), 0 );
}

/**
 * \brief picolibrary::ADC::Sample::Sample( picolibrary::ADC::Sample::Unsigned_Integer )
 *        unit test fixture.
 *
 * \tparam Sample The picolibrary::ADC::Sample instantiation to be tested.
 */
template<typename Sample>
class constructorUnsignedInteger : public Test {
};

/**
 * \brief picolibrary::ADC::Sample::Sample( picolibrary::ADC::Sample::Unsigned_Integer )
 *        unit test fixture.
 */
TYPED_TEST_SUITE( constructorUnsignedInteger, Samples );

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly.
 */
TYPED_TEST( constructorUnsignedInteger, worksProperly )
{
    using Sample = TypeParam;

    auto const unsigned_integer = random_unsigned_integer<Sample>();

    auto const sample = Sample{ unsigned_integer };

    EXPECT_EQ( sample.as_unsigned_integer(), unsigned_integer );
}

/**
 * \brief picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) unit test fixture.
 *
 * \tparam Sample The picolibrary::ADC::Sample instantiation to be tested.
 */
template<typename Sample>
class constructorBypassPreconditionExpectationChecksUnsignedInteger : public Test {
};

/**
 * \brief picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) unit test fixture.
 */
TYPED_TEST_SUITE( constructorBypassPreconditionExpectationChecksUnsignedInteger, Samples );

/**
 * \brief Verify picolibrary::ADC::Sample::Sample(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::ADC::Sample::Unsigned_Integer ) works properly.
 */
TYPED_TEST( constructorBypassPreconditionExpectationChecksUnsignedInteger, worksProperly )
{
    using Sample = TypeParam;

    auto const unsigned_integer = random_unsigned_integer<Sample>();

    auto const sample = Sample{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, unsigned_integer };

    EXPECT_EQ( sample.as_unsigned_integer(), unsigned_integer );
}

/**
 * \brief picolibrary::ADC::Sample::operator==( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ) unit test fixture.
 *
 * \tparam Sample The picolibrary::ADC::Sample instantiation to be tested.
 */
template<typename Sample>
class equalityOperator : public Test {
};

/**
 * \brief picolibrary::ADC::Sample::operator==( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ) unit test fixture.
 */
TYPED_TEST_SUITE( equalityOperator, Samples );

/**
 * \brief Verify picolibrary::ADC::Sample::operator==( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ) works properly.
 */
TYPED_TEST( equalityOperator, worksProperly )
{
    using Sample = TypeParam;

    {
        auto const lhs = random_unsigned_integer<Sample>();
        auto const rhs = lhs;

        EXPECT_TRUE( Sample{ lhs } == Sample{ rhs } );
    }

    {
        auto const [ lhs, rhs ] = random_unique_unsigned_integer_pair<Sample>();

        EXPECT_FALSE( Sample{ lhs } == Sample{ rhs } );
    }
}

/**
 * \brief picolibrary::ADC::Sample::operator!=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ) unit test fixture.
 *
 * \tparam Sample The picolibrary::ADC::Sample instantiation to be tested.
 */
template<typename Sample>
class inequalityOperator : public Test {
};

/**
 * \brief picolibrary::ADC::Sample::operator!=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ) unit test fixture.
 */
TYPED_TEST_SUITE( inequalityOperator, Samples );

/**
 * \brief Verify picolibrary::ADC::Sample::operator!=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ) works properly.
 */
TYPED_TEST( inequalityOperator, worksProperly )
{
    using Sample = TypeParam;

    {
        auto const lhs = random_unsigned_integer<Sample>();
        auto const rhs = lhs;

        EXPECT_FALSE( Sample{ lhs } != Sample{ rhs } );
    }

    {
        auto const [ lhs, rhs ] = random_unique_unsigned_integer_pair<Sample>();

        EXPECT_TRUE( Sample{ lhs } != Sample{ rhs } );
    }
}

/**
 * \brief picolibrary::ADC::Sample::operator<( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ) unit test fixture.
 *
 * \tparam Sample The picolibrary::ADC::Sample instantiation to be tested.
 */
template<typename Sample>
class lessThanOperator : public Test {
};

/**
 * \brief picolibrary::ADC::Sample::operator<( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ) unit test fixture.
 */
TYPED_TEST_SUITE( lessThanOperator, Samples );

/**
 * \brief Verify picolibrary::ADC::Sample::operator<( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ) works properly.
 */
TYPED_TEST( lessThanOperator, worksProperly )
{
    using Sample = TypeParam;

    {
        auto const rhs = random_unsigned_integer<Sample>( 0 + 1 );
        auto const lhs = random_unsigned_integer<Sample>( 0, rhs - 1 );

        EXPECT_TRUE( Sample{ lhs } < Sample{ rhs } );
    }

    {
        auto const rhs = random_unsigned_integer<Sample>();
        auto const lhs = random_unsigned_integer<Sample>( rhs );

        EXPECT_FALSE( Sample{ lhs } < Sample{ rhs } );
    }
}

/**
 * \brief picolibrary::ADC::Sample::operator>( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ) unit test fixture.
 *
 * \tparam Sample The picolibrary::ADC::Sample instantiation to be tested.
 */
template<typename Sample>
class greaterThanOperator : public Test {
};

/**
 * \brief picolibrary::ADC::Sample::operator>( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ) unit test fixture.
 */
TYPED_TEST_SUITE( greaterThanOperator, Samples );

/**
 * \brief Verify picolibrary::ADC::Sample::operator>( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ) works properly.
 */
TYPED_TEST( greaterThanOperator, worksProperly )
{
    using Sample = TypeParam;

    {
        auto const lhs = random_unsigned_integer<Sample>( 0 + 1 );
        auto const rhs = random_unsigned_integer<Sample>( 0, lhs - 1 );

        EXPECT_TRUE( Sample{ lhs } > Sample{ rhs } );
    }

    {
        auto const lhs = random_unsigned_integer<Sample>();
        auto const rhs = random_unsigned_integer<Sample>( lhs );

        EXPECT_FALSE( Sample{ lhs } > Sample{ rhs } );
    }
}

/**
 * \brief picolibrary::ADC::Sample::operator<=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ) unit test fixture.
 *
 * \tparam Sample The picolibrary::ADC::Sample instantiation to be tested.
 */
template<typename Sample>
class lessThanOrEqualToOperator : public Test {
};

/**
 * \brief picolibrary::ADC::Sample::operator<=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ) unit test fixture.
 */
TYPED_TEST_SUITE( lessThanOrEqualToOperator, Samples );

/**
 * \brief Verify picolibrary::ADC::Sample::operator<=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ) works properly.
 */
TYPED_TEST( lessThanOrEqualToOperator, worksProperly )
{
    using Sample = TypeParam;

    {
        auto const lhs = random_unsigned_integer<Sample>();
        auto const rhs = random_unsigned_integer<Sample>( lhs );

        EXPECT_TRUE( Sample{ lhs } <= Sample{ rhs } );
    }

    {
        auto const lhs = random_unsigned_integer<Sample>( 0 + 1 );
        auto const rhs = random_unsigned_integer<Sample>( 0, lhs - 1 );

        EXPECT_FALSE( Sample{ lhs } <= Sample{ rhs } );
    }
}

/**
 * \brief picolibrary::ADC::Sample::operator>=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ) unit test fixture.
 *
 * \tparam Sample The picolibrary::ADC::Sample instantiation to be tested.
 */
template<typename Sample>
class greaterThanOrEqualToOperator : public Test {
};

/**
 * \brief picolibrary::ADC::Sample::operator>=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ) unit test fixture.
 */
TYPED_TEST_SUITE( greaterThanOrEqualToOperator, Samples );

/**
 * \brief Verify picolibrary::ADC::Sample::operator>=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ) works properly.
 */
TYPED_TEST( greaterThanOrEqualToOperator, worksProperly )
{
    using Sample = TypeParam;

    {
        auto const rhs = random_unsigned_integer<Sample>();
        auto const lhs = random_unsigned_integer<Sample>( rhs );

        EXPECT_TRUE( Sample{ lhs } >= Sample{ rhs } );
    }

    {
        auto const rhs = random_unsigned_integer<Sample>( 0 + 1 );
        auto const lhs = random_unsigned_integer<Sample>( 0, rhs - 1 );

        EXPECT_FALSE( Sample{ lhs } >= Sample{ rhs } );
    }
}

/**
 * \brief Execute the picolibrary::ADC::Sample unit tests.
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

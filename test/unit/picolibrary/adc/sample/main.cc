/**
 * picolibrary
 *
 * Copyright 2020-2021, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
#include "picolibrary/testing/unit/random.h"

namespace {

using ::picolibrary::ADC::Sample;
using ::picolibrary::Testing::Unit::random;
using ::testing::Test;

using Sample_Types = ::testing::Types<
    Sample<std::uint8_t, 0, 255>,
    Sample<std::uint_fast16_t, 0, 1023>,
    Sample<std::uint_fast16_t, 0, 4095>,
    Sample<std::uint_fast16_t, 0, 16383>,
    Sample<std::uint_fast16_t, 0, 65535>>;

template<typename Sample>
auto random_unique_sample_values()
{
    using Value = typename Sample::Value;

    auto const a = random<Value>( Sample::MIN, Sample::MAX );
    auto const b = random<Value>( Sample::MIN, Sample::MAX );

    return std::pair<Value, Value>{ a, b != a ? b : b ^ random<Value>( Sample::MIN + 1, Sample::MAX ) };
}

} // namespace

/**
 * \brief picolibrary::ADC::Sample::Sample() unit test fixture.
 *
 * \tparam Sample_Type The picolibrary::ADC::Sample instantiation to be tested.
 */
template<typename Sample_Type>
class constructorDefault : public Test {
};

/**
 * \brief picolibrary::ADC::Sample::Sample() unit test fixture.
 */
TYPED_TEST_SUITE( constructorDefault, Sample_Types );

/**
 * \brief Verify picolibrary::ADC::Sample::Sample() works properly.
 */
TYPED_TEST( constructorDefault, worksProperly )
{
    using Sample = TypeParam;
    using Value  = typename Sample::Value;

    auto const sample = Sample{};

    EXPECT_EQ( sample.min(), Sample::MIN );
    EXPECT_EQ( sample.max(), Sample::MAX );
    EXPECT_EQ( static_cast<Value>( sample ), Value{} );
}

/**
 * \brief picolibrary::ADC::Sample::Sample( Value ) unit test fixture.
 *
 * \tparam Sample_Type The picolibrary::ADC::Sample instantiation to be tested.
 */
template<typename Sample_Type>
class constructorValue : public Test {
};

/**
 * \brief picolibrary::ADC::Sample::Sample( Value ) unit test fixture.
 */
TYPED_TEST_SUITE( constructorValue, Sample_Types );

/**
 * \brief Verify picolibrary::ADC::Sample::Sample( Value ) works properly.
 */
TYPED_TEST( constructorValue, worksProperly )
{
    using Sample = TypeParam;
    using Value  = typename Sample::Value;

    auto const value = random<Value>( Sample::MIN, Sample::MAX );

    auto const sample = Sample{ value };

    EXPECT_EQ( sample.min(), Sample::MIN );
    EXPECT_EQ( sample.max(), Sample::MAX );
    EXPECT_EQ( static_cast<Value>( sample ), value );
}

/**
 * \brief picolibrary::ADC::operator==( picolibrary::ADC::Sample, picolibrary::ADC::Sample
 *        ) unit test fixture.
 *
 * \tparam Sample_Type The picolibrary::ADC::Sample instantiation to be tested.
 */
template<typename Sample_Type>
class equalityOperator : public Test {
};

/**
 * \brief picolibrary::ADC::operator==( picolibrary::ADC::Sample, picolibrary::ADC::Sample
 *        ) unit test fixture.
 */
TYPED_TEST_SUITE( equalityOperator, Sample_Types );

/**
 * \brief Verify picolibrary::ADC::operator==( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ) works properly.
 */
TYPED_TEST( equalityOperator, worksProperly )
{
    using Sample = TypeParam;
    using Value  = typename Sample::Value;

    {
        auto const value = random<Value>( Sample::MIN, Sample::MAX );

        EXPECT_TRUE( Sample{ value } == Sample{ value } );
    }

    {
        auto const [ lhs_value, rhs_value ] = random_unique_sample_values<Sample>();

        EXPECT_FALSE( Sample{ lhs_value } == Sample{ rhs_value } );
    }
}

/**
 * \brief picolibrary::ADC::operator!=( picolibrary::ADC::Sample, picolibrary::ADC::Sample
 *        ) unit test fixture.
 *
 * \tparam Sample_Type The picolibrary::ADC::Sample instantiation to be tested.
 */
template<typename Sample_Type>
class inequalityOperator : public Test {
};

/**
 * \brief picolibrary::ADC::operator!=( picolibrary::ADC::Sample, picolibrary::ADC::Sample
 *        ) unit test fixture.
 */
TYPED_TEST_SUITE( inequalityOperator, Sample_Types );

/**
 * \brief Verify picolibrary::ADC::operator!=( picolibrary::ADC::Sample,
 *        picolibrary::ADC::Sample ) works properly.
 */
TYPED_TEST( inequalityOperator, worksProperly )
{
    using Sample = TypeParam;
    using Value  = typename Sample::Value;

    {
        auto const value = random<Value>( Sample::MIN, Sample::MAX );

        EXPECT_FALSE( Sample{ value } != Sample{ value } );
    }

    {
        auto const [ lhs_value, rhs_value ] = random_unique_sample_values<Sample>();

        EXPECT_TRUE( Sample{ lhs_value } != Sample{ rhs_value } );
    }
}

/**
 * \brief Execute the picolibrary::ADC::Sample unit tests.
 *
 * \param[in] argc The number of arguments to pass to testing::InitGoogleMock().
 * \param[in] argv The array  of arguments to pass to testing::InitGoogleMock().
 *
 * \return See Google Test's RUN_ALL_TESTS().
 */
int main( int argc, char * argv[] )
{
    ::testing::InitGoogleMock( &argc, argv );

    return RUN_ALL_TESTS();
}

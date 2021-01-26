/**
 * picolibrary
 *
 * Copyright 2021 Andrew Countryman <apcountryman@gmail.com>
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

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/adc.h"
#include "picolibrary/testing/unit/random.h"

namespace {

using ::picolibrary::Testing::Unit::random;

} // namespace

/**
 * \brief Verify picolibrary::ADC::Sample works properly.
 */
TEST( sample, worksProperly )
{
    using Value = std::uint_fast16_t;

    constexpr auto MIN = Value{ 0 };
    constexpr auto MAX = Value{ 1023 };

    using Sample = ::picolibrary::ADC::Sample<Value, MIN, MAX>;

    EXPECT_EQ( Sample::MIN, MIN );
    EXPECT_EQ( Sample::MAX, MAX );

    {
        auto const sample = Sample{};

        EXPECT_EQ( sample.min(), MIN );
        EXPECT_EQ( sample.max(), MAX );
        EXPECT_EQ( static_cast<Value>( sample ), Value{} );
    }

    {
        auto const value = random<Value>( MIN, MAX );

        auto const sample = Sample{ value };

        EXPECT_EQ( sample.min(), MIN );
        EXPECT_EQ( sample.max(), MAX );
        EXPECT_EQ( static_cast<Value>( sample ), value );
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

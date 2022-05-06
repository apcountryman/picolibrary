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
 * \brief picolibrary::CRC automated test program.
 */

#include <cstdint>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/crc.h"
#include "picolibrary/testing/automated/random.h"

namespace {

using ::picolibrary::CRC::Bitwise_Calculator;
using ::picolibrary::CRC::Calculation_Parameters;
using ::picolibrary::Testing::Automated::random;
using ::picolibrary::Testing::Automated::random_container;
using ::testing::Test;

} // namespace

/**
 * \brief Calculator implementations automated test calculation register types.
 */
using Registers = ::testing::Types<std::uint8_t, std::uint16_t, std::uint32_t>;

/**
 * \brief Calculator implementations automated test fixture.
 *
 * \tparam Register Calculation register type.
 */
template<typename Register>
class calculatorImplementations : public Test {
};

/**
 * \brief Calculator implementations automated test fixture.
 */
TYPED_TEST_SUITE( calculatorImplementations, Registers );

/**
 * \brief Verify calculator implementations are equivalent.
 */
TYPED_TEST( calculatorImplementations, areEquivalent )
{
    using Register = TypeParam;

    struct {
        Calculation_Parameters<Register> calculation_parameters;
    } const test_cases[]{
        // clang-format off

        { { random<Register>(), random<Register>(), false, false, random<Register>() } },
        { { random<Register>(), random<Register>(), false,  true, random<Register>() } },
        { { random<Register>(), random<Register>(),  true, false, random<Register>() } },
        { { random<Register>(), random<Register>(),  true,  true, random<Register>() } },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const message = random_container<std::vector<std::uint8_t>>();

        auto const remainder = Bitwise_Calculator{ test_case.calculation_parameters }.calculate(
            message.begin(), message.end() );

        static_cast<void>( remainder );
    } // for
}

/**
 * \brief Execute the picolibrary::CRC automated tests.
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

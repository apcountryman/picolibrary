/**
 * picolibrary
 *
 * Copyright 2020 Andrew Countryman <apcountryman@gmail.com>
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
 * \brief picolibrary::CRC unit test program.
 */

#include <cstdint>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/crc.h"
#include "picolibrary/testing/unit/random.h"

namespace {

using ::picolibrary::CRC::Augmented_Byte_Lookup_Table_Calculator;
using ::picolibrary::CRC::Augmented_Nibble_Lookup_Table_Calculator;
using ::picolibrary::CRC::Bitwise_Calculator;
using ::picolibrary::CRC::Direct_Byte_Lookup_Table_Calculator;
using ::picolibrary::CRC::Parameters;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::testing::Test;

} // namespace

/**
 * \brief Calculator equivalence unit test fixture.
 *
 * \tparam T The calculator register type.
 */
template<typename T>
class calculators : public Test {
};

/**
 * \brief Calculator register types.
 */
using Registers = ::testing::Types<std::uint8_t, std::uint16_t, std::uint32_t>;

/**
 * \brief Calculator equivalence unit test fixture type registration.
 */
TYPED_TEST_SUITE( calculators, Registers );

/**
 * \brief Verify picolibrary::CRC::Bitwise_Calculator, and
 *        picolibrary::CRC::Augmented_Byte_Lookup_Table_Calculator are equivalent.
 */
TYPED_TEST( calculators, areEquivalent )
{
    using Register = TypeParam;

    auto const parameters = Parameters<Register>{ .polynomial        = random<Register>(),
                                                  .initial_remainder = random<Register>(),
                                                  .input_is_reflected  = random<bool>(),
                                                  .output_is_reflected = random<bool>(),
                                                  .xor_output = random<Register>() };

    auto const message = random_container<std::vector<std::uint8_t>>();

    auto const expected_remainder = Bitwise_Calculator{ parameters }.calculate(
        message.begin(), message.end() );

    EXPECT_EQ(
        Augmented_Nibble_Lookup_Table_Calculator{ parameters }.calculate(
            message.begin(), message.end() ),
        expected_remainder );
    EXPECT_EQ(
        Augmented_Byte_Lookup_Table_Calculator{ parameters }.calculate(
            message.begin(), message.end() ),
        expected_remainder );
    EXPECT_EQ(
        Direct_Byte_Lookup_Table_Calculator{ parameters }.calculate(
            message.begin(), message.end() ),
        expected_remainder );
}

/**
 * \brief Execute the picolibrary::CRC unit tests.
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

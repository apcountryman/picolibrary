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
 * \brief picolibrary bit manipulation unit test program.
 */

#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/bit_manipulation.h"
#include "picolibrary/testing/unit/random.h"

namespace {

using ::picolibrary::highest_bit_set;
using ::picolibrary::Testing::Unit::random;

} // namespace

/**
 * \brief Verify picolibrary::highest_bit_set() works properly.
 */
TEST( highestBitSet, worksProperly )
{
    {
        struct {
            std::uint8_t      input;
            std::uint_fast8_t highest_bit_set;
        } const test_cases[]{
            // clang-format off

            { random<std::uint8_t>( 0b00000001, 0b00000001 ), 0 },
            { random<std::uint8_t>( 0b00000010, 0b00000011 ), 1 },
            { random<std::uint8_t>( 0b00000100, 0b00000111 ), 2 },
            { random<std::uint8_t>( 0b00001000, 0b00001111 ), 3 },
            { random<std::uint8_t>( 0b00010000, 0b00011111 ), 4 },
            { random<std::uint8_t>( 0b00100000, 0b00111111 ), 5 },
            { random<std::uint8_t>( 0b01000000, 0b01111111 ), 6 },
            { random<std::uint8_t>( 0b10000000, 0b11111111 ), 7 },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            EXPECT_EQ( highest_bit_set( test_case.input ), test_case.highest_bit_set );
        } // for
    }

    {
        struct {
            std::uint16_t     input;
            std::uint_fast8_t highest_bit_set;
        } const test_cases[]{
            // clang-format off

            { random<std::uint16_t>( 0b0000000000000001, 0b0000000000000001 ),  0 },
            { random<std::uint16_t>( 0b0000000000000010, 0b0000000000000011 ),  1 },
            { random<std::uint16_t>( 0b0000000000000100, 0b0000000000000111 ),  2 },
            { random<std::uint16_t>( 0b0000000000001000, 0b0000000000001111 ),  3 },
            { random<std::uint16_t>( 0b0000000000010000, 0b0000000000011111 ),  4 },
            { random<std::uint16_t>( 0b0000000000100000, 0b0000000000111111 ),  5 },
            { random<std::uint16_t>( 0b0000000001000000, 0b0000000001111111 ),  6 },
            { random<std::uint16_t>( 0b0000000010000000, 0b0000000011111111 ),  7 },
            { random<std::uint16_t>( 0b0000000100000000, 0b0000000111111111 ),  8 },
            { random<std::uint16_t>( 0b0000001000000000, 0b0000001111111111 ),  9 },
            { random<std::uint16_t>( 0b0000010000000000, 0b0000011111111111 ), 10 },
            { random<std::uint16_t>( 0b0000100000000000, 0b0000111111111111 ), 11 },
            { random<std::uint16_t>( 0b0001000000000000, 0b0001111111111111 ), 12 },
            { random<std::uint16_t>( 0b0010000000000000, 0b0011111111111111 ), 13 },
            { random<std::uint16_t>( 0b0100000000000000, 0b0111111111111111 ), 14 },
            { random<std::uint16_t>( 0b1000000000000000, 0b1111111111111111 ), 15 },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            EXPECT_EQ( highest_bit_set( test_case.input ), test_case.highest_bit_set );
        } // for
    }

    {
        struct {
            std::uint32_t     input;
            std::uint_fast8_t highest_bit_set;
        } const test_cases[]{
            // clang-format off

            { random<std::uint32_t>( 0b00000000000000000000000000000001, 0b00000000000000000000000000000001 ),  0 },
            { random<std::uint32_t>( 0b00000000000000000000000000000010, 0b00000000000000000000000000000011 ),  1 },
            { random<std::uint32_t>( 0b00000000000000000000000000000100, 0b00000000000000000000000000000111 ),  2 },
            { random<std::uint32_t>( 0b00000000000000000000000000001000, 0b00000000000000000000000000001111 ),  3 },
            { random<std::uint32_t>( 0b00000000000000000000000000010000, 0b00000000000000000000000000011111 ),  4 },
            { random<std::uint32_t>( 0b00000000000000000000000000100000, 0b00000000000000000000000000111111 ),  5 },
            { random<std::uint32_t>( 0b00000000000000000000000001000000, 0b00000000000000000000000001111111 ),  6 },
            { random<std::uint32_t>( 0b00000000000000000000000010000000, 0b00000000000000000000000011111111 ),  7 },
            { random<std::uint32_t>( 0b00000000000000000000000100000000, 0b00000000000000000000000111111111 ),  8 },
            { random<std::uint32_t>( 0b00000000000000000000001000000000, 0b00000000000000000000001111111111 ),  9 },
            { random<std::uint32_t>( 0b00000000000000000000010000000000, 0b00000000000000000000011111111111 ), 10 },
            { random<std::uint32_t>( 0b00000000000000000000100000000000, 0b00000000000000000000111111111111 ), 11 },
            { random<std::uint32_t>( 0b00000000000000000001000000000000, 0b00000000000000000001111111111111 ), 12 },
            { random<std::uint32_t>( 0b00000000000000000010000000000000, 0b00000000000000000011111111111111 ), 13 },
            { random<std::uint32_t>( 0b00000000000000000100000000000000, 0b00000000000000000111111111111111 ), 14 },
            { random<std::uint32_t>( 0b00000000000000001000000000000000, 0b00000000000000001111111111111111 ), 15 },
            { random<std::uint32_t>( 0b00000000000000010000000000000000, 0b00000000000000011111111111111111 ), 16 },
            { random<std::uint32_t>( 0b00000000000000100000000000000000, 0b00000000000000111111111111111111 ), 17 },
            { random<std::uint32_t>( 0b00000000000001000000000000000000, 0b00000000000001111111111111111111 ), 18 },
            { random<std::uint32_t>( 0b00000000000010000000000000000000, 0b00000000000011111111111111111111 ), 19 },
            { random<std::uint32_t>( 0b00000000000100000000000000000000, 0b00000000000111111111111111111111 ), 20 },
            { random<std::uint32_t>( 0b00000000001000000000000000000000, 0b00000000001111111111111111111111 ), 21 },
            { random<std::uint32_t>( 0b00000000010000000000000000000000, 0b00000000011111111111111111111111 ), 22 },
            { random<std::uint32_t>( 0b00000000100000000000000000000000, 0b00000000111111111111111111111111 ), 23 },
            { random<std::uint32_t>( 0b00000001000000000000000000000000, 0b00000001111111111111111111111111 ), 24 },
            { random<std::uint32_t>( 0b00000010000000000000000000000000, 0b00000011111111111111111111111111 ), 25 },
            { random<std::uint32_t>( 0b00000100000000000000000000000000, 0b00000111111111111111111111111111 ), 26 },
            { random<std::uint32_t>( 0b00001000000000000000000000000000, 0b00001111111111111111111111111111 ), 27 },
            { random<std::uint32_t>( 0b00010000000000000000000000000000, 0b00011111111111111111111111111111 ), 28 },
            { random<std::uint32_t>( 0b00100000000000000000000000000000, 0b00111111111111111111111111111111 ), 29 },
            { random<std::uint32_t>( 0b01000000000000000000000000000000, 0b01111111111111111111111111111111 ), 30 },
            { random<std::uint32_t>( 0b10000000000000000000000000000000, 0b11111111111111111111111111111111 ), 31 },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            EXPECT_EQ( highest_bit_set( test_case.input ), test_case.highest_bit_set );
        } // for
    }
}

/**
 * \brief Execute the picolibrary bit manipulation unit tests.
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

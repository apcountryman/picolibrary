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
 * \brief picolibrary::Bit_Manipulation unit test program.
 */

#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/bit_manipulation.h"
#include "picolibrary/testing/unit/random.h"

namespace {

using ::picolibrary::highest_bit_set;
using ::picolibrary::mask;
using ::picolibrary::reflect;
using ::picolibrary::Testing::Unit::random;

} // namespace

/**
 * \brief Verify picolibrary::highest_bit_set() works properly with std::uint8_t input.
 */
TEST( highestBitSet, uint8WorksProperly )
{
    struct {
        std::uint8_t      input;
        std::uint_fast8_t highest_bit_set;
    } const test_cases[]{
        // clang-format off

        { random<std::uint8_t>( 0b00000000, 0b00000001 ), 0 },
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

/**
 * \brief Verify picolibrary::highest_bit_set() works properly with std::uint16_t input.
 */
TEST( highestBitSet, uint16WorksProperly )
{
    struct {
        std::uint16_t     input;
        std::uint_fast8_t highest_bit_set;
    } const test_cases[]{
        // clang-format off

        { random<std::uint16_t>( 0b0000000000000000, 0b0000000000000001 ),  0 },
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

/**
 * \brief Verify picolibrary::highest_bit_set() works properly with std::uint32_t input.
 */
TEST( highestBitSet, uint32WorksProperly )
{
    struct {
        std::uint32_t     input;
        std::uint_fast8_t highest_bit_set;
    } const test_cases[]{
        // clang-format off

        { random<std::uint32_t>( 0b00000000000000000000000000000000, 0b00000000000000000000000000000001 ),  0 },
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

/**
 * \brief Verify picolibrary::mask() works properly for std::uint8_t masks.
 */
TEST( mask, uint8WorksProperly )
{
    struct {
        std::uint_fast8_t size;
        std::uint_fast8_t bit;
        std::uint8_t      mask;
    } const test_cases[]{
        // clang-format off

        { 0, 0, 0b00000000 },
        { 0, 1, 0b00000000 },
        { 0, 2, 0b00000000 },
        { 0, 3, 0b00000000 },
        { 0, 4, 0b00000000 },
        { 0, 5, 0b00000000 },
        { 0, 6, 0b00000000 },
        { 0, 7, 0b00000000 },

        { 1, 0, 0b00000001 },
        { 1, 1, 0b00000010 },
        { 1, 2, 0b00000100 },
        { 1, 3, 0b00001000 },
        { 1, 4, 0b00010000 },
        { 1, 5, 0b00100000 },
        { 1, 6, 0b01000000 },
        { 1, 7, 0b10000000 },

        { 2, 0, 0b00000011 },
        { 2, 1, 0b00000110 },
        { 2, 2, 0b00001100 },
        { 2, 3, 0b00011000 },
        { 2, 4, 0b00110000 },
        { 2, 5, 0b01100000 },
        { 2, 6, 0b11000000 },

        { 3, 0, 0b00000111 },
        { 3, 1, 0b00001110 },
        { 3, 2, 0b00011100 },
        { 3, 3, 0b00111000 },
        { 3, 4, 0b01110000 },
        { 3, 5, 0b11100000 },

        { 4, 0, 0b00001111 },
        { 4, 1, 0b00011110 },
        { 4, 2, 0b00111100 },
        { 4, 3, 0b01111000 },
        { 4, 4, 0b11110000 },

        { 5, 0, 0b00011111 },
        { 5, 1, 0b00111110 },
        { 5, 2, 0b01111100 },
        { 5, 3, 0b11111000 },

        { 6, 0, 0b00111111 },
        { 6, 1, 0b01111110 },
        { 6, 2, 0b11111100 },

        { 7, 0, 0b01111111 },
        { 7, 1, 0b11111110 },

        { 8, 0, 0b11111111 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        EXPECT_EQ( mask<std::uint8_t>( test_case.size, test_case.bit ), test_case.mask );
    } // for
}

/**
 * \brief Verify picolibrary::reflect() works properly with std::uint8_t input.
 */
TEST( reflect, uint8WorksProperly )
{
    struct {
        std::uint8_t value;
        std::uint8_t reflection;
    } const test_cases[]{
        // clang-format off

        { 0b00000000, 0b00000000 },
        { 0b11111111, 0b11111111 },

        { 0b00000001, 0b10000000 },
        { 0b00000010, 0b01000000 },
        { 0b00000100, 0b00100000 },
        { 0b00001000, 0b00010000 },
        { 0b00010000, 0b00001000 },
        { 0b00100000, 0b00000100 },
        { 0b01000000, 0b00000010 },
        { 0b10000000, 0b00000001 },

        { 0b00000011, 0b11000000 },
        { 0b00000111, 0b11100000 },
        { 0b00001111, 0b11110000 },
        { 0b00011111, 0b11111000 },
        { 0b00111111, 0b11111100 },
        { 0b01111111, 0b11111110 },

        { 0b11111110, 0b01111111 },
        { 0b11111100, 0b00111111 },
        { 0b11111000, 0b00011111 },
        { 0b11110000, 0b00001111 },
        { 0b11100000, 0b00000111 },
        { 0b11000000, 0b00000011 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        EXPECT_EQ( reflect( test_case.value ), test_case.reflection );
    } // for
}

/**
 * \brief Verify picolibrary::reflect() works properly with std::uint16_t input.
 */
TEST( reflect, uint16WorksProperly )
{
    struct {
        std::uint16_t value;
        std::uint16_t reflection;
    } const test_cases[]{
        { 0b0000000000000000, 0b0000000000000000 },
        { 0b1111111111111111, 0b1111111111111111 },

        { 0b0000000000000001, 0b1000000000000000 },
        { 0b0000000000000010, 0b0100000000000000 },
        { 0b0000000000000100, 0b0010000000000000 },
        { 0b0000000000001000, 0b0001000000000000 },
        { 0b0000000000010000, 0b0000100000000000 },
        { 0b0000000000100000, 0b0000010000000000 },
        { 0b0000000001000000, 0b0000001000000000 },
        { 0b0000000010000000, 0b0000000100000000 },
        { 0b0000000100000000, 0b0000000010000000 },
        { 0b0000001000000000, 0b0000000001000000 },
        { 0b0000010000000000, 0b0000000000100000 },
        { 0b0000100000000000, 0b0000000000010000 },
        { 0b0001000000000000, 0b0000000000001000 },
        { 0b0010000000000000, 0b0000000000000100 },
        { 0b0100000000000000, 0b0000000000000010 },
        { 0b1000000000000000, 0b0000000000000001 },

        { 0b0000000000000011, 0b1100000000000000 },
        { 0b0000000000000111, 0b1110000000000000 },
        { 0b0000000000001111, 0b1111000000000000 },
        { 0b0000000000011111, 0b1111100000000000 },
        { 0b0000000000111111, 0b1111110000000000 },
        { 0b0000000001111111, 0b1111111000000000 },
        { 0b0000000011111111, 0b1111111100000000 },
        { 0b0000000111111111, 0b1111111110000000 },
        { 0b0000001111111111, 0b1111111111000000 },
        { 0b0000011111111111, 0b1111111111100000 },
        { 0b0000111111111111, 0b1111111111110000 },
        { 0b0001111111111111, 0b1111111111111000 },
        { 0b0011111111111111, 0b1111111111111100 },
        { 0b0111111111111111, 0b1111111111111110 },

        { 0b1111111111111110, 0b0111111111111111 },
        { 0b1111111111111100, 0b0011111111111111 },
        { 0b1111111111111000, 0b0001111111111111 },
        { 0b1111111111110000, 0b0000111111111111 },
        { 0b1111111111100000, 0b0000011111111111 },
        { 0b1111111111000000, 0b0000001111111111 },
        { 0b1111111110000000, 0b0000000111111111 },
        { 0b1111111100000000, 0b0000000011111111 },
        { 0b1111111000000000, 0b0000000001111111 },
        { 0b1111110000000000, 0b0000000000111111 },
        { 0b1111100000000000, 0b0000000000011111 },
        { 0b1111000000000000, 0b0000000000001111 },
        { 0b1110000000000000, 0b0000000000000111 },
        { 0b1100000000000000, 0b0000000000000011 },
    };

    for ( auto const test_case : test_cases ) {
        EXPECT_EQ( reflect( test_case.value ), test_case.reflection );
    } // for
}

/**
 * \brief Verify picolibrary::reflect() works properly with std::uint32_t input.
 */
TEST( reflect, uint32WorksProperly )
{
    struct {
        std::uint32_t value;
        std::uint32_t reflection;
    } const test_cases[]{
        { 0b00000000000000000000000000000000, 0b00000000000000000000000000000000 },
        { 0b11111111111111111111111111111111, 0b11111111111111111111111111111111 },

        { 0b00000000000000000000000000000001, 0b10000000000000000000000000000000 },
        { 0b00000000000000000000000000000010, 0b01000000000000000000000000000000 },
        { 0b00000000000000000000000000000100, 0b00100000000000000000000000000000 },
        { 0b00000000000000000000000000001000, 0b00010000000000000000000000000000 },
        { 0b00000000000000000000000000010000, 0b00001000000000000000000000000000 },
        { 0b00000000000000000000000000100000, 0b00000100000000000000000000000000 },
        { 0b00000000000000000000000001000000, 0b00000010000000000000000000000000 },
        { 0b00000000000000000000000010000000, 0b00000001000000000000000000000000 },
        { 0b00000000000000000000000100000000, 0b00000000100000000000000000000000 },
        { 0b00000000000000000000001000000000, 0b00000000010000000000000000000000 },
        { 0b00000000000000000000010000000000, 0b00000000001000000000000000000000 },
        { 0b00000000000000000000100000000000, 0b00000000000100000000000000000000 },
        { 0b00000000000000000001000000000000, 0b00000000000010000000000000000000 },
        { 0b00000000000000000010000000000000, 0b00000000000001000000000000000000 },
        { 0b00000000000000000100000000000000, 0b00000000000000100000000000000000 },
        { 0b00000000000000001000000000000000, 0b00000000000000010000000000000000 },
        { 0b00000000000000010000000000000000, 0b00000000000000001000000000000000 },
        { 0b00000000000000100000000000000000, 0b00000000000000000100000000000000 },
        { 0b00000000000001000000000000000000, 0b00000000000000000010000000000000 },
        { 0b00000000000010000000000000000000, 0b00000000000000000001000000000000 },
        { 0b00000000000100000000000000000000, 0b00000000000000000000100000000000 },
        { 0b00000000001000000000000000000000, 0b00000000000000000000010000000000 },
        { 0b00000000010000000000000000000000, 0b00000000000000000000001000000000 },
        { 0b00000000100000000000000000000000, 0b00000000000000000000000100000000 },
        { 0b00000001000000000000000000000000, 0b00000000000000000000000010000000 },
        { 0b00000010000000000000000000000000, 0b00000000000000000000000001000000 },
        { 0b00000100000000000000000000000000, 0b00000000000000000000000000100000 },
        { 0b00001000000000000000000000000000, 0b00000000000000000000000000010000 },
        { 0b00010000000000000000000000000000, 0b00000000000000000000000000001000 },
        { 0b00100000000000000000000000000000, 0b00000000000000000000000000000100 },
        { 0b01000000000000000000000000000000, 0b00000000000000000000000000000010 },
        { 0b10000000000000000000000000000000, 0b00000000000000000000000000000001 },

        { 0b00000000000000000000000000000011, 0b11000000000000000000000000000000 },
        { 0b00000000000000000000000000000111, 0b11100000000000000000000000000000 },
        { 0b00000000000000000000000000001111, 0b11110000000000000000000000000000 },
        { 0b00000000000000000000000000011111, 0b11111000000000000000000000000000 },
        { 0b00000000000000000000000000111111, 0b11111100000000000000000000000000 },
        { 0b00000000000000000000000001111111, 0b11111110000000000000000000000000 },
        { 0b00000000000000000000000011111111, 0b11111111000000000000000000000000 },
        { 0b00000000000000000000000111111111, 0b11111111100000000000000000000000 },
        { 0b00000000000000000000001111111111, 0b11111111110000000000000000000000 },
        { 0b00000000000000000000011111111111, 0b11111111111000000000000000000000 },
        { 0b00000000000000000000111111111111, 0b11111111111100000000000000000000 },
        { 0b00000000000000000001111111111111, 0b11111111111110000000000000000000 },
        { 0b00000000000000000011111111111111, 0b11111111111111000000000000000000 },
        { 0b00000000000000000111111111111111, 0b11111111111111100000000000000000 },
        { 0b00000000000000001111111111111111, 0b11111111111111110000000000000000 },
        { 0b00000000000000011111111111111111, 0b11111111111111111000000000000000 },
        { 0b00000000000000111111111111111111, 0b11111111111111111100000000000000 },
        { 0b00000000000001111111111111111111, 0b11111111111111111110000000000000 },
        { 0b00000000000011111111111111111111, 0b11111111111111111111000000000000 },
        { 0b00000000000111111111111111111111, 0b11111111111111111111100000000000 },
        { 0b00000000001111111111111111111111, 0b11111111111111111111110000000000 },
        { 0b00000000011111111111111111111111, 0b11111111111111111111111000000000 },
        { 0b00000000111111111111111111111111, 0b11111111111111111111111100000000 },
        { 0b00000001111111111111111111111111, 0b11111111111111111111111110000000 },
        { 0b00000011111111111111111111111111, 0b11111111111111111111111111000000 },
        { 0b00000111111111111111111111111111, 0b11111111111111111111111111100000 },
        { 0b00001111111111111111111111111111, 0b11111111111111111111111111110000 },
        { 0b00011111111111111111111111111111, 0b11111111111111111111111111111000 },
        { 0b00111111111111111111111111111111, 0b11111111111111111111111111111100 },
        { 0b01111111111111111111111111111111, 0b11111111111111111111111111111110 },

        { 0b11111111111111111111111111111110, 0b01111111111111111111111111111111 },
        { 0b11111111111111111111111111111100, 0b00111111111111111111111111111111 },
        { 0b11111111111111111111111111111000, 0b00011111111111111111111111111111 },
        { 0b11111111111111111111111111110000, 0b00001111111111111111111111111111 },
        { 0b11111111111111111111111111100000, 0b00000111111111111111111111111111 },
        { 0b11111111111111111111111111000000, 0b00000011111111111111111111111111 },
        { 0b11111111111111111111111110000000, 0b00000001111111111111111111111111 },
        { 0b11111111111111111111111100000000, 0b00000000111111111111111111111111 },
        { 0b11111111111111111111111000000000, 0b00000000011111111111111111111111 },
        { 0b11111111111111111111110000000000, 0b00000000001111111111111111111111 },
        { 0b11111111111111111111100000000000, 0b00000000000111111111111111111111 },
        { 0b11111111111111111111000000000000, 0b00000000000011111111111111111111 },
        { 0b11111111111111111110000000000000, 0b00000000000001111111111111111111 },
        { 0b11111111111111111100000000000000, 0b00000000000000111111111111111111 },
        { 0b11111111111111111000000000000000, 0b00000000000000011111111111111111 },
        { 0b11111111111111110000000000000000, 0b00000000000000001111111111111111 },
        { 0b11111111111111100000000000000000, 0b00000000000000000111111111111111 },
        { 0b11111111111111000000000000000000, 0b00000000000000000011111111111111 },
        { 0b11111111111110000000000000000000, 0b00000000000000000001111111111111 },
        { 0b11111111111100000000000000000000, 0b00000000000000000000111111111111 },
        { 0b11111111111000000000000000000000, 0b00000000000000000000011111111111 },
        { 0b11111111110000000000000000000000, 0b00000000000000000000001111111111 },
        { 0b11111111100000000000000000000000, 0b00000000000000000000000111111111 },
        { 0b11111111000000000000000000000000, 0b00000000000000000000000011111111 },
        { 0b11111110000000000000000000000000, 0b00000000000000000000000001111111 },
        { 0b11111100000000000000000000000000, 0b00000000000000000000000000111111 },
        { 0b11111000000000000000000000000000, 0b00000000000000000000000000011111 },
        { 0b11110000000000000000000000000000, 0b00000000000000000000000000001111 },
        { 0b11100000000000000000000000000000, 0b00000000000000000000000000000111 },
        { 0b11000000000000000000000000000000, 0b00000000000000000000000000000011 },
    };

    for ( auto const test_case : test_cases ) {
        EXPECT_EQ( reflect( test_case.value ), test_case.reflection );
    } // for
}

/**
 * \brief Execute the picolibrary::Bit_Manipulation unit tests.
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

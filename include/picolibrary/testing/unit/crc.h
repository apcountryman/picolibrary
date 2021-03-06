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
 * \brief picolibrary::Testing::Unit::CRC interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_CRC_H
#define PICOLIBRARY_TESTING_UNIT_CRC_H

#include <cstdint>
#include <string_view>
#include <type_traits>

#include "gtest/gtest.h"
#include "picolibrary/crc.h"

/**
 * \brief Cyclic Redundancy Check (CRC) unit testing facilities.
 */
namespace picolibrary::Testing::Unit::CRC {
} // namespace picolibrary::Testing::Unit::CRC

/**
 * \brief std::uint8_t register calculator unit test fixture.
 *
 * \tparam Calculator_Adapter A picolibrary::Testing::Unit::CRC::Calculator_Adapter
 *         containing the type of calculator to be tested.
 */
template<typename Calculator_Adapter>
class CalculatorUint8Register : public ::testing::Test {
};

/**
 * \brief std::uint8_t register calculator unit test fixture.
 */
TYPED_TEST_SUITE_P( CalculatorUint8Register );

/**
 * \brief Verify the std::uint8_t register calculator works properly.
 */
TYPED_TEST_P( CalculatorUint8Register, worksProperly )
{
    // #lizard forgives the length

    using Register   = std::uint8_t;
    using Calculator = TypeParam;

    static_assert( std::is_same_v<typename Calculator::Register, Register> );

    struct {
        ::picolibrary::CRC::Parameters<Register> parameters;
        Register                                 result;
    } const test_cases[]{
        { .parameters = { .polynomial          = 0x31,
                          .initial_remainder   = 0x00,
                          .input_is_reflected  = false,
                          .output_is_reflected = false,
                          .xor_output          = 0x00 },
          .result     = 0xA2 },
        { .parameters = { .polynomial          = 0x31,
                          .initial_remainder   = 0xFF,
                          .input_is_reflected  = false,
                          .output_is_reflected = false,
                          .xor_output          = 0x00 },
          .result     = 0x29 },
        { .parameters = { .polynomial          = 0x31,
                          .initial_remainder   = 0x00,
                          .input_is_reflected  = true,
                          .output_is_reflected = true,
                          .xor_output          = 0x00 },
          .result     = 0xA1 },
        { .parameters = { .polynomial          = 0x31,
                          .initial_remainder   = 0xFF,
                          .input_is_reflected  = true,
                          .output_is_reflected = true,
                          .xor_output          = 0x00 },
          .result     = 0x70 },
        { .parameters = { .polynomial          = 0x31,
                          .initial_remainder   = 0x00,
                          .input_is_reflected  = false,
                          .output_is_reflected = false,
                          .xor_output          = 0xFF },
          .result     = 0x5D },
        { .parameters = { .polynomial          = 0x31,
                          .initial_remainder   = 0xFF,
                          .input_is_reflected  = false,
                          .output_is_reflected = false,
                          .xor_output          = 0xFF },
          .result     = 0xD6 },
        { .parameters = { .polynomial          = 0x31,
                          .initial_remainder   = 0x00,
                          .input_is_reflected  = true,
                          .output_is_reflected = true,
                          .xor_output          = 0xFF },
          .result     = 0x5E },
        { .parameters = { .polynomial          = 0x31,
                          .initial_remainder   = 0xFF,
                          .input_is_reflected  = true,
                          .output_is_reflected = true,
                          .xor_output          = 0xFF },
          .result     = 0x8F },
    };

    for ( auto const & test_case : test_cases ) {
        auto const calculator = Calculator{ test_case.parameters };

        auto const message = std::string_view{ "123456789" };

        EXPECT_EQ( calculator.calculate( message.begin(), message.end() ), test_case.result );
    } // for
}

/**
 * \brief std::uint8_t register calculator unit test fixture test registration.
 */
REGISTER_TYPED_TEST_SUITE_P( CalculatorUint8Register, worksProperly );

/**
 * \brief std::uint16_t register calculator unit test fixture.
 *
 * \tparam Calculator_Adapter A picolibrary::Testing::Unit::CRC::Calculator_Adapter
 *         containing the type of calculator to be tested.
 */
template<typename Calculator_Adapter>
class CalculatorUint16Register : public ::testing::Test {
};

/**
 * \brief std::uint16_t register calculator unit test fixture.
 */
TYPED_TEST_SUITE_P( CalculatorUint16Register );

/**
 * \brief Verify the std::uint16_t register calculator works properly.
 */
TYPED_TEST_P( CalculatorUint16Register, worksProperly )
{
    // #lizard forgives the length

    using Register   = std::uint16_t;
    using Calculator = TypeParam;

    static_assert( std::is_same_v<typename Calculator::Register, Register> );

    struct {
        ::picolibrary::CRC::Parameters<Register> parameters;
        Register                                 result;
    } const test_cases[]{
        { .parameters = { .polynomial          = 0x1021,
                          .initial_remainder   = 0x0000,
                          .input_is_reflected  = false,
                          .output_is_reflected = false,
                          .xor_output          = 0x0000 },
          .result     = 0x31C3 },
        { .parameters = { .polynomial          = 0x1021,
                          .initial_remainder   = 0xFFFF,
                          .input_is_reflected  = false,
                          .output_is_reflected = false,
                          .xor_output          = 0x0000 },
          .result     = 0xE5CC },
        { .parameters = { .polynomial          = 0x1021,
                          .initial_remainder   = 0x0000,
                          .input_is_reflected  = true,
                          .output_is_reflected = true,
                          .xor_output          = 0x0000 },
          .result     = 0x2189 },
        { .parameters = { .polynomial          = 0x1021,
                          .initial_remainder   = 0xFFFF,
                          .input_is_reflected  = true,
                          .output_is_reflected = true,
                          .xor_output          = 0x0000 },
          .result     = 0xD1A2 },
        { .parameters = { .polynomial          = 0x1021,
                          .initial_remainder   = 0x0000,
                          .input_is_reflected  = false,
                          .output_is_reflected = false,
                          .xor_output          = 0xFFFF },
          .result     = 0xCE3C },
        { .parameters = { .polynomial          = 0x1021,
                          .initial_remainder   = 0xFFFF,
                          .input_is_reflected  = false,
                          .output_is_reflected = false,
                          .xor_output          = 0xFFFF },
          .result     = 0x1A33 },
        { .parameters = { .polynomial          = 0x1021,
                          .initial_remainder   = 0x0000,
                          .input_is_reflected  = true,
                          .output_is_reflected = true,
                          .xor_output          = 0xFFFF },
          .result     = 0xDE76 },
        { .parameters = { .polynomial          = 0x1021,
                          .initial_remainder   = 0xFFFF,
                          .input_is_reflected  = true,
                          .output_is_reflected = true,
                          .xor_output          = 0xFFFF },
          .result     = 0x2E5D },
    };

    for ( auto const & test_case : test_cases ) {
        auto const calculator = Calculator{ test_case.parameters };

        auto const message = std::string_view{ "123456789" };

        EXPECT_EQ( calculator.calculate( message.begin(), message.end() ), test_case.result );
    } // for
}

/**
 * \brief std::uint16_t register calculator unit test fixture test registration.
 */
REGISTER_TYPED_TEST_SUITE_P( CalculatorUint16Register, worksProperly );

/**
 * \brief std::uint32_t register calculator unit test fixture.
 *
 * \tparam Calculator_Adapter A picolibrary::Testing::Unit::CRC::Calculator_Adapter
 *         containing the type of calculator to be tested.
 */
template<typename Calculator_Adapter>
class CalculatorUint32Register : public ::testing::Test {
};

/**
 * \brief std::uint32_t register calculator unit test fixture.
 */
TYPED_TEST_SUITE_P( CalculatorUint32Register );

/**
 * \brief Verify the std::uint32_t register calculator works properly.
 */
TYPED_TEST_P( CalculatorUint32Register, worksProperly )
{
    // #lizard forgives the length

    using Register   = std::uint32_t;
    using Calculator = TypeParam;

    static_assert( std::is_same_v<typename Calculator::Register, Register> );

    struct {
        ::picolibrary::CRC::Parameters<Register> parameters;
        Register                                 result;
    } const test_cases[]{
        { .parameters = { .polynomial          = 0x04C11DB7,
                          .initial_remainder   = 0x00000000,
                          .input_is_reflected  = false,
                          .output_is_reflected = false,
                          .xor_output          = 0x00000000 },
          .result     = 0x89A1897F },
        { .parameters = { .polynomial          = 0x04C11DB7,
                          .initial_remainder   = 0xFFFFFFFF,
                          .input_is_reflected  = false,
                          .output_is_reflected = false,
                          .xor_output          = 0x00000000 },
          .result     = 0x373C5870 },
        { .parameters = { .polynomial          = 0x04C11DB7,
                          .initial_remainder   = 0x00000000,
                          .input_is_reflected  = true,
                          .output_is_reflected = true,
                          .xor_output          = 0x00000000 },
          .result     = 0x2DFD2D88 },
        { .parameters = { .polynomial          = 0x04C11DB7,
                          .initial_remainder   = 0xFFFFFFFF,
                          .input_is_reflected  = true,
                          .output_is_reflected = true,
                          .xor_output          = 0x00000000 },
          .result     = 0xDD7694F5 },
        { .parameters = { .polynomial          = 0x04C11DB7,
                          .initial_remainder   = 0x00000000,
                          .input_is_reflected  = false,
                          .output_is_reflected = false,
                          .xor_output          = 0xFFFFFFFF },
          .result     = 0x765E7680 },
        { .parameters = { .polynomial          = 0x04C11DB7,
                          .initial_remainder   = 0xFFFFFFFF,
                          .input_is_reflected  = false,
                          .output_is_reflected = false,
                          .xor_output          = 0xFFFFFFFF },
          .result     = 0xC8C3A78F },
        { .parameters = { .polynomial          = 0x04C11DB7,
                          .initial_remainder   = 0x00000000,
                          .input_is_reflected  = true,
                          .output_is_reflected = true,
                          .xor_output          = 0xFFFFFFFF },
          .result     = 0xD202D277 },
        { .parameters = { .polynomial          = 0x04C11DB7,
                          .initial_remainder   = 0xFFFFFFFF,
                          .input_is_reflected  = true,
                          .output_is_reflected = true,
                          .xor_output          = 0xFFFFFFFF },
          .result     = 0x22896B0A },
    };

    for ( auto const & test_case : test_cases ) {
        auto const calculator = Calculator{ test_case.parameters };

        auto const message = std::string_view{ "123456789" };

        EXPECT_EQ( calculator.calculate( message.begin(), message.end() ), test_case.result );
    } // for
}

/**
 * \brief std::uint32_t register calculator unit test fixture test registration.
 */
REGISTER_TYPED_TEST_SUITE_P( CalculatorUint32Register, worksProperly );

#endif // PICOLIBRARY_TESTING_UNIT_CRC_H

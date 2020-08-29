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
 * \brief picolibrary::Testing::Unit::CRC interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_CRC_H
#define PICOLIBRARY_TESTING_UNIT_CRC_H

#include <cstdint>
#include <string_view>

#include "gtest/gtest.h"
#include "picolibrary/crc.h"

/**
 * \brief Cyclic Redundancy Check (CRC) unit testing facilities.
 */
namespace picolibrary::Testing::Unit::CRC {

/**
 * \brief Adapter that allows a calculator to be passed to type-parameterized unit tests
 *        without instantiating the template.
 *
 * \tparam Calculator_Type The type of calculator to be tested.
 */
template<template<typename> typename Calculator_Type>
struct Calculator_Adapter {
    /**
     * \brief The type of calculator to be tested.
     */
    template<typename Register>
    using Calculator = Calculator_Type<Register>;
};

/**
 * \brief std::uint8_t register calculator unit test fixture.
 *
 * \tparam Calculator_Adapter A picolibrary::Testing::Unit::CRC::Calculator_Adapter
 *         containing the type of calculator to be tested.
 */
template<typename Calculator_Adapter>
class uint8RegisterCalculator : public ::testing::Test {
};

/**
 * \brief std::uint8_t register calculator unit test fixture.
 */
TYPED_TEST_SUITE_P( uint8RegisterCalculator );

/**
 * \brief Verify the std::uint8_t register calculator works properly.
 */
TYPED_TEST_P( uint8RegisterCalculator, worksProperly )
{
    // #lizard forgives the length

    using Register   = std::uint8_t;
    using Calculator = typename TypeParam::Calculator;

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
REGISTER_TYPED_TEST_SUITE_P( uint8RegisterCalculator, worksProperly );

} // namespace picolibrary::Testing::Unit::CRC

#endif // PICOLIBRARY_TESTING_UNIT_CRC_H

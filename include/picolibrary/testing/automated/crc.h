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
 * \brief picolibrary::Testing::Automated::CRC interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_CRC_H
#define PICOLIBRARY_TESTING_AUTOMATED_CRC_H

#include <cstdint>
#include <string_view>
#include <type_traits>

#include "gtest/gtest.h"
#include "picolibrary/crc.h"

/**
 * \brief Cyclic Redundancy Check (CRC) automated testing facilities.
 */
namespace picolibrary::Testing::Automated::CRC {
} // namespace picolibrary::Testing::Automated::CRC

/**
 * \brief Calculator implementation std::uint8_t register automated test fixture.
 *
 * \tparam Calculator The calculator implementation to be tested.
 */
template<typename Calculator>
class calculatorUint8Register : ::testing::Test {
};

/**
 * \brief Calculator implementation std::uint8_t register automated test fixture.
 */
TYPED_TEST_SUITE_P( calculatorUint8Register );

/**
 * \brief Verify the calculator implementation works properly for std::uint8_t register
 *        calculations..
 */
TYPED_TEST_P( calculatorUint8Register, worksProperly )
{
    using Register   = std::uint8_t;
    using Calculator = TypeParam;

    static_assert( std::is_same_v<typename Calculator::Register, Register> );

    struct {
        ::picolibrary::CRC::Calculation_Parameters<Register> calculation_parameters;
        Register                                             remainder;
    } const test_cases[]{
        // clang-format off

        { { 0x31, 0x00, false, false, 0x00 }, 0xA2 },
        { { 0x31, 0xFF, false, false, 0x00 }, 0x29 },
        { { 0x31, 0x00,  true,  true, 0x00 }, 0xA1 },
        { { 0x31, 0xFF,  true,  true, 0x00 }, 0x70 },
        { { 0x31, 0x00, false, false, 0xFF }, 0x5D },
        { { 0x31, 0xFF, false, false, 0xFF }, 0xD6 },
        { { 0x31, 0x00,  true,  true, 0xFF }, 0x5E },
        { { 0x31, 0xFF,  true,  true, 0xFF }, 0x8F },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const calculator = Calculator{ test_case.calculation_parameters };

        auto const message = std::string_view{ "123456789" };

        EXPECT_EQ( calculator.calculate( message.begin(), message.end() ), test_case.remainder );
    } // for
}

/**
 * \brief Calculator implementation std::uint8_t register automated test fixture test
 *        registration.
 */
REGISTER_TYPED_TEST_SUITE_P( calculatorUint8Register, worksProperly );

/**
 * \brief Calculator implementation std::uint16_t register automated test fixture.
 *
 * \tparam Calculator The calculator implementation to be tested.
 */
template<typename Calculator>
class calculatorUint16Register : ::testing::Test {
};

/**
 * \brief Calculator implementation std::uint16_t register automated test fixture.
 */
TYPED_TEST_SUITE_P( calculatorUint16Register );

/**
 * \brief Verify the calculator implementation works properly for std::uint16_t register
 *        calculations..
 */
TYPED_TEST_P( calculatorUint16Register, worksProperly )
{
    using Register   = std::uint16_t;
    using Calculator = TypeParam;

    static_assert( std::is_same_v<typename Calculator::Register, Register> );

    struct {
        ::picolibrary::CRC::Calculation_Parameters<Register> calculation_parameters;
        Register                                             remainder;
    } const test_cases[]{
        // clang-format off

        { { 0x1021, 0x0000, false, false, 0x0000 }, 0x31C3 },
        { { 0x1021, 0xFFFF, false, false, 0x0000 }, 0xE5CC },
        { { 0x1021, 0x0000,  true,  true, 0x0000 }, 0x2189 },
        { { 0x1021, 0xFFFF,  true,  true, 0x0000 }, 0xD1A2 },
        { { 0x1021, 0x0000, false, false, 0xFFFF }, 0xCE3C },
        { { 0x1021, 0xFFFF, false, false, 0xFFFF }, 0x1A33 },
        { { 0x1021, 0x0000,  true,  true, 0xFFFF }, 0xDE76 },
        { { 0x1021, 0xFFFF,  true,  true, 0xFFFF }, 0x2E5D },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const calculator = Calculator{ test_case.calculation_parameters };

        auto const message = std::string_view{ "123456789" };

        EXPECT_EQ( calculator.calculate( message.begin(), message.end() ), test_case.remainder );
    } // for
}

/**
 * \brief Calculator implementation std::uint16_t register automated test fixture test
 *        registration.
 */
REGISTER_TYPED_TEST_SUITE_P( calculatorUint16Register, worksProperly );

/**
 * \brief Calculator implementation std::uint32_t register automated test fixture.
 *
 * \tparam Calculator The calculator implementation to be tested.
 */
template<typename Calculator>
class calculatorUint32Register : ::testing::Test {
};

/**
 * \brief Calculator implementation std::uint32_t register automated test fixture.
 */
TYPED_TEST_SUITE_P( calculatorUint32Register );

/**
 * \brief Verify the calculator implementation works properly for std::uint32_t register
 *        calculations..
 */
TYPED_TEST_P( calculatorUint32Register, worksProperly )
{
    using Register   = std::uint32_t;
    using Calculator = TypeParam;

    static_assert( std::is_same_v<typename Calculator::Register, Register> );

    struct {
        ::picolibrary::CRC::Calculation_Parameters<Register> calculation_parameters;
        Register                                             remainder;
    } const test_cases[]{
        // clang-format off

        { { 0x04C11DB7, 0x00000000, false, false, 0x00000000 }, 0x89A1897F },
        { { 0x04C11DB7, 0xFFFFFFFF, false, false, 0x00000000 }, 0x373C5870 },
        { { 0x04C11DB7, 0x00000000,  true,  true, 0x00000000 }, 0x2DFD2D88 },
        { { 0x04C11DB7, 0xFFFFFFFF,  true,  true, 0x00000000 }, 0xDD7694F5 },
        { { 0x04C11DB7, 0x00000000, false, false, 0xFFFFFFFF }, 0x765E7680 },
        { { 0x04C11DB7, 0xFFFFFFFF, false, false, 0xFFFFFFFF }, 0xC8C3A78F },
        { { 0x04C11DB7, 0x00000000,  true,  true, 0xFFFFFFFF }, 0xD202D277 },
        { { 0x04C11DB7, 0xFFFFFFFF,  true,  true, 0xFFFFFFFF }, 0x22896B0A },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const calculator = Calculator{ test_case.calculation_parameters };

        auto const message = std::string_view{ "123456789" };

        EXPECT_EQ( calculator.calculate( message.begin(), message.end() ), test_case.remainder );
    } // for
}

/**
 * \brief Calculator implementation std::uint32_t register automated test fixture test
 *        registration.
 */
REGISTER_TYPED_TEST_SUITE_P( calculatorUint32Register, worksProperly );

#endif // PICOLIBRARY_TESTING_AUTOMATED_CRC_H

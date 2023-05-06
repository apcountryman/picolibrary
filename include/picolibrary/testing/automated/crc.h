/**
 * picolibrary
 *
 * Copyright 2020-2023, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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

#include <array>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <limits>
#include <ostream>
#include <string_view>

#include "picolibrary/crc.h"

/**
 * \brief CRC automated testing facilities.
 */
namespace picolibrary::Testing::Automated::CRC {

/**
 * \brief Calculator test case.
 *
 * \tparam Register Calculation register type.
 */
template<typename Register>
struct Calculator_Test_Case {
    /**
     * \brief Calculation parameters.
     */
    ::picolibrary::CRC::Calculation_Parameters<Register> calculation_parameters;

    /**
     * \brief Message.
     */
    std::string_view message;

    /**
     * \brief Remainder for the message.
     */
    Register remainder;
};

/**
 * \brief Insertion operator.
 *
 * \tparam Register Calculation register type.
 *
 * \param[out] stream The stream to write to.
 * \param[in] test_case The test case to write.
 *
 * \return stream
 */
template<typename Register>
auto operator<<( std::ostream & stream, Calculator_Test_Case<Register> const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".calculation_parameters = { "
                  << ".polynomial = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<Register>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast64_t>( test_case.calculation_parameters.polynomial )
                  << ", "
                  << ".initial_remainder = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<Register>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast64_t>( test_case.calculation_parameters.initial_remainder )
                  << ", "
                  << ".input_is_reflected = " << std::boolalpha << test_case.calculation_parameters.input_is_reflected
                  << ", "
                  << ".output_is_reflected = " << std::boolalpha << test_case.calculation_parameters.output_is_reflected
                  << ", "
                  << ".xor_output = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<Register>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast64_t>( test_case.calculation_parameters.xor_output )
                  << " }, "
                  << ".message = \"" << test_case.message << "\""
                  << ", "
                  << ".remainder = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<Register>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast64_t>( test_case.remainder )
                  << " }";

    // clang-format on
}

/**
 * \brief Calculator test cases (std::uint8_t register).
 */
inline constexpr auto CALCULATOR_TEST_CASES_8 = std::array<Calculator_Test_Case<std::uint8_t>, 12>{ {
    // clang-format off

    { { 0x31, 0x00, false, false, 0x00 }, "123456789", 0xA2 },
    { { 0x31, 0xFF, false, false, 0x00 }, "123456789", 0x29 },
    { { 0x31, 0x00,  true,  true, 0x00 }, "123456789", 0xA1 },
    { { 0x31, 0xFF,  true,  true, 0x00 }, "123456789", 0x70 },
    { { 0x31, 0x00, false, false, 0xFF }, "123456789", 0x5D },
    { { 0x31, 0xFF, false, false, 0xFF }, "123456789", 0xD6 },
    { { 0x31, 0x00,  true,  true, 0xFF }, "123456789", 0x5E },
    { { 0x31, 0xFF,  true,  true, 0xFF }, "123456789", 0x8F },

    { { 0xBF, 0x83, false, false, 0x3F }, "Mjqszr9",        0x9E },
    { { 0x86, 0x4B, false,  true, 0x6A }, "h5j23yM7Ueo7Dl", 0x52 },
    { { 0xCA, 0x45,  true, false, 0x47 }, "mJJlUYvF4f",     0xC1 },
    { { 0xDF, 0x2E,  true,  true, 0x96 }, "xkthGXxV",       0x61 },

    // clang-format on
} };

/**
 * \brief Calculator test cases (std::uint16_t register).
 */
inline constexpr auto CALCULATOR_TEST_CASES_16 = std::array<Calculator_Test_Case<std::uint16_t>, 12>{ {
    // clang-format off

    { { 0x1021, 0x0000, false, false, 0x0000 }, "123456789", 0x31C3 },
    { { 0x1021, 0xFFFF, false, false, 0x0000 }, "123456789", 0xE5CC },
    { { 0x1021, 0x0000,  true,  true, 0x0000 }, "123456789", 0x2189 },
    { { 0x1021, 0xFFFF,  true,  true, 0x0000 }, "123456789", 0xD1A2 },
    { { 0x1021, 0x0000, false, false, 0xFFFF }, "123456789", 0xCE3C },
    { { 0x1021, 0xFFFF, false, false, 0xFFFF }, "123456789", 0x1A33 },
    { { 0x1021, 0x0000,  true,  true, 0xFFFF }, "123456789", 0xDE76 },
    { { 0x1021, 0xFFFF,  true,  true, 0xFFFF }, "123456789", 0x2E5D },

    { { 0xAF8E, 0x0D44, false, false, 0x2BE3 }, "Mjqszr9",        0xE331 },
    { { 0x26EA, 0xE190, false,  true, 0xEC80 }, "h5j23yM7Ueo7Dl", 0xA206 },
    { { 0x85A5, 0x0F80,  true, false, 0xD15F }, "mJJlUYvF4f",     0xAE0C },
    { { 0x13BF, 0xD0B4,  true,  true, 0x860B }, "xkthGXxV",       0x8697 },

    // clang-format on
} };

/**
 * \brief Calculator test cases (std::uint32_t register).
 */
inline constexpr auto CALCULATOR_TEST_CASES_32 = std::array<Calculator_Test_Case<std::uint32_t>, 12>{ {
    // clang-format off

    { { 0x04C11DB7, 0x00000000, false, false, 0x00000000 }, "123456789", 0x89A1897F },
    { { 0x04C11DB7, 0xFFFFFFFF, false, false, 0x00000000 }, "123456789", 0x373C5870 },
    { { 0x04C11DB7, 0x00000000,  true,  true, 0x00000000 }, "123456789", 0x2DFD2D88 },
    { { 0x04C11DB7, 0xFFFFFFFF,  true,  true, 0x00000000 }, "123456789", 0xDD7694F5 },
    { { 0x04C11DB7, 0x00000000, false, false, 0xFFFFFFFF }, "123456789", 0x765E7680 },
    { { 0x04C11DB7, 0xFFFFFFFF, false, false, 0xFFFFFFFF }, "123456789", 0xC8C3A78F },
    { { 0x04C11DB7, 0x00000000,  true,  true, 0xFFFFFFFF }, "123456789", 0xD202D277 },
    { { 0x04C11DB7, 0xFFFFFFFF,  true,  true, 0xFFFFFFFF }, "123456789", 0x22896B0A },

    { { 0x02839EF9, 0x5980E453, false, false, 0x40984B74 }, "Mjqszr9",        0x6990B082 },
    { { 0x020A6474, 0x5B2B8BA8, false,  true, 0x09DB56B7 }, "h5j23yM7Ueo7Dl", 0x217D2BB0 },
    { { 0x96D0AC21, 0xDC2CE27E,  true, false, 0xBD315863 }, "mJJlUYvF4f",     0x7A534C63 },
    { { 0x3454DAF7, 0xD438BD2A,  true,  true, 0x7D1CDAE6 }, "xkthGXxV",       0xDEF3467B },

    // clang-format on
} };

} // namespace picolibrary::Testing::Automated::CRC

#endif // PICOLIBRARY_TESTING_AUTOMATED_CRC_H

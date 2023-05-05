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
 * \brief picolibrary::CRC automated test program.
 */

#include <cstdint>
#include <iomanip>
#include <ios>
#include <ostream>
#include <string>
#include <tuple>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/crc.h"

namespace {

using ::picolibrary::CRC::Augmented_Byte_Indexed_Lookup_Table_Calculator;
using ::picolibrary::CRC::Augmented_Nibble_Indexed_Lookup_Table_Calculator;
using ::picolibrary::CRC::Bitwise_Calculator;
using ::picolibrary::CRC::Calculation_Parameters;
using ::picolibrary::CRC::Direct_Byte_Indexed_Lookup_Table_Calculator;
using ::picolibrary::CRC::Direct_Nibble_Indexed_Lookup_Table_Calculator;
using ::testing::Combine;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

} // namespace

namespace picolibrary::CRC {

template<typename Register>
auto operator<<( std::ostream & stream, Calculation_Parameters<Register> const & calculation_parameters )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".polynomial = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<Register>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast32_t>( calculation_parameters.polynomial )
                  << ", "
                  << ".initial_remainder = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<Register>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast32_t>( calculation_parameters.initial_remainder )
                  << ", "
                  << ".input_is_reflected = " << std::boolalpha << calculation_parameters.input_is_reflected
                  << ", "
                  << ".output_is_reflected = " << std::boolalpha << calculation_parameters.output_is_reflected
                  << ", "
                  << ".xor_output = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<Register>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast32_t>( calculation_parameters.xor_output )
                  << " }";

    // clang-format on
}

} // namespace picolibrary::CRC

/**
 * \brief Messages.
 */
std::string const MESSAGES[]{
    // clang-format off

    "pEbRTqL8ry3h",
    "iCyd1eZWb",
    "02CpY1okk4Lp",
    "RQbAMSgHuX5mAWw",
    "mkUxjYpECMG",

    // clang-format on
};

/**
 * \brief Calculation parameters (std::uint8_t register).
 */
Calculation_Parameters<std::uint8_t> const CALCULATION_PARAMETERS_8[]{
    // clang-format off

    { 0x7E, 0xD4, false, false, 0x1D },
    { 0x7E, 0xD4, false,  true, 0x1D },
    { 0x7E, 0xD4,  true, false, 0x1D },
    { 0x7E, 0xD4,  true,  true, 0x1D },

    { 0x68, 0x12, false, false, 0x2C },
    { 0x68, 0x12, false,  true, 0x2C },
    { 0x68, 0x12,  true, false, 0x2C },
    { 0x68, 0x12,  true,  true, 0x2C },

    { 0x20, 0xDB, false, false, 0xD5 },
    { 0x20, 0xDB, false,  true, 0xD5 },
    { 0x20, 0xDB,  true, false, 0xD5 },
    { 0x20, 0xDB,  true,  true, 0xD5 },

    { 0xBE, 0x39, false, false, 0x74 },
    { 0xBE, 0x39, false,  true, 0x74 },
    { 0xBE, 0x39,  true, false, 0x74 },
    { 0xBE, 0x39,  true,  true, 0x74 },

    { 0x68, 0x1F, false, false, 0x12 },
    { 0x68, 0x1F, false,  true, 0x12 },
    { 0x68, 0x1F,  true, false, 0x12 },
    { 0x68, 0x1F,  true,  true, 0x12 },

    // clang-format on
};

/**
 * \brief std::uint8_t register test fixture.
 */
class calculatorImplementations8 :
    public TestWithParam<std::tuple<Calculation_Parameters<std::uint8_t>, std::string>> {
};

INSTANTIATE_TEST_SUITE_P(
    testCases,
    calculatorImplementations8,
    Combine( ValuesIn( CALCULATION_PARAMETERS_8 ), ValuesIn( MESSAGES ) ) );

/**
 * \brief Verify calculator implementations are equivalent.
 */
TEST_P( calculatorImplementations8, areEquivalent )
{
    auto const [ calculation_parameters, message ] = GetParam();

    auto const remainder = Bitwise_Calculator{ calculation_parameters }.calculate(
        message.begin(), message.end() );

    EXPECT_EQ(
        Augmented_Byte_Indexed_Lookup_Table_Calculator{ calculation_parameters }.calculate(
            message.begin(), message.end() ),
        remainder );
    EXPECT_EQ(
        Direct_Byte_Indexed_Lookup_Table_Calculator{ calculation_parameters }.calculate(
            message.begin(), message.end() ),
        remainder );

    EXPECT_EQ(
        Augmented_Nibble_Indexed_Lookup_Table_Calculator{ calculation_parameters }.calculate(
            message.begin(), message.end() ),
        remainder );
    EXPECT_EQ(
        Direct_Nibble_Indexed_Lookup_Table_Calculator{ calculation_parameters }.calculate(
            message.begin(), message.end() ),
        remainder );
}

/**
 * \brief Calculation parameters (std::uint16_t register).
 */
Calculation_Parameters<std::uint16_t> const CALCULATION_PARAMETERS_16[]{
    // clang-format off

    { 0xD213, 0x1584, false, false, 0x8527 },
    { 0xD213, 0x1584, false,  true, 0x8527 },
    { 0xD213, 0x1584,  true, false, 0x8527 },
    { 0xD213, 0x1584,  true,  true, 0x8527 },

    { 0xF670, 0x8871, false, false, 0x5E39 },
    { 0xF670, 0x8871, false,  true, 0x5E39 },
    { 0xF670, 0x8871,  true, false, 0x5E39 },
    { 0xF670, 0x8871,  true,  true, 0x5E39 },

    { 0x151C, 0xAF6D, false, false, 0x6A3E },
    { 0x151C, 0xAF6D, false,  true, 0x6A3E },
    { 0x151C, 0xAF6D,  true, false, 0x6A3E },
    { 0x151C, 0xAF6D,  true,  true, 0x6A3E },

    { 0x2B6C, 0x90BD, false, false, 0x7A27 },
    { 0x2B6C, 0x90BD, false,  true, 0x7A27 },
    { 0x2B6C, 0x90BD,  true, false, 0x7A27 },
    { 0x2B6C, 0x90BD,  true,  true, 0x7A27 },

    { 0x4361, 0x9BB4, false, false, 0x1D4A },
    { 0x4361, 0x9BB4, false,  true, 0x1D4A },
    { 0x4361, 0x9BB4,  true, false, 0x1D4A },
    { 0x4361, 0x9BB4,  true,  true, 0x1D4A },

    // clang-format on
};

/**
 * \brief std::uint16_t register test fixture.
 */
class calculatorImplementations16 :
    public TestWithParam<std::tuple<Calculation_Parameters<std::uint16_t>, std::string>> {
};

INSTANTIATE_TEST_SUITE_P(
    testCases,
    calculatorImplementations16,
    Combine( ValuesIn( CALCULATION_PARAMETERS_16 ), ValuesIn( MESSAGES ) ) );

/**
 * \brief Verify calculator implementations are equivalent.
 */
TEST_P( calculatorImplementations16, areEquivalent )
{
    auto const [ calculation_parameters, message ] = GetParam();

    auto const remainder = Bitwise_Calculator{ calculation_parameters }.calculate(
        message.begin(), message.end() );

    EXPECT_EQ(
        Augmented_Byte_Indexed_Lookup_Table_Calculator{ calculation_parameters }.calculate(
            message.begin(), message.end() ),
        remainder );
    EXPECT_EQ(
        Direct_Byte_Indexed_Lookup_Table_Calculator{ calculation_parameters }.calculate(
            message.begin(), message.end() ),
        remainder );

    EXPECT_EQ(
        Augmented_Nibble_Indexed_Lookup_Table_Calculator{ calculation_parameters }.calculate(
            message.begin(), message.end() ),
        remainder );
    EXPECT_EQ(
        Direct_Nibble_Indexed_Lookup_Table_Calculator{ calculation_parameters }.calculate(
            message.begin(), message.end() ),
        remainder );
}

/**
 * \brief Calculation parameters (std::uint32_t register).
 */
Calculation_Parameters<std::uint32_t> const CALCULATION_PARAMETERS_32[]{
    // clang-format off

    { 0x841DFEDA, 0xFCBAE630, false, false, 0x604B4B11 },
    { 0x841DFEDA, 0xFCBAE630, false,  true, 0x604B4B11 },
    { 0x841DFEDA, 0xFCBAE630,  true, false, 0x604B4B11 },
    { 0x841DFEDA, 0xFCBAE630,  true,  true, 0x604B4B11 },

    { 0x81922144, 0x98B6D229, false, false, 0xB7206F43 },
    { 0x81922144, 0x98B6D229, false,  true, 0xB7206F43 },
    { 0x81922144, 0x98B6D229,  true, false, 0xB7206F43 },
    { 0x81922144, 0x98B6D229,  true,  true, 0xB7206F43 },

    { 0x0D1DBA29, 0xD355BB6F, false, false, 0x1CAC9747 },
    { 0x0D1DBA29, 0xD355BB6F, false,  true, 0x1CAC9747 },
    { 0x0D1DBA29, 0xD355BB6F,  true, false, 0x1CAC9747 },
    { 0x0D1DBA29, 0xD355BB6F,  true,  true, 0x1CAC9747 },

    { 0xFA4B867F, 0x1A716D73, false, false, 0xA6B4C805 },
    { 0xFA4B867F, 0x1A716D73, false,  true, 0xA6B4C805 },
    { 0xFA4B867F, 0x1A716D73,  true, false, 0xA6B4C805 },
    { 0xFA4B867F, 0x1A716D73,  true,  true, 0xA6B4C805 },

    { 0xEE0AF003, 0xB90EE33C, false, false, 0xE1D76444 },
    { 0xEE0AF003, 0xB90EE33C, false,  true, 0xE1D76444 },
    { 0xEE0AF003, 0xB90EE33C,  true, false, 0xE1D76444 },
    { 0xEE0AF003, 0xB90EE33C,  true,  true, 0xE1D76444 },

    // clang-format on
};

/**
 * \brief std::uint32_t register test fixture.
 */
class calculatorImplementations32 :
    public TestWithParam<std::tuple<Calculation_Parameters<std::uint32_t>, std::string>> {
};

INSTANTIATE_TEST_SUITE_P(
    testCases,
    calculatorImplementations32,
    Combine( ValuesIn( CALCULATION_PARAMETERS_32 ), ValuesIn( MESSAGES ) ) );

/**
 * \brief Verify calculator implementations are equivalent.
 */
TEST_P( calculatorImplementations32, areEquivalent )
{
    auto const [ calculation_parameters, message ] = GetParam();

    auto const remainder = Bitwise_Calculator{ calculation_parameters }.calculate(
        message.begin(), message.end() );

    EXPECT_EQ(
        Augmented_Byte_Indexed_Lookup_Table_Calculator{ calculation_parameters }.calculate(
            message.begin(), message.end() ),
        remainder );
    EXPECT_EQ(
        Direct_Byte_Indexed_Lookup_Table_Calculator{ calculation_parameters }.calculate(
            message.begin(), message.end() ),
        remainder );

    EXPECT_EQ(
        Augmented_Nibble_Indexed_Lookup_Table_Calculator{ calculation_parameters }.calculate(
            message.begin(), message.end() ),
        remainder );
    EXPECT_EQ(
        Direct_Nibble_Indexed_Lookup_Table_Calculator{ calculation_parameters }.calculate(
            message.begin(), message.end() ),
        remainder );
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

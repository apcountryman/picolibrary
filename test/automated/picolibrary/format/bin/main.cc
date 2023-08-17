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
 * \brief picolibrary::Format::Bin automated test program.
 */

#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/format.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/stream.h"

namespace {

using ::picolibrary::Format::Bin;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::Mock_Output_Stream;
using ::picolibrary::Testing::Automated::Output_String_Stream;
using ::picolibrary::Testing::Automated::Reliable_Output_String_Stream;
using ::testing::A;
using ::testing::Return;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

} // namespace

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) properly handles a put error.
 */
TEST( outputFormatterFormatBinPrintOutputStreamErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 216 };

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( Bin{ 0b11000010010001110100010101101011 } );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print() test
 *        case.
 *
 * \tparam Integer The type of integer to format.
 */
template<typename Integer>
struct outputFormatterFormatBinPrint_Test_Case {
    /**
     * \brief The integer to be formatted.
     */
    Integer value;

    /**
     * \brief The formatted integer.
     */
    std::string_view bin;
};

template<typename Integer>
auto operator<<( std::ostream & stream, outputFormatterFormatBinPrint_Test_Case<Integer> const & test_case )
    -> std::ostream &
{
    return stream << test_case.bin;
}

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print()
 *        std::int8_t test cases.
 */
outputFormatterFormatBinPrint_Test_Case<std::int8_t> const outputFormatterFormatBinPrintI8_TEST_CASES[]{
    // clang-format off

    { static_cast<std::int8_t>( 0b00000000 ), "0b00000000" },
    { static_cast<std::int8_t>( 0b11111111 ), "0b11111111" },

    { static_cast<std::int8_t>( 0b10101110 ), "0b10101110" },
    { static_cast<std::int8_t>( 0b01001100 ), "0b01001100" },
    { static_cast<std::int8_t>( 0b00100010 ), "0b00100010" },
    { static_cast<std::int8_t>( 0b01100001 ), "0b01100001" },
    { static_cast<std::int8_t>( 0b00110011 ), "0b00110011" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) std::int8_t test fixture.
 */
class outputFormatterFormatBinPrintOutputStreamI8 :
    public TestWithParam<outputFormatterFormatBinPrint_Test_Case<std::int8_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatBinPrintOutputStreamI8, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( Bin{ test_case.value } );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.bin );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatBinPrintOutputStreamI8, ValuesIn( outputFormatterFormatBinPrintI8_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) std::int8_t test fixture.
 */
class outputFormatterFormatBinPrintReliableOutputStreamI8 :
    public TestWithParam<outputFormatterFormatBinPrint_Test_Case<std::int8_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatBinPrintReliableOutputStreamI8, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( Bin{ test_case.value } );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.bin );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatBinPrintReliableOutputStreamI8, ValuesIn( outputFormatterFormatBinPrintI8_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print()
 *        std::uint8_t test cases.
 */
outputFormatterFormatBinPrint_Test_Case<std::uint8_t> const outputFormatterFormatBinPrintU8_TEST_CASES[]{
    // clang-format off

    { 0b00000000, "0b00000000" },
    { 0b11111111, "0b11111111" },

    { 0b10101110, "0b10101110" },
    { 0b01001100, "0b01001100" },
    { 0b00100010, "0b00100010" },
    { 0b01100001, "0b01100001" },
    { 0b00110011, "0b00110011" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) std::uint8_t test fixture.
 */
class outputFormatterFormatBinPrintOutputStreamU8 :
    public TestWithParam<outputFormatterFormatBinPrint_Test_Case<std::uint8_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatBinPrintOutputStreamU8, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( Bin{ test_case.value } );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.bin );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatBinPrintOutputStreamU8, ValuesIn( outputFormatterFormatBinPrintU8_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) std::uint8_t test fixture.
 */
class outputFormatterFormatBinPrintReliableOutputStreamU8 :
    public TestWithParam<outputFormatterFormatBinPrint_Test_Case<std::uint8_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatBinPrintReliableOutputStreamU8, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( Bin{ test_case.value } );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.bin );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatBinPrintReliableOutputStreamU8, ValuesIn( outputFormatterFormatBinPrintU8_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print()
 *        std::int16_t test cases.
 */
outputFormatterFormatBinPrint_Test_Case<std::int16_t> const outputFormatterFormatBinPrintI16_TEST_CASES[]{
    // clang-format off

    { static_cast<std::int16_t>( 0b0000000000000000 ), "0b0000000000000000" },
    { static_cast<std::int16_t>( 0b1111111111111111 ), "0b1111111111111111" },

    { static_cast<std::int16_t>( 0b1100101000000110 ), "0b1100101000000110" },
    { static_cast<std::int16_t>( 0b0110000000011111 ), "0b0110000000011111" },
    { static_cast<std::int16_t>( 0b0011100110110100 ), "0b0011100110110100" },
    { static_cast<std::int16_t>( 0b0010000110100010 ), "0b0010000110100010" },
    { static_cast<std::int16_t>( 0b1100111110100011 ), "0b1100111110100011" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) std::int16_t test fixture.
 */
class outputFormatterFormatBinPrintOutputStreamI16 :
    public TestWithParam<outputFormatterFormatBinPrint_Test_Case<std::int16_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatBinPrintOutputStreamI16, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( Bin{ test_case.value } );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.bin );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatBinPrintOutputStreamI16, ValuesIn( outputFormatterFormatBinPrintI16_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) std::int16_t test fixture.
 */
class outputFormatterFormatBinPrintReliableOutputStreamI16 :
    public TestWithParam<outputFormatterFormatBinPrint_Test_Case<std::int16_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatBinPrintReliableOutputStreamI16, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( Bin{ test_case.value } );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.bin );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatBinPrintReliableOutputStreamI16, ValuesIn( outputFormatterFormatBinPrintI16_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print()
 *        std::uint16_t test cases.
 */
outputFormatterFormatBinPrint_Test_Case<std::uint16_t> const outputFormatterFormatBinPrintU16_TEST_CASES[]{
    // clang-format off

    { 0b0000000000000000, "0b0000000000000000" },
    { 0b1111111111111111, "0b1111111111111111" },

    { 0b1100101000000110, "0b1100101000000110" },
    { 0b0110000000011111, "0b0110000000011111" },
    { 0b0011100110110100, "0b0011100110110100" },
    { 0b0010000110100010, "0b0010000110100010" },
    { 0b1100111110100011, "0b1100111110100011" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) std::uint16_t test fixture.
 */
class outputFormatterFormatBinPrintOutputStreamU16 :
    public TestWithParam<outputFormatterFormatBinPrint_Test_Case<std::uint16_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatBinPrintOutputStreamU16, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( Bin{ test_case.value } );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.bin );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatBinPrintOutputStreamU16, ValuesIn( outputFormatterFormatBinPrintU16_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) std::uint16_t test fixture.
 */
class outputFormatterFormatBinPrintReliableOutputStreamU16 :
    public TestWithParam<outputFormatterFormatBinPrint_Test_Case<std::uint16_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatBinPrintReliableOutputStreamU16, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( Bin{ test_case.value } );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.bin );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatBinPrintReliableOutputStreamU16, ValuesIn( outputFormatterFormatBinPrintU16_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print()
 *        std::int32_t test cases.
 */
outputFormatterFormatBinPrint_Test_Case<std::int32_t> const outputFormatterFormatBinPrintI32_TEST_CASES[]{
    // clang-format off

    { static_cast<std::int32_t>( 0b00000000000000000000000000000000 ), "0b00000000000000000000000000000000" },
    { static_cast<std::int32_t>( 0b11111111111111111111111111111111 ), "0b11111111111111111111111111111111" },

    { static_cast<std::int32_t>( 0b01111000000011100010000000011110 ), "0b01111000000011100010000000011110" },
    { static_cast<std::int32_t>( 0b00101110111011110111101100000000 ), "0b00101110111011110111101100000000" },
    { static_cast<std::int32_t>( 0b10101011001010001111000101010010 ), "0b10101011001010001111000101010010" },
    { static_cast<std::int32_t>( 0b11100010000110100110101010011001 ), "0b11100010000110100110101010011001" },
    { static_cast<std::int32_t>( 0b11011001110010011000010111111100 ), "0b11011001110010011000010111111100" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) std::int32_t test fixture.
 */
class outputFormatterFormatBinPrintOutputStreamI32 :
    public TestWithParam<outputFormatterFormatBinPrint_Test_Case<std::int32_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatBinPrintOutputStreamI32, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( Bin{ test_case.value } );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.bin );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatBinPrintOutputStreamI32, ValuesIn( outputFormatterFormatBinPrintI32_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) std::int32_t test fixture.
 */
class outputFormatterFormatBinPrintReliableOutputStreamI32 :
    public TestWithParam<outputFormatterFormatBinPrint_Test_Case<std::int32_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatBinPrintReliableOutputStreamI32, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( Bin{ test_case.value } );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.bin );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatBinPrintReliableOutputStreamI32, ValuesIn( outputFormatterFormatBinPrintI32_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print()
 *        std::uint32_t test cases.
 */
outputFormatterFormatBinPrint_Test_Case<std::uint32_t> const outputFormatterFormatBinPrintU32_TEST_CASES[]{
    // clang-format off

    { 0b00000000000000000000000000000000, "0b00000000000000000000000000000000" },
    { 0b11111111111111111111111111111111, "0b11111111111111111111111111111111" },

    { 0b01111000000011100010000000011110, "0b01111000000011100010000000011110" },
    { 0b00101110111011110111101100000000, "0b00101110111011110111101100000000" },
    { 0b10101011001010001111000101010010, "0b10101011001010001111000101010010" },
    { 0b11100010000110100110101010011001, "0b11100010000110100110101010011001" },
    { 0b11011001110010011000010111111100, "0b11011001110010011000010111111100" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) std::uint32_t test fixture.
 */
class outputFormatterFormatBinPrintOutputStreamU32 :
    public TestWithParam<outputFormatterFormatBinPrint_Test_Case<std::uint32_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatBinPrintOutputStreamU32, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( Bin{ test_case.value } );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.bin );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatBinPrintOutputStreamU32, ValuesIn( outputFormatterFormatBinPrintU32_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) std::uint32_t test fixture.
 */
class outputFormatterFormatBinPrintReliableOutputStreamU32 :
    public TestWithParam<outputFormatterFormatBinPrint_Test_Case<std::uint32_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatBinPrintReliableOutputStreamU32, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( Bin{ test_case.value } );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.bin );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatBinPrintReliableOutputStreamU32, ValuesIn( outputFormatterFormatBinPrintU32_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print()
 *        std::int64_t test cases.
 */
outputFormatterFormatBinPrint_Test_Case<std::int64_t> const outputFormatterFormatBinPrintI64_TEST_CASES[]{
    // clang-format off

    { static_cast<std::int64_t>( 0b0000000000000000000000000000000000000000000000000000000000000000 ), "0b0000000000000000000000000000000000000000000000000000000000000000" },
    { static_cast<std::int64_t>( 0b1111111111111111111111111111111111111111111111111111111111111111 ), "0b1111111111111111111111111111111111111111111111111111111111111111" },

    { static_cast<std::int64_t>( 0b0100100100001001011001110111111001010010001110011010111001011001 ), "0b0100100100001001011001110111111001010010001110011010111001011001" },
    { static_cast<std::int64_t>( 0b0000101101010101011010110101001101100010011011111010101011101000 ), "0b0000101101010101011010110101001101100010011011111010101011101000" },
    { static_cast<std::int64_t>( 0b0001011000011000100000111001100110000101010001100101000010001110 ), "0b0001011000011000100000111001100110000101010001100101000010001110" },
    { static_cast<std::int64_t>( 0b1111100001110010111110011111101010101111101010000001001110001100 ), "0b1111100001110010111110011111101010101111101010000001001110001100" },
    { static_cast<std::int64_t>( 0b1110001100011011000111000010110011110011101101010011010100100011 ), "0b1110001100011011000111000010110011110011101101010011010100100011" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) std::int64_t test fixture.
 */
class outputFormatterFormatBinPrintOutputStreamI64 :
    public TestWithParam<outputFormatterFormatBinPrint_Test_Case<std::int64_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatBinPrintOutputStreamI64, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( Bin{ test_case.value } );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.bin );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatBinPrintOutputStreamI64, ValuesIn( outputFormatterFormatBinPrintI64_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) std::int64_t test fixture.
 */
class outputFormatterFormatBinPrintReliableOutputStreamI64 :
    public TestWithParam<outputFormatterFormatBinPrint_Test_Case<std::int64_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatBinPrintReliableOutputStreamI64, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( Bin{ test_case.value } );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.bin );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatBinPrintReliableOutputStreamI64, ValuesIn( outputFormatterFormatBinPrintI64_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print()
 *        std::uint64_t test cases.
 */
outputFormatterFormatBinPrint_Test_Case<std::uint64_t> const outputFormatterFormatBinPrintU64_TEST_CASES[]{
    // clang-format off

    { 0b0000000000000000000000000000000000000000000000000000000000000000, "0b0000000000000000000000000000000000000000000000000000000000000000" },
    { 0b1111111111111111111111111111111111111111111111111111111111111111, "0b1111111111111111111111111111111111111111111111111111111111111111" },

    { 0b0100100100001001011001110111111001010010001110011010111001011001, "0b0100100100001001011001110111111001010010001110011010111001011001" },
    { 0b0000101101010101011010110101001101100010011011111010101011101000, "0b0000101101010101011010110101001101100010011011111010101011101000" },
    { 0b0001011000011000100000111001100110000101010001100101000010001110, "0b0001011000011000100000111001100110000101010001100101000010001110" },
    { 0b1111100001110010111110011111101010101111101010000001001110001100, "0b1111100001110010111110011111101010101111101010000001001110001100" },
    { 0b1110001100011011000111000010110011110011101101010011010100100011, "0b1110001100011011000111000010110011110011101101010011010100100011" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) std::uint64_t test fixture.
 */
class outputFormatterFormatBinPrintOutputStreamU64 :
    public TestWithParam<outputFormatterFormatBinPrint_Test_Case<std::uint64_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatBinPrintOutputStreamU64, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( Bin{ test_case.value } );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.bin );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatBinPrintOutputStreamU64, ValuesIn( outputFormatterFormatBinPrintU64_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) std::uint64_t test fixture.
 */
class outputFormatterFormatBinPrintReliableOutputStreamU64 :
    public TestWithParam<outputFormatterFormatBinPrint_Test_Case<std::uint64_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Bin<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatBinPrintReliableOutputStreamU64, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( Bin{ test_case.value } );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.bin );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatBinPrintReliableOutputStreamU64, ValuesIn( outputFormatterFormatBinPrintU64_TEST_CASES ) );

/**
 * \brief Execute the picolibrary::Format::Bin automated tests.
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

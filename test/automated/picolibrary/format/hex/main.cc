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
 * \brief picolibrary::Format::Hex automated test program.
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

using ::picolibrary::Format::Hex;
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
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) properly handles a put error.
 */
TEST( outputFormatterFormatHexPrintOutputStreamErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 216 };

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( Hex{ 0x48B18626 } );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print() test
 *        case.
 *
 * \tparam Integer The type of integer to format.
 */
template<typename Integer>
struct outputFormatterFormatHexPrint_Test_Case {
    /**
     * \brief The integer to be formatted.
     */
    Integer value;

    /**
     * \brief The formatted integer.
     */
    std::string_view hex;
};

template<typename Integer>
auto operator<<( std::ostream & stream, outputFormatterFormatHexPrint_Test_Case<Integer> const & test_case )
    -> std::ostream &
{
    return stream << test_case.hex;
}

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print()
 *        std::int8_t test cases.
 */
outputFormatterFormatHexPrint_Test_Case<std::int8_t> const outputFormatterFormatHexPrintI8_TEST_CASES[]{
    // clang-format off

    { static_cast<std::int8_t>( 0x00 ), "0x00" },
    { static_cast<std::int8_t>( 0xFF ), "0xFF" },

    { static_cast<std::int8_t>( 0xDE ), "0xDE" },
    { static_cast<std::int8_t>( 0x09 ), "0x09" },
    { static_cast<std::int8_t>( 0x94 ), "0x94" },
    { static_cast<std::int8_t>( 0xA4 ), "0xA4" },
    { static_cast<std::int8_t>( 0x1B ), "0x1B" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) std::int8_t test fixture.
 */
class outputFormatterFormatHexPrintOutputStreamI8 :
    public TestWithParam<outputFormatterFormatHexPrint_Test_Case<std::int8_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatHexPrintOutputStreamI8, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( Hex{ test_case.value } );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.hex );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatHexPrintOutputStreamI8, ValuesIn( outputFormatterFormatHexPrintI8_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) std::int8_t test fixture.
 */
class outputFormatterFormatHexPrintReliableOutputStreamI8 :
    public TestWithParam<outputFormatterFormatHexPrint_Test_Case<std::int8_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatHexPrintReliableOutputStreamI8, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( Hex{ test_case.value } );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.hex );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatHexPrintReliableOutputStreamI8, ValuesIn( outputFormatterFormatHexPrintI8_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print()
 *        std::uint8_t test cases.
 */
outputFormatterFormatHexPrint_Test_Case<std::uint8_t> const outputFormatterFormatHexPrintU8_TEST_CASES[]{
    // clang-format off

    { 0x00, "0x00" },
    { 0xFF, "0xFF" },

    { 0xDE, "0xDE" },
    { 0x09, "0x09" },
    { 0x94, "0x94" },
    { 0xA4, "0xA4" },
    { 0x1B, "0x1B" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) std::uint8_t test fixture.
 */
class outputFormatterFormatHexPrintOutputStreamU8 :
    public TestWithParam<outputFormatterFormatHexPrint_Test_Case<std::uint8_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatHexPrintOutputStreamU8, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( Hex{ test_case.value } );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.hex );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatHexPrintOutputStreamU8, ValuesIn( outputFormatterFormatHexPrintU8_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) std::uint8_t test fixture.
 */
class outputFormatterFormatHexPrintReliableOutputStreamU8 :
    public TestWithParam<outputFormatterFormatHexPrint_Test_Case<std::uint8_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatHexPrintReliableOutputStreamU8, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( Hex{ test_case.value } );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.hex );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatHexPrintReliableOutputStreamU8, ValuesIn( outputFormatterFormatHexPrintU8_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print()
 *        std::int16_t test cases.
 */
outputFormatterFormatHexPrint_Test_Case<std::int16_t> const outputFormatterFormatHexPrintI16_TEST_CASES[]{
    // clang-format off

    { static_cast<std::int16_t>( 0x0000 ), "0x0000" },
    { static_cast<std::int16_t>( 0xFFFF ), "0xFFFF" },

    { static_cast<std::int16_t>( 0x2CDA ), "0x2CDA" },
    { static_cast<std::int16_t>( 0x42B0 ), "0x42B0" },
    { static_cast<std::int16_t>( 0xFC2B ), "0xFC2B" },
    { static_cast<std::int16_t>( 0xF18A ), "0xF18A" },
    { static_cast<std::int16_t>( 0x6763 ), "0x6763" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) std::int16_t test fixture.
 */
class outputFormatterFormatHexPrintOutputStreamI16 :
    public TestWithParam<outputFormatterFormatHexPrint_Test_Case<std::int16_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatHexPrintOutputStreamI16, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( Hex{ test_case.value } );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.hex );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatHexPrintOutputStreamI16, ValuesIn( outputFormatterFormatHexPrintI16_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) std::int16_t test fixture.
 */
class outputFormatterFormatHexPrintReliableOutputStreamI16 :
    public TestWithParam<outputFormatterFormatHexPrint_Test_Case<std::int16_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatHexPrintReliableOutputStreamI16, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( Hex{ test_case.value } );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.hex );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatHexPrintReliableOutputStreamI16, ValuesIn( outputFormatterFormatHexPrintI16_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print()
 *        std::uint16_t test cases.
 */
outputFormatterFormatHexPrint_Test_Case<std::uint16_t> const outputFormatterFormatHexPrintU16_TEST_CASES[]{
    // clang-format off

    { 0x0000, "0x0000" },
    { 0xFFFF, "0xFFFF" },

    { 0x2CDA, "0x2CDA" },
    { 0x42B0, "0x42B0" },
    { 0xFC2B, "0xFC2B" },
    { 0xF18A, "0xF18A" },
    { 0x6763, "0x6763" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) std::uint16_t test fixture.
 */
class outputFormatterFormatHexPrintOutputStreamU16 :
    public TestWithParam<outputFormatterFormatHexPrint_Test_Case<std::uint16_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatHexPrintOutputStreamU16, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( Hex{ test_case.value } );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.hex );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatHexPrintOutputStreamU16, ValuesIn( outputFormatterFormatHexPrintU16_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) std::uint16_t test fixture.
 */
class outputFormatterFormatHexPrintReliableOutputStreamU16 :
    public TestWithParam<outputFormatterFormatHexPrint_Test_Case<std::uint16_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatHexPrintReliableOutputStreamU16, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( Hex{ test_case.value } );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.hex );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatHexPrintReliableOutputStreamU16, ValuesIn( outputFormatterFormatHexPrintU16_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print()
 *        std::int32_t test cases.
 */
outputFormatterFormatHexPrint_Test_Case<std::int32_t> const outputFormatterFormatHexPrintI32_TEST_CASES[]{
    // clang-format off

    { static_cast<std::int32_t>( 0x00000000 ), "0x00000000" },
    { static_cast<std::int32_t>( 0xFFFFFFFF ), "0xFFFFFFFF" },

    { static_cast<std::int32_t>( 0x2E4F2EE2 ), "0x2E4F2EE2" },
    { static_cast<std::int32_t>( 0xB5AF945E ), "0xB5AF945E" },
    { static_cast<std::int32_t>( 0x1E930001 ), "0x1E930001" },
    { static_cast<std::int32_t>( 0xD026CA0D ), "0xD026CA0D" },
    { static_cast<std::int32_t>( 0x673CFA82 ), "0x673CFA82" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) std::int32_t test fixture.
 */
class outputFormatterFormatHexPrintOutputStreamI32 :
    public TestWithParam<outputFormatterFormatHexPrint_Test_Case<std::int32_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatHexPrintOutputStreamI32, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( Hex{ test_case.value } );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.hex );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatHexPrintOutputStreamI32, ValuesIn( outputFormatterFormatHexPrintI32_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) std::int32_t test fixture.
 */
class outputFormatterFormatHexPrintReliableOutputStreamI32 :
    public TestWithParam<outputFormatterFormatHexPrint_Test_Case<std::int32_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatHexPrintReliableOutputStreamI32, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( Hex{ test_case.value } );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.hex );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatHexPrintReliableOutputStreamI32, ValuesIn( outputFormatterFormatHexPrintI32_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print()
 *        std::uint32_t test cases.
 */
outputFormatterFormatHexPrint_Test_Case<std::uint32_t> const outputFormatterFormatHexPrintU32_TEST_CASES[]{
    // clang-format off

    { 0x00000000, "0x00000000" },
    { 0xFFFFFFFF, "0xFFFFFFFF" },

    { 0x2E4F2EE2, "0x2E4F2EE2" },
    { 0xB5AF945E, "0xB5AF945E" },
    { 0x1E930001, "0x1E930001" },
    { 0xD026CA0D, "0xD026CA0D" },
    { 0x673CFA82, "0x673CFA82" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) std::uint32_t test fixture.
 */
class outputFormatterFormatHexPrintOutputStreamU32 :
    public TestWithParam<outputFormatterFormatHexPrint_Test_Case<std::uint32_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatHexPrintOutputStreamU32, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( Hex{ test_case.value } );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.hex );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatHexPrintOutputStreamU32, ValuesIn( outputFormatterFormatHexPrintU32_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) std::uint32_t test fixture.
 */
class outputFormatterFormatHexPrintReliableOutputStreamU32 :
    public TestWithParam<outputFormatterFormatHexPrint_Test_Case<std::uint32_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatHexPrintReliableOutputStreamU32, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( Hex{ test_case.value } );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.hex );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatHexPrintReliableOutputStreamU32, ValuesIn( outputFormatterFormatHexPrintU32_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print()
 *        std::int64_t test cases.
 */
outputFormatterFormatHexPrint_Test_Case<std::int64_t> const outputFormatterFormatHexPrintI64_TEST_CASES[]{
    // clang-format off

    { static_cast<std::int64_t>( 0x0000000000000000 ), "0x0000000000000000" },
    { static_cast<std::int64_t>( 0xFFFFFFFFFFFFFFFF ), "0xFFFFFFFFFFFFFFFF" },

    { static_cast<std::int64_t>( 0x3E4665B932D16863 ), "0x3E4665B932D16863" },
    { static_cast<std::int64_t>( 0xF9A347EFFC3C99B0 ), "0xF9A347EFFC3C99B0" },
    { static_cast<std::int64_t>( 0x7D3769A5D58C13D6 ), "0x7D3769A5D58C13D6" },
    { static_cast<std::int64_t>( 0x81BA96326031461B ), "0x81BA96326031461B" },
    { static_cast<std::int64_t>( 0x67951EE6E5A92E01 ), "0x67951EE6E5A92E01" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) std::int64_t test fixture.
 */
class outputFormatterFormatHexPrintOutputStreamI64 :
    public TestWithParam<outputFormatterFormatHexPrint_Test_Case<std::int64_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatHexPrintOutputStreamI64, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( Hex{ test_case.value } );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.hex );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatHexPrintOutputStreamI64, ValuesIn( outputFormatterFormatHexPrintI64_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) std::int64_t test fixture.
 */
class outputFormatterFormatHexPrintReliableOutputStreamI64 :
    public TestWithParam<outputFormatterFormatHexPrint_Test_Case<std::int64_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatHexPrintReliableOutputStreamI64, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( Hex{ test_case.value } );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.hex );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatHexPrintReliableOutputStreamI64, ValuesIn( outputFormatterFormatHexPrintI64_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print()
 *        std::uint64_t test cases.
 */
outputFormatterFormatHexPrint_Test_Case<std::uint64_t> const outputFormatterFormatHexPrintU64_TEST_CASES[]{
    // clang-format off

    { 0x0000000000000000, "0x0000000000000000" },
    { 0xFFFFFFFFFFFFFFFF, "0xFFFFFFFFFFFFFFFF" },

    { 0x3E4665B932D16863, "0x3E4665B932D16863" },
    { 0xF9A347EFFC3C99B0, "0xF9A347EFFC3C99B0" },
    { 0x7D3769A5D58C13D6, "0x7D3769A5D58C13D6" },
    { 0x81BA96326031461B, "0x81BA96326031461B" },
    { 0x67951EE6E5A92E01, "0x67951EE6E5A92E01" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) std::uint64_t test fixture.
 */
class outputFormatterFormatHexPrintOutputStreamU64 :
    public TestWithParam<outputFormatterFormatHexPrint_Test_Case<std::uint64_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatHexPrintOutputStreamU64, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( Hex{ test_case.value } );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.hex );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatHexPrintOutputStreamU64, ValuesIn( outputFormatterFormatHexPrintU64_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) std::uint64_t test fixture.
 */
class outputFormatterFormatHexPrintReliableOutputStreamU64 :
    public TestWithParam<outputFormatterFormatHexPrint_Test_Case<std::uint64_t>> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Hex<Integer>>::print(
 *        picolibrary::Reliable_Output_Stream &, Integer ) works properly.
 */
TEST_P( outputFormatterFormatHexPrintReliableOutputStreamU64, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( Hex{ test_case.value } );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.hex );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatHexPrintReliableOutputStreamU64, ValuesIn( outputFormatterFormatHexPrintU64_TEST_CASES ) );

/**
 * \brief Execute the picolibrary::Format::Hex automated tests.
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

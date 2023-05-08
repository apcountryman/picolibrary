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
 * \brief picolibrary::Format::Hex_Dump automated test program.
 */

#include <ostream>
#include <string>
#include <string_view>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/format.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/stream.h"

namespace {

using ::picolibrary::Format::Hex_Dump;
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
 * \brief Verify picolibrary::Output_Formatter<Hex_Dump<Iterator>>::print(
 *        picolibrary::Output_Stream &, picolibrary::Format::Hex_Dump<Iterator> const & )
 *        properly handles a put error.
 */
TEST( outputFormatterFormatHexDumpPrintOutputStreamErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 153 };

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const data = std::string_view{ "odMJz0Qd28QFM2" };

    auto const result = stream.print( Hex_Dump{ data.begin(), data.end() } );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief picolibrary::Output_Formatter<Hex_Dump<Iterator>>::print() test case.
 */
struct outputFormatterFormatHexDumpPrint_Test_Case {
    /**
     * \brief The data to dump.
     */
    std::string_view data;

    /**
     * \brief The hex dump.
     */
    std::string_view hex_dump;
};

auto operator<<( std::ostream & stream, outputFormatterFormatHexDumpPrint_Test_Case const & test_case )
    -> std::ostream &
{
    return stream << test_case.data;
}

/**
 * \brief picolibrary::Output_Formatter<Hex_Dump<Iterator>>::print() test cases.
 */
outputFormatterFormatHexDumpPrint_Test_Case const outputFormatterFormatHexDumpPrint_TEST_CASES[]{
    // clang-format off

        {
            "",
            "",
        },
        {
            "(Y !d5vz\t^2",
            "0000000000000000  28 59 20 21 64 35 76 7A 09 5E 32                 |(Y !d5vz.^2|     \n"
        },
        {
            ":X;27N8u]hde[e&+",
            "0000000000000000  3A 58 3B 32 37 4E 38 75 5D 68 64 65 5B 65 26 2B  |:X;27N8u]hde[e&+|\n"
        },
        {
            "{yZZk7V!/{>fm[lxV!$e|:",
            "0000000000000000  7B 79 5A 5A 6B 37 56 21 2F 7B 3E 66 6D 5B 6C 78  |{yZZk7V!/{>fm[lx|\n"
            "0000000000000010  56 21 24 65 7C 3A                                |V!$e|:|          \n"
        },
        {
            "/B>wiGoUZ|6cjO(_`T.8jV:RxSUssq!L",
            "0000000000000000  2F 42 3E 77 69 47 6F 55 5A 7C 36 63 6A 4F 28 5F  |/B>wiGoUZ|6cjO(_|\n"
            "0000000000000010  60 54 2E 38 6A 56 3A 52 78 53 55 73 73 71 21 4C  |`T.8jV:RxSUssq!L|\n"
        },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<Hex_Dump<Iterator>>::print(
 *        picolibrary::Output_Stream &, picolibrary::Format::Hex_Dump<Iterator> const & )
 *        test fixture.
 */
class outputFormatterFormatHexDumpPrintOutputStream :
    public TestWithParam<outputFormatterFormatHexDumpPrint_Test_Case> {
};

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterFormatHexDumpPrintOutputStream, ValuesIn( outputFormatterFormatHexDumpPrint_TEST_CASES ) );

/**
 * \brief Verify picolibrary::Output_Formatter<Hex_Dump<Iterator>>::print(
 *        picolibrary::Output_Stream &, picolibrary::Format::Hex_Dump<Iterator> const & )
 *        works properly.
 */
TEST_P( outputFormatterFormatHexDumpPrintOutputStream, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( Hex_Dump{ test_case.data.begin(), test_case.data.end() } );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.hex_dump );
}

/**
 * \brief picolibrary::Output_Formatter<Hex_Dump<Iterator>>::print(
 *        picolibrary::Reliable_Output_Stream &, picolibrary::Format::Hex_Dump<Iterator>
 *        const & ) test fixture.
 */
class outputFormatterFormatHexDumpPrintReliableOutputStream :
    public TestWithParam<outputFormatterFormatHexDumpPrint_Test_Case> {
};

INSTANTIATE_TEST_SUITE_P(
    testCases,
    outputFormatterFormatHexDumpPrintReliableOutputStream,
    ValuesIn( outputFormatterFormatHexDumpPrint_TEST_CASES ) );

/**
 * \brief Verify picolibrary::Output_Formatter<Hex_Dump<Iterator>>::print(
 *        picolibrary::Reliable_Output_Stream &, picolibrary::Format::Hex_Dump<Iterator>
 *        const & ) works properly.
 */
TEST_P( outputFormatterFormatHexDumpPrintReliableOutputStream, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( Hex_Dump{ test_case.data.begin(), test_case.data.end() } );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.hex_dump );
}

/**
 * \brief Execute the picolibrary::Format::Hex_Dump automated tests.
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

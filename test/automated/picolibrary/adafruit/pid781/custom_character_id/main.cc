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
 * \brief picolibrary::Adafruit::PID781::Custom_Character_ID automated test program.
 */

#include <cstdint>
#include <ostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/adafruit/pid781.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/automated/adafruit/pid781.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/stream.h"

namespace {

using ::picolibrary::Result;
using ::picolibrary::Adafruit::PID781::Custom_Character_ID;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::Mock_Output_Stream;
using ::picolibrary::Testing::Automated::Mock_Reliable_Output_Stream;
using ::testing::A;
using ::testing::Eq;
using ::testing::Return;
using ::testing::SafeMatcherCast;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

} // namespace

/**
 * \brief Verify
 *        picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character_ID>::print(
 *        picolibrary::Output_Stream &, picolibrary::Adafruit::PID781::Custom_Character_ID
 *        ) properly handles a put error.
 */
TEST( outputFormatterAdafruitPID781CustomCharacterIDPrintOutputStreamErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 170 };

    EXPECT_CALL( stream.buffer(), put( A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( Custom_Character_ID::_4 );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character_ID>::print()
 *        test case.
 */
struct outputFormatterAdafruitPID781CustomCharacterIDPrint_Test_Case {
    /**
     * \brief The picolibrary::Adafruit::PID781::Custom_Character_ID to format.
     */
    Custom_Character_ID custom_character_id;

    /**
     * \brief The unsigned byte written to the stream.
     */
    std::uint8_t value;
};

auto operator<<( std::ostream & stream, outputFormatterAdafruitPID781CustomCharacterIDPrint_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".custom_character_id = " << test_case.custom_character_id
                  << ", "
                  << ".value = " << static_cast<std::uint_fast16_t>( test_case.value )
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character_ID>::print()
 *        test cases.
 */
outputFormatterAdafruitPID781CustomCharacterIDPrint_Test_Case const outputFormatterAdafruitPID781CustomCharacterIDPrint_TEST_CASES[]{
    // clang-format off

    { Custom_Character_ID::_0, 0 },
    { Custom_Character_ID::_1, 1 },
    { Custom_Character_ID::_2, 2 },
    { Custom_Character_ID::_3, 3 },
    { Custom_Character_ID::_4, 4 },
    { Custom_Character_ID::_5, 5 },
    { Custom_Character_ID::_6, 6 },
    { Custom_Character_ID::_7, 7 },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character_ID>::print(
 *        picolibrary::Output_Stream &, picolibrary::Adafruit::PID781::Custom_Character_ID
 *        ) test fixture.
 */
class outputFormatterAdafruitPID781CustomCharacterIDPrintOutputStream :
    public TestWithParam<outputFormatterAdafruitPID781CustomCharacterIDPrint_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character_ID>::print(
 *        picolibrary::Output_Stream &, picolibrary::Adafruit::PID781::Custom_Character_ID
 *        ) works properly.
 */
TEST_P( outputFormatterAdafruitPID781CustomCharacterIDPrintOutputStream, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Mock_Output_Stream{};

    EXPECT_CALL( stream.buffer(), put( SafeMatcherCast<std::uint8_t>( Eq( test_case.value ) ) ) )
        .WillOnce( Return( Result<void>{} ) );

    auto const result = stream.print( test_case.custom_character_id );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), 1 );

    EXPECT_TRUE( stream.is_nominal() );
}

INSTANTIATE_TEST_SUITE_P(
    testCases,
    outputFormatterAdafruitPID781CustomCharacterIDPrintOutputStream,
    ValuesIn( outputFormatterAdafruitPID781CustomCharacterIDPrint_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character_ID>::print(
 *        picolibrary::Reliable_Output_Stream &,
 *        picolibrary::Adafruit::PID781::Custom_Character_ID ) test fixture.
 */
class outputFormatterAdafruitPID781CustomCharacterIDPrintReliableOutputStream :
    public TestWithParam<outputFormatterAdafruitPID781CustomCharacterIDPrint_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character_ID>::print(
 *        picolibrary::Reliable_Output_Stream &,
 *        picolibrary::Adafruit::PID781::Custom_Character_ID ) works properly.
 */
TEST_P( outputFormatterAdafruitPID781CustomCharacterIDPrintReliableOutputStream, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Mock_Reliable_Output_Stream{};

    EXPECT_CALL( stream.buffer(), put( SafeMatcherCast<std::uint8_t>( Eq( test_case.value ) ) ) );

    auto const n = stream.print( test_case.custom_character_id );

    EXPECT_EQ( n, 1 );

    EXPECT_TRUE( stream.is_nominal() );
}

INSTANTIATE_TEST_SUITE_P(
    testCases,
    outputFormatterAdafruitPID781CustomCharacterIDPrintReliableOutputStream,
    ValuesIn( outputFormatterAdafruitPID781CustomCharacterIDPrint_TEST_CASES ) );

/**
 * \brief Execute the picolibrary::Adafruit::PID781::Custom_Character_ID automated tests.
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

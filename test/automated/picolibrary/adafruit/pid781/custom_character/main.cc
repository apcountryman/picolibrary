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
 * \brief picolibrary::Adafruit::PID781::Custom_Character automated test program.
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
using ::picolibrary::Adafruit::PID781::Custom_Character;
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
 *        picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character>::print(
 *        picolibrary::Output_Stream &, picolibrary::Adafruit::PID781::Custom_Character
 *        const & ) properly handles a put error.
 */
TEST( outputFormatterAdafruitPID781CustomCharacterPrintOutputStreamErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 175 };

    EXPECT_CALL( stream.buffer(), put( A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( Custom_Character{ Custom_Character::Bank::_1,
                                                        Custom_Character::ID::_6,
                                                        {
                                                            0b01100100,
                                                            0b11010110,
                                                            0b01110000,
                                                            0b11010110,
                                                            0b11011110,
                                                            0b00010011,
                                                            0b00111000,
                                                            0b10111010,
                                                        } } );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character>::print(),
 *        and
 *        picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character::ID>::print()
 *        test case.
 */
struct outputFormatterPrint_Test_Case {
    /**
     * \brief The custom character's ID.
     */
    Custom_Character::ID id;

    /**
     * \brief The unsigned byte written to the stream.
     */
    std::uint8_t value;
};

auto operator<<( std::ostream & stream, outputFormatterPrint_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".id = " << test_case.id
                  << ", "
                  << ".value = " << static_cast<std::uint_fast16_t>( test_case.value )
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character>::print(),
 *        and
 *        picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character::ID>::print()
 *        test case.
 */
outputFormatterPrint_Test_Case const outputFormatterPrint_TEST_CASES[]{
    // clang-format off

    { Custom_Character::ID::_0, 0 },
    { Custom_Character::ID::_1, 1 },
    { Custom_Character::ID::_2, 2 },
    { Custom_Character::ID::_3, 3 },
    { Custom_Character::ID::_4, 4 },
    { Custom_Character::ID::_5, 5 },
    { Custom_Character::ID::_6, 6 },
    { Custom_Character::ID::_7, 7 },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character>::print(
 *        picolibrary::Output_Stream &, picolibrary::Adafruit::PID781::Custom_Character
 *        const & ) test fixture.
 */
class outputFormatterAdafruitPID781CustomCharacterPrintOutputStream :
    public TestWithParam<outputFormatterPrint_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character>::print(
 *        picolibrary::Output_Stream &, picolibrary::Adafruit::PID781::Custom_Character
 *        const & ) works properly.
 */
TEST_P( outputFormatterAdafruitPID781CustomCharacterPrintOutputStream, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Mock_Output_Stream{};

    EXPECT_CALL( stream.buffer(), put( SafeMatcherCast<std::uint8_t>( Eq( test_case.value ) ) ) )
        .WillOnce( Return( Result<void>{} ) );

    auto const result = stream.print( Custom_Character{ Custom_Character::Bank::_3,
                                                        test_case.id,
                                                        {
                                                            0b11010010,
                                                            0b00100100,
                                                            0b00110110,
                                                            0b00111110,
                                                            0b10100101,
                                                            0b01011000,
                                                            0b00001001,
                                                            0b01100000,
                                                        } } );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), 1 );

    EXPECT_TRUE( stream.is_nominal() );
}

INSTANTIATE_TEST_SUITE_P(
    testCases,
    outputFormatterAdafruitPID781CustomCharacterPrintOutputStream,
    ValuesIn( outputFormatterPrint_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character>::print(
 *        picolibrary::Reliable_Output_Stream &,
 *        picolibrary::Adafruit::PID781::Custom_Character const & ) test fixture.
 */
class outputFormatterAdafruitPID781CustomCharacterPrintReliableOutputStream :
    public TestWithParam<outputFormatterPrint_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character>::print(
 *        picolibrary::Reliable_Output_Stream &,
 *        picolibrary::Adafruit::PID781::Custom_Character const & ) works properly.
 */
TEST_P( outputFormatterAdafruitPID781CustomCharacterPrintReliableOutputStream, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Mock_Reliable_Output_Stream{};

    EXPECT_CALL( stream.buffer(), put( SafeMatcherCast<std::uint8_t>( Eq( test_case.value ) ) ) );

    auto const n = stream.print( Custom_Character{ Custom_Character::Bank::_2,
                                                   test_case.id,
                                                   {
                                                       0b01100110,
                                                       0b10000100,
                                                       0b10011011,
                                                       0b11100000,
                                                       0b01001101,
                                                       0b11101110,
                                                       0b10101010,
                                                       0b10000001,
                                                   } } );

    EXPECT_EQ( n, 1 );

    EXPECT_TRUE( stream.is_nominal() );
}

INSTANTIATE_TEST_SUITE_P(
    testCases,
    outputFormatterAdafruitPID781CustomCharacterPrintReliableOutputStream,
    ValuesIn( outputFormatterPrint_TEST_CASES ) );

/**
 * \brief Verify
 *        picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character::ID>::print(
 *        picolibrary::Output_Stream &,
 *        picolibrary::Adafruit::PID781::Custom_Character::ID const & ) properly handles a
 *        put error.
 */
TEST( outputFormatterAdafruitPID781CustomCharacterIDPrintOutputStreamErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 7 };

    EXPECT_CALL( stream.buffer(), put( A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( Custom_Character::ID::_5 );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character::ID>::print(
 *        picolibrary::Output_Stream &,
 *        picolibrary::Adafruit::PID781::Custom_Character::ID ) test fixture.
 */
class outputFormatterAdafruitPID781CustomCharacterIDPrintOutputStream :
    public TestWithParam<outputFormatterPrint_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character::ID>::print(
 *        picolibrary::Output_Stream &,
 *        picolibrary::Adafruit::PID781::Custom_Character::ID ) works properly.
 */
TEST_P( outputFormatterAdafruitPID781CustomCharacterIDPrintOutputStream, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Mock_Output_Stream{};

    EXPECT_CALL( stream.buffer(), put( SafeMatcherCast<std::uint8_t>( Eq( test_case.value ) ) ) )
        .WillOnce( Return( Result<void>{} ) );

    auto const result = stream.print( test_case.id );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), 1 );

    EXPECT_TRUE( stream.is_nominal() );
}

INSTANTIATE_TEST_SUITE_P(
    testCases,
    outputFormatterAdafruitPID781CustomCharacterIDPrintOutputStream,
    ValuesIn( outputFormatterPrint_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character::ID>::print(
 *        picolibrary::Reliable_Output_Stream &,
 *        picolibrary::Adafruit::PID781::Custom_Character::ID ) test fixture.
 */
class outputFormatterAdafruitPID781CustomCharacterIDPrintReliableOutputStream :
    public TestWithParam<outputFormatterPrint_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::Output_Formatter<picolibrary::Adafruit::PID781::Custom_Character::ID>::print(
 *        picolibrary::Reliable_Output_Stream &,
 *        picolibrary::Adafruit::PID781::Custom_Character::ID ) works properly.
 */
TEST_P( outputFormatterAdafruitPID781CustomCharacterIDPrintReliableOutputStream, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Mock_Reliable_Output_Stream{};

    EXPECT_CALL( stream.buffer(), put( SafeMatcherCast<std::uint8_t>( Eq( test_case.value ) ) ) );

    auto const n = stream.print( test_case.id );

    EXPECT_EQ( n, 1 );

    EXPECT_TRUE( stream.is_nominal() );
}

INSTANTIATE_TEST_SUITE_P(
    testCases,
    outputFormatterAdafruitPID781CustomCharacterIDPrintReliableOutputStream,
    ValuesIn( outputFormatterPrint_TEST_CASES ) );

/**
 * \brief Execute the picolibrary::Adafruit::PID781::Custom_Character automated tests.
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

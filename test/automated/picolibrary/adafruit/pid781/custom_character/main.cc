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
#include "picolibrary/testing/automated/adafruit/pid781.h"
#include "picolibrary/testing/automated/stream.h"

namespace {

using ::picolibrary::Adafruit::PID781::Custom_Character;
using ::picolibrary::Testing::Automated::Mock_Reliable_Output_Stream;
using ::testing::A;
using ::testing::Eq;
using ::testing::Return;
using ::testing::SafeMatcherCast;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

} // namespace

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
 *        test cases.
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

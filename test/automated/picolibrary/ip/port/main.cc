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
 * \brief picolibrary::IP::Port automated test program.
 */

#include <ios>
#include <ostream>
#include <string>
#include <string_view>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/ip.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/ip.h"
#include "picolibrary/testing/automated/stream.h"

namespace {

using ::picolibrary::IP::Port;
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
 * \brief Verify picolibrary::IP::Port::Port() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const port = Port{};

    EXPECT_TRUE( port.is_any() );
    EXPECT_EQ( port.as_unsigned_integer(), 0 );
}

/**
 * \brief picolibrary::IP::Port::Port( picolibrary::IP::Port::Unsigned_Integer ) test
 *        case.
 */
struct constructorUnsignedInteger_Test_Case {
    /**
     * \brief The port.
     */
    Port::Unsigned_Integer port;

    /**
     * \brief The port is the port that is used to represent any port (0).
     */
    bool is_any;
};

auto operator<<( std::ostream & stream, constructorUnsignedInteger_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".port = " << std::dec << test_case.port
                  << ", "
                  << ".is_any = " << std::boolalpha << test_case.is_any
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::IP::Port::Port( picolibrary::IP::Port::Unsigned_Integer ) test
 *        fixture.
 */
class constructorUnsignedInteger : public TestWithParam<constructorUnsignedInteger_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::Port::Port( picolibrary::IP::Port::Unsigned_Integer )
 *        works properly.
 */
TEST_P( constructorUnsignedInteger, worksProperly )
{
    auto const test_case = GetParam();

    auto const port = Port{ test_case.port };

    EXPECT_EQ( port.is_any(), test_case.is_any );
    EXPECT_EQ( port.as_unsigned_integer(), test_case.port );
}

/**
 * \brief picolibrary::IP::Port::Port( picolibrary::IP::Port::Unsigned_Integer ) test
 *        cases.
 */
constructorUnsignedInteger_Test_Case const constructorUnsignedInteger_TEST_CASES[]{
    // clang-format off

    {     0, true  },
    {     1, false },
    { 20711, false },
    { 65535, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, constructorUnsignedInteger, ValuesIn( constructorUnsignedInteger_TEST_CASES ) );

/**
 * \brief picolibrary::IP::operator==( picolibrary::IP::Port, picolibrary::IP::Port ),
 *        picolibrary::IP::operator!=( picolibrary::IP::Port, picolibrary::IP::Port ),
 *        picolibrary::IP::operator<( picolibrary::IP::Port, picolibrary::IP::Port),
 *        picolibrary::IP::operator>( picolibrary::IP::Port, picolibrary::IP::Port ),
 *        picolibrary::IP::operator<=( picolibrary::IP::Port, picolibrary::IP::Port ), and
 *        picolibrary::IP::operator>=( picolibrary::IP::Port, picolibrary::IP::Port ) test
 *        case.
 */
struct comparisonOperator_Test_Case {
    /**
     * \brief The left hand side of the comparison.
     */
    Port lhs;

    /**
     * \brief The right hand side of the comparison.
     */
    Port rhs;

    /**
     * \brief The comparison result.
     */
    bool comparison_result;
};

auto operator<<( std::ostream & stream, comparisonOperator_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".lhs = " << test_case.lhs
                  << ", "
                  << ".rhs = " << test_case.rhs
                  << ", "
                  << ".comparison_result = " << std::boolalpha << test_case.comparison_result
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::IP::operator==( picolibrary::IP::Port, picolibrary::IP::Port ) test
 *        fixture.
 */
class equalityOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::operator==( picolibrary::IP::Port, picolibrary::IP::Port
 *        ) works properly.
 */
TEST_P( equalityOperator, worksProperly )
{
    auto const test_case = GetParam();

    EXPECT_EQ( test_case.lhs == test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IP::operator==( picolibrary::IP::Port, picolibrary::IP::Port ) test
 *        cases.
 */
comparisonOperator_Test_Case const equalityOperator_TEST_CASES[]{
    // clang-format off

    {  1948, 10417, false },
    { 18347, 18348, false },
    { 18348, 18348, true  },
    { 18349, 18348, false },
    { 59618, 60750, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, equalityOperator, ValuesIn( equalityOperator_TEST_CASES ) );

/**
 * \brief picolibrary::IP::operator!=( picolibrary::IP::Port, picolibrary::IP::Port ) test
 *        fixture.
 */
class inequalityOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::operator!=( picolibrary::IP::Port, picolibrary::IP::Port
 *        ) works properly.
 */
TEST_P( inequalityOperator, worksProperly )
{
    auto const test_case = GetParam();

    EXPECT_EQ( test_case.lhs != test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IP::operator!=( picolibrary::IP::Port, picolibrary::IP::Port ) test
 *        cases.
 */
comparisonOperator_Test_Case const inequalityOperator_TEST_CASES[]{
    // clang-format off

    {  1948, 10417, true  },
    { 18347, 18348, true  },
    { 18348, 18348, false },
    { 18349, 18348, true  },
    { 59618, 60750, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, inequalityOperator, ValuesIn( inequalityOperator_TEST_CASES ) );

/**
 * \brief picolibrary::IP::operator<( picolibrary::IP::Port, picolibrary::IP::Port ) test
 *        fixture.
 */
class lessThanOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::operator<( picolibrary::IP::Port, picolibrary::IP::Port
 *        ) works properly.
 */
TEST_P( lessThanOperator, worksProperly )
{
    auto const test_case = GetParam();

    EXPECT_EQ( test_case.lhs < test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IP::operator<( picolibrary::IP::Port, picolibrary::IP::Port ) test
 *        cases.
 */
comparisonOperator_Test_Case const lessThanOperator_TEST_CASES[]{
    // clang-format off

    {  7444, 9706, true  },
    {  9705, 9706, true  },
    {  9706, 9706, false },
    {  9707, 9706, false },
    { 36246, 9706, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, lessThanOperator, ValuesIn( lessThanOperator_TEST_CASES ) );

/**
 * \brief picolibrary::IP::operator>( picolibrary::IP::Port, picolibrary::IP::Port ) test
 *        fixture.
 */
class greaterThanOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::operator>( picolibrary::IP::Port, picolibrary::IP::Port
 *        ) works properly.
 */
TEST_P( greaterThanOperator, worksProperly )
{
    auto const test_case = GetParam();

    EXPECT_EQ( test_case.lhs > test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IP::operator>( picolibrary::IP::Port, picolibrary::IP::Port ) test
 *        cases.
 */
comparisonOperator_Test_Case const greaterThanOperator_TEST_CASES[]{
    // clang-format off

    {  7444, 9706, false },
    {  9705, 9706, false },
    {  9706, 9706, false },
    {  9707, 9706, true  },
    { 36246, 9706, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, greaterThanOperator, ValuesIn( greaterThanOperator_TEST_CASES ) );

/**
 * \brief picolibrary::IP::operator<=( picolibrary::IP::Port, picolibrary::IP::Port ) test
 *        fixture.
 */
class lessThanOrEqualToOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::operator<=( picolibrary::IP::Port, picolibrary::IP::Port
 *        ) works properly.
 */
TEST_P( lessThanOrEqualToOperator, worksProperly )
{
    auto const test_case = GetParam();

    EXPECT_EQ( test_case.lhs <= test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IP::operator<=( picolibrary::IP::Port, picolibrary::IP::Port ) test
 *        cases.
 */
comparisonOperator_Test_Case const lessThanOrEqualToOperator_TEST_CASES[]{
    // clang-format off

    {  7444, 9706, true  },
    {  9705, 9706, true  },
    {  9706, 9706, true  },
    {  9707, 9706, false },
    { 36246, 9706, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, lessThanOrEqualToOperator, ValuesIn( lessThanOrEqualToOperator_TEST_CASES ) );

/**
 * \brief picolibrary::IP::operator>=( picolibrary::IP::Port, picolibrary::IP::Port ) test
 *        fixture.
 */
class greaterThanOrEqualToOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::operator>=( picolibrary::IP::Port, picolibrary::IP::Port
 *        ) works properly.
 */
TEST_P( greaterThanOrEqualToOperator, worksProperly )
{
    auto const test_case = GetParam();

    EXPECT_EQ( test_case.lhs >= test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IP::operator>=( picolibrary::IP::Port, picolibrary::IP::Port ) test
 *        cases.
 */
comparisonOperator_Test_Case const greaterThanOrEqualToOperator_TEST_CASES[]{
    // clang-format off

    {  7444, 9706, false },
    {  9705, 9706, false },
    {  9706, 9706, true  },
    {  9707, 9706, true  },
    { 36246, 9706, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, greaterThanOrEqualToOperator, ValuesIn( greaterThanOrEqualToOperator_TEST_CASES ) );

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IP::Port>::print(
 *        picolibrary::Output_Stream &, picolibrary::IP::Port ) properly handles a put
 *        error.
 */
TEST( outputFormatterIPPortPrintOutputStreamErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 148 };

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( Port{ 569 } );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief picolibrary::Output_Formatter<picolibrary::IP::Port>::print() test case.
 */
struct outputFormatterIPPortPrint_Test_Case {
    /**
     * \brief The picolibrary::IP::Port to format.
     */
    Port port;

    /**
     * \brief The formatted port.
     */
    std::string_view formatted_port;
};

auto operator<<( std::ostream & stream, outputFormatterIPPortPrint_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".port = " << test_case.port
                  << ", "
                  << ".formatted_port = " << test_case.formatted_port
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Output_Formatter<picolibrary::IP::Port>::print() test cases.
 */
outputFormatterIPPortPrint_Test_Case const outputFormatterIPPortPrint_TEST_CASES[]{
    // clang-format off

    {     0,     "0" },
    { 14535, "14535" },
    { 65535, "65535" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::IP::Port>::print(
 *        picolibrary::Output_Stream &, picolibrary::IP::Port ) test fixture.
 */
class outputFormatterIPPortPrintOutputStream :
    public TestWithParam<outputFormatterIPPortPrint_Test_Case> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IP::Port>::print(
 *        picolibrary::Output_Stream &, picolibrary::IP::Port ) works properly.
 */
TEST_P( outputFormatterIPPortPrintOutputStream, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( test_case.port );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.formatted_port );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterIPPortPrintOutputStream, ValuesIn( outputFormatterIPPortPrint_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::IP::Port>::print(
 *        picolibrary::Reliable_Output_Stream &, picolibrary::IP::Port ) test fixture.
 */
class outputFormatterIPPortPrintReliableOutputStream :
    public TestWithParam<outputFormatterIPPortPrint_Test_Case> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IP::Port>::print(
 *        picolibrary::Reliable_Output_Stream &, picolibrary::IP::Port ) works properly.
 */
TEST_P( outputFormatterIPPortPrintReliableOutputStream, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( test_case.port );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.formatted_port );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterIPPortPrintReliableOutputStream, ValuesIn( outputFormatterIPPortPrint_TEST_CASES ) );

/**
 * \brief Execute the picolibrary::IP::Port automated tests.
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

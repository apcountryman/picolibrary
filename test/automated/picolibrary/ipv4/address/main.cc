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
 * \brief picolibrary::IPv4::Address automated test program.
 */

#include <cstdint>
#include <iomanip>
#include <ios>
#include <limits>
#include <ostream>
#include <string>
#include <string_view>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/ipv4.h"
#include "picolibrary/testing/automated/stream.h"

namespace {

using ::picolibrary::IPv4::Address;
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
 * \brief Verify picolibrary::IPv4::Address::Address() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const address = Address{};

    EXPECT_TRUE( address.is_any() );
    EXPECT_FALSE( address.is_loopback() );
    EXPECT_FALSE( address.is_broadcast() );
    EXPECT_FALSE( address.is_multicast() );
    EXPECT_EQ( address.as_byte_array(), ( Address::Byte_Array{ 0, 0, 0, 0 } ) );
    EXPECT_EQ( address.as_unsigned_integer(), 0x00'00'00'00 );
}

/**
 * \brief picolibrary::IPv4::Address::Address( picolibrary::IPv4::Address::Byte_Array
 *        const & ) and picolibrary::IPv4::Address::Address(
 *        picolibrary::IPv4::Address::Unsigned_Integer const & ) test case.
 */
struct constructor_Test_Case {
    /**
     * \brief The address in its byte array representation.
     */
    Address::Byte_Array byte_array;

    /**
     * \brief The address in its unsigned integer representation.
     */
    Address::Unsigned_Integer unsigned_integer;

    /**
     * \brief The address is the address that is used to represent any address (0.0.0.0).
     */
    bool is_any;

    /**
     * \brief The address is a loopback address (127.0.0.0-127.255.255.255).
     */
    bool is_loopback;

    /**
     * \brief The address is the local network broadcast address (255.255.255.255).
     */
    bool is_broadcast;

    /**
     * \brief The address is a multicast address (224.0.0.0-239.255.255.255).
     */
    bool is_multicast;
};

auto operator<<( std::ostream & stream, constructor_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".byte_array = { " << static_cast<std::uint_fast16_t>( test_case.byte_array[ 0 ] ) << ", "
                                        << static_cast<std::uint_fast16_t>( test_case.byte_array[ 1 ] ) << ", "
                                        << static_cast<std::uint_fast16_t>( test_case.byte_array[ 2 ] ) << ", "
                                        << static_cast<std::uint_fast16_t>( test_case.byte_array[ 3 ] ) << " }"
                  << ", "
                  << ".unsigned_integer = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<Address::Unsigned_Integer>::digits / 4 ) << std::setfill( '0' ) << test_case.unsigned_integer
                  << ", "
                  << ".is_any = " << std::boolalpha << test_case.is_any
                  << ", "
                  << ".is_loopback = " << std::boolalpha << test_case.is_loopback
                  << ", "
                  << ".is_broadcast = " << std::boolalpha << test_case.is_broadcast
                  << ", "
                  << ".is_multicast = " << std::boolalpha << test_case.is_multicast
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::IPv4::Address::Address( picolibrary::IPv4::Address::Byte_Array
 *        const & ) and picolibrary::IPv4::Address::Address(
 *        picolibrary::IPv4::Address::Unsigned_Integer const & ) test cases.
 */
constructor_Test_Case const constructor_TEST_CASES[]{
    // clang-format off

    { { 0, 0, 0, 0 }, 0x00'00'00'00, true,  false, false, false },

    { {   0,   0,   0,   1 }, 0x00'00'00'01, false, false, false, false },
    { {  94, 251,  42,  60 }, 0x5E'FB'2A'3C, false, false, false, false },
    { { 126, 255, 255, 255 }, 0x7E'FF'FF'FF, false, false, false, false },

    { { 127,   0,   0,   0 }, 0x7F'00'00'00, false, true, false, false },
    { { 127, 187, 219, 128 }, 0x7F'BB'DB'80, false, true, false, false },
    { { 127, 255, 255, 255 }, 0x7F'FF'FF'FF, false, true, false, false },

    { { 128,   0,   0,   0 }, 0x80'00'00'00, false, false, false, false },
    { { 215, 243, 162, 166 }, 0xD7'F3'A2'A6, false, false, false, false },
    { { 223, 255, 255, 255 }, 0xDF'FF'FF'FF, false, false, false, false },

    { { 224,   0,   0,   0 }, 0xE0'00'00'00, false, false, false, true },
    { { 229, 172,  99,  89 }, 0xE5'AC'63'59, false, false, false, true },
    { { 239, 255, 255, 255 }, 0xEF'FF'FF'FF, false, false, false, true },

    { { 240,   0,   0,   0 }, 0xF0'00'00'00, false, false, false, false },
    { { 242, 147, 131, 229 }, 0xF2'93'83'E5, false, false, false, false },
    { { 255, 255, 255, 254 }, 0xFF'FF'FF'FE, false, false, false, false },

    { { 255, 255, 255, 255 }, 0xFF'FF'FF'FF, false, false, true, false },

    // clang-format on
};

/**
 * \brief picolibrary::IPv4::Address::Address( picolibrary::IPv4::Address::Byte_Array
 *        const & ) test fixture.
 */
class constructorByteArray : public TestWithParam<constructor_Test_Case> {
};

/**
 * \brief Verify picolibrary::IPv4::Address::Address(
 *        picolibrary::IPv4::Address::Byte_Array const & ) works properly.
 */
TEST_P( constructorByteArray, worksProperly )
{
    auto const test_case = GetParam();

    auto const address = Address{ test_case.byte_array };

    EXPECT_EQ( address.is_any(), test_case.is_any );
    EXPECT_EQ( address.is_loopback(), test_case.is_loopback );
    EXPECT_EQ( address.is_broadcast(), test_case.is_broadcast );
    EXPECT_EQ( address.is_multicast(), test_case.is_multicast );
    EXPECT_EQ( address.as_byte_array(), test_case.byte_array );
    EXPECT_EQ( address.as_unsigned_integer(), test_case.unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P( testCases, constructorByteArray, ValuesIn( constructor_TEST_CASES ) );

/**
 * \brief picolibrary::IPv4::Address::Address(
 *        picolibrary::IPv4::Address::Unsigned_Integer const & ) test fixture.
 */
class constructorUnsignedInteger : public TestWithParam<constructor_Test_Case> {
};

/**
 * \brief Verify picolibrary::IPv4::Address::Address(
 *        picolibrary::IPv4::Address::Unsigned_Integer const & ) works properly.
 */
TEST_P( constructorUnsignedInteger, worksProperly )
{
    auto const test_case = GetParam();

    auto const address = Address{ test_case.unsigned_integer };

    EXPECT_EQ( address.is_any(), test_case.is_any );
    EXPECT_EQ( address.is_loopback(), test_case.is_loopback );
    EXPECT_EQ( address.is_broadcast(), test_case.is_broadcast );
    EXPECT_EQ( address.is_multicast(), test_case.is_multicast );
    EXPECT_EQ( address.as_byte_array(), test_case.byte_array );
    EXPECT_EQ( address.as_unsigned_integer(), test_case.unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P( testCases, constructorUnsignedInteger, ValuesIn( constructor_TEST_CASES ) );

/**
 * \brief picolibrary::IPv4::operator==( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ), picolibrary::IPv4::operator!=(
 *        picolibrary::IPv4::Address const &, picolibrary::IPv4::Address const & ),
 *        picolibrary::IPv4::operator<( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ), picolibrary::IPv4::operator>(
 *        picolibrary::IPv4::Address const &, picolibrary::IPv4::Address const & ),
 *        picolibrary::IPv4::operator<=( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ), and picolibrary::IPv4::operator>=(
 *        picolibrary::IPv4::Address const &, picolibrary::IPv4::Address const & ) test
 *        case.
 */
struct comparisonOperator_Test_Case {
    /**
     * \brief The left hand side of the comparison.
     */
    Address lhs;

    /**
     * \brief The right hand side of the comparison.
     */
    Address rhs;

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
 * \brief picolibrary::IPv4::operator==( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) test fixture.
 */
class equalityOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IPv4::operator==( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) works properly.
 */
TEST_P( equalityOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs == test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IPv4::operator==( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) test cases.
 */
comparisonOperator_Test_Case const equalityOperator_TEST_CASES[]{
    // clang-format off

    { { {  24,  89, 245,  60 } }, { { 142, 148,  44,  38 } }, false },
    { { { 149, 175, 232,  78 } }, { { 149, 175, 232,  79 } }, false },
    { { { 149, 175, 232,  79 } }, { { 149, 175, 232,  79 } }, true  },
    { { { 149, 175, 232,  80 } }, { { 149, 175, 232,  79 } }, false },
    { { { 210,  90, 186, 128 } }, { { 219,  29,  34, 215 } }, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, equalityOperator, ValuesIn( equalityOperator_TEST_CASES ) );

/**
 * \brief picolibrary::IPv4::operator!=( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) test fixture.
 */
class inequalityOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IPv4::operator!=( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) works properly.
 */
TEST_P( inequalityOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs != test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IPv4::operator!=( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) test cases.
 */
comparisonOperator_Test_Case const inequalityOperator_TEST_CASES[]{
    // clang-format off

    { { {  24,  89, 245,  60 } }, { { 142, 148,  44,  38 } }, true  },
    { { { 149, 175, 232,  78 } }, { { 149, 175, 232,  79 } }, true  },
    { { { 149, 175, 232,  79 } }, { { 149, 175, 232,  79 } }, false },
    { { { 149, 175, 232,  80 } }, { { 149, 175, 232,  79 } }, true  },
    { { { 210,  90, 186, 128 } }, { { 219,  29,  34, 215 } }, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, inequalityOperator, ValuesIn( inequalityOperator_TEST_CASES ) );

/**
 * \brief picolibrary::IPv4::operator<( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) test fixture.
 */
class lessThanOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IPv4::operator<( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) works properly.
 */
TEST_P( lessThanOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs < test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IPv4::operator<( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) test cases.
 */
comparisonOperator_Test_Case const lessThanOperator_TEST_CASES[]{
    // clang-format off

    { { {  92, 178,  11, 221 } }, { { 146, 45, 171, 183 } }, true  },
    { { { 146,  45, 171, 182 } }, { { 146, 45, 171, 183 } }, true  },
    { { { 146,  45, 171, 183 } }, { { 146, 45, 171, 183 } }, false },
    { { { 146,  45, 171, 184 } }, { { 146, 45, 171, 183 } }, false },
    { { { 253, 213, 179,  93 } }, { { 146, 45, 171, 183 } }, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, lessThanOperator, ValuesIn( lessThanOperator_TEST_CASES ) );

/**
 * \brief picolibrary::IPv4::operator>( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) test fixture.
 */
class greaterThanOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IPv4::operator>( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) works properly.
 */
TEST_P( greaterThanOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs > test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IPv4::operator>( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) test cases.
 */
comparisonOperator_Test_Case const greaterThanOperator_TEST_CASES[]{
    // clang-format off

    { { {  92, 178,  11, 221 } }, { { 146, 45, 171, 183 } }, false },
    { { { 146,  45, 171, 182 } }, { { 146, 45, 171, 183 } }, false },
    { { { 146,  45, 171, 183 } }, { { 146, 45, 171, 183 } }, false },
    { { { 146,  45, 171, 184 } }, { { 146, 45, 171, 183 } }, true  },
    { { { 253, 213, 179,  93 } }, { { 146, 45, 171, 183 } }, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, greaterThanOperator, ValuesIn( greaterThanOperator_TEST_CASES ) );

/**
 * \brief picolibrary::IPv4::operator<=( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) test fixture.
 */
class lessThanOrEqualToOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IPv4::operator<=( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) works properly.
 */
TEST_P( lessThanOrEqualToOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs <= test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IPv4::operator<=( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) test cases.
 */
comparisonOperator_Test_Case const lessThanOrEqualToOperator_TEST_CASES[]{
    // clang-format off

    { { {  92, 178,  11, 221 } }, { { 146, 45, 171, 183 } }, true  },
    { { { 146,  45, 171, 182 } }, { { 146, 45, 171, 183 } }, true  },
    { { { 146,  45, 171, 183 } }, { { 146, 45, 171, 183 } }, true  },
    { { { 146,  45, 171, 184 } }, { { 146, 45, 171, 183 } }, false },
    { { { 253, 213, 179,  93 } }, { { 146, 45, 171, 183 } }, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, lessThanOrEqualToOperator, ValuesIn( lessThanOrEqualToOperator_TEST_CASES ) );

/**
 * \brief picolibrary::IPv4::operator>=( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) test fixture.
 */
class greaterThanOrEqualToOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IPv4::operator>=( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) works properly.
 */
TEST_P( greaterThanOrEqualToOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs >= test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IPv4::operator>=( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) test cases.
 */
comparisonOperator_Test_Case const greaterThanOrEqualToOperator_TEST_CASES[]{
    // clang-format off

    { { {  92, 178,  11, 221 } }, { { 146, 45, 171, 183 } }, false },
    { { { 146,  45, 171, 182 } }, { { 146, 45, 171, 183 } }, false },
    { { { 146,  45, 171, 183 } }, { { 146, 45, 171, 183 } }, true  },
    { { { 146,  45, 171, 184 } }, { { 146, 45, 171, 183 } }, true  },
    { { { 253, 213, 179,  93 } }, { { 146, 45, 171, 183 } }, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, greaterThanOrEqualToOperator, ValuesIn( greaterThanOrEqualToOperator_TEST_CASES ) );

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IPv4::Address>::print(
 *        picolibrary::Output_Stream &, picolibrary::IPv4::Address const & ) properly
 *        handles a put error.
 */
TEST( outputFormatterIPv4AddressPrintOutputStreamErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 98 };

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( Address{ { 198, 4, 177, 122 } } );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief picolibrary::Output_Formatter<picolibrary::IPv4::Address>::print() test case.
 */
struct outputFormatterIPv4AddressPrint_Test_Case {
    /**
     * \brief The picolibrary::IPv4::Address to format.
     */
    Address address;

    /**
     * \brief The formatted address.
     */
    std::string_view formatted_address;
};

auto operator<<( std::ostream & stream, outputFormatterIPv4AddressPrint_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".address = " << test_case.address
                  << ", "
                  << ".formatted_address = " << test_case.formatted_address
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Output_Formatter<picolibrary::IPv4::Address>::print() test cases.
 */
outputFormatterIPv4AddressPrint_Test_Case const outputFormatterIPv4AddressPrint_TEST_CASES[]{
    // clang-format off

    { { {   0,   0,   0,   0 } }, "0.0.0.0"         },
    { { { 101, 136, 170,  45 } }, "101.136.170.45"  },
    { { { 255, 255, 255, 255 } }, "255.255.255.255" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::IPv4::Address>::print(
 *        picolibrary::Output_Stream &, picolibrary::IPv4::Address const & ) test fixture.
 */
class outputFormatterIPv4AddressPrintOutputStream :
    public TestWithParam<outputFormatterIPv4AddressPrint_Test_Case> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IPv4::Address>::print(
 *        picolibrary::Output_Stream &, picolibrary::IPv4::Address const & ) works
 *        properly.
 */
TEST_P( outputFormatterIPv4AddressPrintOutputStream, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( test_case.address );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.formatted_address );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterIPv4AddressPrintOutputStream, ValuesIn( outputFormatterIPv4AddressPrint_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::IPv4::Address>::print(
 *        picolibrary::Reliable_Output_Stream &, picolibrary::IPv4::Address const & ) test
 *        fixture.
 */
class outputFormatterIPv4AddressPrintReliableOutputStream :
    public TestWithParam<outputFormatterIPv4AddressPrint_Test_Case> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IPv4::Address>::print(
 *        picolibrary::Reliable_Output_Stream &, picolibrary::IPv4::Address const & )
 *        works properly.
 */
TEST_P( outputFormatterIPv4AddressPrintReliableOutputStream, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( test_case.address );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.formatted_address );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterIPv4AddressPrintReliableOutputStream, ValuesIn( outputFormatterIPv4AddressPrint_TEST_CASES ) );

/**
 * \brief Execute the picolibrary::IPv4::Address automated tests.
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

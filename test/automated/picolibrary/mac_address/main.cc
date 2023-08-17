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
 * \brief picolibrary::MAC_Address automated test program.
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
#include "picolibrary/array.h"
#include "picolibrary/mac_address.h"
#include "picolibrary/precondition.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/mac_address.h"
#include "picolibrary/testing/automated/stream.h"

namespace {

using ::picolibrary::array_size_v;
using ::picolibrary::BYPASS_PRECONDITION_EXPECTATION_CHECKS;
using ::picolibrary::MAC_Address;
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
 * \brief Verify picolibrary::MAC_Address::MAC_Address() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const address = MAC_Address{};

    EXPECT_FALSE( address.is_locally_administered() );
    EXPECT_TRUE( address.is_universally_administered() );
    EXPECT_FALSE( address.is_multicast() );
    EXPECT_TRUE( address.is_unicast() );
    EXPECT_EQ( address.as_byte_array(), ( MAC_Address::Byte_Array{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } ) );
    EXPECT_EQ( address.as_unsigned_integer(), 0x00'00'00'00'00'00 );
}

/**
 * \brief picolibrary::MAC_Address::MAC_Address( picolibrary::MAC_Address::Byte_Array
 *        const & ), picolibrary::MAC_Address::MAC_Address(
 *        picolibrary::MAC_Address::Unsigned_Integer const & ), and
 *        picolibrary::MAC_Address::MAC_Address(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::MAC_Address::Unsigned_Integer const & ) test case.
 */
struct constructor_Test_Case {
    /**
     * \brief The address in its byte array representation.
     */
    MAC_Address::Byte_Array byte_array;

    /**
     * \brief The address in its unsigned integer representation.
     */
    MAC_Address::Unsigned_Integer unsigned_integer;

    /**
     * \brief The address is locally administered.
     */
    bool is_locally_administered;

    /**
     * \brief The address is universally administered.
     */
    bool is_universally_administered;

    /**
     * \brief The address is a multicast address.
     */
    bool is_multicast;

    /**
     * \brief The address is a unicast address.
     */
    bool is_unicast;
};

auto operator<<( std::ostream & stream, constructor_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".byte_array = { " << "0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( test_case.byte_array[ 0 ] ) << ", "
                                        << "0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( test_case.byte_array[ 1 ] ) << ", "
                                        << "0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( test_case.byte_array[ 2 ] ) << ", "
                                        << "0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( test_case.byte_array[ 3 ] ) << ", "
                                        << "0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( test_case.byte_array[ 4 ] ) << ", "
                                        << "0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( test_case.byte_array[ 5 ] ) << " }"
                  << ", "
                  << ".unsigned_integer = 0x" << std::hex << std::uppercase << std::setw( array_size_v<MAC_Address::Byte_Array> * std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << test_case.unsigned_integer
                  << ", "
                  << ".is_locally_administered = " << std::boolalpha << test_case.is_locally_administered
                  << ", "
                  << ".is_universally_administered = " << std::boolalpha << test_case.is_universally_administered
                  << ", "
                  << ".is_multicast = " << std::boolalpha << test_case.is_multicast
                  << ", "
                  << ".is_unicast = " << std::boolalpha << test_case.is_unicast
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::MAC_Address::MAC_Address( picolibrary::MAC_Address::Byte_Array
 *        const & ), picolibrary::MAC_Address::MAC_Address(
 *        picolibrary::MAC_Address::Unsigned_Integer const & ), and
 *        picolibrary::MAC_Address::MAC_Address(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::MAC_Address::Unsigned_Integer const & ) test cases.
 */
constructor_Test_Case const constructor_TEST_CASES[]{
    // clang-format off

    { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x00'00'00'00'00'00, false, true, false, true },

    { { 0x3C, 0xCA, 0xD1, 0xE0, 0xB9, 0xDE }, 0x3C'CA'D1'E0'B9'DE, false,  true, false,  true },
    { { 0x3D, 0xCA, 0xD1, 0xE0, 0xB9, 0xDE }, 0x3D'CA'D1'E0'B9'DE, false,  true,  true, false },
    { { 0x3E, 0xCA, 0xD1, 0xE0, 0xB9, 0xDE }, 0x3E'CA'D1'E0'B9'DE,  true, false, false,  true },
    { { 0x3F, 0xCA, 0xD1, 0xE0, 0xB9, 0xDE }, 0x3F'CA'D1'E0'B9'DE,  true, false,  true, false },

    { { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, 0xFF'FF'FF'FF'FF'FF, true, false, true, false },

    // clang-format on
};

/**
 * \brief picolibrary::MAC_Address::MAC_Address( picolibrary::MAC_Address::Byte_Array
 *        const & ) test fixture.
 */
class constructorByteArray : public TestWithParam<constructor_Test_Case> {
};

/**
 * \brief Verify picolibrary::MAC_Address::MAC_Address(
 *        picolibrary::MAC_Address::Byte_Array const & ) works properly.
 */
TEST_P( constructorByteArray, worksProperly )
{
    auto const test_case = GetParam();

    auto const address = MAC_Address{ test_case.byte_array };

    EXPECT_EQ( address.is_locally_administered(), test_case.is_locally_administered );
    EXPECT_EQ( address.is_universally_administered(), test_case.is_universally_administered );
    EXPECT_EQ( address.is_multicast(), test_case.is_multicast );
    EXPECT_EQ( address.is_unicast(), test_case.is_unicast );
    EXPECT_EQ( address.as_byte_array(), test_case.byte_array );
    EXPECT_EQ( address.as_unsigned_integer(), test_case.unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P( testCases, constructorByteArray, ValuesIn( constructor_TEST_CASES ) );

/**
 * \brief picolibrary::MAC_Address::MAC_Address(
 *        picolibrary::MAC_Address::Unsigned_Integer const & ) test fixture.
 */
class constructorUnsignedInteger : public TestWithParam<constructor_Test_Case> {
};

/**
 * \brief Verify picolibrary::MAC_Address::MAC_Address(
 *        picolibrary::MAC_Address::Unsigned_Integer const & ) works properly.
 */
TEST_P( constructorUnsignedInteger, worksProperly )
{
    auto const test_case = GetParam();

    auto const address = MAC_Address{ test_case.unsigned_integer };

    EXPECT_EQ( address.is_locally_administered(), test_case.is_locally_administered );
    EXPECT_EQ( address.is_universally_administered(), test_case.is_universally_administered );
    EXPECT_EQ( address.is_multicast(), test_case.is_multicast );
    EXPECT_EQ( address.is_unicast(), test_case.is_unicast );
    EXPECT_EQ( address.as_byte_array(), test_case.byte_array );
    EXPECT_EQ( address.as_unsigned_integer(), test_case.unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P( testCases, constructorUnsignedInteger, ValuesIn( constructor_TEST_CASES ) );

/**
 * \brief picolibrary::MAC_Address::MAC_Address(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::MAC_Address::Unsigned_Integer const & ) test fixture.
 */
class constructorBypassPreconditionExpectationChecksUnsignedInteger :
    public TestWithParam<constructor_Test_Case> {
};

/**
 * \brief Verify picolibrary::MAC_Address::MAC_Address(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::MAC_Address::Unsigned_Integer const & ) works properly.
 */
TEST_P( constructorBypassPreconditionExpectationChecksUnsignedInteger, worksProperly )
{
    auto const test_case = GetParam();

    auto const address = MAC_Address{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, test_case.unsigned_integer };

    EXPECT_EQ( address.is_locally_administered(), test_case.is_locally_administered );
    EXPECT_EQ( address.is_universally_administered(), test_case.is_universally_administered );
    EXPECT_EQ( address.is_multicast(), test_case.is_multicast );
    EXPECT_EQ( address.is_unicast(), test_case.is_unicast );
    EXPECT_EQ( address.as_byte_array(), test_case.byte_array );
    EXPECT_EQ( address.as_unsigned_integer(), test_case.unsigned_integer );
}

INSTANTIATE_TEST_SUITE_P( testCases, constructorBypassPreconditionExpectationChecksUnsignedInteger, ValuesIn( constructor_TEST_CASES ) );

/**
 * \brief picolibrary::operator==( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ), picolibrary::operator!=(
 *        picolibrary::MAC_Address const &, picolibrary::MAC_Address const & ),
 *        picolibrary::operator<( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ), picolibrary::operator>(
 *        picolibrary::MAC_Address const &, picolibrary::MAC_Address const & ),
 *        picolibrary::operator<=( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ), and picolibrary::operator>=(
 *        picolibrary::MAC_Address const &, picolibrary::MAC_Address const & ) test case.
 */
struct comparisonOperator_Test_Case {
    /**
     * \brief The left hand side of the comparison.
     */
    MAC_Address lhs;

    /**
     * \brief The right hand side of the comparison.
     */
    MAC_Address rhs;

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
 * \brief picolibrary::operator==( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) test fixture.
 */
class equalityOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::operator==( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) works properly.
 */
TEST_P( equalityOperator, worksProperly )
{
    auto const test_case = GetParam();

    EXPECT_EQ( test_case.lhs == test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::operator==( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) test cases.
 */
comparisonOperator_Test_Case const equalityOperator_TEST_CASES[]{
    // clang-format off

    { { { 0x25, 0x62, 0x9D, 0xF9, 0xAA, 0x5E } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, false },
    { { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2E } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, false },
    { { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, true  },
    { { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x30 } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, false },
    { { { 0xD4, 0xEA, 0x7A, 0x75, 0xED, 0xFA } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, equalityOperator, ValuesIn( equalityOperator_TEST_CASES ) );

/**
 * \brief picolibrary::operator!=( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) test fixture.
 */
class inequalityOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::operator!=( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) works properly.
 */
TEST_P( inequalityOperator, worksProperly )
{
    auto const test_case = GetParam();

    EXPECT_EQ( test_case.lhs != test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::operator!=( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) test cases.
 */
comparisonOperator_Test_Case const inequalityOperator_TEST_CASES[]{
    // clang-format off

    { { { 0x25, 0x62, 0x9D, 0xF9, 0xAA, 0x5E } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, true  },
    { { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2E } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, true  },
    { { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, false },
    { { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x30 } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, true  },
    { { { 0xD4, 0xEA, 0x7A, 0x75, 0xED, 0xFA } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, inequalityOperator, ValuesIn( inequalityOperator_TEST_CASES ) );

/**
 * \brief picolibrary::operator<( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) test fixture.
 */
class lessThanOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::operator<( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) works properly.
 */
TEST_P( lessThanOperator, worksProperly )
{
    auto const test_case = GetParam();

    EXPECT_EQ( test_case.lhs < test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::operator<( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) test cases.
 */
comparisonOperator_Test_Case const lessThanOperator_TEST_CASES[]{
    // clang-format off

    { { { 0x25, 0x62, 0x9D, 0xF9, 0xAA, 0x5E } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, true  },
    { { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2E } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, true  },
    { { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, false },
    { { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x30 } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, false },
    { { { 0xD4, 0xEA, 0x7A, 0x75, 0xED, 0xFA } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, lessThanOperator, ValuesIn( lessThanOperator_TEST_CASES ) );

/**
 * \brief picolibrary::operator>( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) test fixture.
 */
class greaterThanOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::operator>( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) works properly.
 */
TEST_P( greaterThanOperator, worksProperly )
{
    auto const test_case = GetParam();

    EXPECT_EQ( test_case.lhs > test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::operator>( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) test cases.
 */
comparisonOperator_Test_Case const greaterThanOperator_TEST_CASES[]{
    // clang-format off

    { { { 0x25, 0x62, 0x9D, 0xF9, 0xAA, 0x5E } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, false },
    { { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2E } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, false },
    { { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, false },
    { { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x30 } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, true  },
    { { { 0xD4, 0xEA, 0x7A, 0x75, 0xED, 0xFA } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, greaterThanOperator, ValuesIn( greaterThanOperator_TEST_CASES ) );

/**
 * \brief picolibrary::operator<=( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) test fixture.
 */
class lessThanOrEqualToOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::operator<=( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) works properly.
 */
TEST_P( lessThanOrEqualToOperator, worksProperly )
{
    auto const test_case = GetParam();

    EXPECT_EQ( test_case.lhs <= test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::operator<=( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) test cases.
 */
comparisonOperator_Test_Case const lessThanOrEqualToOperator_TEST_CASES[]{
    // clang-format off

    { { { 0x25, 0x62, 0x9D, 0xF9, 0xAA, 0x5E } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, true  },
    { { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2E } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, true  },
    { { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, true  },
    { { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x30 } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, false },
    { { { 0xD4, 0xEA, 0x7A, 0x75, 0xED, 0xFA } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, lessThanOrEqualToOperator, ValuesIn( lessThanOrEqualToOperator_TEST_CASES ) );

/**
 * \brief picolibrary::operator>=( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) test fixture.
 */
class greaterThanOrEqualToOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::operator>=( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) works properly.
 */
TEST_P( greaterThanOrEqualToOperator, worksProperly )
{
    auto const test_case = GetParam();

    EXPECT_EQ( test_case.lhs >= test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::operator>=( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) test cases.
 */
comparisonOperator_Test_Case const greaterThanOrEqualToOperator_TEST_CASES[]{
    // clang-format off

    { { { 0x25, 0x62, 0x9D, 0xF9, 0xAA, 0x5E } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, false },
    { { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2E } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, false },
    { { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, true  },
    { { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x30 } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, true  },
    { { { 0xD4, 0xEA, 0x7A, 0x75, 0xED, 0xFA } }, { { 0x68, 0x90, 0x0B, 0x76, 0x9F, 0x2F } }, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, greaterThanOrEqualToOperator, ValuesIn( greaterThanOrEqualToOperator_TEST_CASES ) );

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::MAC_Address>::print(
 *        picolibrary::Output_Stream &, picolibrary::MAC_Address const & ) properly
 *        handles a put error.
 */
TEST( outputFormatterMACAddressPrintOutputStreamErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 192 };

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( MAC_Address{ { 0xD2, 0x3F, 0x60, 0xBA, 0xBB, 0x33 } } );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief picolibrary::Output_Formatter<picolibrary::MAC_Address>::print() test case.
 */
struct outputFormatterMACAddressPrint_Test_Case {
    /**
     * \brief The picolibrary::MAC_Address to format.
     */
    MAC_Address address;

    /**
     * \brief The formatted address.
     */
    std::string_view formatted_address;
};

auto operator<<( std::ostream & stream, outputFormatterMACAddressPrint_Test_Case const & test_case )
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
 * \brief picolibrary::Output_Formatter<picolibrary::MAC_Address>::print() test cases.
 */
outputFormatterMACAddressPrint_Test_Case const outputFormatterMACAddressPrint_TEST_CASES[]{
    // clang-format off

    { { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } }, "00-00-00-00-00-00" },
    { { { 0x32, 0xBA, 0xCE, 0xA1, 0x32, 0xB5 } }, "32-BA-CE-A1-32-B5" },
    { { { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } }, "FF-FF-FF-FF-FF-FF" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::MAC_Address>::print(
 *        picolibrary::Output_Stream &, picolibrary::MAC_Address const & ) test fixture.
 */
class outputFormatterMACAddressPrintOutputStream :
    public TestWithParam<outputFormatterMACAddressPrint_Test_Case> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::MAC_Address>::print(
 *        picolibrary::Output_Stream &, picolibrary::MAC_Address const & ) works properly.
 */
TEST_P( outputFormatterMACAddressPrintOutputStream, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( test_case.address );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.formatted_address );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterMACAddressPrintOutputStream, ValuesIn( outputFormatterMACAddressPrint_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::MAC_Address>::print(
 *        picolibrary::Reliable_Output_Stream &, picolibrary::MAC_Address const & ) test
 *        fixture.
 */
class outputFormatterMACAddressPrintReliableOutputStream :
    public TestWithParam<outputFormatterMACAddressPrint_Test_Case> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::MAC_Address>::print(
 *        picolibrary::Reliable_Output_Stream &, picolibrary::MAC_Address const & ) works
 *        properly.
 */
TEST_P( outputFormatterMACAddressPrintReliableOutputStream, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( test_case.address );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.formatted_address );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterMACAddressPrintReliableOutputStream, ValuesIn( outputFormatterMACAddressPrint_TEST_CASES ) );

/**
 * \brief Execute the picolibrary::MAC_Address automated tests.
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

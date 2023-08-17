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
 * \brief picolibrary::IP::Address automated test program.
 */

#include <ios>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/ip.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/ip.h"
#include "picolibrary/testing/automated/ipv4.h"
#include "picolibrary/testing/automated/stream.h"

namespace {

using ::picolibrary::IP::Address;
using ::picolibrary::IP::Version;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::Mock_Output_Stream;
using ::picolibrary::Testing::Automated::Output_String_Stream;
using ::picolibrary::Testing::Automated::Reliable_Output_String_Stream;
using ::testing::A;
using ::testing::Return;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

using IPv4_Address = ::picolibrary::IPv4::Address;

} // namespace

/**
 * \brief Verify picolibrary::IP::Address::Address() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const address = Address{};

    EXPECT_EQ( address.version(), Version::UNSPECIFIED );
    EXPECT_TRUE( address.is_unspecified() );
    EXPECT_FALSE( address.is_ipv4() );
    EXPECT_TRUE( address.is_any() );
    EXPECT_FALSE( address.is_loopback() );
    EXPECT_FALSE( address.is_multicast() );
}

/**
 * \brief picolibrary::IP::Address::Address( picolibrary::IPv4::Address const & ) test
 *        case.
 */
struct constructorIPv4Address_Test_Case {
    /**
     * \brief The IPv4 address to store.
     */
    IPv4_Address ipv4_address;

    /**
     * \brief The IPv4 address is an address that is used to represent any address.
     */
    bool is_any;

    /**
     * \brief The IPv4 address is a loopback address.
     */
    bool is_loopback;

    /**
     * \brief The IPv4 address is a multicast address.
     */
    bool is_multicast;
};

auto operator<<( std::ostream & stream, constructorIPv4Address_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".ipv4_address = " << test_case.ipv4_address
                  << ", "
                  << ".is_any = " << std::boolalpha << test_case.is_any
                  << ", "
                  << ".is_loopback = " << std::boolalpha << test_case.is_loopback
                  << ", "
                  << ".is_multicast = " << std::boolalpha << test_case.is_multicast
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::IP::Address::Address( picolibrary::IPv4::Address const & ) test
 *        fixture.
 */
class constructorIPv4Address : public TestWithParam<constructorIPv4Address_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::Address::Address( picolibrary::IPv4::Address const & )
 *        works properly.
 */
TEST_P( constructorIPv4Address, worksProperly )
{
    auto const test_case = GetParam();

    auto const address = Address{ test_case.ipv4_address };

    EXPECT_EQ( address.version(), Version::_4 );
    EXPECT_FALSE( address.is_unspecified() );
    EXPECT_TRUE( address.is_ipv4() );
    EXPECT_EQ( address.is_any(), test_case.is_any );
    EXPECT_EQ( address.is_loopback(), test_case.is_loopback );
    EXPECT_EQ( address.is_multicast(), test_case.is_multicast );
    EXPECT_EQ( address.ipv4(), test_case.ipv4_address );
}

/**
 * \brief picolibrary::IP::Address::Address( picolibrary::IPv4::Address const & ) test
 *        cases.
 */
constructorIPv4Address_Test_Case const constructorIPv4Address_TEST_CASES[]{
    // clang-format off

    { { { 0, 0, 0, 0 } }, true, false, false },

    { { {   0,   0,   0,   1 } }, false, false, false },
    { { {  94, 251,  42,  60 } }, false, false, false },
    { { { 126, 255, 255, 255 } }, false, false, false },

    { { { 127,   0,   0,   0 } }, false, true, false },
    { { { 127, 187, 219, 128 } }, false, true, false },
    { { { 127, 255, 255, 255 } }, false, true, false },

    { { { 128,   0,   0,   0 } }, false, false, false },
    { { { 215, 243, 162, 166 } }, false, false, false },
    { { { 223, 255, 255, 255 } }, false, false, false },

    { { { 224,   0,   0,   0 } }, false, false, true },
    { { { 229, 172,  99,  89 } }, false, false, true },
    { { { 239, 255, 255, 255 } }, false, false, true },

    { { { 240,   0,   0,   0 } }, false, false, false },
    { { { 242, 147, 131, 229 } }, false, false, false },
    { { { 255, 255, 255, 255 } }, false, false, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, constructorIPv4Address, ValuesIn( constructorIPv4Address_TEST_CASES ) );

/**
 * \brief Verify picolibrary::IP::Address::Address( picolibrary::IP::Address && ) works
 *        properly when the source of the move was default constructed.
 */
TEST( constructorMove, worksProperlySourceDefaultConstructed )
{
    auto source = Address{};

    auto const address = Address{ std::move( source ) };

    EXPECT_EQ( address.version(), Version::UNSPECIFIED );

    EXPECT_EQ( source.version(), address.version() );
}

/**
 * \brief Verify picolibrary::IP::Address::Address( picolibrary::IP::Address && ) works
 *        properly when the source of the move was constructed from an IPv4 address.
 */
TEST( constructorMove, worksProperlySourceIPv4Address )
{
    auto const ipv4_address = IPv4_Address{ { 208, 141, 112, 90 } };

    auto source = Address{ ipv4_address };

    auto const address = Address{ std::move( source ) };

    EXPECT_EQ( address.version(), Version::_4 );
    EXPECT_EQ( address.ipv4(), ipv4_address );

    EXPECT_EQ( source.version(), address.version() );
    EXPECT_EQ( source.ipv4(), address.ipv4() );
}

/**
 * \brief Verify picolibrary::IP::Address::Address( picolibrary::IP::Address const & )
 *        works properly when the original to copy was default constructed.
 */
TEST( constructorCopy, worksProperlyOriginalDefaultConstructed )
{
    auto const original = Address{};

    auto const address = Address{ original };

    EXPECT_EQ( address.version(), Version::UNSPECIFIED );

    EXPECT_EQ( original.version(), address.version() );
}

/**
 * \brief Verify picolibrary::IP::Address::Address( picolibrary::IP::Address const & )
 *        works properly when the original to copy was constructed from an IPv4 address.
 */
TEST( constructorCopy, worksProperlyOriginalIPv4Address )
{
    auto const ipv4_address = IPv4_Address{ { 208, 141, 112, 90 } };

    auto const original = Address{ ipv4_address };

    auto const address = Address{ original };

    EXPECT_EQ( address.version(), Version::_4 );
    EXPECT_EQ( address.ipv4(), ipv4_address );

    EXPECT_EQ( original.version(), address.version() );
    EXPECT_EQ( original.ipv4(), address.ipv4() );
}

/**
 * \brief Verify picolibrary::IP::Address::operator=( picolibrary::IP::Address && ) works
 *        properly when the expression to be assigned was default constructed and the
 *        assigned to object was default constructed.
 */
TEST( assignmentOperatorMove, worksProperlyExpressionDefaultConstructedObjectDefaultConstructed )
{
    auto expression = Address{};
    auto object     = Address{};

    object = std::move( expression );

    EXPECT_EQ( object.version(), Version::UNSPECIFIED );

    EXPECT_EQ( expression.version(), object.version() );
}

/**
 * \brief Verify picolibrary::IP::Address::operator=( picolibrary::IP::Address && ) works
 *        properly when the expression to be assigned was constructed from an IPv4 address
 *        and the assigned to object was default constructed.
 */
TEST( assignmentOperatorMove, worksProperlyExpressionIPv4AddressObjectDefaultConstructed )
{
    auto const expression_ipv4_address = IPv4_Address{ { 41, 45, 56, 45 } };

    auto expression = Address{ expression_ipv4_address };
    auto object     = Address{};

    object = std::move( expression );

    EXPECT_EQ( object.version(), Version::_4 );
    EXPECT_EQ( object.ipv4(), expression_ipv4_address );

    EXPECT_EQ( expression.version(), object.version() );
    EXPECT_EQ( expression.ipv4(), object.ipv4() );
}

/**
 * \brief Verify picolibrary::IP::Address::operator=( picolibrary::IP::Address && ) works
 *        properly when the expression to be assigned was default constructed and the
 *        assigned to object was constructed from an IPv4 address.
 */
TEST( assignmentOperatorMove, worksProperlyExpressionDefaultConstructedObjectIPv4Address )
{
    auto expression = Address{};
    auto object     = Address{ IPv4_Address{ { 16, 232, 96, 31 } } };

    object = std::move( expression );

    EXPECT_EQ( object.version(), Version::UNSPECIFIED );

    EXPECT_EQ( expression.version(), object.version() );
}

/**
 * \brief Verify picolibrary::IP::Address::operator=( picolibrary::IP::Address && ) works
 *        properly when the expression to be assigned was constructed from an IPv4 address
 *        and the assigned to object was constructed from an IPv4 address.
 */
TEST( assignmentOperatorMove, worksProperlyExpressionIPv4AddressObjectIPv4Address )
{
    auto const expression_ipv4_address = IPv4_Address{ { 41, 45, 56, 45 } };

    auto expression = Address{ expression_ipv4_address };
    auto object     = Address{ IPv4_Address{ { 16, 232, 96, 31 } } };

    object = std::move( expression );

    EXPECT_EQ( object.version(), Version::_4 );
    EXPECT_EQ( object.ipv4(), expression_ipv4_address );

    EXPECT_EQ( expression.version(), object.version() );
    EXPECT_EQ( expression.ipv4(), object.ipv4() );
}

/**
 * \brief Verify picolibrary::IP::Address::operator=( picolibrary::IP::Address && ) works
 *        properly when self assignment occurs, and the address was default constructed.
 */
TEST( assignmentOperatorMove, worksProperlySelfAssignmentDefaultConstructed )
{
    auto address = Address{};

    address = std::move( address );

    EXPECT_EQ( address.version(), Version::UNSPECIFIED );
}

/**
 * \brief Verify picolibrary::IP::Address::operator=( picolibrary::IP::Address && ) works
 *        properly when self assignment occurs, and the address was constructed from an
 *        IPv4 address.
 */
TEST( assignmentOperatorMove, worksProperlySelfAssignmentIPv4Address )
{
    auto const ipv4_address = IPv4_Address{ { 208, 141, 112, 90 } };

    auto address = Address{ ipv4_address };

    address = std::move( address );

    EXPECT_EQ( address.version(), Version::_4 );
    EXPECT_EQ( address.ipv4(), ipv4_address );
}

/**
 * \brief Verify picolibrary::IP::Address::operator=( picolibrary::IP::Address const & )
 *        works properly when the expression to be assigned was default constructed and
 *        the assigned to object was default constructed.
 */
TEST( assignmentOperatorCopy, worksProperlyExpressionDefaultConstructedObjectDefaultConstructed )
{
    auto const expression = Address{};
    auto       object     = Address{};

    object = expression;

    EXPECT_EQ( object.version(), Version::UNSPECIFIED );

    EXPECT_EQ( expression.version(), object.version() );
}

/**
 * \brief Verify picolibrary::IP::Address::operator=( picolibrary::IP::Address const & )
 *        works properly when the expression to be assigned was constructed from an IPv4
 *        address and the assigned to object was default constructed.
 */
TEST( assignmentOperatorCopy, worksProperlyExpressionIPv4AddressObjectDefaultConstructed )
{
    auto const expression_ipv4_address = IPv4_Address{ { 41, 45, 56, 45 } };

    auto const expression = Address{ expression_ipv4_address };
    auto       object     = Address{};

    object = expression;

    EXPECT_EQ( object.version(), Version::_4 );
    EXPECT_EQ( object.ipv4(), expression_ipv4_address );

    EXPECT_EQ( expression.version(), object.version() );
    EXPECT_EQ( expression.ipv4(), object.ipv4() );
}

/**
 * \brief Verify picolibrary::IP::Address::operator=( picolibrary::IP::Address const & )
 *        works properly when the expression to be assigned was default constructed and
 *        the assigned to object was constructed from an IPv4 address.
 */
TEST( assignmentOperatorCopy, worksProperlyExpressionDefaultConstructedObjectIPv4Address )
{
    auto const expression = Address{};
    auto       object     = Address{ IPv4_Address{ { 16, 232, 96, 31 } } };

    object = expression;

    EXPECT_EQ( object.version(), Version::UNSPECIFIED );

    EXPECT_EQ( expression.version(), object.version() );
}

/**
 * \brief Verify picolibrary::IP::Address::operator=( picolibrary::IP::Address const & )
 *        works properly when the expression to be assigned was constructed from an IPv4
 *        address and the assigned to object was constructed from an IPv4 address.
 */
TEST( assignmentOperatorCopy, worksProperlyExpressionIPv4AddressObjectIPv4Address )
{
    auto const expression_ipv4_address = IPv4_Address{ { 41, 45, 56, 45 } };

    auto const expression = Address{ expression_ipv4_address };
    auto       object     = Address{ IPv4_Address{ { 16, 232, 96, 31 } } };

    object = expression;

    EXPECT_EQ( object.version(), Version::_4 );
    EXPECT_EQ( object.ipv4(), expression_ipv4_address );

    EXPECT_EQ( expression.version(), object.version() );
    EXPECT_EQ( expression.ipv4(), object.ipv4() );
}

/**
 * \brief Verify picolibrary::IP::Address::operator=( picolibrary::IP::Address const & )
 *        works properly when self assignment occurs, and the address was default
 *        constructed.
 */
TEST( assignmentOperatorCopy, worksProperlySelfAssignmentDefaultConstructed )
{
    auto address = Address{};

    address = address;

    EXPECT_EQ( address.version(), Version::UNSPECIFIED );
}

/**
 * \brief Verify picolibrary::IP::Address::operator=( picolibrary::IP::Address const & )
 *        works properly when self assignment occurs, and the address was constructed from
 *        an IPv4 address.
 */
TEST( assignmentOperatorCopy, worksProperlySelfAssignmentIPv4Address )
{
    auto const ipv4_address = IPv4_Address{ { 208, 141, 112, 90 } };

    auto address = Address{ ipv4_address };

    address = address;

    EXPECT_EQ( address.version(), Version::_4 );
    EXPECT_EQ( address.ipv4(), ipv4_address );
}

/**
 * \brief picolibrary::IP::operator==( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ), picolibrary::IP::operator!=(
 *        picolibrary::IP::Address const &, picolibrary::IP::Address const & ),
 *        picolibrary::IP::operator<( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ), picolibrary::IP::operator>(
 *        picolibrary::IP::Address const &, picolibrary::IP::Address const & ),
 *        picolibrary::IP::operator<=( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ), and picolibrary::IP::operator>=(
 *        picolibrary::IP::Address const &, picolibrary::IP::Address const & ) test case.
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
 * \brief picolibrary::IP::operator==( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) test fixture.
 */
class equalityOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::operator==( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) works properly.
 */
TEST_P( equalityOperator, worksProperly )
{
    auto const test_case = GetParam();

    EXPECT_EQ( test_case.lhs == test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IP::operator==( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) test cases.
 */
comparisonOperator_Test_Case const equalityOperator_TEST_CASES[]{
    // clang-format off

    { {}, {}, true },

    { IPv4_Address{ {  24,  89, 245,  60 } }, IPv4_Address{ { 142, 148,  44,  38 } }, false },
    { IPv4_Address{ { 149, 175, 232,  78 } }, IPv4_Address{ { 149, 175, 232,  79 } }, false },
    { IPv4_Address{ { 149, 175, 232,  79 } }, IPv4_Address{ { 149, 175, 232,  79 } }, true  },
    { IPv4_Address{ { 149, 175, 232,  80 } }, IPv4_Address{ { 149, 175, 232,  79 } }, false },
    { IPv4_Address{ { 210,  90, 186, 128 } }, IPv4_Address{ { 219,  29,  34, 215 } }, false },

    { {}, IPv4_Address{ {   0,   0,   0,   0 } }, false },
    { {}, IPv4_Address{ { 208, 141, 112,  90 } }, false },
    { {}, IPv4_Address{ { 255, 255, 255, 255 } }, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, equalityOperator, ValuesIn( equalityOperator_TEST_CASES ) );

/**
 * \brief picolibrary::IP::operator!=( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) test fixture.
 */
class inequalityOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::operator!=( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) works properly.
 */
TEST_P( inequalityOperator, worksProperly )
{
    auto const test_case = GetParam();

    EXPECT_EQ( test_case.lhs != test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IP::operator!=( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) test cases.
 */
comparisonOperator_Test_Case const inequalityOperator_TEST_CASES[]{
    // clang-format off

    { {}, {}, false },

    { IPv4_Address{ {  24,  89, 245,  60 } }, IPv4_Address{ { 142, 148,  44,  38 } }, true  },
    { IPv4_Address{ { 149, 175, 232,  78 } }, IPv4_Address{ { 149, 175, 232,  79 } }, true  },
    { IPv4_Address{ { 149, 175, 232,  79 } }, IPv4_Address{ { 149, 175, 232,  79 } }, false },
    { IPv4_Address{ { 149, 175, 232,  80 } }, IPv4_Address{ { 149, 175, 232,  79 } }, true  },
    { IPv4_Address{ { 210,  90, 186, 128 } }, IPv4_Address{ { 219,  29,  34, 215 } }, true  },

    { {}, IPv4_Address{ {   0,   0,   0,   0 } }, true },
    { {}, IPv4_Address{ { 208, 141, 112,  90 } }, true },
    { {}, IPv4_Address{ { 255, 255, 255, 255 } }, true },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, inequalityOperator, ValuesIn( inequalityOperator_TEST_CASES ) );

/**
 * \brief picolibrary::IP::operator<( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) test fixture.
 */
class lessThanOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::operator<( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) works properly.
 */
TEST_P( lessThanOperator, worksProperly )
{
    auto const test_case = GetParam();

    EXPECT_EQ( test_case.lhs < test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IP::operator<( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) test cases.
 */
comparisonOperator_Test_Case const lessThanOperator_TEST_CASES[]{
    // clang-format off

    { {}, {}, false },

    { IPv4_Address{ {  92, 178,  11, 221 } }, IPv4_Address{ { 146, 45, 171, 183 } }, true  },
    { IPv4_Address{ { 146,  45, 171, 182 } }, IPv4_Address{ { 146, 45, 171, 183 } }, true  },
    { IPv4_Address{ { 146,  45, 171, 183 } }, IPv4_Address{ { 146, 45, 171, 183 } }, false },
    { IPv4_Address{ { 146,  45, 171, 184 } }, IPv4_Address{ { 146, 45, 171, 183 } }, false },
    { IPv4_Address{ { 253, 213, 179,  93 } }, IPv4_Address{ { 146, 45, 171, 183 } }, false },

    { {}, IPv4_Address{ {   0,   0,   0,   0 } }, true },
    { {}, IPv4_Address{ { 208, 141, 112,  90 } }, true },
    { {}, IPv4_Address{ { 255, 255, 255, 255 } }, true },

    { IPv4_Address{ {   0,   0,   0,   0 } }, {}, false },
    { IPv4_Address{ { 208, 141, 112,  90 } }, {}, false },
    { IPv4_Address{ { 255, 255, 255, 255 } }, {}, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, lessThanOperator, ValuesIn( lessThanOperator_TEST_CASES ) );

/**
 * \brief picolibrary::IP::operator>( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) test fixture.
 */
class greaterThanOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::operator>( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) works properly.
 */
TEST_P( greaterThanOperator, worksProperly )
{
    auto const test_case = GetParam();

    EXPECT_EQ( test_case.lhs > test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IP::operator>( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) test cases.
 */
comparisonOperator_Test_Case const greaterThanOperator_TEST_CASES[]{
    // clang-format off

    { {}, {}, false },

    { IPv4_Address{ {  92, 178,  11, 221 } }, IPv4_Address{ { 146, 45, 171, 183 } }, false },
    { IPv4_Address{ { 146,  45, 171, 182 } }, IPv4_Address{ { 146, 45, 171, 183 } }, false },
    { IPv4_Address{ { 146,  45, 171, 183 } }, IPv4_Address{ { 146, 45, 171, 183 } }, false },
    { IPv4_Address{ { 146,  45, 171, 184 } }, IPv4_Address{ { 146, 45, 171, 183 } }, true  },
    { IPv4_Address{ { 253, 213, 179,  93 } }, IPv4_Address{ { 146, 45, 171, 183 } }, true  },

    { {}, IPv4_Address{ {   0,   0,   0,   0 } }, false },
    { {}, IPv4_Address{ { 208, 141, 112,  90 } }, false },
    { {}, IPv4_Address{ { 255, 255, 255, 255 } }, false },

    { IPv4_Address{ {   0,   0,   0,   0 } }, {}, true },
    { IPv4_Address{ { 208, 141, 112,  90 } }, {}, true },
    { IPv4_Address{ { 255, 255, 255, 255 } }, {}, true },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, greaterThanOperator, ValuesIn( greaterThanOperator_TEST_CASES ) );

/**
 * \brief picolibrary::IP::operator<=( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) test fixture.
 */
class lessThanOrEqualToOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::operator<=( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) works properly.
 */
TEST_P( lessThanOrEqualToOperator, worksProperly )
{
    auto const test_case = GetParam();

    EXPECT_EQ( test_case.lhs <= test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IP::operator<=( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) test cases.
 */
comparisonOperator_Test_Case const lessThanOrEqualToOperator_TEST_CASES[]{
    // clang-format off

    { {}, {}, true },

    { IPv4_Address{ {  92, 178,  11, 221 } }, IPv4_Address{ { 146, 45, 171, 183 } }, true  },
    { IPv4_Address{ { 146,  45, 171, 182 } }, IPv4_Address{ { 146, 45, 171, 183 } }, true  },
    { IPv4_Address{ { 146,  45, 171, 183 } }, IPv4_Address{ { 146, 45, 171, 183 } }, true  },
    { IPv4_Address{ { 146,  45, 171, 184 } }, IPv4_Address{ { 146, 45, 171, 183 } }, false },
    { IPv4_Address{ { 253, 213, 179,  93 } }, IPv4_Address{ { 146, 45, 171, 183 } }, false },

    { {}, IPv4_Address{ {   0,   0,   0,   0 } }, true },
    { {}, IPv4_Address{ { 208, 141, 112,  90 } }, true },
    { {}, IPv4_Address{ { 255, 255, 255, 255 } }, true },

    { IPv4_Address{ {   0,   0,   0,   0 } }, {}, false },
    { IPv4_Address{ { 208, 141, 112,  90 } }, {}, false },
    { IPv4_Address{ { 255, 255, 255, 255 } }, {}, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, lessThanOrEqualToOperator, ValuesIn( lessThanOrEqualToOperator_TEST_CASES ) );

/**
 * \brief picolibrary::IP::operator>=( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) test fixture.
 */
class greaterThanOrEqualToOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::operator>=( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) works properly.
 */
TEST_P( greaterThanOrEqualToOperator, worksProperly )
{
    auto const test_case = GetParam();

    EXPECT_EQ( test_case.lhs >= test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IP::operator>=( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) test cases.
 */
comparisonOperator_Test_Case const greaterThanOrEqualToOperator_TEST_CASES[]{
    // clang-format off

    { {}, {}, true },

    { IPv4_Address{ {  92, 178,  11, 221 } }, IPv4_Address{ { 146, 45, 171, 183 } }, false },
    { IPv4_Address{ { 146,  45, 171, 182 } }, IPv4_Address{ { 146, 45, 171, 183 } }, false },
    { IPv4_Address{ { 146,  45, 171, 183 } }, IPv4_Address{ { 146, 45, 171, 183 } }, true  },
    { IPv4_Address{ { 146,  45, 171, 184 } }, IPv4_Address{ { 146, 45, 171, 183 } }, true  },
    { IPv4_Address{ { 253, 213, 179,  93 } }, IPv4_Address{ { 146, 45, 171, 183 } }, true  },

    { {}, IPv4_Address{ {   0,   0,   0,   0 } }, false },
    { {}, IPv4_Address{ { 208, 141, 112,  90 } }, false },
    { {}, IPv4_Address{ { 255, 255, 255, 255 } }, false },

    { IPv4_Address{ {   0,   0,   0,   0 } }, {}, true },
    { IPv4_Address{ { 208, 141, 112,  90 } }, {}, true },
    { IPv4_Address{ { 255, 255, 255, 255 } }, {}, true },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, greaterThanOrEqualToOperator, ValuesIn( greaterThanOrEqualToOperator_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::IP::Address>::print(
 *        picolibrary::Output_Stream &, picolibrary::IP::Address const & ) error handling
 *        test fixture.
 */
class outputFormatterIPAddressPrintOutputStreamErrorHandling : public TestWithParam<Address> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IP::Address>::print(
 *        picolibrary::Output_Stream &, picolibrary::IP::Address const & ) properly
 *        handles a put error.
 */
TEST_P( outputFormatterIPAddressPrintOutputStreamErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 107 };

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( GetParam() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief picolibrary::Output_Formatter<picolibrary::IP::Address>::print(
 *        picolibrary::Output_Stream &, picolibrary::IP::Address const & ) error handling
 *        test cases.
 */
Address const outputFormatterIPAddressPrintOutputStreamErrorHandling_TEST_CASES[]{
    // clang-format off

    {},

    { IPv4_Address{ { 101, 136, 170, 45 } } },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P(
    testCases,
    outputFormatterIPAddressPrintOutputStreamErrorHandling,
    ValuesIn( outputFormatterIPAddressPrintOutputStreamErrorHandling_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::IP::Address>::print() test case.
 */
struct outputFormatterIPAddressPrint_Test_Case {
    /**
     * \brief The picolibrary::IP::Address to format.
     */
    Address address;

    /**
     * \brief The formatted address.
     */
    std::string_view formatted_address;
};

auto operator<<( std::ostream & stream, outputFormatterIPAddressPrint_Test_Case const & test_case )
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
 * \brief picolibrary::Output_Formatter<picolibrary::IP::Address>::print() test cases.
 */
outputFormatterIPAddressPrint_Test_Case const outputFormatterIPAddressPrint_TEST_CASES[]{
    // clang-format off

    { {}, "ANY" },

    { IPv4_Address{ {   0,   0,   0,   0 } }, "ANY"             },
    { IPv4_Address{ {   0,   0,   0,   1 } }, "0.0.0.1"         },
    { IPv4_Address{ { 101, 136, 170,  45 } }, "101.136.170.45"  },
    { IPv4_Address{ { 255, 255, 255, 255 } }, "255.255.255.255" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::IP::Address>::print(
 *        picolibrary::Output_Stream &, picolibrary::IP::Address const & ) test fixture.
 */
class outputFormatterIPAddressPrintOutputStream :
    public TestWithParam<outputFormatterIPAddressPrint_Test_Case> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IP::Address>::print(
 *        picolibrary::Output_Stream &, picolibrary::IP::Address const & ) works properly.
 */
TEST_P( outputFormatterIPAddressPrintOutputStream, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( test_case.address );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.formatted_address );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterIPAddressPrintOutputStream, ValuesIn( outputFormatterIPAddressPrint_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::IP::Address>::print(
 *        picolibrary::Reliable_Output_Stream &, picolibrary::IP::Address const & ) test
 *        fixture.
 */
class outputFormatterIPAddressPrintReliableOutputStream :
    public TestWithParam<outputFormatterIPAddressPrint_Test_Case> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IP::Address>::print(
 *        picolibrary::Reliable_Output_Stream &, picolibrary::IP::Address const & ) works
 *        properly.
 */
TEST_P( outputFormatterIPAddressPrintReliableOutputStream, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( test_case.address );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.formatted_address );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterIPAddressPrintReliableOutputStream, ValuesIn( outputFormatterIPAddressPrint_TEST_CASES ) );

/**
 * \brief Execute the picolibrary::IP::Address automated tests.
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

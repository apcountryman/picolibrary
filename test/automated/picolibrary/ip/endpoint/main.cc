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
 * \brief picolibrary::IP::Endpoint automated test program.
 */

#include <ios>
#include <ostream>
#include <string>
#include <string_view>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/ip.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/ip.h"
#include "picolibrary/testing/automated/stream.h"

namespace {

using ::picolibrary::IP::Address;
using ::picolibrary::IP::Endpoint;
using ::picolibrary::IP::Port;
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
 * \brief Verify picolibrary::IP::Endpoint::Endpoint() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const endpoint = Endpoint{};

    EXPECT_EQ( endpoint.address(), Address{} );
    EXPECT_EQ( endpoint.port(), Port{} );
}

/**
 * \brief picolibrary::IP::Endpoint::Endpoint( picolibrary::IP::Port ) test fixture.
 */
class constructorPort : public TestWithParam<Port> {
};

/**
 * \brief Verify picolibrary::IP::Endpoint::Endpoint( picolibrary::IP::Port ) works
 *        properly.
 */
TEST_P( constructorPort, worksProperly )
{
    auto const port = GetParam();

    auto const endpoint = Endpoint{ port };

    EXPECT_EQ( endpoint.address(), Address{} );
    EXPECT_EQ( endpoint.port(), port );
}

/**
 * \brief picolibrary::IP::Endpoint::Endpoint( picolibrary::IP::Port ) test cases.
 */
Port const constructorPort_TEST_CASES[]{
    0,
    20711,
    65535,
};

INSTANTIATE_TEST_SUITE_P( testCases, constructorPort, ValuesIn( constructorPort_TEST_CASES ) );

/**
 * \brief picolibrary::IP::Endpoint::Endpoint( picolibrary::IP::Address,
 *        picolibrary::IP::Port ) test case.
 */
struct constructorAddressPort_Test_Case {
    /**
     * \brief The endpoint's address.
     */
    Address address;

    /**
     * \brief The endpoint's port.
     */
    Port port;
};

auto operator<<( std::ostream & stream, constructorAddressPort_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".address = " << test_case.address
                  << ", "
                  << ".port = " << test_case.port
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::IP::Endpoint::Endpoint( picolibrary::IP::Address,
 *        picolibrary::IP::Port ) test fixture.
 */
class constructorAddressPort : public TestWithParam<constructorAddressPort_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::Endpoint::Endpoint( picolibrary::IP::Address,
 *        picolibrary::IP::Port ) works properly.
 */
TEST_P( constructorAddressPort, worksProperly )
{
    auto const test_case = GetParam();

    auto const endpoint = Endpoint{ test_case.address, test_case.port };

    EXPECT_EQ( endpoint.address(), test_case.address );
    EXPECT_EQ( endpoint.port(), test_case.port );
}

/**
 * \brief picolibrary::IP::Endpoint::Endpoint( picolibrary::IP::Address,
 *        picolibrary::IP::Port ) test cases.
 */
constructorAddressPort_Test_Case const constructorAddressPort_TEST_CASES[]{
    // clang-format off

    { IPv4_Address{ {   0,   0,   0,   0 } },  6496 },
    { IPv4_Address{ { 255, 255, 255, 255 } }, 26459 },

    { IPv4_Address{ {  36,  97, 110,   5 } },     0 },
    { IPv4_Address{ { 209, 107, 160, 192 } }, 65535 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, constructorAddressPort, ValuesIn( constructorAddressPort_TEST_CASES ) );

/**
 * \brief picolibrary::IP::operator==( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ), picolibrary::IP::operator!=(
 *        picolibrary::IP::Endpoint const &, picolibrary::IP::Endpoint const & ),
 *        picolibrary::IP::operator<( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ), picolibrary::IP::operator>(
 *        picolibrary::IP::Endpoint const &, picolibrary::IP::Endpoint const & ),
 *        picolibrary::IP::operator<=( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ), and picolibrary::IP::operator>=(
 *        picolibrary::IP::Endpoint const &, picolibrary::IP::Endpoint const & ) test
 *        case.
 */
struct comparisonOperator_Test_Case {
    /**
     * \brief The left hand side of the comparison.
     */
    Endpoint lhs;

    /**
     * \brief The right hand side of the comparison.
     */
    Endpoint rhs;

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
 * \brief picolibrary::IP::operator==( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) test fixture.
 */
class equalityOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::operator==( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) works properly.
 */
TEST_P( equalityOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs == test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IP::operator==( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) test cases.
 */
comparisonOperator_Test_Case const equalityOperator_TEST_CASES[]{
    // clang-format off

    { { {},  1948 }, { {}, 10417 }, false },
    { { {}, 18347 }, { {}, 18348 }, false },
    { { {}, 18348 }, { {}, 18348 },  true },
    { { {}, 18349 }, { {}, 18348 }, false },
    { { {}, 59618 }, { {}, 60750 }, false },

    { { IPv4_Address{ {  24,  89, 245,  60 } },  1948 }, { IPv4_Address{ { 142, 148,  44,  38 } }, 10417 }, false },
    { { IPv4_Address{ {  24,  89, 245,  60 } }, 18347 }, { IPv4_Address{ { 142, 148,  44,  38 } }, 18348 }, false },
    { { IPv4_Address{ {  24,  89, 245,  60 } }, 18348 }, { IPv4_Address{ { 142, 148,  44,  38 } }, 18348 }, false },
    { { IPv4_Address{ {  24,  89, 245,  60 } }, 18349 }, { IPv4_Address{ { 142, 148,  44,  38 } }, 18348 }, false },
    { { IPv4_Address{ {  24,  89, 245,  60 } }, 59618 }, { IPv4_Address{ { 142, 148,  44,  38 } }, 60750 }, false },
    { { IPv4_Address{ { 149, 175, 232,  78 } },  1948 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 10417 }, false },
    { { IPv4_Address{ { 149, 175, 232,  78 } }, 18347 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 18348 }, false },
    { { IPv4_Address{ { 149, 175, 232,  78 } }, 18348 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 18348 }, false },
    { { IPv4_Address{ { 149, 175, 232,  78 } }, 18349 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 18348 }, false },
    { { IPv4_Address{ { 149, 175, 232,  78 } }, 59618 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 60750 }, false },
    { { IPv4_Address{ { 149, 175, 232,  79 } },  1948 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 10417 }, false },
    { { IPv4_Address{ { 149, 175, 232,  79 } }, 18347 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 18348 }, false },
    { { IPv4_Address{ { 149, 175, 232,  79 } }, 18348 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 18348 }, true  },
    { { IPv4_Address{ { 149, 175, 232,  79 } }, 18349 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 18348 }, false },
    { { IPv4_Address{ { 149, 175, 232,  79 } }, 59618 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 60750 }, false },
    { { IPv4_Address{ { 149, 175, 232,  80 } },  1948 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 10417 }, false },
    { { IPv4_Address{ { 149, 175, 232,  80 } }, 18347 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 18348 }, false },
    { { IPv4_Address{ { 149, 175, 232,  80 } }, 18348 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 18348 }, false },
    { { IPv4_Address{ { 149, 175, 232,  80 } }, 18349 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 18348 }, false },
    { { IPv4_Address{ { 149, 175, 232,  80 } }, 59618 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 60750 }, false },
    { { IPv4_Address{ { 210,  90, 186, 128 } },  1948 }, { IPv4_Address{ { 219,  29,  34, 215 } }, 10417 }, false },
    { { IPv4_Address{ { 210,  90, 186, 128 } }, 18347 }, { IPv4_Address{ { 219,  29,  34, 215 } }, 18348 }, false },
    { { IPv4_Address{ { 210,  90, 186, 128 } }, 18348 }, { IPv4_Address{ { 219,  29,  34, 215 } }, 18348 }, false },
    { { IPv4_Address{ { 210,  90, 186, 128 } }, 18349 }, { IPv4_Address{ { 219,  29,  34, 215 } }, 18348 }, false },
    { { IPv4_Address{ { 210,  90, 186, 128 } }, 59618 }, { IPv4_Address{ { 219,  29,  34, 215 } }, 60750 }, false },

    { { {},  1948 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 10417 }, false },
    { { {}, 18347 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 18348 }, false },
    { { {}, 18348 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 18348 }, false },
    { { {}, 18349 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 18348 }, false },
    { { {}, 59618 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 60750 }, false },
    { { {},  1948 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 10417 }, false },
    { { {}, 18347 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 18348 }, false },
    { { {}, 18348 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 18348 }, false },
    { { {}, 18349 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 18348 }, false },
    { { {}, 59618 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 60750 }, false },
    { { {},  1948 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 10417 }, false },
    { { {}, 18347 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 18348 }, false },
    { { {}, 18348 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 18348 }, false },
    { { {}, 18349 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 18348 }, false },
    { { {}, 59618 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 60750 }, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, equalityOperator, ValuesIn( equalityOperator_TEST_CASES ) );

/**
 * \brief picolibrary::IP::operator!=( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) test fixture.
 */
class inequalityOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::operator!=( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) works properly.
 */
TEST_P( inequalityOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs != test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IP::operator!=( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) test cases.
 */
comparisonOperator_Test_Case const inequalityOperator_TEST_CASES[]{
    // clang-format off

    { { {},  1948 }, { {}, 10417 },  true },
    { { {}, 18347 }, { {}, 18348 },  true },
    { { {}, 18348 }, { {}, 18348 }, false },
    { { {}, 18349 }, { {}, 18348 },  true },
    { { {}, 59618 }, { {}, 60750 },  true },

    { { IPv4_Address{ {  24,  89, 245,  60 } },  1948 }, { IPv4_Address{ { 142, 148,  44,  38 } }, 10417 },  true },
    { { IPv4_Address{ {  24,  89, 245,  60 } }, 18347 }, { IPv4_Address{ { 142, 148,  44,  38 } }, 18348 },  true },
    { { IPv4_Address{ {  24,  89, 245,  60 } }, 18348 }, { IPv4_Address{ { 142, 148,  44,  38 } }, 18348 },  true },
    { { IPv4_Address{ {  24,  89, 245,  60 } }, 18349 }, { IPv4_Address{ { 142, 148,  44,  38 } }, 18348 },  true },
    { { IPv4_Address{ {  24,  89, 245,  60 } }, 59618 }, { IPv4_Address{ { 142, 148,  44,  38 } }, 60750 },  true },
    { { IPv4_Address{ { 149, 175, 232,  78 } },  1948 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 10417 },  true },
    { { IPv4_Address{ { 149, 175, 232,  78 } }, 18347 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 18348 },  true },
    { { IPv4_Address{ { 149, 175, 232,  78 } }, 18348 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 18348 },  true },
    { { IPv4_Address{ { 149, 175, 232,  78 } }, 18349 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 18348 },  true },
    { { IPv4_Address{ { 149, 175, 232,  78 } }, 59618 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 60750 },  true },
    { { IPv4_Address{ { 149, 175, 232,  79 } },  1948 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 10417 },  true },
    { { IPv4_Address{ { 149, 175, 232,  79 } }, 18347 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 18348 },  true },
    { { IPv4_Address{ { 149, 175, 232,  79 } }, 18348 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 18348 }, false },
    { { IPv4_Address{ { 149, 175, 232,  79 } }, 18349 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 18348 },  true },
    { { IPv4_Address{ { 149, 175, 232,  79 } }, 59618 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 60750 },  true },
    { { IPv4_Address{ { 149, 175, 232,  80 } },  1948 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 10417 },  true },
    { { IPv4_Address{ { 149, 175, 232,  80 } }, 18347 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 18348 },  true },
    { { IPv4_Address{ { 149, 175, 232,  80 } }, 18348 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 18348 },  true },
    { { IPv4_Address{ { 149, 175, 232,  80 } }, 18349 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 18348 },  true },
    { { IPv4_Address{ { 149, 175, 232,  80 } }, 59618 }, { IPv4_Address{ { 149, 175, 232,  79 } }, 60750 },  true },
    { { IPv4_Address{ { 210,  90, 186, 128 } },  1948 }, { IPv4_Address{ { 219,  29,  34, 215 } }, 10417 },  true },
    { { IPv4_Address{ { 210,  90, 186, 128 } }, 18347 }, { IPv4_Address{ { 219,  29,  34, 215 } }, 18348 },  true },
    { { IPv4_Address{ { 210,  90, 186, 128 } }, 18348 }, { IPv4_Address{ { 219,  29,  34, 215 } }, 18348 },  true },
    { { IPv4_Address{ { 210,  90, 186, 128 } }, 18349 }, { IPv4_Address{ { 219,  29,  34, 215 } }, 18348 },  true },
    { { IPv4_Address{ { 210,  90, 186, 128 } }, 59618 }, { IPv4_Address{ { 219,  29,  34, 215 } }, 60750 },  true },

    { { {},  1948 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 10417 }, true },
    { { {}, 18347 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 18348 }, true },
    { { {}, 18348 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 18348 }, true },
    { { {}, 18349 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 18348 }, true },
    { { {}, 59618 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 60750 }, true },
    { { {},  1948 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 10417 }, true },
    { { {}, 18347 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 18348 }, true },
    { { {}, 18348 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 18348 }, true },
    { { {}, 18349 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 18348 }, true },
    { { {}, 59618 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 60750 }, true },
    { { {},  1948 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 10417 }, true },
    { { {}, 18347 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 18348 }, true },
    { { {}, 18348 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 18348 }, true },
    { { {}, 18349 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 18348 }, true },
    { { {}, 59618 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 60750 }, true },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, inequalityOperator, ValuesIn( inequalityOperator_TEST_CASES ) );

/**
 * \brief picolibrary::IP::operator<( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) test fixture.
 */
class lessThanOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::operator<( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) works properly.
 */
TEST_P( lessThanOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs < test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IP::operator<( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) test cases.
 */
comparisonOperator_Test_Case const lessThanOperator_TEST_CASES[]{
    // clang-format off

    { { {},  7444 }, { {}, 9706 }, true  },
    { { {},  9705 }, { {}, 9706 }, true  },
    { { {},  9706 }, { {}, 9706 }, false },
    { { {},  9707 }, { {}, 9706 }, false },
    { { {}, 36246 }, { {}, 9706 }, false },

    { { IPv4_Address{ {  92, 178,  11, 221 } },  7444 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ {  92, 178,  11, 221 } },  9705 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ {  92, 178,  11, 221 } },  9706 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ {  92, 178,  11, 221 } },  9707 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ {  92, 178,  11, 221 } }, 36246 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 182 } },  7444 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 182 } },  9705 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 182 } },  9706 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 182 } },  9707 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 182 } }, 36246 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 183 } },  7444 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 183 } },  9705 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 183 } },  9706 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 183 } },  9707 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 183 } }, 36246 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 184 } },  7444 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 184 } },  9705 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 184 } },  9706 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 184 } },  9707 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 184 } }, 36246 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 253, 213, 179,  93 } },  7444 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 253, 213, 179,  93 } },  9705 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 253, 213, 179,  93 } },  9706 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 253, 213, 179,  93 } },  9707 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 253, 213, 179,  93 } }, 36246 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },

    { { {},  7444 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 9706 }, true },
    { { {},  9705 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 9706 }, true },
    { { {},  9706 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 9706 }, true },
    { { {},  9707 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 9706 }, true },
    { { {}, 36246 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 9706 }, true },
    { { {},  7444 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 9706 }, true },
    { { {},  9705 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 9706 }, true },
    { { {},  9706 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 9706 }, true },
    { { {},  9707 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 9706 }, true },
    { { {}, 36246 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 9706 }, true },
    { { {},  7444 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 9706 }, true },
    { { {},  9705 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 9706 }, true },
    { { {},  9706 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 9706 }, true },
    { { {},  9707 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 9706 }, true },
    { { {}, 36246 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 9706 }, true },

    { { IPv4_Address{ {   0,   0,   0,   0 } },  7444 }, { {}, 9706 }, false },
    { { IPv4_Address{ {   0,   0,   0,   0 } },  9705 }, { {}, 9706 }, false },
    { { IPv4_Address{ {   0,   0,   0,   0 } },  9706 }, { {}, 9706 }, false },
    { { IPv4_Address{ {   0,   0,   0,   0 } },  9707 }, { {}, 9706 }, false },
    { { IPv4_Address{ {   0,   0,   0,   0 } }, 36246 }, { {}, 9706 }, false },
    { { IPv4_Address{ { 208, 141, 112,  90 } },  7444 }, { {}, 9706 }, false },
    { { IPv4_Address{ { 208, 141, 112,  90 } },  9705 }, { {}, 9706 }, false },
    { { IPv4_Address{ { 208, 141, 112,  90 } },  9706 }, { {}, 9706 }, false },
    { { IPv4_Address{ { 208, 141, 112,  90 } },  9707 }, { {}, 9706 }, false },
    { { IPv4_Address{ { 208, 141, 112,  90 } }, 36246 }, { {}, 9706 }, false },
    { { IPv4_Address{ { 255, 255, 255, 255 } },  7444 }, { {}, 9706 }, false },
    { { IPv4_Address{ { 255, 255, 255, 255 } },  9705 }, { {}, 9706 }, false },
    { { IPv4_Address{ { 255, 255, 255, 255 } },  9706 }, { {}, 9706 }, false },
    { { IPv4_Address{ { 255, 255, 255, 255 } },  9707 }, { {}, 9706 }, false },
    { { IPv4_Address{ { 255, 255, 255, 255 } }, 36246 }, { {}, 9706 }, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, lessThanOperator, ValuesIn( lessThanOperator_TEST_CASES ) );

/**
 * \brief picolibrary::IP::operator>( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) test fixture.
 */
class greaterThanOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::operator>( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) works properly.
 */
TEST_P( greaterThanOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs > test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IP::operator>( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) test cases.
 */
comparisonOperator_Test_Case const greaterThanOperator_TEST_CASES[]{
    // clang-format off

    { { {},  7444 }, { {}, 9706 }, false },
    { { {},  9705 }, { {}, 9706 }, false },
    { { {},  9706 }, { {}, 9706 }, false },
    { { {},  9707 }, { {}, 9706 }, true  },
    { { {}, 36246 }, { {}, 9706 }, true  },

    { { IPv4_Address{ {  92, 178,  11, 221 } },  7444 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ {  92, 178,  11, 221 } },  9705 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ {  92, 178,  11, 221 } },  9706 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ {  92, 178,  11, 221 } },  9707 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ {  92, 178,  11, 221 } }, 36246 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 182 } },  7444 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 182 } },  9705 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 182 } },  9706 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 182 } },  9707 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 182 } }, 36246 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 183 } },  7444 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 183 } },  9705 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 183 } },  9706 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 183 } },  9707 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 183 } }, 36246 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 184 } },  7444 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 184 } },  9705 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 184 } },  9706 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 184 } },  9707 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 184 } }, 36246 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 253, 213, 179,  93 } },  7444 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 253, 213, 179,  93 } },  9705 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 253, 213, 179,  93 } },  9706 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 253, 213, 179,  93 } },  9707 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 253, 213, 179,  93 } }, 36246 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },

    { { {},  7444 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 9706 }, false },
    { { {},  9705 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 9706 }, false },
    { { {},  9706 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 9706 }, false },
    { { {},  9707 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 9706 }, false },
    { { {}, 36246 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 9706 }, false },
    { { {},  7444 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 9706 }, false },
    { { {},  9705 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 9706 }, false },
    { { {},  9706 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 9706 }, false },
    { { {},  9707 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 9706 }, false },
    { { {}, 36246 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 9706 }, false },
    { { {},  7444 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 9706 }, false },
    { { {},  9705 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 9706 }, false },
    { { {},  9706 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 9706 }, false },
    { { {},  9707 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 9706 }, false },
    { { {}, 36246 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 9706 }, false },

    { { IPv4_Address{ {   0,   0,   0,   0 } },  7444 }, { {}, 9706 }, true },
    { { IPv4_Address{ {   0,   0,   0,   0 } },  9705 }, { {}, 9706 }, true },
    { { IPv4_Address{ {   0,   0,   0,   0 } },  9706 }, { {}, 9706 }, true },
    { { IPv4_Address{ {   0,   0,   0,   0 } },  9707 }, { {}, 9706 }, true },
    { { IPv4_Address{ {   0,   0,   0,   0 } }, 36246 }, { {}, 9706 }, true },
    { { IPv4_Address{ { 208, 141, 112,  90 } },  7444 }, { {}, 9706 }, true },
    { { IPv4_Address{ { 208, 141, 112,  90 } },  9705 }, { {}, 9706 }, true },
    { { IPv4_Address{ { 208, 141, 112,  90 } },  9706 }, { {}, 9706 }, true },
    { { IPv4_Address{ { 208, 141, 112,  90 } },  9707 }, { {}, 9706 }, true },
    { { IPv4_Address{ { 208, 141, 112,  90 } }, 36246 }, { {}, 9706 }, true },
    { { IPv4_Address{ { 255, 255, 255, 255 } },  7444 }, { {}, 9706 }, true },
    { { IPv4_Address{ { 255, 255, 255, 255 } },  9705 }, { {}, 9706 }, true },
    { { IPv4_Address{ { 255, 255, 255, 255 } },  9706 }, { {}, 9706 }, true },
    { { IPv4_Address{ { 255, 255, 255, 255 } },  9707 }, { {}, 9706 }, true },
    { { IPv4_Address{ { 255, 255, 255, 255 } }, 36246 }, { {}, 9706 }, true },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, greaterThanOperator, ValuesIn( greaterThanOperator_TEST_CASES ) );

/**
 * \brief picolibrary::IP::operator<=( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) test fixture.
 */
class lessThanOrEqualToOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::operator<=( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) works properly.
 */
TEST_P( lessThanOrEqualToOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs <= test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IP::operator<=( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) test cases.
 */
comparisonOperator_Test_Case const lessThanOrEqualToOperator_TEST_CASES[]{
    // clang-format off

    { { {},  7444 }, { {}, 9706 }, true  },
    { { {},  9705 }, { {}, 9706 }, true  },
    { { {},  9706 }, { {}, 9706 }, true  },
    { { {},  9707 }, { {}, 9706 }, false },
    { { {}, 36246 }, { {}, 9706 }, false },

    { { IPv4_Address{ {  92, 178,  11, 221 } },  7444 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ {  92, 178,  11, 221 } },  9705 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ {  92, 178,  11, 221 } },  9706 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ {  92, 178,  11, 221 } },  9707 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ {  92, 178,  11, 221 } }, 36246 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 182 } },  7444 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 182 } },  9705 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 182 } },  9706 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 182 } },  9707 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 182 } }, 36246 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 183 } },  7444 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 183 } },  9705 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 183 } },  9706 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 183 } },  9707 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 183 } }, 36246 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 184 } },  7444 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 184 } },  9705 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 184 } },  9706 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 184 } },  9707 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 184 } }, 36246 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 253, 213, 179,  93 } },  7444 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 253, 213, 179,  93 } },  9705 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 253, 213, 179,  93 } },  9706 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 253, 213, 179,  93 } },  9707 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 253, 213, 179,  93 } }, 36246 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },

    { { {},  7444 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 9706 }, true },
    { { {},  9705 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 9706 }, true },
    { { {},  9706 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 9706 }, true },
    { { {},  9707 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 9706 }, true },
    { { {}, 36246 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 9706 }, true },
    { { {},  7444 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 9706 }, true },
    { { {},  9705 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 9706 }, true },
    { { {},  9706 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 9706 }, true },
    { { {},  9707 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 9706 }, true },
    { { {}, 36246 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 9706 }, true },
    { { {},  7444 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 9706 }, true },
    { { {},  9705 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 9706 }, true },
    { { {},  9706 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 9706 }, true },
    { { {},  9707 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 9706 }, true },
    { { {}, 36246 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 9706 }, true },

    { { IPv4_Address{ {   0,   0,   0,   0 } },  7444 }, { {}, 9706 }, false },
    { { IPv4_Address{ {   0,   0,   0,   0 } },  9705 }, { {}, 9706 }, false },
    { { IPv4_Address{ {   0,   0,   0,   0 } },  9706 }, { {}, 9706 }, false },
    { { IPv4_Address{ {   0,   0,   0,   0 } },  9707 }, { {}, 9706 }, false },
    { { IPv4_Address{ {   0,   0,   0,   0 } }, 36246 }, { {}, 9706 }, false },
    { { IPv4_Address{ { 208, 141, 112,  90 } },  7444 }, { {}, 9706 }, false },
    { { IPv4_Address{ { 208, 141, 112,  90 } },  9705 }, { {}, 9706 }, false },
    { { IPv4_Address{ { 208, 141, 112,  90 } },  9706 }, { {}, 9706 }, false },
    { { IPv4_Address{ { 208, 141, 112,  90 } },  9707 }, { {}, 9706 }, false },
    { { IPv4_Address{ { 208, 141, 112,  90 } }, 36246 }, { {}, 9706 }, false },
    { { IPv4_Address{ { 255, 255, 255, 255 } },  7444 }, { {}, 9706 }, false },
    { { IPv4_Address{ { 255, 255, 255, 255 } },  9705 }, { {}, 9706 }, false },
    { { IPv4_Address{ { 255, 255, 255, 255 } },  9706 }, { {}, 9706 }, false },
    { { IPv4_Address{ { 255, 255, 255, 255 } },  9707 }, { {}, 9706 }, false },
    { { IPv4_Address{ { 255, 255, 255, 255 } }, 36246 }, { {}, 9706 }, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, lessThanOrEqualToOperator, ValuesIn( lessThanOrEqualToOperator_TEST_CASES ) );

/**
 * \brief picolibrary::IP::operator>=( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) test fixture.
 */
class greaterThanOrEqualToOperator : public TestWithParam<comparisonOperator_Test_Case> {
};

/**
 * \brief Verify picolibrary::IP::operator>=( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) works properly.
 */
TEST_P( greaterThanOrEqualToOperator, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ( test_case.lhs >= test_case.rhs, test_case.comparison_result );
}

/**
 * \brief picolibrary::IP::operator>=( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) test cases.
 */
comparisonOperator_Test_Case const greaterThanOrEqualToOperator_TEST_CASES[]{
    // clang-format off

    { { {},  7444 }, { {}, 9706 }, false },
    { { {},  9705 }, { {}, 9706 }, false },
    { { {},  9706 }, { {}, 9706 }, true  },
    { { {},  9707 }, { {}, 9706 }, true  },
    { { {}, 36246 }, { {}, 9706 }, true  },

    { { IPv4_Address{ {  92, 178,  11, 221 } },  7444 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ {  92, 178,  11, 221 } },  9705 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ {  92, 178,  11, 221 } },  9706 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ {  92, 178,  11, 221 } },  9707 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ {  92, 178,  11, 221 } }, 36246 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 182 } },  7444 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 182 } },  9705 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 182 } },  9706 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 182 } },  9707 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 182 } }, 36246 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 183 } },  7444 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 183 } },  9705 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, false },
    { { IPv4_Address{ { 146,  45, 171, 183 } },  9706 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 183 } },  9707 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 183 } }, 36246 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 184 } },  7444 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 184 } },  9705 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 184 } },  9706 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 184 } },  9707 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 146,  45, 171, 184 } }, 36246 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 253, 213, 179,  93 } },  7444 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 253, 213, 179,  93 } },  9705 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 253, 213, 179,  93 } },  9706 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 253, 213, 179,  93 } },  9707 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },
    { { IPv4_Address{ { 253, 213, 179,  93 } }, 36246 }, { IPv4_Address{ { 146, 45, 171, 183 } }, 9706 }, true  },

    { { {},  7444 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 9706 }, false },
    { { {},  9705 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 9706 }, false },
    { { {},  9706 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 9706 }, false },
    { { {},  9707 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 9706 }, false },
    { { {}, 36246 }, { IPv4_Address{ {   0,   0,   0,   0 } }, 9706 }, false },
    { { {},  7444 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 9706 }, false },
    { { {},  9705 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 9706 }, false },
    { { {},  9706 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 9706 }, false },
    { { {},  9707 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 9706 }, false },
    { { {}, 36246 }, { IPv4_Address{ { 208, 141, 112,  90 } }, 9706 }, false },
    { { {},  7444 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 9706 }, false },
    { { {},  9705 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 9706 }, false },
    { { {},  9706 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 9706 }, false },
    { { {},  9707 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 9706 }, false },
    { { {}, 36246 }, { IPv4_Address{ { 255, 255, 255, 255 } }, 9706 }, false },

    { { IPv4_Address{ {   0,   0,   0,   0 } },  7444 }, { {}, 9706 }, true },
    { { IPv4_Address{ {   0,   0,   0,   0 } },  9705 }, { {}, 9706 }, true },
    { { IPv4_Address{ {   0,   0,   0,   0 } },  9706 }, { {}, 9706 }, true },
    { { IPv4_Address{ {   0,   0,   0,   0 } },  9707 }, { {}, 9706 }, true },
    { { IPv4_Address{ {   0,   0,   0,   0 } }, 36246 }, { {}, 9706 }, true },
    { { IPv4_Address{ { 208, 141, 112,  90 } },  7444 }, { {}, 9706 }, true },
    { { IPv4_Address{ { 208, 141, 112,  90 } },  9705 }, { {}, 9706 }, true },
    { { IPv4_Address{ { 208, 141, 112,  90 } },  9706 }, { {}, 9706 }, true },
    { { IPv4_Address{ { 208, 141, 112,  90 } },  9707 }, { {}, 9706 }, true },
    { { IPv4_Address{ { 208, 141, 112,  90 } }, 36246 }, { {}, 9706 }, true },
    { { IPv4_Address{ { 255, 255, 255, 255 } },  7444 }, { {}, 9706 }, true },
    { { IPv4_Address{ { 255, 255, 255, 255 } },  9705 }, { {}, 9706 }, true },
    { { IPv4_Address{ { 255, 255, 255, 255 } },  9706 }, { {}, 9706 }, true },
    { { IPv4_Address{ { 255, 255, 255, 255 } },  9707 }, { {}, 9706 }, true },
    { { IPv4_Address{ { 255, 255, 255, 255 } }, 36246 }, { {}, 9706 }, true },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, greaterThanOrEqualToOperator, ValuesIn( greaterThanOrEqualToOperator_TEST_CASES ) );

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IP::Endpoint>::print(
 *        picolibrary::Output_Stream &, picolibrary::IP::Endpoint const & ) properly
 *        handles a put error.
 */
TEST( outputFormatterIPEndpointPrintOutputStreamErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 48 };

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( Endpoint{ IPv4_Address{ { 219, 160, 114, 100 } }, 569 } );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief picolibrary::Output_Formatter<picolibrary::IP::Endpoint>::print() test case.
 */
struct outputFormatterIPEndpointPrint_Test_Case {
    /**
     * \brief The picolibrary::IP::Endpoint to format.
     */
    Endpoint endpoint;

    /**
     * \brief The formatted endpoint.
     */
    std::string_view formatted_endpoint;
};

auto operator<<( std::ostream & stream, outputFormatterIPEndpointPrint_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".endpoint = " << test_case.endpoint
                  << ", "
                  << ".formatted_endpoint = " << test_case.formatted_endpoint
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Output_Formatter<picolibrary::IP::Endpoint>::print() test cases.
 */
outputFormatterIPEndpointPrint_Test_Case const outputFormatterIPEndpointPrint_TEST_CASES[]{
    // clang-format off

    { {}, "ANY:0" },

    { {     0 }, "ANY:0"     },
    { { 14535 }, "ANY:14535" },
    { { 65535 }, "ANY:65535" },

    { { IPv4_Address{ {   0,   0,   0,   0 } },     0 }, "ANY:0"                 },
    { { IPv4_Address{ {   0,   0,   0,   0 } }, 14535 }, "ANY:14535"             },
    { { IPv4_Address{ {   0,   0,   0,   0 } }, 65535 }, "ANY:65535"             },
    { { IPv4_Address{ { 101, 136, 170,  45 } },     0 }, "101.136.170.45:0"      },
    { { IPv4_Address{ { 101, 136, 170,  45 } }, 14535 }, "101.136.170.45:14535"  },
    { { IPv4_Address{ { 101, 136, 170,  45 } }, 65535 }, "101.136.170.45:65535"  },
    { { IPv4_Address{ { 255, 255, 255, 255 } },     0 }, "255.255.255.255:0"     },
    { { IPv4_Address{ { 255, 255, 255, 255 } }, 14535 }, "255.255.255.255:14535" },
    { { IPv4_Address{ { 255, 255, 255, 255 } }, 65535 }, "255.255.255.255:65535" },

    // clang-format on
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::IP::Endpoint>::print(
 *        picolibrary::Output_Stream &, picolibrary::IP::Endpoint const & ) test fixture.
 */
class outputFormatterIPEndpointPrintOutputStream :
    public TestWithParam<outputFormatterIPEndpointPrint_Test_Case> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IP::Endpoint>::print(
 *        picolibrary::Output_Stream &, picolibrary::IP::Endpoint const & ) works
 *        properly.
 */
TEST_P( outputFormatterIPEndpointPrintOutputStream, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Output_String_Stream{};

    auto const result = stream.print( test_case.endpoint );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.formatted_endpoint );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterIPEndpointPrintOutputStream, ValuesIn( outputFormatterIPEndpointPrint_TEST_CASES ) );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::IP::Endpoint>::print(
 *        picolibrary::Reliable_Output_Stream &, picolibrary::IP::Endpoint const & ) test
 *        fixture.
 */
class outputFormatterIPEndpointPrintReliableOutputStream :
    public TestWithParam<outputFormatterIPEndpointPrint_Test_Case> {
};

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IP::Endpoint>::print(
 *        picolibrary::Reliable_Output_Stream &, picolibrary::IP::Endpoint const & ) works
 *        properly.
 */
TEST_P( outputFormatterIPEndpointPrintReliableOutputStream, worksProperly )
{
    auto const test_case = GetParam();

    auto stream = Reliable_Output_String_Stream{};

    auto const n = stream.print( test_case.endpoint );

    EXPECT_EQ( n, stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), test_case.formatted_endpoint );
}

INSTANTIATE_TEST_SUITE_P( testCases, outputFormatterIPEndpointPrintReliableOutputStream, ValuesIn( outputFormatterIPEndpointPrint_TEST_CASES ) );

/**
 * \brief Execute the picolibrary::IP::Endpoint automated tests.
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

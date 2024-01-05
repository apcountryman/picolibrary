/**
 * picolibrary
 *
 * Copyright 2020-2024, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::Generic_Error_Category automated test program.
 */

#include <cstdint>
#include <ios>
#include <ostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/utility.h"

namespace {

using ::picolibrary::Error_ID;
using ::picolibrary::Generic_Error;
using ::picolibrary::Generic_Error_Category;
using ::picolibrary::to_underlying;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

} // namespace

/**
 * \brief picolibrary::Generic_Error_Category::error_description() test case.
 */
struct errorDescription_Test_Case {
    /**
     * \brief The error ID whose description is to be got.
     */
    Error_ID id;

    /**
     * \brief The error ID's description.
     */
    char const * error_description;
};

auto operator<<( std::ostream & stream, errorDescription_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".id = " << std::dec << static_cast<std::uint_fast16_t>( test_case.id )
                  << ", "
                  << ".error_description = " << test_case.error_description
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Generic_Error_Category::error_description() test fixture.
 */
class errorDescription : public TestWithParam<errorDescription_Test_Case> {
};

/**
 * \brief Verify picolibrary::Generic_Error_Category::error_description() works properly.
 */
TEST_P( errorDescription, worksProperly )
{
    auto const test_case = GetParam();

    EXPECT_STREQ(
        Generic_Error_Category::instance().error_description( test_case.id ), test_case.error_description );
}

/**
 * \brief picolibrary::Generic_Error_Category::error_description() test cases.
 */
errorDescription_Test_Case const errorDescription_TEST_CASES[]{
    // clang-format off

    { to_underlying( Generic_Error::ARBITRATION_LOST ),                 "ARBITRATION_LOST"                 },
    { to_underlying( Generic_Error::BUS_ERROR ),                        "BUS_ERROR"                        },
    { to_underlying( Generic_Error::ENDPOINT_IN_USE ),                  "ENDPOINT_IN_USE"                  },
    { to_underlying( Generic_Error::EPHEMERAL_PORTS_EXHAUSTED ),        "EPHEMERAL_PORTS_EXHAUSTED"        },
    { to_underlying( Generic_Error::EXCESSIVE_MESSAGE_SIZE ),           "EXCESSIVE_MESSAGE_SIZE"           },
    { to_underlying( Generic_Error::INSUFFICIENT_CAPACITY ),            "INSUFFICIENT_CAPACITY"            },
    { to_underlying( Generic_Error::INSUFFICIENT_SOCKETS_AVAILABLE ),   "INSUFFICIENT_SOCKETS_AVAILABLE"   },
    { to_underlying( Generic_Error::INVALID_ARGUMENT ),                 "INVALID_ARGUMENT"                 },
    { to_underlying( Generic_Error::IO_STREAM_DEGRADED ),               "IO_STREAM_DEGRADED"               },
    { to_underlying( Generic_Error::LOGIC_ERROR ),                      "LOGIC_ERROR"                      },
    { to_underlying( Generic_Error::NONRESPONSIVE_DEVICE ),             "NONRESPONSIVE_DEVICE"             },
    { to_underlying( Generic_Error::NOT_CONNECTED ),                    "NOT_CONNECTED"                    },
    { to_underlying( Generic_Error::NO_SOCKETS_AVAILABLE ),             "NO_SOCKETS_AVAILABLE"             },
    { to_underlying( Generic_Error::OPERATION_TIMEOUT ),                "OPERATION_TIMEOUT"                },
    { to_underlying( Generic_Error::OUT_OF_RANGE ),                     "OUT_OF_RANGE"                     },
    { to_underlying( Generic_Error::RUNTIME_ERROR ),                    "RUNTIME_ERROR"                    },
    { to_underlying( Generic_Error::UNEXPECTED_EVENT_HANDLING_RESULT ), "UNEXPECTED_EVENT_HANDLING_RESULT" },
    { to_underlying( Generic_Error::WOULD_BLOCK ),                      "WOULD_BLOCK"                      },
    { to_underlying( Generic_Error::WOULD_OVERFLOW ),                   "WOULD_OVERFLOW"                   },
    { to_underlying( Generic_Error::WOULD_UNDERFLOW ),                  "WOULD_UNDERFLOW"                  },

    { to_underlying( Generic_Error::WOULD_UNDERFLOW ) + 1, "UNKNOWN" },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, errorDescription, ValuesIn( errorDescription_TEST_CASES ) );

/**
 * \brief Execute the picolibrary::Generic_Error_Category automated tests.
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

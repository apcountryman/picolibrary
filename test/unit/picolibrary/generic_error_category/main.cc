/**
 * picolibrary
 *
 * Copyright 2020-2022, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::Generic_Error_Category unit test program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"

namespace {

using ::picolibrary::Error_ID;
using ::picolibrary::Generic_Error;
using ::picolibrary::Generic_Error_Category;

} // namespace

/**
 * \brief Verify picolibrary::Generic_Error_Category::name() works properly.
 */
TEST( name, worksProperly )
{
    EXPECT_STREQ(
        Generic_Error_Category::instance().name(), "::picolibrary::Generic_Error" );
}

/**
 * \brief Verify picolibrary::Generic_Error_Category::error_description() works properly.
 */
TEST( errorDescription, worksProperly )
{
    struct {
        Generic_Error id;
        char const *  description;
    } const test_cases[]{
        // clang-format off

        { Generic_Error::INVALID_ARGUMENT, "INVALID_ARGUMENT" },
        { Generic_Error::LOGIC_ERROR,      "LOGIC_ERROR"      },
        { Generic_Error::OUT_OF_RANGE,     "OUT_OF_RANGE"     },
        { Generic_Error::RUNTIME_ERROR,    "RUNTIME_ERROR"    },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        EXPECT_STREQ(
            Generic_Error_Category::instance().error_description(
                static_cast<Error_ID>( test_case.id ) ),
            test_case.description );
    } // for

    EXPECT_STREQ(
        Generic_Error_Category::instance().error_description(
            static_cast<Error_ID>( Generic_Error::RUNTIME_ERROR ) + 1 ),
        "UNKNOWN" );
}

/**
 * \brief Execute the picolibrary::Generic_Error_Category unit tests.
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
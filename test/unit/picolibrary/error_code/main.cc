/**
 * picolibrary
 *
 * Copyright 2020 Andrew Countryman <apcountryman@gmail.com>
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
 * \brief picolibrary::Error_Code unit test program.
 */

#include "gmock/gmock.h"
#include "picolibrary/error.h"

namespace {

using ::picolibrary::Error_Code;

} // namespace

/**
 * \brief Verify picolibrary::Error_Code::Error_Code() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const error = Error_Code{};

    EXPECT_FALSE( error );
    EXPECT_NE( &error.category(), nullptr );
    EXPECT_STREQ( error.category().name(), "::picolibrary::Default_Error" );
    EXPECT_EQ( error.id(), 0 );
}

/**
 * \brief Execute the picolibrary::Error_Code unit tests.
 *
 * \param[in] argc The number of arguments to pass to testing::InitGoogleMock().
 * \param[in] argv The array  of arguments to pass to testing::InitGoogleMock().
 *
 * \return See Google Test's RUN_ALL_TESTS().
 */
int main( int argc, char * argv[] )
{
    ::testing::InitGoogleMock( &argc, argv );

    return RUN_ALL_TESTS();
}

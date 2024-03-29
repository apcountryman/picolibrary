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
 * \brief picolibrary::Generic_Error automated test program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Error_ID;
using ::picolibrary::Generic_Error;
using ::picolibrary::Generic_Error_Category;

} // namespace

/**
 * \brief Verify picolibrary::make_error_code( picolibrary::Generic_Error ) works
 *        properly.
 */
TEST( makeErrorCode, worksProperly )
{
    auto const id = Error_ID{ 226 };

    auto const error = Error_Code{ static_cast<Generic_Error>( id ) };

    EXPECT_EQ( &error.category(), &Generic_Error_Category::instance() );
    EXPECT_EQ( error.id(), id );
}

/**
 * \brief Execute the picolibrary::Generic_Error automated tests.
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

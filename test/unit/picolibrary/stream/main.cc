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
 * \brief picolibrary::Stream unit test program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/stream.h"

namespace {

using ::picolibrary::Stream;

} // namespace

/**
 * \brief Verify picolibrary::Stream::Stream() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const stream = Stream{};

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_FALSE( stream.error_present() );
    EXPECT_TRUE( stream );
    EXPECT_FALSE( not stream );
    EXPECT_FALSE( stream.end_of_file_reached() );
}

/**
 * \brief Execute the picolibrary::Stream unit tests.
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

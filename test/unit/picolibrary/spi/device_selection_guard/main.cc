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
 * \brief picolibrary::SPI::Device_Selection_Guard unit test program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/spi.h"
#include "picolibrary/testing/unit/spi.h"

namespace {

using ::picolibrary::Testing::Unit::SPI::Mock_Device_Selector;

using Device_Selection_Guard = ::picolibrary::SPI::Device_Selection_Guard<Mock_Device_Selector>;

} // namespace

/**
 * \brief Verify picolibrary::SPI::Device_Selection_Guard::Device_Selection_Guard() works
 *        properly.
 */
TEST( constructorDefault, worksProperly )
{
    Device_Selection_Guard{};
}

/**
 * \brief Execute the picolibrary::SPI::Device_Selection_Guard unit tests.
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

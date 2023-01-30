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
 * \brief picolibrary::SPI::Device_Selection_Guard automated test program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/spi.h"
#include "picolibrary/testing/automated/spi.h"

namespace {

using ::picolibrary::SPI::Device_Selection_Guard;
using ::picolibrary::Testing::Automated::SPI::Mock_Device_Selector;

} // namespace

/**
 * \brief Verify picolibrary::SPI::Device_Selection_Guard works properly.
 */
TEST( deviceSelectionGuard, worksProperly )
{
    auto device_selector = Mock_Device_Selector{};

    EXPECT_CALL( device_selector, select() );

    auto const guard = Device_Selection_Guard{ device_selector };

    EXPECT_CALL( device_selector, deselect() );
}

/**
 * \brief Execute the picolibrary::SPI::Device_Selection_Guard automated tests.
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

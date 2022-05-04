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
 * \brief picolibrary::SPI::GPIO_Output_Pin_Device_Selector automated test program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/spi.h"
#include "picolibrary/testing/automated/gpio.h"

namespace {

using Device_Selector =
    ::picolibrary::SPI::GPIO_Output_Pin_Device_Selector<::picolibrary::Testing::Automated::GPIO::Mock_Output_Pin>;

} // namespace

/**
 * \brief Verify picolibrary::SPI::GPIO_Output_Pin_Device_Selector::select() works
 *        properly.
 */
TEST( select, worksProperly )
{
    auto device_selector = Device_Selector{};

    EXPECT_CALL( device_selector, transition_to_high() );

    device_selector.select();
}

/**
 * \brief Verify picolibrary::SPI::GPIO_Output_Pin_Device_Selector::deselect() works
 *        properly.
 */
TEST( deselect, worksProperly )
{
    auto device_deselector = Device_Selector{};

    EXPECT_CALL( device_deselector, transition_to_low() );

    device_deselector.deselect();
}

/**
 * \brief Execute the picolibrary::SPI::GPIO_Output_Pin_Device_Selector automated tests.
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

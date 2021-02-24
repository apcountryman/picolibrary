/**
 * picolibrary
 *
 * Copyright 2020-2021, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::SPI::GPIO_Output_Pin_Device_Selector unit test program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/spi.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/gpio.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/void.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::testing::Return;

using Device_Selector =
    ::picolibrary::SPI::GPIO_Output_Pin_Device_Selector<::picolibrary::Testing::Unit::GPIO::Mock_Output_Pin>;

} // namespace

/**
 * \brief Verify picolibrary::SPI::GPIO_Output_Pin_Device_Selector::select() properly
 *        handles a selection error.
 */
TEST( select, selectionError )
{
    auto device_selector = Device_Selector{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( device_selector, transition_to_high() ).WillOnce( Return( error ) );

    auto const result = device_selector.select();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::SPI::GPIO_Output_Pin_Device_Selector::select() works
 *        properly.
 */
TEST( select, worksProperly )
{
    auto device_selector = Device_Selector{};

    EXPECT_CALL( device_selector, transition_to_high() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( device_selector.select().is_error() );
}

/**
 * \brief Verify picolibrary::SPI::GPIO_Output_Pin_Device_Selector::dedeselect() properly
 *        handles a deselection error.
 */
TEST( deselect, deselectionError )
{
    auto device_selector = Device_Selector{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( device_selector, transition_to_low() ).WillOnce( Return( error ) );

    auto const result = device_selector.deselect();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::SPI::GPIO_Output_Pin_Device_Selector::deselect() works
 *        properly.
 */
TEST( deselect, worksProperly )
{
    auto device_selector = Device_Selector{};

    EXPECT_CALL( device_selector, transition_to_low() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( device_selector.deselect().is_error() );
}

/**
 * \brief Execute the picolibrary::SPI::GPIO_Output_Pin_Device_Selector unit tests.
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

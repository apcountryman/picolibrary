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
 * \brief picolibrary::SPI::Device unit test program.
 */

#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/spi.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/spi.h"
#include "picolibrary/utility.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::SPI::make_device_selection_guard;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::SPI::Mock_Controller;
using ::picolibrary::Testing::Unit::SPI::Mock_Device_Selector;
using ::testing::Return;

class Device : public ::picolibrary::SPI::Device<Mock_Controller, Mock_Device_Selector::Handle> {
  public:
    constexpr Device(
        Mock_Controller &              controller,
        Mock_Controller::Configuration configuration,
        Mock_Device_Selector::Handle   device_selector ) noexcept :
        ::picolibrary::SPI::Device<Mock_Controller, Mock_Device_Selector::Handle>{ controller,
                                                                                   configuration,
                                                                                   std::move( device_selector ) }
    {
    }

    using ::picolibrary::SPI::Device<Mock_Controller, Mock_Device_Selector::Handle>::initialize;
    using ::picolibrary::SPI::Device<Mock_Controller, Mock_Device_Selector::Handle>::device_selector;
};

} // namespace

/**
 * \brief Verify picolibrary::SPI::Device::initialize() properly handles an initialization
 *        error.
 */
TEST( initialize, initializationError )
{
    auto controller      = Mock_Controller{};
    auto device_selector = Mock_Device_Selector{};

    auto device = Device{ controller,
                          random<Mock_Controller::Configuration>(),
                          device_selector.handle() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( device_selector, initialize() ).WillOnce( Return( error ) );

    auto const result = device.initialize();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::SPI::Device::initialize() works properly.
 */
TEST( initialize, worksProperly )
{
    auto controller      = Mock_Controller{};
    auto device_selector = Mock_Device_Selector{};

    auto device = Device{ controller,
                          random<Mock_Controller::Configuration>(),
                          device_selector.handle() };

    EXPECT_CALL( device_selector, initialize() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( device.initialize().is_error() );
}

/**
 * \brief Verify picolibrary::SPI::Device::device_selector() works properly.
 */
TEST( deviceSelector, worksProperly )
{
    auto controller      = Mock_Controller{};
    auto device_selector = Mock_Device_Selector{};

    auto const device = Device{ controller,
                                random<Mock_Controller::Configuration>(),
                                device_selector.handle() };

    ON_CALL( device_selector, select() ).WillByDefault( Return( Result<Void, Error_Code>{} ) );
    ON_CALL( device_selector, deselect() ).WillByDefault( Return( Result<Void, Error_Code>{} ) );

    auto result = make_device_selection_guard( device.device_selector() );
}

/**
 * \brief Execute the picolibrary::SPI::Device unit tests.
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

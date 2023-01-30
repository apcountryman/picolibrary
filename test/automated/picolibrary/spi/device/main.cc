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
 * \brief picolibrary::SPI::Device automated test program.
 */

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/spi.h"
#include "picolibrary/testing/automated/random.h"
#include "picolibrary/testing/automated/spi.h"

namespace {

using ::picolibrary::SPI::Device_Selection_Guard;
using ::picolibrary::Testing::Automated::random;
using ::picolibrary::Testing::Automated::random_container;
using ::picolibrary::Testing::Automated::SPI::Mock_Controller;
using ::picolibrary::Testing::Automated::SPI::Mock_Device_Selector;
using ::testing::A;
using ::testing::Return;

class Device : public ::picolibrary::SPI::Device<Mock_Controller, Mock_Device_Selector::Handle> {
  public:
    Device(
        Mock_Controller &                      controller,
        Mock_Controller::Configuration const & configuration,
        Mock_Device_Selector::Handle           device_selector ) noexcept :
        ::picolibrary::SPI::Device<Mock_Controller, Mock_Device_Selector::Handle>{ controller,
                                                                                   configuration,
                                                                                   std::move( device_selector ) }
    {
    }

    using ::picolibrary::SPI::Device<Mock_Controller, Mock_Device_Selector::Handle>::initialize;

    using ::picolibrary::SPI::Device<Mock_Controller, Mock_Device_Selector::Handle>::configure;

    using ::picolibrary::SPI::Device<Mock_Controller, Mock_Device_Selector::Handle>::device_selector;

    using ::picolibrary::SPI::Device<Mock_Controller, Mock_Device_Selector::Handle>::exchange;

    using ::picolibrary::SPI::Device<Mock_Controller, Mock_Device_Selector::Handle>::receive;

    using ::picolibrary::SPI::Device<Mock_Controller, Mock_Device_Selector::Handle>::transmit;
};

} // namespace

/**
 * \brief Verify picolibrary::SPI::Device::initialize() works properly.
 */
TEST( initialize, worksProperly )
{
    auto controller      = Mock_Controller{};
    auto device_selector = Mock_Device_Selector{};

    auto const device = Device{ controller,
                                random<Mock_Controller::Configuration>(),
                                device_selector.handle() };

    EXPECT_CALL( device_selector, initialize() );

    device.initialize();
}

/**
 * \brief Verify picolibrary::SPI::Device::configure() works properly.
 */
TEST( configure, worksProperly )
{
    auto       controller      = Mock_Controller{};
    auto const configuration   = random<Mock_Controller::Configuration>();
    auto       device_selector = Mock_Device_Selector{};

    auto const device = Device{ controller, configuration, device_selector.handle() };

    EXPECT_CALL( controller, configure( configuration ) );

    device.configure();
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

    EXPECT_CALL( device_selector, select() );
    EXPECT_CALL( device_selector, deselect() );

    Device_Selection_Guard{ device.device_selector() };
}

/**
 * \brief Verify picolibrary::SPI::Device::exchange( std::uint8_t ) works properly.
 */
TEST( exchange, worksProperly )
{
    auto controller      = Mock_Controller{};
    auto device_selector = Mock_Device_Selector{};

    auto const device = Device{ controller,
                                random<Mock_Controller::Configuration>(),
                                device_selector.handle() };

    auto const tx = random<std::uint8_t>();
    auto const rx = random<std::uint8_t>();

    EXPECT_CALL( controller, exchange( tx ) ).WillOnce( Return( rx ) );

    EXPECT_EQ( device.exchange( tx ), rx );
}

/**
 * \brief Verify picolibrary::SPI::Device::exchange( std::uint8_t const *, std::uint8_t
 *        const *, std::uint8_t *, std::uint8_t * ) works properly.
 */
TEST( exchangeBlock, worksProperly )
{
    auto controller      = Mock_Controller{};
    auto device_selector = Mock_Device_Selector{};

    auto const device = Device{ controller,
                                random<Mock_Controller::Configuration>(),
                                device_selector.handle() };

    auto const size        = random<std::size_t>( 0, 15 );
    auto const tx          = random_container<std::vector<std::uint8_t>>( size );
    auto const rx_expected = random_container<std::vector<std::uint8_t>>( size );

    EXPECT_CALL( controller, exchange( tx ) ).WillOnce( Return( rx_expected ) );

    auto rx = std::vector<std::uint8_t>( size );
    device.exchange( &*tx.begin(), &*tx.end(), &*rx.begin(), &*rx.end() );

    EXPECT_EQ( rx, rx_expected );
}

/**
 * \brief Verify picolibrary::SPI::Device::receive() works properly.
 */
TEST( receive, worksProperly )
{
    auto controller      = Mock_Controller{};
    auto device_selector = Mock_Device_Selector{};

    auto const device = Device{ controller,
                                random<Mock_Controller::Configuration>(),
                                device_selector.handle() };

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( controller, receive() ).WillOnce( Return( data ) );

    EXPECT_EQ( device.receive(), data );
}

/**
 * \brief Verify picolibrary::SPI::Device::receive( std::uint8_t *, std::uint8_t * ) works
 *        properly.
 */
TEST( receiveBlock, worksProperly )
{
    auto controller      = Mock_Controller{};
    auto device_selector = Mock_Device_Selector{};

    auto const device = Device{ controller,
                                random<Mock_Controller::Configuration>(),
                                device_selector.handle() };

    auto const size          = random<std::size_t>( 0, 15 );
    auto const data_expected = random_container<std::vector<std::uint8_t>>( size );

    EXPECT_CALL( controller, receive( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( data_expected ) );

    auto data = std::vector<std::uint8_t>( size );
    device.receive( &*data.begin(), &*data.end() );

    EXPECT_EQ( data, data_expected );
}

/**
 * \brief Verify picolibrary::SPI::Device::transmit( std::uint8_t ) works properly.
 */
TEST( transmit, worksProperly )
{
    auto controller      = Mock_Controller{};
    auto device_selector = Mock_Device_Selector{};

    auto const device = Device{ controller,
                                random<Mock_Controller::Configuration>(),
                                device_selector.handle() };

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( controller, transmit( data ) );

    device.transmit( data );
}

/**
 * \brief Verify picolibrary::SPI::Device::transmit( std::uint8_t const *, std::uint8_t
 *        const * ) works properly.
 */
TEST( transmitBlock, worksProperly )
{
    auto controller      = Mock_Controller{};
    auto device_selector = Mock_Device_Selector{};

    auto const device = Device{ controller,
                                random<Mock_Controller::Configuration>(),
                                device_selector.handle() };

    auto const data = random_container<std::vector<std::uint8_t>>();

    EXPECT_CALL( controller, transmit( data ) );

    device.transmit( &*data.begin(), &*data.end() );
}

/**
 * \brief Execute the picolibrary::SPI::Device automated tests.
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

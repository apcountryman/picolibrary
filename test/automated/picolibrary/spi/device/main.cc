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
 * \brief picolibrary::SPI::Device automated test program.
 */

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/spi.h"
#include "picolibrary/testing/automated/spi.h"

namespace {

using ::picolibrary::SPI::Device_Selection_Guard;
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

    auto const device = Device{ controller, 161, device_selector.handle() };

    EXPECT_CALL( device_selector, initialize() );

    device.initialize();
}

/**
 * \brief Verify picolibrary::SPI::Device::configure() works properly.
 */
TEST( configure, worksProperly )
{
    auto       controller      = Mock_Controller{};
    auto const configuration   = Mock_Controller::Configuration{ 224 };
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

    auto const device = Device{ controller, 207, device_selector.handle() };

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

    auto const device = Device{ controller, 192, device_selector.handle() };

    auto const tx = std::uint8_t{ 0x48 };
    auto const rx = std::uint8_t{ 0xC8 };

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

    auto const device = Device{ controller, 140, device_selector.handle() };

    auto const tx          = std::vector<std::uint8_t>{ 0x9C, 0x73, 0x38, 0x65, 0xC6 };
    auto       rx          = std::vector<std::uint8_t>( 5 );
    auto const rx_expected = std::vector<std::uint8_t>{ 0x56, 0x8C, 0xC2, 0x29, 0x85 };

    EXPECT_CALL( controller, exchange( tx ) ).WillOnce( Return( rx_expected ) );

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

    auto const device = Device{ controller, 201, device_selector.handle() };

    auto const data = std::uint8_t{ 0xDD };

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

    auto const device = Device{ controller, 122, device_selector.handle() };

    auto       data          = std::vector<std::uint8_t>( 4 );
    auto const data_expected = std::vector<std::uint8_t>{ 0xEC, 0x22, 0x3F, 0xA5 };

    EXPECT_CALL( controller, receive( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( data_expected ) );

    device.receive( &*data.begin(), &*data.end() );

    EXPECT_EQ( data, data_expected );
}

/**
 * \brief Verify picolibrary::SPI::Device::transmit() works properly.
 */
TEST( transmit, worksProperly )
{
    auto controller      = Mock_Controller{};
    auto device_selector = Mock_Device_Selector{};

    auto const device = Device{ controller, 74, device_selector.handle() };

    auto const data = std::uint8_t{ 0xEC };

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

    auto const device = Device{ controller, 44, device_selector.handle() };

    auto const data = std::vector<std::uint8_t>{ 0xBF, 0x46, 0x55, 0x3D, 0x5C, 0x68 };

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

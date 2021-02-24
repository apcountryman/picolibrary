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
 * \brief picolibrary::SPI::Device unit test program.
 */

#include <cstdint>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/spi.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/spi.h"
#include "picolibrary/void.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::SPI::make_device_selection_guard;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::picolibrary::Testing::Unit::SPI::Mock_Controller;
using ::picolibrary::Testing::Unit::SPI::Mock_Device_Selector;
using ::testing::_;
using ::testing::A;
using ::testing::Return;

class Device : public ::picolibrary::SPI::Device<Mock_Controller, Mock_Device_Selector::Handle> {
  public:
    Device( Mock_Controller & controller, Mock_Controller::Configuration configuration, Mock_Device_Selector::Handle device_selector ) noexcept
        :
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
 * \brief Verify picolibrary::SPI::Device::configure() properly handles a configuration
 *        error.
 */
TEST( configure, configurationError )
{
    auto controller      = Mock_Controller{};
    auto device_selector = Mock_Device_Selector{};

    auto device = Device{ controller,
                          random<Mock_Controller::Configuration>(),
                          device_selector.handle() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( controller, configure( _ ) ).WillOnce( Return( error ) );

    auto const result = device.configure();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
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

    EXPECT_CALL( controller, configure( configuration ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( device.configure().is_error() );
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

    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    static_cast<void>( make_device_selection_guard( device.device_selector() ) );
}

/**
 * \brief Verify picolibrary::SPI::Device::exchange( std::uint8_t ) properly handles an
 *        exchange error.
 */
TEST( exchange, exchangeError )
{
    auto controller      = Mock_Controller{};
    auto device_selector = Mock_Device_Selector{};

    auto const device = Device{ controller,
                                random<Mock_Controller::Configuration>(),
                                device_selector.handle() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( controller, exchange( A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = device.exchange( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
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

    auto const result = device.exchange( tx );

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), rx );
}

/**
 * \brief Verify picolibrary::SPI::Device::exchange( std::uint8_t const *, std::uint8_t
 *        const *, std::uint8_t *, std::uint8_t * ) properly handles an exchange error.
 */
TEST( exchangeBlock, exchangeError )
{
    auto controller      = Mock_Controller{};
    auto device_selector = Mock_Device_Selector{};

    auto const device = Device{ controller,
                                random<Mock_Controller::Configuration>(),
                                device_selector.handle() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( controller, exchange( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );

    auto const size = random<std::uint_fast8_t>();
    auto const tx   = random_container<std::vector<std::uint8_t>>( size );
    auto       rx   = std::vector<std::uint8_t>( size );
    auto const result = device.exchange( &*tx.begin(), &*tx.end(), &*rx.begin(), &*rx.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
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

    auto const size        = random<std::uint_fast8_t>();
    auto const tx          = random_container<std::vector<std::uint8_t>>( size );
    auto const rx_expected = random_container<std::vector<std::uint8_t>>( size );

    EXPECT_CALL( controller, exchange( tx ) ).WillOnce( Return( rx_expected ) );

    auto rx = std::vector<std::uint8_t>( size );
    EXPECT_FALSE( device.exchange( &*tx.begin(), &*tx.end(), &*rx.begin(), &*rx.end() ).is_error() );

    EXPECT_EQ( rx, rx_expected );
}

/**
 * \brief Verify picolibrary::SPI::Device::receive() properly handles a reception error.
 */
TEST( receive, receptionError )
{
    auto controller      = Mock_Controller{};
    auto device_selector = Mock_Device_Selector{};

    auto const device = Device{ controller,
                                random<Mock_Controller::Configuration>(),
                                device_selector.handle() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( controller, receive() ).WillOnce( Return( error ) );

    auto const result = device.receive();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
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

    auto const result = device.receive();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::SPI::Device::receive( std::uint8_t *, std::uint8_t * )
 *        properly handles a reception error.
 */
TEST( receiveBlock, receptionError )
{
    auto controller      = Mock_Controller{};
    auto device_selector = Mock_Device_Selector{};

    auto const device = Device{ controller,
                                random<Mock_Controller::Configuration>(),
                                device_selector.handle() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( controller, receive( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );

    auto       rx     = std::vector<std::uint8_t>( random<std::uint_fast8_t>() );
    auto const result = device.receive( &*rx.begin(), &*rx.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
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

    auto const size        = random<std::uint_fast8_t>();
    auto const rx_expected = random_container<std::vector<std::uint8_t>>( size );

    EXPECT_CALL( controller, receive( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( rx_expected ) );

    auto rx = std::vector<std::uint8_t>( size );
    EXPECT_FALSE( device.receive( &*rx.begin(), &*rx.end() ).is_error() );

    EXPECT_EQ( rx, rx_expected );
}

/**
 * \brief Verify picolibrary::SPI::Device::transmit( std::uint8_t ) properly handles a
 *        transmission error.
 */
TEST( transmit, transmissionError )
{
    auto controller      = Mock_Controller{};
    auto device_selector = Mock_Device_Selector{};

    auto const device = Device{ controller,
                                random<Mock_Controller::Configuration>(),
                                device_selector.handle() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( controller, transmit( A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = device.transmit( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
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

    EXPECT_CALL( controller, transmit( data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( device.transmit( data ).is_error() );
}

/**
 * \brief Verify picolibrary::SPI::Device::transmit( std::uint8_t const *, std::uint8_t
 *        const * ) properly handles a transmission error.
 */
TEST( transmitBlock, transmissionError )
{
    auto controller      = Mock_Controller{};
    auto device_selector = Mock_Device_Selector{};

    auto const device = Device{ controller,
                                random<Mock_Controller::Configuration>(),
                                device_selector.handle() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( controller, transmit( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );

    auto const tx     = random_container<std::vector<std::uint8_t>>();
    auto const result = device.transmit( &*tx.begin(), &*tx.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::SPI::Device::transmit( std::uint8_t const *, std::uint8_t
 *        const *  ) works properly.
 */
TEST( transmitBlock, worksProperly )
{
    auto controller      = Mock_Controller{};
    auto device_selector = Mock_Device_Selector{};

    auto const device = Device{ controller,
                                random<Mock_Controller::Configuration>(),
                                device_selector.handle() };

    auto const tx = random_container<std::vector<std::uint8_t>>();

    EXPECT_CALL( controller, transmit( tx ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( device.transmit( &*tx.begin(), &*tx.end() ).is_error() );
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

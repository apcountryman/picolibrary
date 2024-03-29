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
 * \brief picolibrary::Testing::Automated::SPI interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_SPI_H
#define PICOLIBRARY_TESTING_AUTOMATED_SPI_H

#include <algorithm>
#include <cstdint>
#include <vector>

#include "gmock/gmock.h"
#include "picolibrary/testing/automated/mock_handle.h"

/**
 * \brief SPI automated testing facilities.
 */
namespace picolibrary::Testing::Automated::SPI {

/**
 * \brief Mock basic controller.
 */
class Mock_Basic_Controller {
  public:
    using Configuration = std::uint_fast8_t;

    class Handle : public Mock_Handle<Mock_Basic_Controller> {
      public:
        using Configuration = Mock_Basic_Controller::Configuration;

        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Basic_Controller & mock ) noexcept :
            Mock_Handle<Mock_Basic_Controller>{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        void initialize()
        {
            Mock_Handle<Mock_Basic_Controller>::mock().initialize();
        }

        void configure( Configuration const & configuration )
        {
            Mock_Handle<Mock_Basic_Controller>::mock().configure( configuration );
        }

        auto exchange( std::uint8_t data ) -> std::uint8_t
        {
            return Mock_Handle<Mock_Basic_Controller>::mock().exchange( data );
        }
    };

    Mock_Basic_Controller() = default;

    Mock_Basic_Controller( Mock_Basic_Controller && ) = delete;

    Mock_Basic_Controller( Mock_Basic_Controller const & ) = delete;

    ~Mock_Basic_Controller() noexcept = default;

    auto operator=( Mock_Basic_Controller && ) = delete;

    auto operator=( Mock_Basic_Controller const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    MOCK_METHOD( void, initialize, () );

    MOCK_METHOD( void, configure, (Configuration const &));

    MOCK_METHOD( std::uint8_t, exchange, ( std::uint8_t ) );
};

/**
 * \brief Mock controller.
 */
class Mock_Controller : public Mock_Basic_Controller {
  public:
    class Handle : public Mock_Basic_Controller::Handle {
      public:
        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Controller & mock ) noexcept :
            Mock_Basic_Controller::Handle{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        auto mock() noexcept -> Mock_Controller &
        {
            return static_cast<Mock_Controller &>( Mock_Basic_Controller::Handle::mock() );
        }

        auto mock() const noexcept -> Mock_Controller const &
        {
            return static_cast<Mock_Controller const &>( Mock_Basic_Controller::Handle::mock() );
        }

        using Mock_Basic_Controller::Handle::exchange;

        void exchange( std::uint8_t const * tx_begin, std::uint8_t const * tx_end, std::uint8_t * rx_begin, std::uint8_t * rx_end )
        {
            mock().exchange( tx_begin, tx_end, rx_begin, rx_end );
        }

        auto receive() -> std::uint8_t
        {
            return mock().receive();
        }

        void receive( std::uint8_t * begin, std::uint8_t * end )
        {
            mock().receive( begin, end );
        }

        void transmit( std::uint8_t data )
        {
            mock().transmit( data );
        }

        void transmit( std::uint8_t const * begin, std::uint8_t const * end )
        {
            mock().transmit( begin, end );
        }
    };

    Mock_Controller() = default;

    Mock_Controller( Mock_Controller && ) = delete;

    Mock_Controller( Mock_Controller const & ) = delete;

    ~Mock_Controller() noexcept = default;

    auto operator=( Mock_Controller && ) = delete;

    auto operator=( Mock_Controller const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    using Mock_Basic_Controller::exchange;
    using Mock_Basic_Controller::gmock_exchange;

    MOCK_METHOD( std::vector<std::uint8_t>, exchange, (std::vector<std::uint8_t>));

    void exchange( std::uint8_t const * tx_begin, std::uint8_t const * tx_end, std::uint8_t * rx_begin, std::uint8_t * rx_end )
    {
        static_cast<void>( rx_end );

        auto const data = exchange( std::vector<std::uint8_t>{ tx_begin, tx_end } );

        std::copy( data.begin(), data.end(), rx_begin );
    }

    MOCK_METHOD( std::uint8_t, receive, () );
    MOCK_METHOD( std::vector<std::uint8_t>, receive, (std::vector<std::uint8_t>));

    void receive( std::uint8_t * begin, std::uint8_t * end )
    {
        static_cast<void>( end );

        auto const data = receive( std::vector<std::uint8_t>{} );

        std::copy( data.begin(), data.end(), begin );
    }

    MOCK_METHOD( void, transmit, ( std::uint8_t ) );
    MOCK_METHOD( void, transmit, (std::vector<std::uint8_t>));

    void transmit( std::uint8_t const * begin, std::uint8_t const * end )
    {
        transmit( std::vector<std::uint8_t>{ begin, end } );
    }
};

/**
 * \brief Mock device selector.
 */
class Mock_Device_Selector {
  public:
    class Handle : public Mock_Handle<Mock_Device_Selector> {
      public:
        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Device_Selector & mock ) noexcept :
            Mock_Handle<Mock_Device_Selector>{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        void initialize()
        {
            Mock_Handle<Mock_Device_Selector>::mock().initialize();
        }

        void select()
        {
            Mock_Handle<Mock_Device_Selector>::mock().select();
        }

        void deselect()
        {
            Mock_Handle<Mock_Device_Selector>::mock().deselect();
        }
    };

    Mock_Device_Selector() = default;

    Mock_Device_Selector( Mock_Device_Selector && ) = delete;

    Mock_Device_Selector( Mock_Device_Selector const & ) = delete;

    ~Mock_Device_Selector() noexcept = default;

    auto operator=( Mock_Device_Selector && ) = delete;

    auto operator=( Mock_Device_Selector const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    MOCK_METHOD( void, initialize, () );

    MOCK_METHOD( void, select, () );

    MOCK_METHOD( void, deselect, () );
};

/**
 * \brief Mock device.
 */
class Mock_Device {
  public:
    Mock_Device() noexcept
    {
    }

    Mock_Device( Mock_Controller &, Mock_Controller::Configuration const &, Mock_Device_Selector::Handle ) noexcept
    {
    }

    Mock_Device( Mock_Device && ) = delete;

    Mock_Device( Mock_Device const & ) = delete;

    ~Mock_Device() noexcept = default;

    auto operator=( Mock_Device && ) = delete;

    auto operator=( Mock_Device const & ) = delete;

    MOCK_METHOD( void, initialize, (), ( const ) );

    MOCK_METHOD( void, configure, (), ( const ) );

    MOCK_METHOD( Mock_Device_Selector::Handle &, device_selector, (), ( const ) );

    MOCK_METHOD( std::uint8_t, exchange, ( std::uint8_t ), ( const ) );
    MOCK_METHOD( std::vector<std::uint8_t>, exchange, (std::vector<std::uint8_t>), ( const ) );

    void exchange( std::uint8_t const * tx_begin, std::uint8_t const * tx_end, std::uint8_t * rx_begin, std::uint8_t * rx_end ) const
    {
        static_cast<void>( rx_end );

        auto const data = exchange( std::vector<std::uint8_t>{ tx_begin, tx_end } );

        std::copy( data.begin(), data.end(), rx_begin );
    }

    MOCK_METHOD( std::uint8_t, receive, (), ( const ) );
    MOCK_METHOD( std::vector<std::uint8_t>, receive, (std::vector<std::uint8_t>), ( const ) );

    void receive( std::uint8_t * begin, std::uint8_t * end ) const
    {
        static_cast<void>( end );

        auto const data = receive( std::vector<std::uint8_t>{} );

        std::copy( data.begin(), data.end(), begin );
    }

    MOCK_METHOD( void, transmit, ( std::uint8_t ), ( const ) );
    MOCK_METHOD( void, transmit, (std::vector<std::uint8_t>), ( const ) );

    void transmit( std::uint8_t const * begin, std::uint8_t const * end ) const
    {
        transmit( std::vector<std::uint8_t>{ begin, end } );
    }
};

} // namespace picolibrary::Testing::Automated::SPI

#endif // PICOLIBRARY_TESTING_AUTOMATED_SPI_H

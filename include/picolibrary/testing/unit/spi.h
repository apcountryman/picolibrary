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
 * \brief picolibrary::Testing::Unit::SPI interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_SPI_H
#define PICOLIBRARY_TESTING_UNIT_SPI_H

#include <algorithm>
#include <cstdint>
#include <vector>

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/mock_handle.h"
#include "picolibrary/void.h"

/**
 * \brief Serial Peripheral Interface (SPI) unit testing facilities.
 */
namespace picolibrary::Testing::Unit::SPI {

/**
 * \brief Mock SPI basic controller.
 */
class Mock_Basic_Controller {
  public:
    using Configuration = std::uint_fast16_t;

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

        auto initialize()
        {
            return mock().initialize();
        }

        auto configure( Configuration configuration )
        {
            return mock().configure( configuration );
        }

        auto exchange( std::uint8_t data )
        {
            return mock().exchange( data );
        }
    };

    Mock_Basic_Controller() = default;

    Mock_Basic_Controller( Mock_Basic_Controller && ) = delete;

    Mock_Basic_Controller( Mock_Basic_Controller const & ) = delete;

    ~Mock_Basic_Controller() noexcept = default;

    auto operator=( Mock_Basic_Controller && ) = delete;

    auto operator=( Mock_Basic_Controller const & ) = delete;

    auto handle() noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );

    MOCK_METHOD( (Result<Void, Error_Code>), configure, ( Configuration ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), exchange, ( std::uint8_t ) );
};

/**
 * \brief Mock SPI controller.
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

        auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        auto & mock() noexcept
        {
            return static_cast<Mock_Controller &>( Mock_Basic_Controller::Handle::mock() );
        }

        auto const & mock() const noexcept
        {
            return static_cast<Mock_Controller const &>( Mock_Basic_Controller::Handle::mock() );
        }

        using Mock_Basic_Controller::Handle::exchange;

        auto exchange( std::uint8_t const * tx_begin, std::uint8_t const * tx_end, std::uint8_t * rx_begin, std::uint8_t * rx_end )
        {
            return mock().exchange( tx_begin, tx_end, rx_begin, rx_end );
        }

        auto receive()
        {
            return mock().receive();
        }

        auto receive( std::uint8_t * begin, std::uint8_t * end )
        {
            return mock().receive( begin, end );
        }

        auto transmit( std::uint8_t data )
        {
            return mock().transmit( data );
        }

        auto transmit( std::uint8_t const * begin, std::uint8_t const * end )
        {
            return mock().transmit( begin, end );
        }
    };

    Mock_Controller() = default;

    Mock_Controller( Mock_Controller && ) = delete;

    Mock_Controller( Mock_Controller const & ) = delete;

    ~Mock_Controller() noexcept = default;

    auto operator=( Mock_Controller && ) = delete;

    auto operator=( Mock_Controller const & ) = delete;

    auto handle() noexcept
    {
        return Handle{ *this };
    }

    using Mock_Basic_Controller::exchange;
    using Mock_Basic_Controller::gmock_exchange;

    MOCK_METHOD( (Result<std::vector<std::uint8_t>, Error_Code>), exchange, (std::vector<std::uint8_t>));

    auto exchange( std::uint8_t const * tx_begin, std::uint8_t const * tx_end, std::uint8_t * rx_begin, std::uint8_t * rx_end )
        -> Result<Void, Error_Code>
    {
        static_cast<void>( rx_end );

        auto const result = exchange( std::vector<std::uint8_t>{ tx_begin, tx_end } );

        if ( result.is_error() ) {
            return result.error();
        } // if

        std::for_each( result.value().begin(), result.value().end(), [ &rx_begin ]( auto data ) {
            *rx_begin = data;

            ++rx_begin;
        } );

        return {};
    }

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), receive, () );
    MOCK_METHOD( (Result<std::vector<std::uint8_t>, Error_Code>), receive, (std::vector<std::uint8_t>));

    auto receive( std::uint8_t * begin, std::uint8_t * end ) -> Result<Void, Error_Code>
    {
        static_cast<void>( end );

        auto const result = receive( std::vector<std::uint8_t>{} );

        if ( result.is_error() ) {
            return result.error();
        } // if

        std::for_each( result.value().begin(), result.value().end(), [ &begin ]( auto data ) {
            *begin = data;

            ++begin;
        } );

        return {};
    }

    MOCK_METHOD( (Result<Void, Error_Code>), transmit, ( std::uint8_t ) );
    MOCK_METHOD( (Result<Void, Error_Code>), transmit, (std::vector<std::uint8_t>));

    auto transmit( std::uint8_t const * begin, std::uint8_t const * end ) -> Result<Void, Error_Code>
    {
        return transmit( std::vector<std::uint8_t>{ begin, end } );
    }
};

/**
 * \brief Mock SPI device selector.
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

        auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        auto initialize()
        {
            return mock().initialize();
        }

        auto select()
        {
            return mock().select();
        }

        auto deselect()
        {
            return mock().deselect();
        }
    };

    Mock_Device_Selector() = default;

    Mock_Device_Selector( Mock_Device_Selector && ) = delete;

    Mock_Device_Selector( Mock_Device_Selector const & ) = delete;

    ~Mock_Device_Selector() noexcept = default;

    auto operator=( Mock_Device_Selector && ) = delete;

    auto operator=( Mock_Device_Selector const & ) = delete;

    auto handle() noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );

    MOCK_METHOD( (Result<Void, Error_Code>), select, () );
    MOCK_METHOD( (Result<Void, Error_Code>), deselect, () );
};

/**
 * \brief Mock SPI device.
 */
class Mock_Device {
  public:
    using Controller = Mock_Controller;

    using Device_Selector = Mock_Device_Selector::Handle;

    Mock_Device() = default;

    Mock_Device( Controller &, Controller::Configuration, Device_Selector )
    {
    }

    Mock_Device( Mock_Device && ) = delete;

    Mock_Device( Mock_Device const & ) = delete;

    ~Mock_Device() noexcept = default;

    auto operator=( Mock_Device && ) = delete;

    auto operator=( Mock_Device const & ) = delete;

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );

    MOCK_METHOD( (Result<Void, Error_Code>), configure, (), ( const ) );

    MOCK_METHOD( Device_Selector &, device_selector, (), ( const ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), exchange, ( std::uint8_t ), ( const ) );
    MOCK_METHOD( (Result<std::vector<std::uint8_t>, Error_Code>), exchange, (std::vector<std::uint8_t>), ( const ) );

    auto exchange( std::uint8_t const * tx_begin, std::uint8_t const * tx_end, std::uint8_t * rx_begin, std::uint8_t * rx_end ) const
        -> Result<Void, Error_Code>
    {
        static_cast<void>( rx_end );

        auto const result = exchange( std::vector<std::uint8_t>{ tx_begin, tx_end } );

        if ( result.is_error() ) {
            return result.error();
        } // if

        std::for_each( result.value().begin(), result.value().end(), [ &rx_begin ]( auto data ) {
            *rx_begin = data;

            ++rx_begin;
        } );

        return {};
    }

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), receive, (), ( const ) );
    MOCK_METHOD( (Result<std::vector<std::uint8_t>, Error_Code>), receive, (std::vector<std::uint8_t>), ( const ) );

    auto receive( std::uint8_t * begin, std::uint8_t * end ) const -> Result<Void, Error_Code>
    {
        static_cast<void>( end );

        auto const result = receive( std::vector<std::uint8_t>{} );

        if ( result.is_error() ) {
            return result.error();
        } // if

        std::for_each( result.value().begin(), result.value().end(), [ &begin ]( auto data ) {
            *begin = data;

            ++begin;
        } );

        return {};
    }

    MOCK_METHOD( (Result<Void, Error_Code>), transmit, ( std::uint8_t ), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), transmit, (std::vector<std::uint8_t>), ( const ) );

    auto transmit( std::uint8_t const * begin, std::uint8_t const * end ) const
        -> Result<Void, Error_Code>
    {
        return transmit( std::vector<std::uint8_t>{ begin, end } );
    }
};

} // namespace picolibrary::Testing::Unit::SPI

#endif // PICOLIBRARY_TESTING_UNIT_SPI_H

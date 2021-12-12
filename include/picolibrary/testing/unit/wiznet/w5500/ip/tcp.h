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
 * \brief picolibrary::Testing::Unit::WIZnet::W5500::IP::TCP interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_WIZNET_W5500_IP_TCP_H
#define PICOLIBRARY_TESTING_UNIT_WIZNET_W5500_IP_TCP_H

#include <cstdint>
#include <vector>

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/mock_handle.h"
#include "picolibrary/void.h"
#include "picolibrary/wiznet/w5500.h"

/**
 * \brief WIZnet W5500 TCP over IP unit testing facilities.
 */
namespace picolibrary::Testing::Unit::WIZnet::W5500::IP::TCP {

/**
 * \brief Mock WIZnet W5500 IP TCP client socket.
 */
class Mock_Client {
  public:
    using Size = std::uint16_t;

    enum class State : std::uint_fast8_t {
        UNINITIALIZED,
        INITIALIZED,
        BOUND,
        CONNECTING,
        CONNECTED,
    };

    class Handle : public Mock_Handle<Mock_Client> {
      public:
        using Size = Mock_Client::Size;

        using State = Mock_Client::State;

        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Client & mock ) noexcept : Mock_Handle<Mock_Client>{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        auto state() const
        {
            return mock().state();
        }

        auto socket_id() const
        {
            return mock().socket_id();
        }

        auto socket_interrupt_mask() const
        {
            return mock().socket_interrupt_mask();
        }

        auto enable_interrupts( std::uint8_t mask )
        {
            return mock().enable_interrupts( mask );
        }

        auto disable_interrupts( std::uint8_t mask )
        {
            return mock().disable_interrupts( mask );
        }

        auto disable_interrupts()
        {
            return mock().disable_interrupts();
        }

        auto enabled_interrupts() const
        {
            return mock().enabled_interrupts();
        }

        auto interrupt_context() const
        {
            return mock().interrupt_context();
        }

        auto clear_interrupts()
        {
            return mock().clear_interrupts();
        }

        auto clear_interrupts( std::uint8_t mask )
        {
            return mock().clear_interrupts( mask );
        }

        auto configure_no_delayed_ack( ::picolibrary::WIZnet::W5500::No_Delayed_ACK no_delayed_ack_configuration )
        {
            return mock().configure_no_delayed_ack( no_delayed_ack_configuration );
        }

        auto no_delayed_ack_configuration() const
        {
            return mock().no_delayed_ack_configuration();
        }

        auto configure_maximum_segment_size( std::uint16_t maximum_segment_size )
        {
            return mock().configure_maximum_segment_size( maximum_segment_size );
        }

        auto maximum_segment_size() const
        {
            return mock().maximum_segment_size();
        }

        auto configure_time_to_live( std::uint8_t time_to_live )
        {
            return mock().configure_time_to_live( time_to_live );
        }

        auto time_to_live() const
        {
            return mock().time_to_live();
        }

        auto configure_keepalive_period( std::uint8_t keepalive_period )
        {
            return mock().configure_keepalive_period( keepalive_period );
        }

        auto keepalive_period() const
        {
            return mock().keepalive_period();
        }

        auto bind()
        {
            return mock().bind();
        }

        auto bind( ::picolibrary::IP::TCP::Endpoint const & endpoint )
        {
            return mock().bind( endpoint );
        }

        auto connect( ::picolibrary::IP::TCP::Endpoint const & endpoint )
        {
            return mock().connect( endpoint );
        }

        auto is_connected() const
        {
            return mock().is_connected();
        }

        auto remote_endpoint() const
        {
            return mock().remote_endpoint();
        }

        auto local_endpoint() const
        {
            return mock().local_endpoint();
        }

        auto outstanding() const
        {
            return mock().outstanding();
        }

        auto transmit( std::uint8_t const * begin, std::uint8_t const * end )
        {
            return mock().transmit( begin, end );
        }

        auto is_transmitting() const
        {
            return mock().is_transmitting();
        }

        auto transmit_keepalive()
        {
            return mock().transmit_keepalive();
        }

        auto available() const
        {
            return mock().available();
        }

        auto receive( std::uint8_t * begin, std::uint8_t * end )
        {
            return mock().receive( begin, end );
        }

        auto shutdown()
        {
            return mock().shutdown();
        }

        auto close()
        {
            return mock().close();
        }
    };

    Mock_Client() = default;

    Mock_Client( Mock_Client && ) = delete;

    Mock_Client( Mock_Client const & ) = delete;

    ~Mock_Client() noexcept = default;

    auto operator=( Mock_Client && ) = delete;

    auto operator=( Mock_Client const & ) = delete;

    auto handle()
    {
        return Handle{ *this };
    }

    MOCK_METHOD( State, state, (), ( const ) );

    MOCK_METHOD( ::picolibrary::WIZnet::W5500::Socket_ID, socket_id, (), ( const ) );
    MOCK_METHOD( std::uint8_t, socket_interrupt_mask, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), enable_interrupts, ( std::uint8_t ) );
    MOCK_METHOD( (Result<Void, Error_Code>), disable_interrupts, ( std::uint8_t ) );
    MOCK_METHOD( (Result<Void, Error_Code>), disable_interrupts, () );
    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), enabled_interrupts, (), ( const ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), interrupt_context, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), clear_interrupts, () );
    MOCK_METHOD( (Result<Void, Error_Code>), clear_interrupts, ( std::uint8_t ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_no_delayed_ack, ( ::picolibrary::WIZnet::W5500::No_Delayed_ACK ) );
    MOCK_METHOD(
        (Result<::picolibrary::WIZnet::W5500::No_Delayed_ACK, Error_Code>),
        no_delayed_ack_configuration,
        (),
        ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_maximum_segment_size, ( std::uint16_t ) );
    MOCK_METHOD( (Result<std::uint16_t, Error_Code>), maximum_segment_size, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_time_to_live, ( std::uint8_t ) );
    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), time_to_live, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_keepalive_period, ( std::uint8_t ) );
    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), keepalive_period, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), bind, () );
    MOCK_METHOD( (Result<Void, Error_Code>), bind, (::picolibrary::IP::TCP::Endpoint const &));

    MOCK_METHOD( (Result<Void, Error_Code>), connect, (::picolibrary::IP::TCP::Endpoint const &));

    MOCK_METHOD( (Result<bool, Error_Code>), is_connected, (), ( const ) );

    MOCK_METHOD( (Result<::picolibrary::IP::TCP::Endpoint, Error_Code>), remote_endpoint, (), ( const ) );
    MOCK_METHOD( (Result<::picolibrary::IP::TCP::Endpoint, Error_Code>), local_endpoint, (), ( const ) );

    MOCK_METHOD( (Result<Size, Error_Code>), outstanding, (), ( const ) );
    MOCK_METHOD( (Result<std::uint8_t const *, Error_Code>), transmit, (std::vector<std::uint8_t>));

    auto transmit( std::uint8_t const * begin, std::uint8_t const * end )
        -> Result<std::uint8_t const *, Error_Code>
    {
        return transmit( std::vector<std::uint8_t>{ begin, end } );
    }

    MOCK_METHOD( bool, is_transmitting, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), transmit_keepalive, () );

    MOCK_METHOD( (Result<Size, Error_Code>), available, (), ( const ) );
    MOCK_METHOD( (Result<std::vector<std::uint8_t>, Error_Code>), receive, () );

    auto receive( std::uint8_t * begin, std::uint8_t * end ) -> Result<std::uint8_t *, Error_Code>
    {
        static_cast<void>( end );

        auto const result = receive();
        if ( result.is_error() ) {
            return result.error();
        } // if

        std::for_each( result.value().begin(), result.value().end(), [ &begin ]( auto data ) {
            *begin = data;

            ++begin;
        } );

        return begin;
    }

    MOCK_METHOD( (Result<Void, Error_Code>), shutdown, () );

    MOCK_METHOD( (Result<Void, Void>), close, () );
};

} // namespace picolibrary::Testing::Unit::WIZnet::W5500::IP::TCP

#endif // PICOLIBRARY_TESTING_UNIT_WIZNET_W5500_IP_TCP_H

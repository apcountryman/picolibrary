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
 * \brief picolibrary::Testing::Automated::WIZnet::W5500::IP::UDP interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_IP_UDP_H
#define PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_IP_UDP_H

#include <cstdint>
#include <vector>

#include "gmock/gmock.h"
#include "picolibrary/ip/udp.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/automated/mock_handle.h"
#include "picolibrary/wiznet/w5500.h"

/**
 * \brief WIZnet W5500 UDP over IP automated testing facilities.
 */
namespace picolibrary::Testing::Automated::WIZnet::W5500::IP::UDP {

/**
 * \brief Mock socket.
 */
class Mock_Socket {
  public:
    enum class State : std::uint_fast8_t {
        UNINITIALIZED,
        INITIALIZED,
        BOUND,
    };

    class Handle : public Mock_Handle<Mock_Socket> {
      public:
        using State = Mock_Socket::State;

        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Socket & mock ) noexcept : Mock_Handle<Mock_Socket>{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        auto state() const -> State
        {
            return mock().state();
        }

        auto socket_id() const -> ::picolibrary::WIZnet::W5500::Socket_ID
        {
            return mock().socket_id();
        }

        auto socket_interrupt_mask() const -> std::uint8_t
        {
            return mock().socket_interrupt_mask();
        }

        void configure_broadcast_blocking( ::picolibrary::WIZnet::W5500::Broadcast_Blocking broadcast_blocking_configuration )
        {
            mock().configure_broadcast_blocking( broadcast_blocking_configuration );
        }

        auto broadcast_blocking_configuration() const -> ::picolibrary::WIZnet::W5500::Broadcast_Blocking
        {
            return mock().broadcast_blocking_configuration();
        }

        void configure_unicast_blocking( ::picolibrary::WIZnet::W5500::Unicast_Blocking unicast_blocking_configuration )
        {
            mock().configure_unicast_blocking( unicast_blocking_configuration );
        }

        auto unicast_blocking_configuration() const -> ::picolibrary::WIZnet::W5500::Unicast_Blocking
        {
            return mock().unicast_blocking_configuration();
        }

        void configure_time_to_live( std::uint8_t time_to_live )
        {
            mock().configure_time_to_live( time_to_live );
        }

        auto time_to_live() const -> std::uint8_t
        {
            return mock().time_to_live();
        }

        void enable_interrupts( std::uint8_t mask )
        {
            mock().enable_interrupts( mask );
        }

        void disable_interrupts( std::uint8_t mask )
        {
            mock().disable_interrupts( mask );
        }

        void disable_interrupts()
        {
            mock().disable_interrupts();
        }

        auto enabled_interrupts() const -> std::uint8_t
        {
            return mock().enabled_interrupts();
        }

        auto interrupt_context() const -> std::uint8_t
        {
            return mock().interrupt_context();
        }

        void clear_interrupts( std::uint8_t mask )
        {
            mock().clear_interrupts( mask );
        }

        void bind()
        {
            mock().bind();
        }

        void bind( ::picolibrary::IP::UDP::Endpoint const & endpoint )
        {
            mock().bind( endpoint );
        }

        auto local_endpoint() const -> ::picolibrary::IP::UDP::Endpoint
        {
            return mock().local_endpoint();
        }

        auto is_transmitting() const -> bool
        {
            return mock().is_transmitting();
        }

        auto transmit(
            ::picolibrary::IP::UDP::Endpoint const & endpoint,
            std::uint8_t const *                     begin,
            std::uint8_t const *                     end ) -> Result<void>
        {
            return mock().transmit( endpoint, begin, end );
        }

        auto receive( std::uint8_t * begin, std::uint8_t * end )
            -> Result<::picolibrary::IP::UDP::Reception_Result>
        {
            return mock().receive( begin, end );
        }

        void close()
        {
            mock().close();
        }
    };

    Mock_Socket() = default;

    Mock_Socket( Mock_Socket && ) = delete;

    ~Mock_Socket() noexcept = default;

    auto operator=( Mock_Socket && ) = delete;

    auto operator=( Mock_Socket const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return { *this };
    }

    MOCK_METHOD( State, state, (), ( const ) );

    MOCK_METHOD( ::picolibrary::WIZnet::W5500::Socket_ID, socket_id, (), ( const ) );

    MOCK_METHOD( std::uint8_t, socket_interrupt_mask, (), ( const ) );

    MOCK_METHOD( void, configure_broadcast_blocking, ( ::picolibrary::WIZnet::W5500::Broadcast_Blocking ) );
    MOCK_METHOD( ::picolibrary::WIZnet::W5500::Broadcast_Blocking, broadcast_blocking_configuration, (), ( const ) );

    MOCK_METHOD( void, configure_unicast_blocking, ( ::picolibrary::WIZnet::W5500::Unicast_Blocking ) );
    MOCK_METHOD( ::picolibrary::WIZnet::W5500::Unicast_Blocking, unicast_blocking_configuration, (), ( const ) );

    MOCK_METHOD( void, configure_time_to_live, ( std::uint8_t ) );
    MOCK_METHOD( std::uint8_t, time_to_live, (), ( const ) );

    MOCK_METHOD( void, enable_interrupts, ( std::uint8_t ) );
    MOCK_METHOD( void, disable_interrupts, ( std::uint8_t ) );
    MOCK_METHOD( void, disable_interrupts, () );
    MOCK_METHOD( std::uint8_t, enabled_interrupts, (), ( const ) );

    MOCK_METHOD( std::uint8_t, interrupt_context, (), ( const ) );
    MOCK_METHOD( void, clear_interrupts, ( std::uint8_t ) );

    MOCK_METHOD( void, bind, () );
    MOCK_METHOD( void, bind, (::picolibrary::IP::UDP::Endpoint const &));

    MOCK_METHOD( ::picolibrary::IP::UDP::Endpoint, local_endpoint, (), ( const ) );

    MOCK_METHOD( bool, is_transmitting, (), ( const ) );

    MOCK_METHOD( Result<void>, transmit, (::picolibrary::IP::UDP::Endpoint const &, std::vector<std::uint8_t>));

    auto transmit( ::picolibrary::IP::UDP::Endpoint const & endpoint, std::uint8_t const * begin, std::uint8_t const * end )
        -> Result<void>
    {
        return transmit( endpoint, std::vector<std::uint8_t>{ begin, end } );
    }

    MOCK_METHOD( (Result<std::pair<::picolibrary::IP::UDP::Endpoint, std::vector<std::uint8_t>>>), receive, () );

    auto receive( std::uint8_t * begin, std::uint8_t * end )
        -> Result<::picolibrary::IP::UDP::Reception_Result>
    {
        static_cast<void>( end );

        auto const result = receive();
        if ( result.is_error() ) {
            return result.error();
        } // if

        for ( auto const data : result.value().second ) {
            *begin = data;

            ++begin;
        } // for

        return ::picolibrary::IP::UDP::Reception_Result{ result.value().first, begin };
    }

    MOCK_METHOD( void, close, () );
};

} // namespace picolibrary::Testing::Automated::WIZnet::W5500::IP::UDP

#endif // PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_IP_UDP_H

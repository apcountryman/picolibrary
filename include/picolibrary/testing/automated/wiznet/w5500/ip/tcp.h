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
 * \brief picolibrary::Testing::Automated::WIZnet::W5500::IP::TCP interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_IP_TCP_H
#define PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_IP_TCP_H

#include <cstdint>
#include <vector>

#include "gmock/gmock.h"
#include "picolibrary/fixed_capacity_vector.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/automated/mock_handle.h"
#include "picolibrary/wiznet/w5500.h"

/**
 * \brief WIZnet W5500 TCP over IP automated testing facilities.
 */
namespace picolibrary::Testing::Automated::WIZnet::W5500::IP::TCP {

/**
 * \brief Mock client socket.
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

        void configure_no_delayed_ack_usage( ::picolibrary::WIZnet::W5500::No_Delayed_ACK_Usage no_delayed_ack_usage_configuration )
        {
            mock().configure_no_delayed_ack_usage( no_delayed_ack_usage_configuration );
        }

        auto no_delayed_ack_usage_configuration() const -> ::picolibrary::WIZnet::W5500::No_Delayed_ACK_Usage
        {
            return mock().no_delayed_ack_usage_configuration();
        }

        void configure_maximum_segment_size( std::uint16_t maximum_segment_size )
        {
            mock().configure_maximum_segment_size( maximum_segment_size );
        }

        auto maximum_segment_size() const -> std::uint16_t
        {
            return mock().maximum_segment_size();
        }

        void configure_time_to_live( std::uint8_t time_to_live )
        {
            mock().configure_time_to_live( time_to_live );
        }

        auto time_to_live() const -> std::uint8_t
        {
            return mock().time_to_live();
        }

        void configure_keepalive_period( std::uint8_t keepalive_period )
        {
            mock().configure_keepalive_period( keepalive_period );
        }

        auto keepalive_period() const -> std::uint8_t
        {
            return mock().keepalive_period();
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

        void bind( ::picolibrary::IP::TCP::Endpoint const & endpoint )
        {
            mock().bind( endpoint );
        }

        auto connect( ::picolibrary::IP::TCP::Endpoint const & endpoint ) -> Result<void>
        {
            return mock().connect( endpoint );
        }

        auto is_connected() const -> bool
        {
            return mock().is_connected();
        }

        auto remote_endpoint() const -> ::picolibrary::IP::TCP::Endpoint
        {
            return mock().remote_endpoint();
        }

        auto local_endpoint() const -> ::picolibrary::IP::TCP::Endpoint
        {
            return mock().local_endpoint();
        }

        auto outstanding() const -> Size
        {
            return mock().outstanding();
        }

        auto is_transmitting() const -> bool
        {
            return mock().is_transmitting();
        }

        auto transmit( std::uint8_t const * begin, std::uint8_t const * end )
            -> Result<std::uint8_t const *>
        {
            return mock().transmit( begin, end );
        }

        auto transmit_keepalive() -> Result<void>
        {
            return mock().transmit_keepalive();
        }

        auto available() const -> Size
        {
            return mock().available();
        }

        auto receive( std::uint8_t * begin, std::uint8_t * end ) -> Result<std::uint8_t *>
        {
            return mock().receive( begin, end );
        }

        void shutdown()
        {
            mock().shutdown();
        }

        void close()
        {
            mock().close();
        }
    };

    Mock_Client() = default;

    Mock_Client( Mock_Client && ) = delete;

    Mock_Client( Mock_Client const & ) = delete;

    ~Mock_Client() noexcept = default;

    auto operator=( Mock_Client && ) = delete;

    auto operator=( Mock_Client const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return { *this };
    }

    MOCK_METHOD( State, state, (), ( const ) );

    MOCK_METHOD( ::picolibrary::WIZnet::W5500::Socket_ID, socket_id, (), ( const ) );

    MOCK_METHOD( std::uint8_t, socket_interrupt_mask, (), ( const ) );

    MOCK_METHOD( void, configure_no_delayed_ack_usage, ( ::picolibrary::WIZnet::W5500::No_Delayed_ACK_Usage ) );
    MOCK_METHOD( ::picolibrary::WIZnet::W5500::No_Delayed_ACK_Usage, no_delayed_ack_usage_configuration, (), ( const ) );

    MOCK_METHOD( void, configure_maximum_segment_size, ( std::uint16_t ) );
    MOCK_METHOD( std::uint16_t, maximum_segment_size, (), ( const ) );

    MOCK_METHOD( void, configure_time_to_live, ( std::uint8_t ) );
    MOCK_METHOD( std::uint8_t, time_to_live, (), ( const ) );

    MOCK_METHOD( void, configure_keepalive_period, ( std::uint8_t ) );
    MOCK_METHOD( std::uint8_t, keepalive_period, (), ( const ) );

    MOCK_METHOD( void, enable_interrupts, ( std::uint8_t ) );
    MOCK_METHOD( void, disable_interrupts, ( std::uint8_t ) );
    MOCK_METHOD( void, disable_interrupts, () );
    MOCK_METHOD( std::uint8_t, enabled_interrupts, (), ( const ) );

    MOCK_METHOD( std::uint8_t, interrupt_context, (), ( const ) );
    MOCK_METHOD( void, clear_interrupts, ( std::uint8_t ) );

    MOCK_METHOD( void, bind, () );
    MOCK_METHOD( void, bind, (::picolibrary::IP::TCP::Endpoint const &));

    MOCK_METHOD( (Result<void>), connect, (::picolibrary::IP::TCP::Endpoint const &));

    MOCK_METHOD( bool, is_connected, (), ( const ) );

    MOCK_METHOD( ::picolibrary::IP::TCP::Endpoint, remote_endpoint, (), ( const ) );
    MOCK_METHOD( ::picolibrary::IP::TCP::Endpoint, local_endpoint, (), ( const ) );

    MOCK_METHOD( Size, outstanding, (), ( const ) );

    MOCK_METHOD( bool, is_transmitting, (), ( const ) );

    MOCK_METHOD( (Result<std::uint8_t const *>), transmit, (std::vector<std::uint8_t>));

    auto transmit( std::uint8_t const * begin, std::uint8_t const * end )
        -> Result<std::uint8_t const *>
    {
        return transmit( std::vector<std::uint8_t>{ begin, end } );
    }

    MOCK_METHOD( (Result<void>), transmit_keepalive, () );

    MOCK_METHOD( Size, available, (), ( const ) );

    MOCK_METHOD( (Result<std::vector<std::uint8_t>>), receive, () );

    auto receive( std::uint8_t * begin, std::uint8_t * end ) -> Result<std::uint8_t *>
    {
        static_cast<void>( end );

        auto const result = receive();
        if ( result.is_error() ) {
            return result.error();
        } // if

        for ( auto const data : result.value() ) {
            *begin = data;

            ++begin;
        } // for

        return begin;
    }

    MOCK_METHOD( void, shutdown, () );

    MOCK_METHOD( void, close, () );
};

/**
 * \brief Mock server connection handler socket.
 */
class Mock_Server_Connection_Handler {
  public:
    using Size = std::uint16_t;

    enum class State : std::uint_fast8_t {
        UNINITIALIZED,
        CONNECTED,
    };

    class Handle : public Mock_Handle<Mock_Server_Connection_Handler> {
      public:
        using Size = Mock_Server_Connection_Handler::Size;

        using State = Mock_Server_Connection_Handler::State;

        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Server_Connection_Handler & mock ) noexcept :
            Mock_Handle<Mock_Server_Connection_Handler>{ mock }
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

        auto no_delayed_ack_usage_configuration() const -> ::picolibrary::WIZnet::W5500::No_Delayed_ACK_Usage
        {
            return mock().no_delayed_ack_usage_configuration();
        }

        auto maximum_segment_size() const -> std::uint16_t
        {
            return mock().maximum_segment_size();
        }

        auto time_to_live() const -> std::uint8_t
        {
            return mock().time_to_live();
        }

        auto keepalive_period() const -> std::uint8_t
        {
            return mock().keepalive_period();
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

        auto is_connected() const -> bool
        {
            return mock().is_connected();
        }

        auto remote_endpoint() const -> ::picolibrary::IP::TCP::Endpoint
        {
            return mock().remote_endpoint();
        }

        auto local_endpoint() const -> ::picolibrary::IP::TCP::Endpoint
        {
            return mock().local_endpoint();
        }

        auto outstanding() const -> Size
        {
            return mock().outstanding();
        }

        auto is_transmitting() const -> bool
        {
            return mock().is_transmitting();
        }

        auto transmit( std::uint8_t const * begin, std::uint8_t const * end )
            -> Result<std::uint8_t const *>
        {
            return mock().transmit( begin, end );
        }

        auto transmit_keepalive() -> Result<void>
        {
            return mock().transmit_keepalive();
        }

        auto available() const -> Size
        {
            return mock().available();
        }

        auto receive( std::uint8_t * begin, std::uint8_t * end ) -> Result<std::uint8_t *>
        {
            return mock().receive( begin, end );
        }

        void shutdown()
        {
            mock().shutdown();
        }

        void close()
        {
            mock().close();
        }
    };

    Mock_Server_Connection_Handler() = default;

    Mock_Server_Connection_Handler( Mock_Server_Connection_Handler && ) = delete;

    Mock_Server_Connection_Handler( Mock_Server_Connection_Handler const & ) = delete;

    ~Mock_Server_Connection_Handler() noexcept = default;

    auto operator=( Mock_Server_Connection_Handler && ) = delete;

    auto operator=( Mock_Server_Connection_Handler const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return { *this };
    }

    MOCK_METHOD( State, state, (), ( const ) );

    MOCK_METHOD( ::picolibrary::WIZnet::W5500::Socket_ID, socket_id, (), ( const ) );

    MOCK_METHOD( std::uint8_t, socket_interrupt_mask, (), ( const ) );

    MOCK_METHOD( ::picolibrary::WIZnet::W5500::No_Delayed_ACK_Usage, no_delayed_ack_usage_configuration, (), ( const ) );

    MOCK_METHOD( std::uint16_t, maximum_segment_size, (), ( const ) );

    MOCK_METHOD( std::uint8_t, time_to_live, (), ( const ) );

    MOCK_METHOD( std::uint8_t, keepalive_period, (), ( const ) );

    MOCK_METHOD( std::uint8_t, enabled_interrupts, (), ( const ) );

    MOCK_METHOD( std::uint8_t, interrupt_context, (), ( const ) );
    MOCK_METHOD( void, clear_interrupts, ( std::uint8_t ) );

    MOCK_METHOD( bool, is_connected, (), ( const ) );

    MOCK_METHOD( ::picolibrary::IP::TCP::Endpoint, remote_endpoint, (), ( const ) );
    MOCK_METHOD( ::picolibrary::IP::TCP::Endpoint, local_endpoint, (), ( const ) );

    MOCK_METHOD( Size, outstanding, (), ( const ) );

    MOCK_METHOD( bool, is_transmitting, (), ( const ) );

    MOCK_METHOD( (Result<std::uint8_t const *>), transmit, (std::vector<std::uint8_t>));

    auto transmit( std::uint8_t const * begin, std::uint8_t const * end )
        -> Result<std::uint8_t const *>
    {
        return transmit( std::vector<std::uint8_t>{ begin, end } );
    }

    MOCK_METHOD( (Result<void>), transmit_keepalive, () );

    MOCK_METHOD( Size, available, (), ( const ) );

    MOCK_METHOD( (Result<std::vector<std::uint8_t>>), receive, () );

    auto receive( std::uint8_t * begin, std::uint8_t * end ) -> Result<std::uint8_t *>
    {
        static_cast<void>( end );

        auto const result = receive();
        if ( result.is_error() ) {
            return result.error();
        } // if

        for ( auto const data : result.value() ) {
            *begin = data;

            ++begin;
        } // for

        return begin;
    }

    MOCK_METHOD( void, shutdown, () );

    MOCK_METHOD( void, close, () );
};

/**
 * \brief Mock client socket.
 */
class Mock_Server {
  public:
    using Connection_Handler = Mock_Server_Connection_Handler::Handle;

    using Socket_IDs = Fixed_Capacity_Vector<::picolibrary::WIZnet::W5500::Socket_ID, 8>;

    enum class State : std::uint_fast8_t {
        UNINITIALIZED,
        INITIALIZED,
        BOUND,
        LISTENING,
    };

    class Handle : public Mock_Handle<Mock_Server> {
      public:
        using Connection_Handler = Mock_Server::Connection_Handler;

        using Socket_IDs = Mock_Server::Socket_IDs;

        using State = Mock_Server::State;

        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Server & mock ) noexcept : Mock_Handle<Mock_Server>{ mock }
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

        auto socket_ids() const -> Socket_IDs
        {
            return mock().socket_ids();
        }

        auto socket_interrupt_mask() const -> std::uint8_t
        {
            return mock().socket_interrupt_mask();
        }

        void configure_no_delayed_ack_usage( ::picolibrary::WIZnet::W5500::No_Delayed_ACK_Usage no_delayed_ack_usage_configuration )
        {
            mock().configure_no_delayed_ack_usage( no_delayed_ack_usage_configuration );
        }

        auto no_delayed_ack_usage_configuration() const -> ::picolibrary::WIZnet::W5500::No_Delayed_ACK_Usage
        {
            return mock().no_delayed_ack_usage_configuration();
        }

        void configure_maximum_segment_size( std::uint16_t maximum_segment_size )
        {
            mock().configure_maximum_segment_size( maximum_segment_size );
        }

        auto maximum_segment_size() const -> std::uint16_t
        {
            return mock().maximum_segment_size();
        }

        void configure_time_to_live( std::uint8_t time_to_live )
        {
            mock().configure_time_to_live( time_to_live );
        }

        auto time_to_live() const -> std::uint8_t
        {
            return mock().time_to_live();
        }

        void configure_keepalive_period( std::uint8_t keepalive_period )
        {
            mock().configure_keepalive_period( keepalive_period );
        }

        auto keepalive_period() const -> std::uint8_t
        {
            return mock().keepalive_period();
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

        void bind( ::picolibrary::IP::TCP::Endpoint const & endpoint )
        {
            mock().bind( endpoint );
        }

        void listen( std::uint_fast8_t backlog )
        {
            mock().listen( backlog );
        }

        auto is_listening() const -> bool
        {
            return mock().is_listening();
        }

        auto local_endpoint() const -> ::picolibrary::IP::TCP::Endpoint
        {
            return mock().local_endpoint();
        }

        auto accept() -> Result<Connection_Handler>
        {
            return mock().accept();
        }

        void deallocate_socket( std::uint_fast8_t, ::picolibrary::WIZnet::W5500::Socket_ID socket_id )
        {
            mock().deallocate_socket( {}, socket_id );
        }

        void close()
        {
            mock().close();
        }
    };

    Mock_Server() = default;

    Mock_Server( Mock_Server && ) = delete;

    Mock_Server( Mock_Server const & ) = delete;

    ~Mock_Server() noexcept = default;

    auto operator=( Mock_Server && ) = delete;

    auto operator=( Mock_Server const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return { *this };
    }

    MOCK_METHOD( State, state, (), ( const ) );

    MOCK_METHOD( Socket_IDs, socket_ids, (), ( const ) );

    MOCK_METHOD( std::uint8_t, socket_interrupt_mask, (), ( const ) );

    MOCK_METHOD( void, configure_no_delayed_ack_usage, ( ::picolibrary::WIZnet::W5500::No_Delayed_ACK_Usage ) );
    MOCK_METHOD( ::picolibrary::WIZnet::W5500::No_Delayed_ACK_Usage, no_delayed_ack_usage_configuration, (), ( const ) );

    MOCK_METHOD( void, configure_maximum_segment_size, ( std::uint16_t ) );
    MOCK_METHOD( std::uint16_t, maximum_segment_size, (), ( const ) );

    MOCK_METHOD( void, configure_time_to_live, ( std::uint8_t ) );
    MOCK_METHOD( std::uint8_t, time_to_live, (), ( const ) );

    MOCK_METHOD( void, configure_keepalive_period, ( std::uint8_t ) );
    MOCK_METHOD( std::uint8_t, keepalive_period, (), ( const ) );

    MOCK_METHOD( void, enable_interrupts, ( std::uint8_t ) );
    MOCK_METHOD( void, disable_interrupts, ( std::uint8_t ) );
    MOCK_METHOD( void, disable_interrupts, () );
    MOCK_METHOD( std::uint8_t, enabled_interrupts, (), ( const ) );

    MOCK_METHOD( std::uint8_t, interrupt_context, (), ( const ) );
    MOCK_METHOD( void, clear_interrupts, ( std::uint8_t ) );

    MOCK_METHOD( void, bind, () );
    MOCK_METHOD( void, bind, (::picolibrary::IP::TCP::Endpoint const &));

    MOCK_METHOD( void, listen, ( std::uint_fast8_t ) );
    MOCK_METHOD( bool, is_listening, (), ( const ) );

    MOCK_METHOD( ::picolibrary::IP::TCP::Endpoint, local_endpoint, (), ( const ) );

    MOCK_METHOD( (Result<Connection_Handler>), accept, () );

    MOCK_METHOD( void, deallocate_socket, ( std::uint_fast8_t, ::picolibrary::WIZnet::W5500::Socket_ID ) );

    MOCK_METHOD( void, close, () );
};

} // namespace picolibrary::Testing::Automated::WIZnet::W5500::IP::TCP

#endif // PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_IP_TCP_H

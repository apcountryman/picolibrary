/**
 * picolibrary
 *
 * Copyright 2020-2022, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP interface.
 */

#ifndef PICOLIBRARY_WIZNET_W5500_IP_TCP_H
#define PICOLIBRARY_WIZNET_W5500_IP_TCP_H

#include <cstdint>

#include "picolibrary/error.h"
#include "picolibrary/ip.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/precondition.h"
#include "picolibrary/utility.h"
#include "picolibrary/wiznet/w5500.h"

/**
 * \brief WIZnet W5500 Transmission Control Protocol (TCP) over IP facilities.
 */
namespace picolibrary::WIZnet::W5500::IP::TCP {

/**
 * \brief Client socket.
 *
 * \tparam Driver The type of driver used to interact with the W5500.
 * \tparam Network_Stack The type of network stack the socket is associated with.
 */
template<typename Driver, typename Network_Stack>
class Client {
  public:
    /**
     * \brief The unsigned integer type used to  report transmit/received buffer
     *        information.
     */
    using Size = std::uint16_t;

    /**
     * \brief Socket state.
     */
    enum class State : std::uint_fast8_t {
        UNINITIALIZED, ///< Uninitialized.
        INITIALIZED,   ///< Initialized.
        BOUND,         ///< Bound.
    };

    /**
     * \brief Constructor.
     */
    constexpr Client() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] driver The driver used to interact with the W5500.
     * \param[in] socket_id The socket's socket ID.
     * \param[in] network_stack The network stack the socket is associated with.
     */
    constexpr Client( Driver & driver, Socket_ID socket_id, Network_Stack & network_stack ) noexcept :
        m_state{ State::INITIALIZED },
        m_driver{ &driver },
        m_socket_id{ socket_id },
        m_network_stack{ &network_stack }
    {
    }

#ifdef PICOLIBRARY_ENABLE_AUTOMATED_TESTING
    /**
     * \brief Constructor.
     *
     * \param[in] state The socket's initial state.
     * \param[in] driver The driver used to interact with the W5500.
     * \param[in] socket_id The socket's socket ID.
     * \param[in] network_stack The network stack the socket is associated with.
     */
    constexpr Client( State state, Driver & driver, Socket_ID socket_id, Network_Stack & network_stack ) noexcept
        :
        m_state{ state },
        m_driver{ &driver },
        m_socket_id{ socket_id },
        m_network_stack{ &network_stack }
    {
    }
#endif // PICOLIBRARY_ENABLE_AUTOMATED_TESTING

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Client( Client && source ) noexcept :
        m_state{ source.m_state },
        m_driver{ source.m_driver },
        m_socket_id{ source.m_socket_id },
        m_network_stack{ source.m_network_stack }
    {
        source.m_state         = State::UNINITIALIZED;
        source.m_driver        = nullptr;
        source.m_network_stack = nullptr;
    }

    Client( Client const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Client() noexcept
    {
        close();
    }

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Client && expression ) noexcept -> Client &
    {
        if ( &expression != this ) {
            close();

            m_state         = expression.m_state;
            m_driver        = expression.m_driver;
            m_socket_id     = expression.m_socket_id;
            m_network_stack = expression.m_network_stack;

            expression.m_state         = State::UNINITIALIZED;
            expression.m_driver        = nullptr;
            expression.m_network_stack = nullptr;
        } // if

        return *this;
    }

    auto operator=( Client const & ) = delete;

    /**
     * \brief Get the socket's state.
     *
     * \return The socket's state.
     */
    constexpr auto state() const noexcept -> State
    {
        return m_state;
    }

    /**
     * \brief Get the socket's socket ID.
     *
     * \return The socket's socket ID.
     */
    constexpr auto socket_id() const noexcept -> Socket_ID
    {
        return m_socket_id;
    }

    /**
     * \brief Get the socket's socket interrupt mask (mask used when checking the network
     *        stack's socket interrupt context).
     *
     * \return The socket's interrupt mask.
     */
    constexpr auto socket_interrupt_mask() const noexcept -> std::uint8_t
    {
        return 1 << ( to_underlying( m_socket_id ) >> Control_Byte::Bit::SOCKET );
    }

    /**
     * \brief Bind the socket to a local endpoint.
     *
     * \pre the socket is in a state that allows it to be bound to a local endpoint
     * \pre the socket is not already bound to a local endpoint
     * \pre the W5500 is responsive
     *
     * \param[in] endpoint The local endpoint to bind the socket to.
     *
     * \pre endpoint is a valid local endpoint
     * \pre endpoint is not already in use
     * \pre if an ephemeral port is requested, an ephemeral port is available
     */
    void bind( ::picolibrary::IP::TCP::Endpoint const & endpoint = ::picolibrary::IP::TCP::Endpoint{} ) noexcept
    {
        expect( m_state == State::INITIALIZED, Generic_Error::LOGIC_ERROR );

        expect(
            endpoint.address().version() == ::picolibrary::IP::Version::UNSPECIFIED
                or endpoint.address().version() == ::picolibrary::IP::Version::_4,
            Generic_Error::INVALID_ARGUMENT );

        if ( not endpoint.address().is_any() ) {
            expect(
                endpoint.address().ipv4().as_byte_array() == m_driver->read_sipr(),
                Generic_Error::INVALID_ARGUMENT );
        } // if

        m_driver->write_sn_port(
            m_socket_id,
            m_network_stack->tcp_port_allocator().allocate( *m_driver, endpoint.port() ).as_unsigned_integer() );

        m_driver->write_sn_cr( m_socket_id, SN_CR::COMMAND_OPEN );
        while ( m_driver->read_sn_cr( m_socket_id ) ) {} // while

        for ( ;; ) {
            switch ( m_driver->read_sn_sr( m_socket_id ) ) {
                case SN_SR::STATUS_SOCK_CLOSED: break;
                case SN_SR::STATUS_SOCK_INIT: m_state = State::BOUND; return;
                default: expect( false, m_network_stack->nonresponsive_device_error() );
            } // switch
        }     // for
    }

    /**
     * \brief Close the socket.
     */
    void close() noexcept
    {
        if ( m_state == Client::State::UNINITIALIZED ) {
            return;
        } // if

        if ( m_state != Client::State::INITIALIZED ) {
            m_driver->write_sn_cr( m_socket_id, SN_CR::COMMAND_CLOSE );
            while ( m_driver->read_sn_cr( m_socket_id ) ) {} // while

            auto closed = false;
            do {
                switch ( m_driver->read_sn_sr( m_socket_id ) ) {
                    case SN_SR::STATUS_SOCK_CLOSED: closed = true; break;
                    case SN_SR::STATUS_SOCK_INIT: break;
                    case SN_SR::STATUS_SOCK_ESTABLISHED: break;
                    case SN_SR::STATUS_SOCK_CLOSE_WAIT: break;
                    case SN_SR::STATUS_SOCK_SYNSENT: break;
                    case SN_SR::STATUS_SOCK_FIN_WAIT: break;
                    case SN_SR::STATUS_SOCK_CLOSING: break;
                    case SN_SR::STATUS_SOCK_TIME_WAIT: break;
                    case SN_SR::STATUS_SOCK_LAST_ACK: break;
                    default:
                        expect( false, m_network_stack->nonresponsive_device_error() );
                } // switch
            } while ( not closed );

            m_driver->write_sn_ir( m_socket_id, Socket_Interrupt::ALL );

            auto const port = m_driver->read_sn_port( m_socket_id );
            m_driver->write_sn_port( m_socket_id, SN_PORT::RESET );
            m_network_stack->tcp_port_allocator().deallocate( port );

            m_driver->write_sn_dhar( m_socket_id, SN_DHAR::RESET );
            m_driver->write_sn_dipr( m_socket_id, SN_DIPR::RESET );
            m_driver->write_sn_dport( m_socket_id, SN_DPORT::RESET );
        } // if

        m_driver->write_sn_mr( m_socket_id, SN_MR::RESET );
        m_driver->write_sn_mssr( m_socket_id, SN_MSSR::RESET );
        m_driver->write_sn_ttl( m_socket_id, SN_TTL::RESET );
        m_driver->write_sn_imr( m_socket_id, SN_IMR::RESET );
        m_driver->write_sn_kpalvtr( m_socket_id, SN_KPALVTR::RESET );

        m_network_stack->deallocate_socket( m_socket_id );

        m_state = Client::State::UNINITIALIZED;
    }

  private:
    /**
     * \brief The socket's state.
     */
    State m_state{};

    /**
     * \brief The driver used to interact with the W5500.
     */
    Driver * m_driver{};

    /**
     * \brief The socket's socket ID.
     */
    Socket_ID m_socket_id{};

    /**
     * \brief The network stack the socket is associated with.
     */
    Network_Stack * m_network_stack{};
};

} // namespace picolibrary::WIZnet::W5500::IP::TCP

#endif // PICOLIBRARY_WIZNET_W5500_IP_TCP_H

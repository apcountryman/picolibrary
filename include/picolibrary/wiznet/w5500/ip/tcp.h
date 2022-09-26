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
#include "picolibrary/result.h"
#include "picolibrary/utility.h"
#include "picolibrary/void.h"
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
        CONNECTING,    ///< Connecting.
        CONNECTED,     ///< Connected.
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
     * \param[in] is_transmitting The socket's initial data transmission in progress
     *            status.
     */
    constexpr Client( State state, Driver & driver, Socket_ID socket_id, Network_Stack & network_stack, bool is_transmitting = false ) noexcept
        :
        m_state{ state },
        m_driver{ &driver },
        m_socket_id{ socket_id },
        m_network_stack{ &network_stack },
        m_is_transmitting{ is_transmitting }
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
        m_network_stack{ source.m_network_stack },
        m_is_transmitting{ source.m_is_transmitting }
    {
        source.m_state         = State::UNINITIALIZED;
        source.m_driver        = nullptr;
        source.m_network_stack = nullptr;
    }

    Client( Client const & ) = delete;

    /**
     * \brief Destructor.
     *
     * \pre the W5500 is responsive
     */
    ~Client() noexcept
    {
        close();
    }

    /**
     * \brief Assignment operator.
     *
     * \pre the W5500 is responsive
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Client && expression ) noexcept -> Client &
    {
        if ( &expression != this ) {
            close();

            m_state           = expression.m_state;
            m_driver          = expression.m_driver;
            m_socket_id       = expression.m_socket_id;
            m_network_stack   = expression.m_network_stack;
            m_is_transmitting = expression.m_is_transmitting;

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
     * \brief Configure the socket's no delayed ACK usage (defaults to disabled).
     *
     * \attention A socket's No delayed ACK usage must be configured before the socket is
     *            bound to a local endpoint.
     *
     * \param[in] no_delayed_ack_usage_configuration The desired no delayed ACK usage
     *            configuration.
     */
    void configure_no_delayed_ack_usage( No_Delayed_ACK_Usage no_delayed_ack_usage_configuration ) noexcept
    {
        m_driver->write_sn_mr( m_socket_id, to_underlying( no_delayed_ack_usage_configuration ) );
    }

    /**
     * \brief Get the socket's no delayed ACK usage configuration.
     *
     * \return The socket's no delayed ACK usage configuration.
     */
    auto no_delayed_ack_usage_configuration() const noexcept -> No_Delayed_ACK_Usage
    {
        return static_cast<No_Delayed_ACK_Usage>( m_driver->read_sn_mr( m_socket_id ) & SN_MR::Mask::ND );
    }

    /**
     * \brief Configure the socket's maximum segment size (defaults to 0x0000).
     *
     * \attention A socket's maximum segment size must be configured before the socket
     *            connects to a remote endpoint.
     *
     * \param[in] maximum_segment_size The desired maximum segment size.
     */
    void configure_maximum_segment_size( std::uint16_t maximum_segment_size ) noexcept
    {
        m_driver->write_sn_mssr( m_socket_id, maximum_segment_size );
    }

    /**
     * \brief Get the socket's maximum segment size.
     *
     * \return The socket's maximum segment size.
     */
    auto maximum_segment_size() const noexcept -> std::uint16_t
    {
        return m_driver->read_sn_mssr( m_socket_id );
    }

    /**
     * \brief Configure the socket's IPv4 packet time to live field value (defaults to
     *        0x80).
     *
     * \param[in] time_to_live The desired IPv4 packet time to live field value.
     */
    void configure_time_to_live( std::uint8_t time_to_live ) noexcept
    {
        m_driver->write_sn_ttl( m_socket_id, time_to_live );
    }

    /**
     * \brief Get the socket's IPv4 packet time to live field value.
     *
     * \return The socket's IPv4 packet time to live field value.
     */
    auto time_to_live() const noexcept -> std::uint8_t
    {
        return m_driver->read_sn_ttl( m_socket_id );
    }

    /**
     * \brief Configure the socket's keepalive packet transmission period (SN_KPALVTR
     *        register value, defaults to 0x00).
     *
     * \param[in] keepalive_period The desired keepalive packet transmission period
     *            (SN_KPALVTR register value).
     */
    void configure_keepalive_period( std::uint8_t keepalive_period ) noexcept
    {
        m_driver->write_sn_kpalvtr( m_socket_id, keepalive_period );
    }

    /**
     * \brief Get the socket's keepalive packet transmission period (SN_KPALVTR register
     *        value.
     *
     * \return The socket's keepalive packet transmission period.
     */
    auto keepalive_period() const noexcept -> std::uint8_t
    {
        return m_driver->read_sn_kpalvtr( m_socket_id );
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
    // NOLINTNEXTLINE(readability-function-size)
    void bind( ::picolibrary::IP::TCP::Endpoint const & endpoint = ::picolibrary::IP::TCP::Endpoint{} ) noexcept
    {
        // #lizard forgives the length

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

        m_driver->write_sn_mr(
            m_socket_id, ( m_driver->read_sn_mr( m_socket_id ) & ~SN_MR::Mask::P ) | SN_MR::P_TCP );

        m_driver->write_sn_cr( m_socket_id, SN_CR::COMMAND_OPEN );
        while ( m_driver->read_sn_cr( m_socket_id ) ) {} // while

        for ( ;; ) {
            switch ( m_driver->read_sn_sr( m_socket_id ) ) {
                case SN_SR::STATUS_SOCK_CLOSED: break;
                case SN_SR::STATUS_SOCK_INIT: m_state = State::BOUND; return;
                default: expect( m_network_stack->nonresponsive_device_error() );
            } // switch
        }     // for
    }

    /**
     * \brief Connect to a remote endpoint.
     *
     * \pre the socket is in a state that allows it to connect to a remote endpoint
     * \pre the socket is not already connected to a remote endpoint
     * \pre the W5500 is responsive
     *
     * \param[in] endpoint The remote endpoint to connect to.
     *
     * \pre endpoint is a valid remote endpoint
     *
     * \return Nothing if connection to the remote endpoint succeeded.
     * \return picolibrary::Generic_Error::WOULD_BLOCK if connecting to the remote
     *         endpoint cannot succeed immediately.
     * \return picolibrary::Generic_Error::OPERATION_TIMEOUT if connecting to the remote
     *         endpoint timed out.
     */
    // NOLINTNEXTLINE(readability-function-size)
    auto connect( ::picolibrary::IP::TCP::Endpoint const & endpoint ) noexcept -> Result<Void, Error_Code>
    {
        // #lizard forgives the length

        if ( m_state == State::BOUND ) {
            expect(
                endpoint.address().is_ipv4() and not endpoint.address().is_any()
                    and not endpoint.port().is_any(),
                Generic_Error::INVALID_ARGUMENT );

            m_driver->write_sn_dipr( m_socket_id, endpoint.address().ipv4().as_byte_array() );
            m_driver->write_sn_dport( m_socket_id, endpoint.port().as_unsigned_integer() );

            m_driver->write_sn_cr( m_socket_id, SN_CR::COMMAND_CONNECT );
            while ( m_driver->read_sn_cr( m_socket_id ) ) {} // while

            m_state = State::CONNECTING;

            return Generic_Error::WOULD_BLOCK;
        } // if

        if ( m_state == State::CONNECTING ) {
            switch ( m_driver->read_sn_sr( m_socket_id ) ) {
                case SN_SR::STATUS_SOCK_CLOSED: return Generic_Error::OPERATION_TIMEOUT;
                case SN_SR::STATUS_SOCK_ESTABLISHED: [[fallthrough]];
                case SN_SR::STATUS_SOCK_CLOSE_WAIT: m_state = State::CONNECTED; return {};
                case SN_SR::STATUS_SOCK_SYNSENT: return Generic_Error::WOULD_BLOCK;
                default: expect( m_network_stack->nonresponsive_device_error() );
            } // switch
        }     // if

        expect( Generic_Error::LOGIC_ERROR );
    }

    /**
     * \brief Check if the socket is connected to a remote endpoint.
     *
     * \pre the W5500 is responsive
     *
     * \return true if the socket is connected to a remote endpoint.
     * \return false if the socket is not connected to a remote endpoint.
     */
    auto is_connected() const noexcept -> bool
    {
        switch ( m_driver->read_sn_sr( m_socket_id ) ) {
            case SN_SR::STATUS_SOCK_CLOSED: // NOLINT(bugprone-branch-clone)
                return false;
            case SN_SR::STATUS_SOCK_INIT: // NOLINT(bugprone-branch-clone)
                return false;
            case SN_SR::STATUS_SOCK_ESTABLISHED: // NOLINT(bugprone-branch-clone)
                return true;
            case SN_SR::STATUS_SOCK_CLOSE_WAIT: // NOLINT(bugprone-branch-clone)
                return false;
            case SN_SR::STATUS_SOCK_SYNSENT: // NOLINT(bugprone-branch-clone)
                return false;
            case SN_SR::STATUS_SOCK_FIN_WAIT: // NOLINT(bugprone-branch-clone)
                return false;
            case SN_SR::STATUS_SOCK_CLOSING: // NOLINT(bugprone-branch-clone)
                return false;
            case SN_SR::STATUS_SOCK_TIME_WAIT: // NOLINT(bugprone-branch-clone)
                return false;
            case SN_SR::STATUS_SOCK_LAST_ACK: // NOLINT(bugprone-branch-clone)
                return false;
            default: expect( m_network_stack->nonresponsive_device_error() );
        } // switch
    }

    /**
     * \brief Get the connection's remote endpoint.
     *
     * \return The connection's remote endpoint.
     */
    auto remote_endpoint() const noexcept -> ::picolibrary::IP::TCP::Endpoint
    {
        return { { m_driver->read_sn_dipr( m_socket_id ) }, m_driver->read_sn_dport( m_socket_id ) };
    }

    /**
     * \brief Get the connection's local endpoint.
     *
     * \return The connection's local endpoint.
     */
    auto local_endpoint() const noexcept -> ::picolibrary::IP::TCP::Endpoint
    {
        return { { m_driver->read_sipr() }, m_driver->read_sn_port( m_socket_id ) };
    }

    /**
     * \brief Get the amount of data that has yet to be transmitted to the remote
     *        endpoint.
     *
     * \pre the W5500 is responsive
     *
     * \return The amount of data that has yet to be transmitted to the remote endpoint.
     */
    auto outstanding() const noexcept -> Size
    {
        auto const buffer_size = static_cast<Size>(
            to_underlying( m_network_stack->socket_buffer_size() ) * 1024 );
        auto const sn_tx_fsr = m_driver->read_sn_tx_fsr( m_socket_id );

        expect( sn_tx_fsr <= buffer_size, m_network_stack->nonresponsive_device_error() );

        return buffer_size - sn_tx_fsr;
    }

    /**
     * \brief Check if data transmission is in progress.
     *
     * \return true if data transmission is in progress.
     * \return false if data transmission is not in progress.
     */
    constexpr auto is_transmitting() const noexcept -> bool
    {
        return m_is_transmitting;
    }

    /**
     * \brief Transmit data to the remote endpoint.
     *
     * \pre the socket has connected to a remote endpoint
     * \pre the W5500 is responsive
     *
     * \param[in] begin The beginning of the block of data to write to the socket's
     *            transmit buffer.
     * \param[in] end The end of the block of data to write to the socket's transmit
     *            buffer.
     *
     * \return The end of the data that was written to the socket's transmit buffer if
     *         writing data to the socket's transmit buffer succeeded.
     * \return picolibrary::Generic_Error::NOT_CONNECTED if the socket is not connected to
     *         a remote endpoint.
     * \return picolibrary::Generic_Error::WOULD_BLOCK if no data could be written to the
     *         socket's transmit buffer without blocking.
     */
    // NOLINTNEXTLINE(readability-function-size)
    auto transmit( std::uint8_t const * begin, std::uint8_t const * end ) noexcept
        -> Result<std::uint8_t const *, Error_Code>
    {
        // #lizard forgives the length

        expect( m_state == State::CONNECTED, Generic_Error::LOGIC_ERROR );

        switch ( m_driver->read_sn_sr( m_socket_id ) ) {
            case SN_SR::STATUS_SOCK_CLOSED: // NOLINT(bugprone-branch-clone)
                return Generic_Error::NOT_CONNECTED;
            case SN_SR::STATUS_SOCK_ESTABLISHED: // NOLINT(bugprone-branch-clone)
                break;
            case SN_SR::STATUS_SOCK_CLOSE_WAIT: // NOLINT(bugprone-branch-clone)
                return Generic_Error::NOT_CONNECTED;
            case SN_SR::STATUS_SOCK_FIN_WAIT: // NOLINT(bugprone-branch-clone)
                return Generic_Error::NOT_CONNECTED;
            case SN_SR::STATUS_SOCK_CLOSING: // NOLINT(bugprone-branch-clone)
                return Generic_Error::NOT_CONNECTED;
            case SN_SR::STATUS_SOCK_TIME_WAIT: // NOLINT(bugprone-branch-clone)
                return Generic_Error::NOT_CONNECTED;
            case SN_SR::STATUS_SOCK_LAST_ACK: // NOLINT(bugprone-branch-clone)
                return Generic_Error::NOT_CONNECTED;
            default: expect( m_network_stack->nonresponsive_device_error() );
        } // switch

        if ( m_is_transmitting ) {
            if ( not( m_driver->read_sn_ir( m_socket_id ) & Socket_Interrupt::DATA_TRANSMITTED ) ) {
                return Generic_Error::WOULD_BLOCK;
            } // if

            m_driver->write_sn_ir( m_socket_id, Socket_Interrupt::DATA_TRANSMITTED );

            m_is_transmitting = false;
        } // if

        if ( begin == end ) {
            return end;
        } // if

        auto const buffer_size = static_cast<Size>(
            to_underlying( m_network_stack->socket_buffer_size() ) * 1024 );
        auto const sn_tx_fsr = m_driver->read_sn_tx_fsr( m_socket_id );

        expect( sn_tx_fsr <= buffer_size, m_network_stack->nonresponsive_device_error() );

        if ( sn_tx_fsr == 0 ) {
            return Generic_Error::WOULD_BLOCK;
        } // if

        if ( end - begin > sn_tx_fsr ) {
            end = begin + sn_tx_fsr;
        } // if

        auto const sn_tx_wr = m_driver->read_sn_tx_wr( m_socket_id );
        m_driver->write_tx_buffer( m_socket_id, sn_tx_wr, begin, end );
        m_driver->write_sn_tx_wr( m_socket_id, sn_tx_wr + ( end - begin ) );

        m_driver->write_sn_cr( m_socket_id, SN_CR::COMMAND_SEND );
        while ( m_driver->read_sn_cr( m_socket_id ) ) {} // while

        m_is_transmitting = true;

        return end;
    }

    /**
     * \brief Manually transmit a keepalive packet.
     *
     * \pre the socket has connected to a remote endpoint
     * \pre the W5500 is responsive
     *
     * \return Nothing if keepalive packet transmission succeeded.
     * \return picolibrary::Generic_Error::NOT_CONNECTED if the socket is not connected to
     *         a remote endpoint.
     */
    // NOLINTNEXTLINE(readability-function-size)
    auto transmit_keepalive() noexcept -> Result<Void, Error_Code>
    {
        // #lizard forgives the length

        expect( m_state == State::CONNECTED, Generic_Error::LOGIC_ERROR );

        switch ( m_driver->read_sn_sr( m_socket_id ) ) {
            case SN_SR::STATUS_SOCK_CLOSED: // NOLINT(bugprone-branch-clone)
                return Generic_Error::NOT_CONNECTED;
            case SN_SR::STATUS_SOCK_ESTABLISHED: // NOLINT(bugprone-branch-clone)
                break;
            case SN_SR::STATUS_SOCK_CLOSE_WAIT: // NOLINT(bugprone-branch-clone)
                return Generic_Error::NOT_CONNECTED;
            case SN_SR::STATUS_SOCK_FIN_WAIT: // NOLINT(bugprone-branch-clone)
                return Generic_Error::NOT_CONNECTED;
            case SN_SR::STATUS_SOCK_CLOSING: // NOLINT(bugprone-branch-clone)
                return Generic_Error::NOT_CONNECTED;
            case SN_SR::STATUS_SOCK_TIME_WAIT: // NOLINT(bugprone-branch-clone)
                return Generic_Error::NOT_CONNECTED;
            case SN_SR::STATUS_SOCK_LAST_ACK: // NOLINT(bugprone-branch-clone)
                return Generic_Error::NOT_CONNECTED;
            default: expect( m_network_stack->nonresponsive_device_error() );
        } // switch

        m_driver->write_sn_cr( m_socket_id, SN_CR::COMMAND_SEND_KEEP );
        while ( m_driver->read_sn_cr( m_socket_id ) ) {} // while

        return {};
    }

    /**
     * \brief Get the amount of data that is immediately available to be received from the
     *        remote endpoint.
     *
     * \pre the W5500 is responsive
     *
     * \return The amount of data that is immediately available to be received from the
     *         remote endpoint.
     */
    auto available() const noexcept -> Size
    {
        auto const buffer_size = static_cast<Size>(
            to_underlying( m_network_stack->socket_buffer_size() ) * 1024 );
        auto const sn_rx_rsr = m_driver->read_sn_rx_rsr( m_socket_id );

        expect( sn_rx_rsr <= buffer_size, m_network_stack->nonresponsive_device_error() );

        return sn_rx_rsr;
    }

    /**
     * \brief Receive data from the remote endpoint.
     *
     * \pre the socket has connected to a remote endpoint
     * \pre the W5500 is responsive
     *
     * \param[out] begin The beginning of the block of data read from the socket's receive
     *             buffer.
     * \param[out] end The end of the block of data read from the socket's receive buffer.
     *
     * \return The end of the data that was read from the socket's receive buffer if
     *         reading data from the socket's receive buffer succeeded.
     * \return picolibrary::Generic_Error::NOT_CONNECTED if the socket is not connected to
     *         a remote endpoint.
     * \return picolibrary::Generic_Error::WOULD_BLOCK if no data could be read from the
     *         socket's receive buffer without blocking.
     */
    // NOLINTNEXTLINE(readability-function-size)
    auto receive( std::uint8_t * begin, std::uint8_t * end ) noexcept
        -> Result<std::uint8_t *, Error_Code>
    {
        // #lizard forgives the length

        expect( m_state == State::CONNECTED, Generic_Error::LOGIC_ERROR );

        auto close_wait = false;

        switch ( m_driver->read_sn_sr( m_socket_id ) ) {
            case SN_SR::STATUS_SOCK_CLOSED: // NOLINT(bugprone-branch-clone)
                return Generic_Error::NOT_CONNECTED;
            case SN_SR::STATUS_SOCK_ESTABLISHED: // NOLINT(bugprone-branch-clone)
                break;
            case SN_SR::STATUS_SOCK_CLOSE_WAIT: // NOLINT(bugprone-branch-clone)
                close_wait = true;
                break;
            case SN_SR::STATUS_SOCK_FIN_WAIT: // NOLINT(bugprone-branch-clone)
                return Generic_Error::WOULD_BLOCK;
            case SN_SR::STATUS_SOCK_CLOSING: // NOLINT(bugprone-branch-clone)
                return Generic_Error::WOULD_BLOCK;
            case SN_SR::STATUS_SOCK_TIME_WAIT: // NOLINT(bugprone-branch-clone)
                return Generic_Error::WOULD_BLOCK;
            case SN_SR::STATUS_SOCK_LAST_ACK: // NOLINT(bugprone-branch-clone)
                return Generic_Error::WOULD_BLOCK;
            default: expect( m_network_stack->nonresponsive_device_error() );
        } // switch

        auto const buffer_size = static_cast<Size>(
            to_underlying( m_network_stack->socket_buffer_size() ) * 1024 );
        auto const sn_rx_rsr = m_driver->read_sn_rx_rsr( m_socket_id );

        expect( sn_rx_rsr <= buffer_size, m_network_stack->nonresponsive_device_error() );

        if ( sn_rx_rsr == 0 ) {
            return close_wait ? Generic_Error::NOT_CONNECTED : Generic_Error::WOULD_BLOCK;
        } // if

        if ( begin == end ) {
            return end;
        } // if

        if ( end - begin > sn_rx_rsr ) {
            end = begin + sn_rx_rsr;
        } // if

        auto const sn_rx_rd = m_driver->read_sn_rx_rd( m_socket_id );
        m_driver->read_rx_buffer( m_socket_id, sn_rx_rd, begin, end );
        m_driver->write_sn_rx_rd( m_socket_id, sn_rx_rd + ( end - begin ) );

        m_driver->write_sn_cr( m_socket_id, SN_CR::COMMAND_RECV );
        while ( m_driver->read_sn_cr( m_socket_id ) ) {} // while

        return end;
    }

    /**
     * \brief Disable further data transmission and reception.
     *
     * \pre the socket has connected to a remote endpoint
     * \pre the W5500 is responsive
     */
    void shutdown() noexcept
    {
        expect( m_state == State::CONNECTED, Generic_Error::LOGIC_ERROR );

        switch ( m_driver->read_sn_sr( m_socket_id ) ) {
            case SN_SR::STATUS_SOCK_CLOSED: // NOLINT(bugprone-branch-clone)
                return;
            case SN_SR::STATUS_SOCK_ESTABLISHED: // NOLINT(bugprone-branch-clone)
                break;
            case SN_SR::STATUS_SOCK_CLOSE_WAIT: // NOLINT(bugprone-branch-clone)
                break;
            default: expect( m_network_stack->nonresponsive_device_error() );
        } // switch

        m_driver->write_sn_cr( m_socket_id, SN_CR::COMMAND_DISCON );
        while ( m_driver->read_sn_cr( m_socket_id ) ) {} // while
    }

    /**
     * \brief Close the socket.
     *
     * \pre the W5500 is responsive
     */
    // NOLINTNEXTLINE(readability-function-size)
    void close() noexcept
    {
        // #lizard forgives the length

        if ( m_state == State::UNINITIALIZED ) {
            return;
        } // if

        if ( m_state != State::INITIALIZED ) {
            m_driver->write_sn_cr( m_socket_id, SN_CR::COMMAND_CLOSE );
            while ( m_driver->read_sn_cr( m_socket_id ) ) {} // while

            auto closed = false;
            do {
                switch ( m_driver->read_sn_sr( m_socket_id ) ) {
                    case SN_SR::STATUS_SOCK_CLOSED: // NOLINT(bugprone-branch-clone)
                        closed = true;
                        break;
                    case SN_SR::STATUS_SOCK_INIT: // NOLINT(bugprone-branch-clone)
                        break;
                    case SN_SR::STATUS_SOCK_ESTABLISHED: // NOLINT(bugprone-branch-clone)
                        break;
                    case SN_SR::STATUS_SOCK_CLOSE_WAIT: // NOLINT(bugprone-branch-clone)
                        break;
                    case SN_SR::STATUS_SOCK_SYNSENT: // NOLINT(bugprone-branch-clone)
                        break;
                    case SN_SR::STATUS_SOCK_FIN_WAIT: // NOLINT(bugprone-branch-clone)
                        break;
                    case SN_SR::STATUS_SOCK_CLOSING: // NOLINT(bugprone-branch-clone)
                        break;
                    case SN_SR::STATUS_SOCK_TIME_WAIT: // NOLINT(bugprone-branch-clone)
                        break;
                    case SN_SR::STATUS_SOCK_LAST_ACK: // NOLINT(bugprone-branch-clone)
                        break;
                    default: expect( m_network_stack->nonresponsive_device_error() );
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

        m_state = State::UNINITIALIZED;
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

    /**
     * \brief the socket's data transmission in progress flag.
     */
    bool m_is_transmitting{};
};

} // namespace picolibrary::WIZnet::W5500::IP::TCP

#endif // PICOLIBRARY_WIZNET_W5500_IP_TCP_H

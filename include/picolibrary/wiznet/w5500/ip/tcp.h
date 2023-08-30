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
 * \brief picolibrary::WIZnet::W5500::IP::TCP interface.
 */

#ifndef PICOLIBRARY_WIZNET_W5500_IP_TCP_H
#define PICOLIBRARY_WIZNET_W5500_IP_TCP_H

#include <cstdint>
#include <utility>

#include "picolibrary/error.h"
#include "picolibrary/fixed_capacity_vector.h"
#include "picolibrary/ip.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/precondition.h"
#include "picolibrary/result.h"
#include "picolibrary/utility.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/keys.h"

/**
 * \brief WIZnet W5500 TCP over IP facilities.
 */
namespace picolibrary::WIZnet::W5500::IP::TCP {

/**
 * \brief Client socket.
 *
 * \tparam Network_Stack The type of network stack the socket is associated with.
 */
template<typename Network_Stack>
class Client {
  public:
    /**
     * \brief The unsigned integer type used to report transmit/receive buffer
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
     * \param[in] network_stack The network stack the socket is associated with.
     * \param[in] socket_id The socket's hardware socket ID.
     */
    constexpr Client( Socket_Construction_Key, Network_Stack & network_stack, Socket_ID socket_id ) noexcept :
        m_state{ State::INITIALIZED },
        m_network_stack{ &network_stack },
        m_socket_id{ socket_id }
    {
    }

#ifdef PICOLIBRARY_ENABLE_AUTOMATED_TESTING
    /**
     * \brief Constructor.
     *
     * \param[in] network_stack The network stack the socket is associated with.
     * \param[in] socket_id The socket's hardware socket ID.
     * \param[in] state The socket's initial state.
     * \param[in] is_transmitting The socket's initial data transmission in progress
     *            status.
     */
    constexpr Client(
        Network_Stack & network_stack,
        Socket_ID       socket_id,
        State           state           = State::INITIALIZED,
        bool            is_transmitting = false ) noexcept :
        m_state{ state },
        m_network_stack{ &network_stack },
        m_socket_id{ socket_id },
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
        m_network_stack{ source.m_network_stack },
        m_socket_id{ source.m_socket_id },
        m_is_transmitting{ source.m_is_transmitting }
    {
        source.m_state         = State::UNINITIALIZED;
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

            m_state           = expression.m_state;
            m_network_stack   = expression.m_network_stack;
            m_socket_id       = expression.m_socket_id;
            m_is_transmitting = expression.m_is_transmitting;

            expression.m_state         = State::UNINITIALIZED;
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
     * \brief Get the socket's hardware socket ID.
     *
     * \return The socket's hardware socket ID.
     */
    constexpr auto socket_id() const noexcept -> Socket_ID
    {
        return m_socket_id;
    }

    /**
     * \brief Get the socket's socket interrupt mask (mask used when checking the network
     *        stack's socket interrupt context).
     *
     * \return The socket's socket interrupt mask.
     */
    constexpr auto socket_interrupt_mask() const noexcept -> std::uint8_t
    {
        return 1 << ( to_underlying( m_socket_id ) >> Control_Byte::Bit::SOCKET );
    }

    /**
     * \brief Configure the socket's no delayed ACK usage (defaults to disabled).
     *
     * \pre picolibrary::WIZnet::W5500::IP::TCP::Client::state() ==
     *      picolibrary::WIZnet::W5500::IP::TCP::Client::State::INITIALIZED
     *
     * \param[in] no_delayed_ack_usage_configuration The desired no delayed ACK usage
     *            configuration.
     */
    void configure_no_delayed_ack_usage( No_Delayed_ACK_Usage no_delayed_ack_usage_configuration ) noexcept
    {
        PICOLIBRARY_EXPECT( m_state == State::INITIALIZED, Generic_Error::LOGIC_ERROR );

        m_network_stack->driver( {} ).write_sn_mr(
            m_socket_id, to_underlying( no_delayed_ack_usage_configuration ) );
    }

    /**
     * \brief Get the socket's no delayed ACK usage configuration.
     *
     * \return The socket's no delayed ACK usage configuration.
     */
    auto no_delayed_ack_usage_configuration() const noexcept -> No_Delayed_ACK_Usage
    {
        return static_cast<No_Delayed_ACK_Usage>(
            m_network_stack->driver( {} ).read_sn_mr( m_socket_id ) & SN_MR::Mask::ND );
    }

    /**
     * \brief Configure the socket's maximum segment size (defaults to 0x0000).
     *
     * \pre picolibrary::WIZnet::W5500::IP::TCP::Client::state() ==
     *      picolibrary::WIZnet::W5500::IP::TCP::Client::State::INITIALIZED
     *
     * \param[in] maximum_segment_size The desired maximum segment size.
     */
    void configure_maximum_segment_size( std::uint16_t maximum_segment_size ) noexcept
    {
        PICOLIBRARY_EXPECT( m_state == State::INITIALIZED, Generic_Error::LOGIC_ERROR );

        m_network_stack->driver( {} ).write_sn_mssr( m_socket_id, maximum_segment_size );
    }

    /**
     * \brief Get the socket's maximum segment size.
     *
     * \return The socket's maximum segment size.
     */
    auto maximum_segment_size() const noexcept -> std::uint16_t
    {
        return m_network_stack->driver( {} ).read_sn_mssr( m_socket_id );
    }

    /**
     * \brief Configure the socket's IPv4 time to live field value (defaults to 0x80).
     *
     * \pre picolibrary::WIZnet::W5500::IP::TCP::Client::state() ==
     *      picolibrary::WIZnet::W5500::IP::TCP::Client::State::INITIALIZED
     *
     * \param[in] time_to_live The desired IPv4 time to live field value.
     */
    void configure_time_to_live( std::uint8_t time_to_live ) noexcept
    {
        PICOLIBRARY_EXPECT( m_state == State::INITIALIZED, Generic_Error::LOGIC_ERROR );

        m_network_stack->driver( {} ).write_sn_ttl( m_socket_id, time_to_live );
    }

    /**
     * \brief Get the socket's IPv4 time to live field value.
     *
     * \return The socket's IPv4 time to live field value.
     */
    auto time_to_live() const noexcept -> std::uint8_t
    {
        return m_network_stack->driver( {} ).read_sn_ttl( m_socket_id );
    }

    /**
     * \brief Configure the socket's keepalive packet transmission period (SN_KPALVTR
     *        register value, defaults to 0x00).
     *
     * \pre picolibrary::WIZnet::W5500::IP::TCP::Client::state() ==
     *      picolibrary::WIZnet::W5500::IP::TCP::Client::State::INITIALIZED
     *
     * \param[in] keepalive_period The desired keepalive packet transmission period.
     */
    void configure_keepalive_period( std::uint8_t keepalive_period ) noexcept
    {
        PICOLIBRARY_EXPECT( m_state == State::INITIALIZED, Generic_Error::LOGIC_ERROR );

        m_network_stack->driver( {} ).write_sn_kpalvtr( m_socket_id, keepalive_period );
    }

    /**
     * \brief Get the socket's keepalive packet transmission period (SN_KPALVTR register
     *        value).
     *
     * \return The socket's keepalive packet transmission period.
     */
    auto keepalive_period() const noexcept -> std::uint8_t
    {
        return m_network_stack->driver( {} ).read_sn_kpalvtr( m_socket_id );
    }

    /**
     * \brief Enable interrupts.
     *
     * \param[in] mask The mask identifying the interrupts to enable.
     */
    void enable_interrupts( std::uint8_t mask ) noexcept
    {
        auto & driver = m_network_stack->driver( {} );

        driver.write_sn_imr( m_socket_id, driver.read_sn_imr( m_socket_id ) | mask );
    }

    /**
     * \brief Disable interrupts.
     *
     * \param[in] mask The mask identifying the interrupts to disable.
     */
    void disable_interrupts( std::uint8_t mask ) noexcept
    {
        auto & driver = m_network_stack->driver( {} );

        driver.write_sn_imr( m_socket_id, driver.read_sn_imr( m_socket_id ) & ~mask );
    }

    /**
     * \brief Disable all interrupts.
     */
    void disable_interrupts() noexcept
    {
        m_network_stack->driver( {} ).write_sn_imr( m_socket_id, 0x00 );
    }

    /**
     * \brief Get a mask identifying the interrupts that are enabled.
     *
     * \return A mask identifying the interrupts that are enabled.
     */
    auto enabled_interrupts() const noexcept -> std::uint8_t
    {
        return m_network_stack->driver( {} ).read_sn_imr( m_socket_id );
    }

    /**
     * \brief Get the interrupt context (SN_IR register value).
     *
     * \return The socket's interrupt context.
     */
    auto interrupt_context() const noexcept -> std::uint8_t
    {
        return m_network_stack->driver( {} ).read_sn_ir( m_socket_id );
    }

    /**
     * \brief Clear interrupts.
     *
     * \param[in] mask The mask identifying the interrupts to clear.
     */
    void clear_interrupts( std::uint8_t mask ) noexcept
    {
        m_network_stack->driver( {} ).write_sn_ir( m_socket_id, mask );

        if ( mask & Socket_Interrupt::DATA_TRANSMITTED ) {
            m_is_transmitting = false;
        } // if
    }

    /**
     * \brief Bind the socket to a local endpoint.
     *
     * \pre the socket is in a state that allows it to be bound to a local endpoint
     * \pre the socket is not already bound to a local endpoint
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

        PICOLIBRARY_EXPECT( m_state == State::INITIALIZED, Generic_Error::LOGIC_ERROR );

        PICOLIBRARY_EXPECT(
            endpoint.address().version() == ::picolibrary::IP::Version::UNSPECIFIED
                or endpoint.address().version() == ::picolibrary::IP::Version::_4,
            Generic_Error::INVALID_ARGUMENT );

        auto & driver = m_network_stack->driver( {} );

        if ( not endpoint.address().is_any() ) {
            PICOLIBRARY_EXPECT(
                endpoint.address().ipv4().as_byte_array() == driver.read_sipr(),
                Generic_Error::INVALID_ARGUMENT );
        } // if

        driver.write_sn_port(
            m_socket_id,
            m_network_stack->tcp_port_allocator( {} ).allocate( driver, endpoint.port() ).as_unsigned_integer() );

        driver.write_sn_mr(
            m_socket_id, ( driver.read_sn_mr( m_socket_id ) & ~SN_MR::Mask::P ) | SN_MR::P_TCP );

        driver.write_sn_cr( m_socket_id, SN_CR::COMMAND_OPEN );
        while ( driver.read_sn_cr( m_socket_id ) ) {} // while

        while ( driver.read_sn_sr( m_socket_id ) != SN_SR::STATUS_SOCK_INIT ) {} // while

        m_state = State::BOUND;
    }

    /**
     * \brief Connect to a remote endpoint.
     *
     * \pre the socket is in a state that allows it to connect to a remote endpoint
     * \pre the socket is not already connected to a remote endpoint
     *
     * \param[in] endpoint The remote endpoint to connect to.
     *
     * \pre endpoint is a valid remote endpoint
     *
     * \return Nothing if connecting to the remote endpoint succeeded.
     * \return picolibrary::Generic_Error::WOULD_BLOCK if connecting to the remote
     *         endpoint cannot succeed immediately.
     * \return picolibrary::Generic_Error::OPERATION_TIMEOUT if connecting to the remote
     *         endpoint timed out.
     */
    // NOLINTNEXTLINE(readability-function-size)
    auto connect( ::picolibrary::IP::TCP::Endpoint const & endpoint ) noexcept -> Result<void>
    {
        // #lizard forgives the length

        auto & driver = m_network_stack->driver( {} );

        if ( m_state == State::BOUND ) {
            PICOLIBRARY_EXPECT(
                endpoint.address().is_ipv4() and not endpoint.address().is_any()
                    and not endpoint.port().is_any(),
                Generic_Error::INVALID_ARGUMENT );

            driver.write_sn_dipr( m_socket_id, endpoint.address().ipv4().as_byte_array() );
            driver.write_sn_dport( m_socket_id, endpoint.port().as_unsigned_integer() );

            driver.write_sn_cr( m_socket_id, SN_CR::COMMAND_CONNECT );
            while ( driver.read_sn_cr( m_socket_id ) ) {} // while

            m_state = State::CONNECTING;

            return Generic_Error::WOULD_BLOCK;
        } // if

        if ( m_state == State::CONNECTING ) {
            switch ( driver.read_sn_sr( m_socket_id ) ) {
                case SN_SR::STATUS_SOCK_CLOSED: return Generic_Error::OPERATION_TIMEOUT;
                case SN_SR::STATUS_SOCK_ESTABLISHED: [[fallthrough]];
                case SN_SR::STATUS_SOCK_CLOSE_WAIT: m_state = State::CONNECTED; return {};
                default: return Generic_Error::WOULD_BLOCK;
            } // switch
        }     // if

        PICOLIBRARY_EXPECTATION_NOT_MET( Generic_Error::LOGIC_ERROR );
    }

    /**
     * \brief Check if the socket is connected to a remote endpoint.
     *
     * \return true if the socket is connected to a remote endpoint.
     * \return false if the socket is not connected to a remote endpoint.
     */
    auto is_connected() const noexcept -> bool
    {
        return m_network_stack->driver( {} ).read_sn_sr( m_socket_id ) == SN_SR::STATUS_SOCK_ESTABLISHED;
    }

    /**
     * \brief Get the connection's remote endpoint.
     *
     * \return The connection's remote endpoint.
     */
    auto remote_endpoint() const noexcept -> ::picolibrary::IP::TCP::Endpoint
    {
        auto const & driver = m_network_stack->driver( {} );

        return { { driver.read_sn_dipr( m_socket_id ) }, driver.read_sn_dport( m_socket_id ) };
    }

    /**
     * \brief Get the connection's local endpoint.
     *
     * \return The connection's local endpoint.
     */
    auto local_endpoint() const noexcept -> ::picolibrary::IP::TCP::Endpoint
    {
        auto const & driver = m_network_stack->driver( {} );

        return { { driver.read_sipr() }, driver.read_sn_port( m_socket_id ) };
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
        auto const sn_tx_fsr = m_network_stack->driver( {} ).read_sn_tx_fsr( m_socket_id );

        PICOLIBRARY_EXPECT( sn_tx_fsr <= buffer_size, m_network_stack->nonresponsive_device_error() );

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
        -> Result<std::uint8_t const *>
    {
        // #lizard forgives the length

        PICOLIBRARY_EXPECT( m_state == State::CONNECTED, Generic_Error::LOGIC_ERROR );

        auto & driver = m_network_stack->driver( {} );

        if ( driver.read_sn_sr( m_socket_id ) != SN_SR::STATUS_SOCK_ESTABLISHED ) {
            return Generic_Error::NOT_CONNECTED;
        } // if

        if ( m_is_transmitting ) {
            if ( not( driver.read_sn_ir( m_socket_id ) & Socket_Interrupt::DATA_TRANSMITTED ) ) {
                return Generic_Error::WOULD_BLOCK;
            } // if

            driver.write_sn_ir( m_socket_id, Socket_Interrupt::DATA_TRANSMITTED );

            m_is_transmitting = false;
        } // if

        if ( begin == end ) {
            return end;
        } // if

        auto const buffer_size = static_cast<Size>(
            to_underlying( m_network_stack->socket_buffer_size() ) * 1024 );
        auto const sn_tx_fsr = driver.read_sn_tx_fsr( m_socket_id );

        PICOLIBRARY_EXPECT( sn_tx_fsr <= buffer_size, m_network_stack->nonresponsive_device_error() );

        if ( sn_tx_fsr == 0 ) {
            return Generic_Error::WOULD_BLOCK;
        } // if

        if ( static_cast<std::uintptr_t>( end - begin ) > sn_tx_fsr ) {
            end = begin + sn_tx_fsr;
        } // if

        auto const sn_tx_wr = driver.read_sn_tx_wr( m_socket_id );
        driver.write_tx_buffer( m_socket_id, sn_tx_wr, begin, end );
        driver.write_sn_tx_wr( m_socket_id, sn_tx_wr + ( end - begin ) );

        driver.write_sn_cr( m_socket_id, SN_CR::COMMAND_SEND );
        while ( driver.read_sn_cr( m_socket_id ) ) {} // while

        m_is_transmitting = true;

        return end;
    }

    /**
     * \brief Manually transmit a keepalive packet.
     *
     * \pre the socket has connected to a remote endpoint
     *
     * \return Nothing if keepalive packet transmission succeeded.
     * \return picolibrary::Generic_Error::NOT_CONNECTED if the socket is not connected to
     *         a remote endpoint.
     */
    auto transmit_keepalive() noexcept -> Result<void>
    {
        PICOLIBRARY_EXPECT( m_state == State::CONNECTED, Generic_Error::LOGIC_ERROR );

        auto & driver = m_network_stack->driver( {} );

        if ( driver.read_sn_sr( m_socket_id ) != SN_SR::STATUS_SOCK_ESTABLISHED ) {
            return Generic_Error::NOT_CONNECTED;
        } // if

        driver.write_sn_cr( m_socket_id, SN_CR::COMMAND_SEND_KEEP );
        while ( driver.read_sn_cr( m_socket_id ) ) {} // while

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
        auto const sn_rx_rsr = m_network_stack->driver( {} ).read_sn_rx_rsr( m_socket_id );

        PICOLIBRARY_EXPECT( sn_rx_rsr <= buffer_size, m_network_stack->nonresponsive_device_error() );

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
    auto receive( std::uint8_t * begin, std::uint8_t * end ) noexcept -> Result<std::uint8_t *>
    {
        // #lizard forgives the length

        PICOLIBRARY_EXPECT( m_state == State::CONNECTED, Generic_Error::LOGIC_ERROR );

        auto & driver = m_network_stack->driver( {} );

        auto close_wait = false;

        switch ( driver.read_sn_sr( m_socket_id ) ) {
            case SN_SR::STATUS_SOCK_CLOSED: return Generic_Error::NOT_CONNECTED;
            case SN_SR::STATUS_SOCK_CLOSE_WAIT: close_wait = true; [[fallthrough]];
            case SN_SR::STATUS_SOCK_ESTABLISHED: break;
            default: return Generic_Error::WOULD_BLOCK;
        } // switch

        auto const buffer_size = static_cast<Size>(
            to_underlying( m_network_stack->socket_buffer_size() ) * 1024 );
        auto const sn_rx_rsr = driver.read_sn_rx_rsr( m_socket_id );

        PICOLIBRARY_EXPECT( sn_rx_rsr <= buffer_size, m_network_stack->nonresponsive_device_error() );

        if ( sn_rx_rsr == 0 ) {
            return close_wait ? Generic_Error::NOT_CONNECTED : Generic_Error::WOULD_BLOCK;
        } // if

        if ( begin == end ) {
            return end;
        } // if

        if ( static_cast<std::uintptr_t>( end - begin ) > sn_rx_rsr ) {
            end = begin + sn_rx_rsr;
        } // if

        auto const sn_rx_rd = driver.read_sn_rx_rd( m_socket_id );
        driver.read_rx_buffer( m_socket_id, sn_rx_rd, begin, end );
        driver.write_sn_rx_rd( m_socket_id, sn_rx_rd + ( end - begin ) );

        driver.write_sn_cr( m_socket_id, SN_CR::COMMAND_RECV );
        while ( driver.read_sn_cr( m_socket_id ) ) {} // while

        return end;
    }

    /**
     * \brief Disable further data transmission and reception.
     *
     * \pre the socket has connected to a remote endpoint
     */
    void shutdown() noexcept
    {
        PICOLIBRARY_EXPECT( m_state == State::CONNECTED, Generic_Error::LOGIC_ERROR );

        auto & driver = m_network_stack->driver( {} );

        if ( driver.read_sn_sr( m_socket_id ) == SN_SR::STATUS_SOCK_CLOSED ) {
            return;
        } // if

        driver.write_sn_cr( m_socket_id, SN_CR::COMMAND_DISCON );
        while ( driver.read_sn_cr( m_socket_id ) ) {} // while
    }

    /**
     * \brief Close the socket.
     */
    constexpr void close() noexcept
    {
        if ( m_state == State::UNINITIALIZED ) {
            return;
        } // if

        if ( m_state != State::INITIALIZED ) {
            m_network_stack->tcp_port_allocator( {} ).deallocate(
                m_network_stack->driver( {} ).read_sn_port( m_socket_id ) );
        } // if

        m_network_stack->deallocate_socket( {}, m_socket_id );

        m_state = State::UNINITIALIZED;
    }

  private:
    /**
     * \brief The socket's state.
     */
    State m_state{};

    /**
     * \brief The network stack the socket is associated with.
     */
    Network_Stack * m_network_stack{};

    /**
     * \brief The socket's hardware socket ID.
     */
    Socket_ID m_socket_id{};

    /**
     * \brief The socket's data transmission in progress flag.
     */
    bool m_is_transmitting{};
};

/**
 * \brief Serverr socket.
 *
 * \tparam Network_Stack The type of network stack the socket is associated with.
 * \tparam Acceptor The type of acceptor socket the socket is associated with.
 */
template<typename Network_Stack, typename Acceptor>
class Server {
  public:
    /**
     * \brief The unsigned integer type used to report transmit/receive buffer information.
     */
    using Size = std::uint16_t;

    /**
     * \brief Socket state.
     */
    enum class State : std::uint_fast8_t {
        UNINITIALIZED, ///< Uninitialized.
        CONNECTED,     ///< Connected.
    };

    /**
     * \brief Constructor.
     */
    constexpr Server() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] network_stack The network stack the socket is associated with.
     * \param[in] acceptor The acceptor socket the socket is associated with.
     * \param[in] socket_id The socket's hardware socket ID.
     */
    constexpr Server( Socket_Construction_Key, Network_Stack & network_stack, Acceptor & acceptor, Socket_ID socket_id ) noexcept
        :
        m_state{ State::CONNECTED },
        m_network_stack{ &network_stack },
        m_acceptor{ &acceptor },
        m_socket_id{ socket_id }
    {
    }

#ifdef PICOLIBRARY_ENABLE_AUTOMATED_TESTING
    /**
     * \brief Constructor.
     *
     * \param[in] network_stack The network stack the socket is associated with.
     * \param[in] socket_id The socket's hardware socket ID.
     * \param[in] is_transmitting The socket's initial data transmission in progress
     *            status.
     */
    constexpr Server( Network_Stack & network_stack, Acceptor & acceptor, Socket_ID socket_id, bool is_transmitting = false ) noexcept
        :
        m_state{ State::CONNECTED },
        m_network_stack{ &network_stack },
        m_acceptor{ &acceptor },
        m_socket_id{ socket_id },
        m_is_transmitting{ is_transmitting }
    {
    }
#endif // PICOLIBRARY_ENABLE_AUTOMATED_TESTING

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Server( Server && source ) noexcept :
        m_state{ source.m_state },
        m_network_stack{ source.m_network_stack },
        m_acceptor{ source.m_acceptor },
        m_socket_id{ source.m_socket_id },
        m_is_transmitting{ source.m_is_transmitting }
    {
        source.m_state         = State::UNINITIALIZED;
        source.m_network_stack = nullptr;
        source.m_acceptor      = nullptr;
    }

    Server( Server const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Server() noexcept
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
    constexpr auto operator=( Server && expression ) noexcept -> Server &
    {
        if ( &expression != this ) {
            close();

            m_state           = expression.m_state;
            m_network_stack   = expression.m_network_stack;
            m_acceptor        = expression.m_acceptor;
            m_socket_id       = expression.m_socket_id;
            m_is_transmitting = expression.m_is_transmitting;

            expression.m_state         = State::UNINITIALIZED;
            expression.m_network_stack = nullptr;
            expression.m_acceptor      = nullptr;
        } // if

        return *this;
    }

    auto operator=( Server const & ) = delete;

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
     * \brief Get the socket's hardware socket ID.
     *
     * \return The socket's hardware socket ID.
     */
    constexpr auto socket_id() const noexcept -> Socket_ID
    {
        return m_socket_id;
    }

    /**
     * \brief Get the socket's no delayed ACK usage configuration.
     *
     * \return The socket's no delayed ACK usage configuration.
     */
    auto no_delayed_ack_usage_configuration() const noexcept -> No_Delayed_ACK_Usage
    {
        return static_cast<No_Delayed_ACK_Usage>(
            m_network_stack->driver( {} ).read_sn_mr( m_socket_id ) & SN_MR::Mask::ND );
    }

    /**
     * \brief Get the socket's maximum segment size.
     *
     * \return The socket's maximum segment size.
     */
    auto maximum_segment_size() const noexcept -> std::uint16_t
    {
        return m_network_stack->driver( {} ).read_sn_mssr( m_socket_id );
    }

    /**
     * \brief Get the socket's IPv4 time to live field value.
     *
     * \return The socket's IPv4 time to live field value.
     */
    auto time_to_live() const noexcept -> std::uint8_t
    {
        return m_network_stack->driver( {} ).read_sn_ttl( m_socket_id );
    }

    /**
     * \brief Get the socket's keepalive packet transmission period (SN_KPALVTR register
     *        value).
     *
     * \return The socket's keepalive packet transmission period.
     */
    auto keepalive_period() const noexcept -> std::uint8_t
    {
        return m_network_stack->driver( {} ).read_sn_kpalvtr( m_socket_id );
    }

    /**
     * \brief Get the socket's socket interrupt mask (mask used when checking the network
     *        stack's socket interrupt context).
     *
     * \return The socket's socket interrupt mask.
     */
    constexpr auto socket_interrupt_mask() const noexcept -> std::uint8_t
    {
        return 1 << ( to_underlying( m_socket_id ) >> Control_Byte::Bit::SOCKET );
    }

    /**
     * \brief Get a mask identifying the interrupts that are enabled.
     *
     * \return A mask identifying the interrupts that are enabled.
     */
    auto enabled_interrupts() const noexcept -> std::uint8_t
    {
        return m_network_stack->driver( {} ).read_sn_imr( m_socket_id );
    }

    /**
     * \brief Get the interrupt context (SN_IR register value).
     *
     * \return The socket's interrupt context.
     */
    auto interrupt_context() const noexcept -> std::uint8_t
    {
        return m_network_stack->driver( {} ).read_sn_ir( m_socket_id );
    }

    /**
     * \brief Clear interrupts.
     *
     * \param[in] mask The mask identifying the interrupts to clear.
     */
    void clear_interrupts( std::uint8_t mask ) noexcept
    {
        m_network_stack->driver( {} ).write_sn_ir( m_socket_id, mask );

        if ( mask & Socket_Interrupt::DATA_TRANSMITTED ) {
            m_is_transmitting = false;
        } // if
    }

    /**
     * \brief Check if the socket is connected to a remote endpoint.
     *
     * \return true if the socket is connected to a remote endpoint.
     * \return false if the socket is not connected to a remote endpoint.
     */
    auto is_connected() const noexcept -> bool
    {
        return m_network_stack->driver( {} ).read_sn_sr( m_socket_id ) == SN_SR::STATUS_SOCK_ESTABLISHED;
    }

    /**
     * \brief Get the connection's remote endpoint.
     *
     * \return The connection's remote endpoint.
     */
    auto remote_endpoint() const noexcept -> ::picolibrary::IP::TCP::Endpoint
    {
        auto const & driver = m_network_stack->driver( {} );

        return { { driver.read_sn_dipr( m_socket_id ) }, driver.read_sn_dport( m_socket_id ) };
    }

    /**
     * \brief Get the connection's local endpoint.
     *
     * \return The connection's local endpoint.
     */
    auto local_endpoint() const noexcept -> ::picolibrary::IP::TCP::Endpoint
    {
        auto const & driver = m_network_stack->driver( {} );

        return { { driver.read_sipr() }, driver.read_sn_port( m_socket_id ) };
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
        auto const sn_tx_fsr = m_network_stack->driver( {} ).read_sn_tx_fsr( m_socket_id );

        PICOLIBRARY_EXPECT( sn_tx_fsr <= buffer_size, m_network_stack->nonresponsive_device_error() );

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
        -> Result<std::uint8_t const *>
    {
        // #lizard forgives the length

        PICOLIBRARY_EXPECT( m_state == State::CONNECTED, Generic_Error::LOGIC_ERROR );

        auto & driver = m_network_stack->driver( {} );

        if ( driver.read_sn_sr( m_socket_id ) != SN_SR::STATUS_SOCK_ESTABLISHED ) {
            return Generic_Error::NOT_CONNECTED;
        } // if

        if ( m_is_transmitting ) {
            if ( not( driver.read_sn_ir( m_socket_id ) & Socket_Interrupt::DATA_TRANSMITTED ) ) {
                return Generic_Error::WOULD_BLOCK;
            } // if

            driver.write_sn_ir( m_socket_id, Socket_Interrupt::DATA_TRANSMITTED );

            m_is_transmitting = false;
        } // if

        if ( begin == end ) {
            return end;
        } // if

        auto const buffer_size = static_cast<Size>(
            to_underlying( m_network_stack->socket_buffer_size() ) * 1024 );
        auto const sn_tx_fsr = driver.read_sn_tx_fsr( m_socket_id );

        PICOLIBRARY_EXPECT( sn_tx_fsr <= buffer_size, m_network_stack->nonresponsive_device_error() );

        if ( sn_tx_fsr == 0 ) {
            return Generic_Error::WOULD_BLOCK;
        } // if

        if ( static_cast<std::uintptr_t>( end - begin ) > sn_tx_fsr ) {
            end = begin + sn_tx_fsr;
        } // if

        auto const sn_tx_wr = driver.read_sn_tx_wr( m_socket_id );
        driver.write_tx_buffer( m_socket_id, sn_tx_wr, begin, end );
        driver.write_sn_tx_wr( m_socket_id, sn_tx_wr + ( end - begin ) );

        driver.write_sn_cr( m_socket_id, SN_CR::COMMAND_SEND );
        while ( driver.read_sn_cr( m_socket_id ) ) {} // while

        m_is_transmitting = true;

        return end;
    }

    /**
     * \brief Manually transmit a keepalive packet.
     *
     * \pre the socket has connected to a remote endpoint
     *
     * \return Nothing if keepalive packet transmission succeeded.
     * \return picolibrary::Generic_Error::NOT_CONNECTED if the socket is not connected to
     *         a remote endpoint.
     */
    auto transmit_keepalive() noexcept -> Result<void>
    {
        PICOLIBRARY_EXPECT( m_state == State::CONNECTED, Generic_Error::LOGIC_ERROR );

        auto & driver = m_network_stack->driver( {} );

        if ( driver.read_sn_sr( m_socket_id ) != SN_SR::STATUS_SOCK_ESTABLISHED ) {
            return Generic_Error::NOT_CONNECTED;
        } // if

        driver.write_sn_cr( m_socket_id, SN_CR::COMMAND_SEND_KEEP );
        while ( driver.read_sn_cr( m_socket_id ) ) {} // while

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
        auto const sn_rx_rsr = m_network_stack->driver( {} ).read_sn_rx_rsr( m_socket_id );

        PICOLIBRARY_EXPECT( sn_rx_rsr <= buffer_size, m_network_stack->nonresponsive_device_error() );

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
    auto receive( std::uint8_t * begin, std::uint8_t * end ) noexcept -> Result<std::uint8_t *>
    {
        // #lizard forgives the length

        PICOLIBRARY_EXPECT( m_state == State::CONNECTED, Generic_Error::LOGIC_ERROR );

        auto & driver = m_network_stack->driver( {} );

        auto close_wait = false;

        switch ( driver.read_sn_sr( m_socket_id ) ) {
            case SN_SR::STATUS_SOCK_CLOSED: return Generic_Error::NOT_CONNECTED;
            case SN_SR::STATUS_SOCK_CLOSE_WAIT: close_wait = true; [[fallthrough]];
            case SN_SR::STATUS_SOCK_ESTABLISHED: break;
            default: return Generic_Error::WOULD_BLOCK;
        } // switch

        auto const buffer_size = static_cast<Size>(
            to_underlying( m_network_stack->socket_buffer_size() ) * 1024 );
        auto const sn_rx_rsr = driver.read_sn_rx_rsr( m_socket_id );

        PICOLIBRARY_EXPECT( sn_rx_rsr <= buffer_size, m_network_stack->nonresponsive_device_error() );

        if ( sn_rx_rsr == 0 ) {
            return close_wait ? Generic_Error::NOT_CONNECTED : Generic_Error::WOULD_BLOCK;
        } // if

        if ( begin == end ) {
            return end;
        } // if

        if ( static_cast<std::uintptr_t>( end - begin ) > sn_rx_rsr ) {
            end = begin + sn_rx_rsr;
        } // if

        auto const sn_rx_rd = driver.read_sn_rx_rd( m_socket_id );
        driver.read_rx_buffer( m_socket_id, sn_rx_rd, begin, end );
        driver.write_sn_rx_rd( m_socket_id, sn_rx_rd + ( end - begin ) );

        driver.write_sn_cr( m_socket_id, SN_CR::COMMAND_RECV );
        while ( driver.read_sn_cr( m_socket_id ) ) {} // while

        return end;
    }

    /**
     * \brief Disable further data transmission and reception.
     *
     * \pre the socket has connected to a remote endpoint
     */
    void shutdown() noexcept
    {
        PICOLIBRARY_EXPECT( m_state == State::CONNECTED, Generic_Error::LOGIC_ERROR );

        auto & driver = m_network_stack->driver( {} );

        if ( driver.read_sn_sr( m_socket_id ) == SN_SR::STATUS_SOCK_CLOSED ) {
            return;
        } // if

        driver.write_sn_cr( m_socket_id, SN_CR::COMMAND_DISCON );
        while ( driver.read_sn_cr( m_socket_id ) ) {} // while
    }

    /**
     * \brief Close the socket.
     */
    // NOLINTNEXTLINE(readability-function-size)
    constexpr void close() noexcept
    {
        // #lizard forgives the length

        if ( m_state == State::UNINITIALIZED ) {
            return;
        } // if

        if ( not m_network_stack->tcp_server_is_detached( m_socket_id ) ) {
            m_acceptor->deallocate_socket( {}, m_socket_id );
        } else {
            auto & driver = m_network_stack->driver( {} );

            auto const sn_port = driver.read_sn_port( m_socket_id );

            auto       deallocate_port = true;
            auto const sockets         = m_network_stack->sockets();
            for ( auto socket = std::uint_fast8_t{}; socket < sockets; ++socket ) {
                auto const socket_id = static_cast<Socket_ID>( socket << Control_Byte::Bit::SOCKET );

                if ( socket_id != m_socket_id
                     and ( driver.read_sn_mr( socket_id ) & SN_MR::Mask::P ) == SN_MR::P_TCP
                     and driver.read_sn_port( socket_id ) == sn_port ) {
                    deallocate_port = false;
                } // if
            }     // for

            if ( deallocate_port ) {
                m_network_stack->tcp_port_allocator( {} ).deallocate( sn_port );
            } // if

            m_network_stack->deallocate_socket( {}, m_socket_id );
        } // else

        m_state = State::UNINITIALIZED;
    }

  private:
    /**
     * \brief The socket's state.
     */
    State m_state{};

    /**
     * \brief The network stack the socket is associated with.
     */
    Network_Stack * m_network_stack{};

    /**
     * \brief The acceptor socket the socket is associated with.
     */
    Acceptor * m_acceptor{};

    /**
     * \brief The socket's hardware socket ID.
     */
    Socket_ID m_socket_id{};

    /**
     * \brief The socket's data transmission in progress flag.
     */
    bool m_is_transmitting{};
};

/**
 * \brief Acceptor socket.
 *
 * \tparam Network_Stack The type of network stack the socket is associated with.
 */
template<typename Network_Stack>
class Acceptor {
  public:
    /**
     * \brief The type of server socket produced by the acceptor socket.
     */
    using Server = ::picolibrary::WIZnet::W5500::IP::TCP::Server<Network_Stack, Acceptor>;

    /**
     * \brief Hardware socket IDs.
     */
    using Socket_IDs = Fixed_Capacity_Vector<Socket_ID, SOCKETS>;

    /**
     * \brief Socket state.
     */
    enum class State : std::uint_fast8_t {
        UNINITIALIZED, ///< Uninitialized.
        INITIALIZED,   ///< Initialized.
        BOUND,         ///< Bound.
        LISTENING,     ///< Listening.
    };

    /**
     * \brief Constructor.
     */
    constexpr Acceptor() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] network_stack The network stack the socket is associated with.
     * \param[in] socket_ids The socket's hardware socket IDs.
     *
     * \pre not socket_ids.empty()
     */
    constexpr Acceptor( Socket_Construction_Key, Network_Stack & network_stack, Socket_IDs const & socket_ids ) noexcept
        :
        m_state{ State::INITIALIZED },
        m_network_stack{ &network_stack },
        m_sockets{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, socket_ids.begin(), socket_ids.end() }
    {
        PICOLIBRARY_EXPECT( not socket_ids.empty(), Generic_Error::INVALID_ARGUMENT );
    }

#ifdef PICOLIBRARY_ENABLE_AUTOMATED_TESTING
    /**
     * \brief Constructor.
     *
     * \param[in] network_stack The network stack the socket is associated with.
     * \param[in] socket_ids The socket's hardware socket IDs.
     * \param[in] state The socket's initial state.
     *
     * \pre not socket_ids.empty()
     */
    constexpr Acceptor( Network_Stack & network_stack, Socket_IDs const & socket_ids, State state = State::INITIALIZED ) noexcept
        :
        m_state{ state },
        m_network_stack{ &network_stack },
        m_sockets{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, socket_ids.begin(), socket_ids.end() }
    {
        PICOLIBRARY_EXPECT( not socket_ids.empty(), Generic_Error::INVALID_ARGUMENT );
    }
#endif // PICOLIBRARY_ENABLE_AUTOMATED_TESTING

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Acceptor( Acceptor && source ) noexcept :
        m_state{ source.m_state },
        m_network_stack{ source.m_network_stack },
        m_sockets{ std::move( source.m_sockets ) },
        m_accept_i{ source.m_accept_i }
    {
        source.m_state         = State::UNINITIALIZED;
        source.m_network_stack = nullptr;
    }

    Acceptor( Acceptor const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Acceptor() noexcept
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
    constexpr auto operator=( Acceptor && expression ) noexcept -> Acceptor &
    {
        if ( &expression != this ) {
            close();

            m_state         = expression.m_state;
            m_network_stack = expression.m_network_stack;
            m_sockets       = std::move( expression.m_sockets );
            m_accept_i      = expression.m_accept_i;

            expression.m_state         = State::UNINITIALIZED;
            expression.m_network_stack = nullptr;
        } // if

        return *this;
    }

    auto operator=( Acceptor const & ) = delete;

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
     * \brief Get the socket's hardware socket IDs.
     *
     * \return The socket's hardware socket IDs.
     */
    auto socket_ids() const noexcept -> Socket_IDs
    {
        auto socket_ids = Socket_IDs{};

        for ( auto const socket : m_sockets ) {
            socket_ids.push_back( BYPASS_PRECONDITION_EXPECTATION_CHECKS, socket.id );
        } // for

        return socket_ids;
    }

    /**
     * \brief Get the socket's socket interrupt mask (mask used when checking the network
     *        stack's socket interrupt context).
     */
    constexpr auto socket_interrupt_mask() const noexcept -> std::uint8_t
    {
        auto mask = std::uint8_t{};

        for ( auto const socket : m_sockets ) {
            mask |= 1 << ( to_underlying( socket.id ) >> Control_Byte::Bit::SOCKET );
        } // for

        return mask;
    }

    /**
     * \brief Configure the socket's no delayed ACK usage (defaults to disabled).
     *
     * \pre picolibrary::WIZnet::W5500::IP::TCP::Acceptor::state() ==
     *      picolibrary::WIZnet::W5500::IP::TCP::Acceptor::State::INITIALIZED
     *
     * \param[in] no_delayed_ack_usage_configuration The desired no delayed ACK usage
     *            configuration.
     */
    void configure_no_delayed_ack_usage( No_Delayed_ACK_Usage no_delayed_ack_usage_configuration ) noexcept
    {
        PICOLIBRARY_EXPECT( m_state == State::INITIALIZED, Generic_Error::LOGIC_ERROR );

        auto & driver = m_network_stack->driver( {} );

        for ( auto const socket : m_sockets ) {
            driver.write_sn_mr( socket.id, to_underlying( no_delayed_ack_usage_configuration ) );
        } // for
    }

    /**
     * \brief Get the socket's no delayed ACK usage configuration.
     *
     * \return The socket's no delayed ACK usage configuration.
     */
    auto no_delayed_ack_usage_configuration() const noexcept -> No_Delayed_ACK_Usage
    {
        return static_cast<No_Delayed_ACK_Usage>(
            m_network_stack->driver( {} ).read_sn_mr( m_sockets.front().id ) & SN_MR::Mask::ND );
    }

    /**
     * \brief Configure the socket's maximum segment size (defaults to 0x0000).
     *
     * \pre picolibrary::WIZnet::W5500::IP::TCP::Acceptor::state() ==
     *      picolibrary::WIZnet::W5500::IP::TCP::Acceptor::State::INITIALIZED
     *
     * \param[in] maximum_segment_size The desired maximum segment size.
     */
    void configure_maximum_segment_size( std::uint16_t maximum_segment_size ) noexcept
    {
        PICOLIBRARY_EXPECT( m_state == State::INITIALIZED, Generic_Error::LOGIC_ERROR );

        auto & driver = m_network_stack->driver( {} );

        for ( auto const socket : m_sockets ) {
            driver.write_sn_mssr( socket.id, maximum_segment_size );
        } // for
    }

    /**
     * \brief Get the socket's maximum segment size.
     *
     * \return The socket's maximum segment size.
     */
    auto maximum_segment_size() const noexcept -> std::uint16_t
    {
        return m_network_stack->driver( {} ).read_sn_mssr( m_sockets.front().id );
    }

    /**
     * \brief Configure the socket's IPv4 time to live field value (defaults to 0x80).
     *
     * \pre picolibrary::WIZnet::W5500::IP::TCP::Acceptor::state() ==
     *      picolibrary::WIZnet::W5500::IP::TCP::Acceptor::State::INITIALIZED
     *
     * \param[in] time_to_live The desired IPv4 time to live field value.
     */
    void configure_time_to_live( std::uint8_t time_to_live ) noexcept
    {
        PICOLIBRARY_EXPECT( m_state == State::INITIALIZED, Generic_Error::LOGIC_ERROR );

        auto & driver = m_network_stack->driver( {} );

        for ( auto const socket : m_sockets ) {
            driver.write_sn_ttl( socket.id, time_to_live );
        } // for
    }

    /**
     * \brief Get the socket's IPv4 time to live field value.
     *
     * \return The socket's IPv4 time to live field value.
     */
    auto time_to_live() const noexcept -> std::uint8_t
    {
        return m_network_stack->driver( {} ).read_sn_ttl( m_sockets.front().id );
    }

    /**
     * \brief Configure the socket's keepalive packet transmission period (SN_KPALVTR
     *        register value, defaults to 0x00).
     *
     * \pre picolibrary::WIZnet::W5500::IP::TCP::Acceptor::state() ==
     *      picolibrary::WIZnet::W5500::IP::TCP::Acceptor::State::INITIALIZED
     *
     * \param[in] keepalive_period The desired keepalive packet transmission period.
     */
    void configure_keepalive_period( std::uint8_t keepalive_period ) noexcept
    {
        PICOLIBRARY_EXPECT( m_state == State::INITIALIZED, Generic_Error::LOGIC_ERROR );

        auto & driver = m_network_stack->driver( {} );

        for ( auto const socket : m_sockets ) {
            driver.write_sn_kpalvtr( socket.id, keepalive_period );
        } // for
    }

    /**
     * \brief Get the socket's keepalive packet transmission period (SN_KPALVTR register
     *        value).
     *
     * \return The socket's keepalive packet transmission period.
     */
    auto keepalive_period() const noexcept -> std::uint8_t
    {
        return m_network_stack->driver( {} ).read_sn_kpalvtr( m_sockets.front().id );
    }

    /**
     * \brief Enable interrupts.
     *
     * \param[in] mask The mask identifying the interrupts to enable.
     */
    void enable_interrupts( std::uint8_t mask ) noexcept
    {
        auto & driver = m_network_stack->driver( {} );

        auto const sn_imr = driver.read_sn_imr( m_sockets.front().id );

        for ( auto const socket : m_sockets ) {
            driver.write_sn_imr( socket.id, sn_imr | mask );
        } // for
    }

    /**
     * \brief Disable interrupts.
     *
     * \param[in] mask The mask identifying the interrupts to disable.
     */
    void disable_interrupts( std::uint8_t mask ) noexcept
    {
        auto & driver = m_network_stack->driver( {} );

        auto const sn_imr = driver.read_sn_imr( m_sockets.front().id );

        for ( auto const socket : m_sockets ) {
            driver.write_sn_imr( socket.id, sn_imr & ~mask );
        } // for
    }

    /**
     * \brief Disable all interrupts.
     */
    void disable_interrupts() noexcept
    {
        auto & driver = m_network_stack->driver( {} );

        for ( auto const socket : m_sockets ) {
            driver.write_sn_imr( socket.id, 0x00 );
        } // for
    }

    /**
     * \brief Get a mask identifying the interrupts that are enabled.
     *
     * \return A mask identifying the interrupts that are enabled.
     */
    auto enabled_interrupts() const noexcept -> std::uint8_t
    {
        return m_network_stack->driver( {} ).read_sn_imr( m_sockets.front().id );
    }

    /**
     * \brief Get the interrupt context (SN_IR register values ORed together).
     *
     * \return The socket's interrupt context.
     */
    auto interrupt_context() const noexcept -> std::uint8_t
    {
        auto sn_ir = SN_IR::Type{};

        auto const & driver = m_network_stack->driver( {} );

        for ( auto const socket : m_sockets ) {
            sn_ir |= driver.read_sn_ir( socket.id );
        } // for

        return sn_ir;
    }

    /**
     * \brief Clear interrupts.
     *
     * \param[in] mask The mask identifying the interrupts to clear.
     *
     * \pre not( mask & picolibrary::WIZnet::W5500::Socket_Interrupt::DATA_TRANSMITTED )
     */
    void clear_interrupts( std::uint8_t mask ) noexcept
    {
        PICOLIBRARY_EXPECT( not( mask & Socket_Interrupt::DATA_TRANSMITTED ), Generic_Error::INVALID_ARGUMENT );

        auto & driver = m_network_stack->driver( {} );

        for ( auto const socket : m_sockets ) {
            driver.write_sn_ir( socket.id, mask );
        } // for
    }

    /**
     * \brief Bind the socket to a local endpoint.
     *
     * \pre the socket is in a state that allows it to be bound to a local endpoint
     * \pre the socket is not already bound to a local endpoint
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

        PICOLIBRARY_EXPECT( m_state == State::INITIALIZED, Generic_Error::LOGIC_ERROR );

        PICOLIBRARY_EXPECT(
            endpoint.address().version() == ::picolibrary::IP::Version::UNSPECIFIED
                or endpoint.address().version() == ::picolibrary::IP::Version::_4,
            Generic_Error::INVALID_ARGUMENT );

        auto & driver = m_network_stack->driver( {} );

        if ( not endpoint.address().is_any() ) {
            PICOLIBRARY_EXPECT(
                endpoint.address().ipv4().as_byte_array() == driver.read_sipr(),
                Generic_Error::INVALID_ARGUMENT );
        } // if

        auto const port = m_network_stack->tcp_port_allocator( {} ).allocate(
            driver, endpoint.port() );

        auto const sn_mr = driver.read_sn_mr( m_sockets.front().id );

        for ( auto const socket : m_sockets ) {
            driver.write_sn_port( socket.id, port.as_unsigned_integer() );

            driver.write_sn_mr( socket.id, ( sn_mr & ~SN_MR::Mask::P ) | SN_MR::P_TCP );

            driver.write_sn_cr( socket.id, SN_CR::COMMAND_OPEN );
            while ( driver.read_sn_cr( socket.id ) ) {} // while

            while ( driver.read_sn_sr( socket.id ) != SN_SR::STATUS_SOCK_INIT ) {} // while
        }                                                                          // for

        m_state = State::BOUND;
    }

    /**
     * \brief Listen for incoming connection requests.
     *
     * \pre the socket is in a state that allows it to listen for incoming connection
     *      requests
     * \pre the socket is not already listening for incoming connection requests
     *
     * \param[in] backlog The maximum number of simultaneously connected clients.
     *
     * \pre sufficient resources are available to support the requested backlog
     */
    // NOLINTNEXTLINE(readability-function-size)
    void listen( std::uint_fast8_t backlog ) noexcept
    {
        // #lizard forgives the length

        PICOLIBRARY_EXPECT( m_state == State::BOUND, Generic_Error::LOGIC_ERROR );

        auto & driver = m_network_stack->driver( {} );

        if ( backlog > m_sockets.size() ) {
            auto const socket_ids = m_network_stack->allocate_sockets(
                {}, backlog - m_sockets.size() );

            auto const sn_mr      = driver.read_sn_mr( m_sockets.front().id );
            auto const sn_port    = driver.read_sn_port( m_sockets.front().id );
            auto const sn_mssr    = driver.read_sn_mssr( m_sockets.front().id );
            auto const sn_ttl     = driver.read_sn_ttl( m_sockets.front().id );
            auto const sn_imr     = driver.read_sn_imr( m_sockets.front().id );
            auto const sn_kpalvtr = driver.read_sn_kpalvtr( m_sockets.front().id );

            for ( auto const socket_id : socket_ids ) {
                driver.write_sn_mr( socket_id, sn_mr );
                driver.write_sn_port( socket_id, sn_port );
                driver.write_sn_mssr( socket_id, sn_mssr );
                driver.write_sn_ttl( socket_id, sn_ttl );
                driver.write_sn_imr( socket_id, sn_imr );
                driver.write_sn_kpalvtr( socket_id, sn_kpalvtr );

                driver.write_sn_cr( socket_id, SN_CR::COMMAND_OPEN );
                while ( driver.read_sn_cr( socket_id ) ) {} // while

                while ( driver.read_sn_sr( socket_id ) != SN_SR::STATUS_SOCK_INIT ) {} // while

                m_sockets.push_back( BYPASS_PRECONDITION_EXPECTATION_CHECKS, { socket_id } );
            } // for
        }     // if

        for ( auto const socket : m_sockets ) {
            driver.write_sn_cr( socket.id, SN_CR::COMMAND_LISTEN );
            while ( driver.read_sn_cr( socket.id ) ) {} // while
        }                                               // for

        m_state = State::LISTENING;
    }

    /**
     * \brief Check if the socket is listening for incoming connection requests.
     *
     * \return true if the socket is listening for incoming connection requests.
     * \return false if the socket is not listening for incoming connection requests.
     */
    auto is_listening() const noexcept -> bool
    {
        return m_state == State::LISTENING;
    }

    /**
     * \brief Get the local endpoint on which the socket is listening for incoming
     *        connection requests.
     *
     * \return The local endpoint on which the socket is listening for incoming connection
     *         requests.
     */
    auto local_endpoint() const noexcept -> ::picolibrary::IP::TCP::Endpoint
    {
        auto const & driver = m_network_stack->driver( {} );

        return { { driver.read_sipr() }, driver.read_sn_port( m_sockets.front().id ) };
    }

    /**
     * \brief Accept an incoming connection request.
     *
     * \pre the socket is listening for incoming connection requests
     *
     * \return A server socket for handling the connection if accepting an incoming
     *         connection request succeeded.
     * \return picolibrary::Generic_Error::WOULD_BLOCK if an incoming connection request
     *         could not be accepted without blocking.
     */
    // NOLINTNEXTLINE(readability-function-size)
    auto accept() noexcept -> Result<Server>
    {
        // #lizard forgives the length

        PICOLIBRARY_EXPECT( m_state == State::LISTENING, Generic_Error::LOGIC_ERROR );

        auto & driver = m_network_stack->driver( {} );

        for ( auto n = std::uint_fast8_t{}; n < m_sockets.size(); ++n ) {
            auto & socket = m_sockets[ m_accept_i ];

            ++m_accept_i;
            if ( m_accept_i >= m_sockets.size() ) {
                m_accept_i = 0;
            } // if

            if ( socket.status == Socket::Status::AVAILABLE_FOR_ALLOCATION ) {
                switch ( driver.read_sn_sr( socket.id ) ) {
                    // NOLINTNEXTLINE(bugprone-branch-clone)
                    case SN_SR::STATUS_SOCK_CLOSED: [[fallthrough]];
                    // NOLINTNEXTLINE(bugprone-branch-clone)
                    case SN_SR::STATUS_SOCK_ESTABLISHED: [[fallthrough]];
                    case SN_SR::STATUS_SOCK_CLOSE_WAIT:
                        socket.status = Socket::Status::ALLOCATED;
                        return Server{ {}, *m_network_stack, *this, socket.id };
                } // switch
            }     // if
        }         // for

        return Generic_Error::WOULD_BLOCK;
    }

    /**
     * \brief Deallocate a socket.
     *
     * \param[in] socket_id The hardware socket ID for the socket to deallocate.
     *
     * \pre the socket has been allocated
     */
    // NOLINTNEXTLINE(readability-function-size)
    void deallocate_socket( Acceptor_Socket_Deallocation_Key, Socket_ID socket_id ) noexcept
    {
        // #lizard forgives the length

        for ( auto & socket : m_sockets ) {
            if ( socket_id == socket.id ) {
                PICOLIBRARY_EXPECT(
                    socket.status == Socket::Status::ALLOCATED, Generic_Error::LOGIC_ERROR );

                auto & driver = m_network_stack->driver( {} );

                driver.write_sn_cr( socket.id, SN_CR::COMMAND_CLOSE );
                while ( driver.read_sn_cr( socket.id ) ) {} // while

                while ( driver.read_sn_sr( socket.id ) != SN_SR::STATUS_SOCK_CLOSED ) {} // while

                driver.write_sn_cr( socket_id, SN_CR::COMMAND_OPEN );
                while ( driver.read_sn_cr( socket.id ) ) {} // while

                while ( driver.read_sn_sr( socket.id ) != SN_SR::STATUS_SOCK_INIT ) {} // while

                driver.write_sn_cr( socket.id, SN_CR::COMMAND_LISTEN );
                while ( driver.read_sn_cr( socket.id ) ) {} // while

                socket.status = Socket::Status::AVAILABLE_FOR_ALLOCATION;

                return;
            } // if
        }     // for

        PICOLIBRARY_EXPECTATION_NOT_MET( Generic_Error::LOGIC_ERROR );
    }

    /**
     * \brief Close the socket.
     */
    // NOLINTNEXTLINE(readability-function-size)
    constexpr void close() noexcept
    {
        // #lizard forgives the length

        if ( m_state == State::UNINITIALIZED ) {
            return;
        } // if

        if ( m_state != State::INITIALIZED ) {
            auto deallocate_port = true;
            for ( auto const socket : m_sockets ) {
                if ( socket.status == Socket::Status::ALLOCATED ) {
                    deallocate_port = false;
                } // if
            }     // for

            if ( deallocate_port ) {
                m_network_stack->tcp_port_allocator( {} ).deallocate(
                    m_network_stack->driver( {} ).read_sn_port( m_sockets.front().id ) );
            } // if
        }     // if

        for ( auto const socket : m_sockets ) {
            if ( socket.status == Socket::Status::ALLOCATED ) {
                m_network_stack->detach_tcp_server( {}, socket.id );
            } else {
                m_network_stack->deallocate_socket( {}, socket.id );
            } // else
        }     // for

        m_state = State::UNINITIALIZED;
    }

  private:
    /**
     * \brief Socket information.
     */
    struct Socket {
        /**
         * \brief Socket status.
         */
        enum class Status : std::uint_fast8_t {
            AVAILABLE_FOR_ALLOCATION, ///< Available for allocation.
            ALLOCATED,                ///< Allocated.
        };

        /**
         * \brief The socket's hardware socket ID.
         */
        Socket_ID id{};

        /**
         * \brief The socket's status.
         */
        Status status{};
    };

    /**
     * \brief The socket's state.
     */
    State m_state{};

    /**
     * \brief The network stack the socket is associated with.
     */
    Network_Stack * m_network_stack{};

    /**
     * \brief The socket's socket information.
     */
    Fixed_Capacity_Vector<Socket, SOCKETS> m_sockets{};

    /**
     * \brief Connection request acceptance socket information index.
     */
    std::uint_fast8_t m_accept_i{};
};

} // namespace picolibrary::WIZnet::W5500::IP::TCP

#endif // PICOLIBRARY_WIZNET_W5500_IP_TCP_H

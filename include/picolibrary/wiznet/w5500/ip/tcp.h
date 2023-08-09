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

#include "picolibrary/error.h"
#include "picolibrary/ip.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/precondition.h"
#include "picolibrary/result.h"
#include "picolibrary/utility.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/ip/network_stack_keys.h"

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
     * \param[in] socket_id The socket's socket ID.
     */
    constexpr Client( Network_Stack_Socket_Construction_Key, Network_Stack & network_stack, Socket_ID socket_id ) noexcept
        :
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
     * \param[in] socket_id The socket's socket ID.
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
     *        value.
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
     * \brief The socket's socket ID.
     */
    Socket_ID m_socket_id{};

    /**
     * \brief The socket's data transmission in progress flag.
     */
    bool m_is_transmitting{};
};

} // namespace picolibrary::WIZnet::W5500::IP::TCP

#endif // PICOLIBRARY_WIZNET_W5500_IP_TCP_H

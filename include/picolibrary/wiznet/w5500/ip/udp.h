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
 * \brief picolibrary::WIZnet::W5500::IP::UDP interface.
 */

#ifndef PICOLIBRARY_WIZNET_W5500_IP_UDP_H
#define PICOLIBRARY_WIZNET_W5500_IP_UDP_H

#include <cstdint>
#include <limits>

#include "picolibrary/array.h"
#include "picolibrary/error.h"
#include "picolibrary/ip.h"
#include "picolibrary/ip/udp.h"
#include "picolibrary/precondition.h"
#include "picolibrary/result.h"
#include "picolibrary/utility.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/keys.h"

/**
 * \brief WIZnet W5500 UDP over IP facilities.
 */
namespace picolibrary::WIZnet::W5500::IP::UDP {

/**
 * \brief Socket.
 *
 * \tparam Network_Stack The type of network stack the socket is associated with.
 */
template<typename Network_Stack>
class Socket {
  public:
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
    constexpr Socket() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] network_stack The network stack the socket is associated with.
     * \param[in] socket_id The socket's hardware socket id.
     */
    constexpr Socket( Socket_Construction_Key, Network_Stack & network_stack, Socket_ID socket_id ) noexcept :
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
     * \param[in] socket_id The socket's hardware socket id.
     * \param[in] state The socket's initial state.
     * \param[in] is_transmitting The socket's initial datagram transmission in progress
     *            status.
     */
    constexpr Socket(
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
    constexpr Socket( Socket && source ) noexcept :
        m_state{ source.m_state },
        m_network_stack{ source.m_network_stack },
        m_socket_id{ source.m_socket_id },
        m_is_transmitting{ source.m_is_transmitting }
    {
        source.m_state         = State::UNINITIALIZED;
        source.m_network_stack = nullptr;
    }

    Socket( Socket const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Socket() noexcept
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
    constexpr auto operator=( Socket && expression ) noexcept -> Socket &
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

    auto operator=( Socket const & ) = delete;

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
     * \brief Configure socket broadcast blocking (defaults to disabled).
     *
     * \pre picolibrary::WIZnet::W5500::IP::UDP::Socket::state() ==
     *      picolibrary::WIZnet::W5500::IP::UDP::Socket::State::INITIALIZED
     *
     * \param[in] broadcast_blocking_configuration The desired broadcast blocking
     *            configuration.
     */
    void configure_broadcast_blocking( Broadcast_Blocking broadcast_blocking_configuration ) noexcept
    {
        PICOLIBRARY_EXPECT( m_state == State::INITIALIZED, Generic_Error::LOGIC_ERROR );

        auto & driver = m_network_stack->driver( {} );

        driver.write_sn_mr(
            m_socket_id,
            ( driver.read_sn_mr( m_socket_id ) & ~SN_MR::Mask::BCASTB )
                | to_underlying( broadcast_blocking_configuration ) );
    }

    /**
     * \brief Get the socket broadcast blocking configuration.
     *
     * \return The socket broadcast blocking configuration.
     */
    auto broadcast_blocking_configuration() const noexcept -> Broadcast_Blocking
    {
        return static_cast<Broadcast_Blocking>(
            m_network_stack->driver( {} ).read_sn_mr( m_socket_id ) & SN_MR::Mask::BCASTB );
    }

    /**
     * \brief Configure socket unicast blocking (defaults to disabled).
     *
     * \pre picolibrary::WIZnet::W5500::IP::UDP::Socket::state() ==
     *      picolibrary::WIZnet::W5500::IP::UDP::Socket::State::INITIALIZED
     *
     * \param[in] unicast_blocking_configuration The desired unicast blocking
     *            configuration.
     */
    void configure_unicast_blocking( Unicast_Blocking unicast_blocking_configuration ) noexcept
    {
        PICOLIBRARY_EXPECT( m_state == State::INITIALIZED, Generic_Error::LOGIC_ERROR );

        auto & driver = m_network_stack->driver( {} );

        driver.write_sn_mr(
            m_socket_id,
            ( driver.read_sn_mr( m_socket_id ) & ~SN_MR::Mask::UCASTB )
                | to_underlying( unicast_blocking_configuration ) );
    }

    /**
     * \brief Get the socket unicast blocking configuration.
     *
     * \return The socket unicast blocking configuration.
     */
    auto unicast_blocking_configuration() const noexcept -> Unicast_Blocking
    {
        return static_cast<Unicast_Blocking>(
            m_network_stack->driver( {} ).read_sn_mr( m_socket_id ) & SN_MR::Mask::UCASTB );
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

        if ( mask & ( Socket_Interrupt::DATA_TRANSMITTED | Socket_Interrupt::ARP_TCP_TIMEOUT ) ) {
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
     * \pre if an ephemeral port is requested, and ephemeral port is available
     */
    void bind( ::picolibrary::IP::UDP::Endpoint const & endpoint = ::picolibrary::IP::UDP::Endpoint{} ) noexcept
    {
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
            m_network_stack->udp_port_allocator( {} ).allocate( driver, endpoint.port() ).as_unsigned_integer() );

        driver.write_sn_mr(
            m_socket_id, ( driver.read_sn_mr( m_socket_id ) & ~SN_MR::Mask::P ) | SN_MR::P_UDP );

        driver.write_sn_cr( m_socket_id, SN_CR::COMMAND_OPEN );
        while ( driver.read_sn_cr( m_socket_id ) ) {} // while

        while ( driver.read_sn_sr( m_socket_id ) != SN_SR::STATUS_SOCK_UDP ) {} // while

        m_state = State::BOUND;
    }

    /**
     * \brief Get the socket's local endpoint.
     *
     * \return The socket's local endpoint.
     */
    auto local_endpoint() const noexcept -> ::picolibrary::IP::UDP::Endpoint
    {
        auto const & driver = m_network_stack->driver( {} );

        return { { driver.read_sipr() }, driver.read_sn_port( m_socket_id ) };
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
     * \brief Transmit a datagram to a remote endpoint.
     *
     * \pre the socket has been bound to a local endpoint
     * \pre the W5500 is responsive
     *
     * \param[in] endpoint The endpoint to transmit the datagram to.
     * \param[in] begin The beginning of the block of data to transmit in the datagram.
     * \param[in] end The end of the block of data to transmit in the datagram.
     *
     * \pre endpoint is a valid remote endpoint
     *
     * \return Nothing if transmitting the datagram succeeded.
     * \return picolibrary::Generic_Error::EXCESSIVE_MESSAGE_SIZE if the block of data is
     *         too large to transmit in a single datagram.
     * \return picolibrary::Generic_Error::WOULD_BLOCK if the datagram cannot be
     *         transmitted without blocking.
     */
    auto transmit(
        ::picolibrary::IP::UDP::Endpoint const & endpoint,
        std::uint8_t const *                     begin,
        std::uint8_t const *                     end ) noexcept -> Result<void>
    {
        PICOLIBRARY_EXPECT(
            endpoint.address().version() == ::picolibrary::IP::Version::_4
                and not endpoint.address().ipv4().is_any() and not endpoint.port().is_any(),
            Generic_Error::INVALID_ARGUMENT );

        auto const buffer_size = static_cast<std::uint16_t>(
            to_underlying( m_network_stack->socket_buffer_size() ) * 1024 );
        auto const data_size = static_cast<std::uintptr_t>( end - begin );

        if ( data_size > buffer_size ) {
            return Generic_Error::EXCESSIVE_MESSAGE_SIZE;
        } // if

        auto & driver = m_network_stack->driver( {} );

        if ( m_is_transmitting ) {
            if ( not( driver.read_sn_ir( m_socket_id )
                      & ( Socket_Interrupt::DATA_TRANSMITTED | Socket_Interrupt::ARP_TCP_TIMEOUT ) ) ) {
                return Generic_Error::WOULD_BLOCK;
            } // if

            driver.write_sn_ir(
                m_socket_id, Socket_Interrupt::DATA_TRANSMITTED | Socket_Interrupt::ARP_TCP_TIMEOUT );

            m_is_transmitting = false;
        } // if

        auto const sn_tx_fsr = driver.read_sn_tx_fsr( m_socket_id );

        PICOLIBRARY_EXPECT( sn_tx_fsr <= buffer_size, m_network_stack->nonresponsive_device_error() );

        if ( data_size > sn_tx_fsr ) {
            return Generic_Error::WOULD_BLOCK;
        } // if

        driver.write_sn_dipr( m_socket_id, endpoint.address().ipv4().as_byte_array() );
        driver.write_sn_dport( m_socket_id, endpoint.port().as_unsigned_integer() );

        auto const sn_tx_wr = driver.read_sn_tx_wr( m_socket_id );
        driver.write_tx_buffer( m_socket_id, sn_tx_wr, begin, end );
        driver.write_sn_tx_wr( m_socket_id, sn_tx_wr + data_size );

        driver.write_sn_cr( m_socket_id, SN_CR::COMMAND_SEND );
        while ( driver.read_sn_cr( m_socket_id ) ) {} // while

        m_is_transmitting = true;

        return {};
    }

    /**
     * \brief Receive a datagram from a remote endpoint.
     *
     * \pre the socket has been bound to a local endpoint
     * \pre the W5500 is responsive
     *
     * \param[out] begin The beginning of the block of data read from the datagram.
     * \param[out] end The end of the block of data read from the datagram.
     *
     * \return The endpoint the datagram was received from and the end of the data read
     *         from the datagram if receiving a datagram succeeded.
     * \return picolibrary::Generic_Error::WOULD_BLOCK if a datagram could not be received
     *         without blocking.
     */
    auto receive( std::uint8_t * begin, std::uint8_t * end ) noexcept
        -> Result<::picolibrary::IP::UDP::Reception_Result>
    {
        auto & driver = m_network_stack->driver( {} );

        auto const buffer_size = static_cast<std::uint16_t>(
            to_underlying( m_network_stack->socket_buffer_size() ) * 1024 );
        auto const sn_rx_rsr = driver.read_sn_rx_rsr( m_socket_id );

        PICOLIBRARY_EXPECT( sn_rx_rsr <= buffer_size, m_network_stack->nonresponsive_device_error() );

        if ( sn_rx_rsr == 0 ) {
            return Generic_Error::WOULD_BLOCK;
        } // if

        Datagram_Info datagram_info;

        PICOLIBRARY_EXPECT(
            sn_rx_rsr >= datagram_info.size(), m_network_stack->nonresponsive_device_error() );

        auto const sn_rx_rd = driver.read_sn_rx_rd( m_socket_id );

        driver.read_rx_buffer( m_socket_id, sn_rx_rd, datagram_info.begin(), datagram_info.end() );

        auto const source_address = get_datagram_source_address( datagram_info );
        auto const source_port    = get_datagram_source_port( datagram_info );
        auto const data_size      = get_datagram_data_size( datagram_info );

        PICOLIBRARY_EXPECT(
            data_size <= ( sn_rx_rsr - datagram_info.size() ),
            m_network_stack->nonresponsive_device_error() );

        if ( static_cast<std::uintptr_t>( end - begin ) > data_size ) {
            end = begin + data_size;
        } // if

        driver.read_rx_buffer( m_socket_id, sn_rx_rd + datagram_info.size(), begin, end );
        driver.write_sn_rx_rd( m_socket_id, sn_rx_rd + datagram_info.size() + data_size );

        driver.write_sn_cr( m_socket_id, SN_CR::COMMAND_RECV );
        while ( driver.read_sn_cr( m_socket_id ) ) {} // while

        return ::picolibrary::IP::UDP::Reception_Result{ { source_address, source_port }, end };
    }

    /**
     * \brief Close the socket.
     */
    constexpr void close() noexcept
    {
        if ( m_state == State::UNINITIALIZED ) {
            return;
        } // if

        if ( m_state == State::BOUND ) {
            m_network_stack->udp_port_allocator( {} ).deallocate(
                m_network_stack->driver( {} ).read_sn_port( m_socket_id ) );
        } // if

        m_network_stack->deallocate_socket( {}, m_socket_id );

        m_state = State::UNINITIALIZED;
    }

  private:
    /**
     * \brief Datagram info.
     */
    using Datagram_Info = Array<std::uint8_t, 8>;

    /**
     * \brief The socket's state.
     */
    State m_state{};

    /**
     * \brief The network stack the socket is associated with.
     */
    Network_Stack * m_network_stack{};

    /**
     * \brief The socket's hardware socket id.
     */
    Socket_ID m_socket_id{};

    /**
     * \brief The socket's datagram transmission in progress flag.
     */
    bool m_is_transmitting{};

    /**
     * \brief Get a received datagram's source address.
     *
     * \param[in] datagram_info The datagram info for the received datagram whose source
     *            address is to be gotten.
     *
     * \return The received datagram's source address.
     */
    static auto get_datagram_source_address( Datagram_Info const & datagram_info ) noexcept
        -> IPv4::Address
    {
        return { { datagram_info[ 0 ], datagram_info[ 1 ], datagram_info[ 2 ], datagram_info[ 3 ] } };
    }

    /**
     * \brief Get a received datagram's source port.
     *
     * \param[in] datagram_info The datagram info for the received datagram whose source
     *            port is to be gotten.
     *
     * \return The received datagram's source port.
     */
    static auto get_datagram_source_port( Datagram_Info const & datagram_info ) noexcept
        -> ::picolibrary::IP::UDP::Port
    {
        return static_cast<std::uint16_t>(
            ( datagram_info[ 4 ] << std::numeric_limits<std::uint8_t>::digits ) | datagram_info[ 5 ] );
    }

    /**
     * \brief Get a received datagram's data size.
     *
     * \param[in] datagram_info The datagram info for the received datagram whose data
     *            size is to be gotten.
     *
     * \return The received datagram's data size.
     */
    static auto get_datagram_data_size( Datagram_Info const & datagram_info ) noexcept -> std::uint16_t
    {
        return static_cast<std::uint16_t>(
            ( datagram_info[ 6 ] << std::numeric_limits<std::uint8_t>::digits ) | datagram_info[ 7 ] );
    }
};

} // namespace picolibrary::WIZnet::W5500::IP::UDP

#endif // PICOLIBRARY_WIZNET_W5500_IP_UDP_H

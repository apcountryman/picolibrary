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
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack interface.
 */

#ifndef PICOLIBRARY_WIZNET_W5500_IP_NETWORK_STACK_H
#define PICOLIBRARY_WIZNET_W5500_IP_NETWORK_STACK_H

#include <cstdint>
#include <utility>

#include "picolibrary/array.h"
#include "picolibrary/error.h"
#include "picolibrary/fixed_capacity_vector.h"
#include "picolibrary/ip.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/mac_address.h"
#include "picolibrary/precondition.h"
#include "picolibrary/utility.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/ip/network_stack_keys.h"
#include "picolibrary/wiznet/w5500/ip/tcp.h"

namespace picolibrary::WIZnet::W5500::IP {

/**
 * \brief Network stack.
 *
 * \tparam Controller The type of controller used to communicate with the W5500.
 * \tparam Device_Selector The type of device selector used to select and deselect the
 *         W5500.
 * \tparam TCP_Port_Allocator The type of TCP over IP port allocator to use.
 * \tparam UDP_Port_Allocator The type of UDP over IP port allocator to use.
 * \tparam Driver The type of driver implementation used by the network stack. The default
 *         driver implementation should be used unless a mock driver implementation is
 *         being injected to support automated testing of this network stack.
 */
template<typename Controller, typename Device_Selector, typename TCP_Port_Allocator, typename UDP_Port_Allocator, typename Driver = ::picolibrary::WIZnet::W5500::Driver<Controller, Device_Selector>>
class Network_Stack {
  public:
    /**
     * \brief The type of TCP client socket that is used to interact with the network
     *        stack.
     */
    using TCP_Client = TCP::Client<Network_Stack>;

    /**
     * \brief Constructor.
     */
    constexpr Network_Stack() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] controller The controller used to communicate with the W5500.
     * \param[in] configuration The controller clock and data exchange bit order
     *            configuration that meets the W5500's communication requirements.
     * \param[in] device_selector The device selector used to select and deselect the
     *            W5500.
     * \param[in] nonresponsive_device_error The fatal error that occurs if an operation
     *            fails due to the W5500 being nonresponsive.
     * \param[in] tcp_port_allocator The TCP over IP port allocator to use.
     * \param[in] udp_port_allocator The UDP over IP port allocator to use.
     */
    constexpr Network_Stack(
        Controller &                               controller,
        typename Controller::Configuration const & configuration,
        Device_Selector                            device_selector,
        Error_Code const &                         nonresponsive_device_error,
        TCP_Port_Allocator                         tcp_port_allocator,
        UDP_Port_Allocator                         udp_port_allocator ) noexcept :
        m_driver{ controller, configuration, std::move( device_selector ) },
        m_nonresponsive_device_error{ nonresponsive_device_error },
        m_tcp_port_allocator{ std::move( tcp_port_allocator ) },
        m_udp_port_allocator{ std::move( udp_port_allocator ) }
    {
        // #lizard forgives the parameter count
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Network_Stack( Network_Stack && source ) noexcept :
        m_driver{ std::move( source.m_driver ) },
        m_nonresponsive_device_error{ std::move( source.m_nonresponsive_device_error ) },
        m_tcp_port_allocator{ std::move( source.m_tcp_port_allocator ) },
        m_udp_port_allocator{ std::move( source.m_udp_port_allocator ) },
        m_socket_buffer_size{ source.m_socket_buffer_size },
        m_sockets{ source.m_sockets },
        m_sockets_available_for_allocation{ source.m_sockets_available_for_allocation },
        m_socket_status{ std::move( source.m_socket_status ) },
        m_tcp_server_is_detached{ std::move( source.m_tcp_server_is_detached ) }
    {
        source.m_socket_buffer_size               = Socket_Buffer_Size::_0_KiB;
        source.m_sockets                          = 0;
        source.m_sockets_available_for_allocation = 0;
        source.m_socket_status                    = {};
        source.m_tcp_server_is_detached           = {};
    }

    Network_Stack( Network_Stack const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Network_Stack() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Network_Stack && expression ) noexcept -> Network_Stack &
    {
        if ( &expression != this ) {
            m_driver                     = std::move( expression.m_driver );
            m_nonresponsive_device_error = std::move( expression.nonresponsive_device_error );
            m_tcp_port_allocator         = std::move( expression.m_tcp_port_allocator );
            m_udp_port_allocator         = std::move( expression.m_udp_port_allocator );
            m_socket_buffer_size         = expression.m_socket_buffer_size;
            m_sockets                    = expression.m_sockets;
            m_sockets_available_for_allocation = expression.m_sockets_available_for_allocation;
            m_socket_status                    = std::move( expression.m_socket_status );
            m_tcp_server_is_detached = std::move( expression.m_tcp_server_is_detached );

            expression.m_socket_buffer_size               = Socket_Buffer_Size::_0_KiB;
            expression.m_sockets                          = 0;
            expression.m_sockets_available_for_allocation = 0;
            expression.m_socket_status                    = {};
            expression.m_tcp_server_is_detached           = {};
        } // if

        return *this;
    }

    auto operator=( Network_Stack const & ) = delete;

    /**
     * \brief Initialize the network stack.
     *
     * \pre the network stack has not already been initialized.
     *
     * \param[in] phy_mode The desired PHY mode.
     * \param[in] ping_blocking_configuration The desired ping blocking configuration.
     * \param[in] arp_forcing_configuration The desired ARP forcing configuration.
     * \param[in] retransmission_retry_time The desired retransmission retry time (RTR
     *            register value).
     * \param[in] retransmission_retry_count The desired retransmission retry count (RCR)
     *            register value).
     * \param[in] mac_address The desired MAC address.
     * \param[in] ipv4_address The desired IPv4 address.
     * \param[in] ipv4_gateway_address The desired IPv4 gateway address.
     * \param[in[ ipv4_subnet_mask The desired IPv4 subnet mask.
     * \param[in] interrupt_assert_wait_time The desired interrupt assert wait time
     *            (INTLEVEL register value).
     * \param[in] socket_buffer_size The desired socket buffer size.
     *
     * \pre phy_mode != picolibrary::WIZnet::W5500::PHY_mode::POWER_DOWN
     * \pre socket_buffer_size is 2 KiB, 4 KiB, 8 KiB, or 16 KiB
     */
    // NOLINTNEXTLINE(readability-function-size)
    void initialize(
        PHY_Mode              phy_mode,
        Ping_Blocking         ping_blocking_configuration,
        ARP_Forcing           arp_forcing_configuration,
        std::uint16_t         retransmission_retry_time,
        std::uint8_t          retransmission_retry_count,
        MAC_Address const &   mac_address,
        IPv4::Address const & ipv4_address,
        IPv4::Address const & ipv4_gateway_address,
        IPv4::Address const & ipv4_subnet_mask,
        std::uint16_t         interrupt_assert_wait_time,
        Socket_Buffer_Size    socket_buffer_size ) noexcept
    {
        // #lizard forgives the length

        PICOLIBRARY_EXPECT( phy_mode != PHY_Mode::POWER_DOWN, Generic_Error::INVALID_ARGUMENT );
        PICOLIBRARY_EXPECT( m_sockets == 0, Generic_Error::LOGIC_ERROR );

        auto sockets = std::uint_fast8_t{ 0 };

        switch ( socket_buffer_size ) {
                // clang-format off

            case Socket_Buffer_Size::_2_KiB:  sockets = 16 /  2; break;
            case Socket_Buffer_Size::_4_KiB:  sockets = 16 /  4; break;
            case Socket_Buffer_Size::_8_KiB:  sockets = 16 /  8; break;
            case Socket_Buffer_Size::_16_KiB: sockets = 16 / 16; break;

                // clang-format on

            default: PICOLIBRARY_EXPECTATION_NOT_MET( Generic_Error::INVALID_ARGUMENT );
        } // switch

        m_driver.initialize();

        m_driver.write_phycfgr( to_underlying( phy_mode ) | PHYCFGR::Mask::RST );
        m_driver.write_phycfgr( to_underlying( phy_mode ) );
        m_driver.write_phycfgr( to_underlying( phy_mode ) | PHYCFGR::Mask::RST );

        m_driver.write_mr( to_underlying( ping_blocking_configuration ) | to_underlying( arp_forcing_configuration ) );

        m_driver.write_rtr( retransmission_retry_time );
        m_driver.write_rcr( retransmission_retry_count );

        m_driver.write_shar( mac_address.as_byte_array() );

        m_driver.write_sipr( ipv4_address.as_byte_array() );
        m_driver.write_gar( ipv4_gateway_address.as_byte_array() );
        m_driver.write_subr( ipv4_subnet_mask.as_byte_array() );

        m_driver.write_intlevel( interrupt_assert_wait_time );

        for ( auto socket = std::uint_fast8_t{ 0 }; socket < sockets; ++socket ) {
            auto const socket_id = static_cast<Socket_ID>( socket << Control_Byte::Bit::SOCKET );

            m_driver.write_sn_rxbuf_size( socket_id, to_underlying( socket_buffer_size ) );
            m_driver.write_sn_txbuf_size( socket_id, to_underlying( socket_buffer_size ) );

            m_socket_status[ socket ] = Socket_Status::AVAILABLE_FOR_ALLOCATION;
        } // for

        for ( auto socket = sockets; socket < SOCKETS; ++socket ) {
            auto const socket_id = static_cast<Socket_ID>( socket << Control_Byte::Bit::SOCKET );

            m_driver.write_sn_rxbuf_size( socket_id, to_underlying( Socket_Buffer_Size::_0_KiB ) );
            m_driver.write_sn_txbuf_size( socket_id, to_underlying( Socket_Buffer_Size::_0_KiB ) );
        } // for

        m_socket_buffer_size               = socket_buffer_size;
        m_sockets                          = sockets;
        m_sockets_available_for_allocation = sockets;
    }

#ifdef PICOLIBRARY_ENABLE_AUTOMATED_TESTING
    /**
     * \brief Access the driver used to interact with the W5500.
     *
     * \return The driver used to interact with the W5500.
     */
    constexpr auto driver() noexcept -> Driver &
    {
        return m_driver;
    }
#endif // PICOLIBRARY_ENABLE_AUTOMATED_TESTING

#ifdef PICOLIBRARY_ENABLE_AUTOMATED_TESTING
    /**
     * \brief Access the driver used to interact with the W5500.
     *
     * \return The driver used to interact with the W5500.
     */
    constexpr auto driver() const noexcept -> Driver const &
    {
        return m_driver;
    }
#endif // PICOLIBRARY_ENABLE_AUTOMATED_TESTING

    /**
     * \brief Access the driver used to interact with the W5500.
     *
     * \return The driver used to interact with the W5500.
     */
    constexpr auto driver( Network_Stack_Driver_Access_Key ) noexcept -> Driver &
    {
        return m_driver;
    }

    /**
     * \brief Get the fatal error that occurs if an operation fails due to the W5500 being
     *        nonresponsive.
     *
     * \return The fatal error that occurs if an operation fails due to the W5500 begin
     *         nonresponsive.
     */
    constexpr auto nonresponsive_device_error() const noexcept -> Error_Code const &
    {
        return m_nonresponsive_device_error;
    }

    /**
     * \brief Check if the W5500 is responsive.
     *
     * \return true if the W5500 is responsive.
     * \return false if the W5500 is not responsive.
     */
    auto w5500_is_responsive() const noexcept -> bool
    {
        return m_driver.read_versionr() == VERSIONR::VERSION;
    }

    /**
     * \brief Get the PHY mode.
     *
     * \return The PHY mode.
     */
    auto phy_mode() const noexcept -> PHY_Mode
    {
        return static_cast<PHY_Mode>(
            m_driver.read_phycfgr() & ( PHYCFGR::Mask::OPMD | PHYCFGR::Mask::OPMDC ) );
    }

    /**
     * \brief Get the link status.
     *
     * \return The link status.
     */
    auto link_status() const noexcept -> Link_Status
    {
        return static_cast<Link_Status>( m_driver.read_phycfgr() & PHYCFGR::Mask::LNK );
    }

    /**
     * \brief Get the link mode.
     *
     * \return The link mode.
     */
    auto link_mode() const noexcept -> Link_Mode
    {
        return static_cast<Link_Mode>( m_driver.read_phycfgr() & PHYCFGR::Mask::DPX );
    }

    /**
     * \brief Get the link speed.
     *
     * \return The link speed.
     */
    auto link_speed() const noexcept -> Link_Speed
    {
        return static_cast<Link_Speed>( m_driver.read_phycfgr() & PHYCFGR::Mask::SPD );
    }

    /**
     * \brief Configure ping blocking.
     *
     * \param[in] ping_blocking_configuration The desired ping blocking configuration.
     */
    void configure_ping_blocking( Ping_Blocking ping_blocking_configuration ) noexcept
    {
        m_driver.write_mr( ( m_driver.read_mr() & ~MR::Mask::PB ) | to_underlying( ping_blocking_configuration ) );
    }

    /**
     * \brief Get the ping blocking configuration.
     *
     * \return The ping blocking configuration.
     */
    auto ping_blocking_configuration() const noexcept -> Ping_Blocking
    {
        return static_cast<Ping_Blocking>( m_driver.read_mr() & MR::Mask::PB );
    }

    /**
     * \brief Get the ARP forcing configuration.
     *
     * \return The ARP forcing configuration.
     */
    auto arp_forcing_configuration() const noexcept -> ARP_Forcing
    {
        return static_cast<ARP_Forcing>( m_driver.read_mr() & MR::Mask::FARP );
    }

    /**
     * \brief Get the retransmission retry time (RTR register value).
     *
     * \return The retransmission retry time.
     */
    auto retransmission_retry_time() const noexcept -> std::uint16_t
    {
        return m_driver.read_rtr();
    }

    /**
     * \brief Get the retransmission retry count (RCR register value).
     *
     * \return The retransmission retry count.
     */
    auto retransmission_retry_count() const noexcept -> std::uint8_t
    {
        return m_driver.read_rcr();
    }

    /**
     * \brief Get the MAC address.
     *
     * \return The MAC address.
     */
    auto mac_address() const noexcept -> MAC_Address
    {
        return m_driver.read_shar();
    }

    /**
     * \brief Get the IPv4 address.
     *
     * \return The IPv4 address.
     */
    auto ipv4_address() const noexcept -> IPv4::Address
    {
        return m_driver.read_sipr();
    }

    /**
     * \brief Get the IPv4 gateway address.
     *
     * \return The IPv4 gateway address.
     */
    auto ipv4_gateway_address() const noexcept -> IPv4::Address
    {
        return m_driver.read_gar();
    }

    /**
     * \brief Get the IPv4 subnet mask.
     *
     * \return The IPv4 subnet mask.
     */
    auto ipv4_subnet_mask() const noexcept -> IPv4::Address
    {
        return m_driver.read_subr();
    }

    /**
     * \brief Get the interrupt assert wait time (INTLEVEL register value).
     *
     * \return The interrupt assert wait time.
     */
    auto interrupt_assert_wait_time() const noexcept -> std::uint16_t
    {
        return m_driver.read_intlevel();
    }

    /**
     * \brief Enable interrupts.
     *
     * \param[in] mask The mask identifying the interrupts to enable.
     */
    void enable_interrupts( std::uint8_t mask ) noexcept
    {
        m_driver.write_imr( m_driver.read_imr() | mask );
    }

    /**
     * \brief Disable interrupts.
     *
     * \param[in] mask The mask identifying the interrupts to disable.
     */
    void disable_interrupts( std::uint8_t mask ) noexcept
    {
        m_driver.write_imr( m_driver.read_imr() & ~mask );
    }

    /**
     * \brief Disable all interrupts.
     */
    void disable_interrupts() noexcept
    {
        m_driver.write_imr( 0x00 );
    }

    /**
     * \brief Get a mask identifying the interrupts that are enabled.
     *
     * \return A mask identifying the interrupts that are enabled.
     */
    auto enabled_interrupts() const noexcept -> std::uint8_t
    {
        return m_driver.read_imr();
    }

    /**
     * \brief Get the interrupt context (IR register value).
     *
     * \return The interrupt context.
     */
    auto interrupt_context() const noexcept -> std::uint8_t
    {
        return m_driver.read_ir();
    }

    /**
     * \brief Clear interrupts.
     *
     * \param[in] mask The mask identifying the interrupts to clear.
     */
    void clear_interrupts( std::uint8_t mask ) noexcept
    {
        m_driver.write_ir( mask );
    }

    /**
     * \brief Enable socket interrupts.
     */
    void enable_socket_interrupts() noexcept
    {
        m_driver.write_simr( 0xFF );
    }

    /**
     * \brief Disable socket interrupts.
     */
    void disable_socket_interrupts() noexcept
    {
        m_driver.write_simr( 0x00 );
    }

    /**
     * \brief Check if socket interrupts are enabled.
     *
     * \return true if socket interrupts are enabled.
     * \return false if socket interrupts are not enabled.
     */
    auto socket_interrupts_are_enabled() const noexcept -> bool
    {
        return m_driver.read_simr();
    }

    /**
     * \brief Get the socket interrupt context (SIR register value).
     *
     * \return The socket interrupt context.
     */
    auto socket_interrupt_context() const noexcept -> std::uint8_t
    {
        return m_driver.read_sir();
    }

    /**
     * \brief Get the endpoint that has been reported to be unreachable.
     *
     * \return The endpoint that has been reported to be unreachable.
     */
    auto unreachable_endpoint() const noexcept -> ::picolibrary::IP::Endpoint
    {
        return { IPv4::Address{ m_driver.read_uipr() }, m_driver.read_uportr() };
    }

    /**
     * \brief Get the socket buffer size.
     *
     * \return The socket buffer size.
     */
    constexpr auto socket_buffer_size() const noexcept -> Socket_Buffer_Size
    {
        return m_socket_buffer_size;
    }

    /**
     * \brief Get the number of sockets the network stack is configured to support.
     *
     * \return The number of sockets the network stack is configured to support.
     */
    constexpr auto sockets() const noexcept -> std::uint_fast8_t
    {
        return m_sockets;
    }

    /**
     * \brief Get the number of sockets that are available for allocation.
     *
     * \return The number of sockets that are available for allocation.
     */
    constexpr auto sockets_available_for_allocation() const noexcept -> std::uint_fast8_t
    {
        return m_sockets_available_for_allocation;
    }

    /**
     * \brief Allocate sockets.
     *
     * \param[in] n The number of sockets to allocate.
     *
     * \pre at least n sockets are available
     *
     * \return The socket IDs for the allocated sockets.
     */
    auto allocate_sockets( Network_Stack_Socket_Allocation_Key, std::uint_fast8_t n ) noexcept
        -> Fixed_Capacity_Vector<Socket_ID, SOCKETS>
    {
        PICOLIBRARY_EXPECT( n <= m_sockets_available_for_allocation, Generic_Error::INSUFFICIENT_SOCKETS_AVAILABLE );

        auto socket_ids = Fixed_Capacity_Vector<Socket_ID, SOCKETS>{};

        for ( auto socket = std::uint_fast8_t{}; socket_ids.size() < n; ++socket ) {
            if ( m_socket_status[ socket ] == Socket_Status::AVAILABLE_FOR_ALLOCATION ) {
                m_socket_status[ socket ] = Socket_Status::ALLOCATED;

                socket_ids.push_back(
                    BYPASS_PRECONDITION_EXPECTATION_CHECKS,
                    static_cast<Socket_ID>( socket << Control_Byte::Bit::SOCKET ) );
            } // if
        }     // for

        m_sockets_available_for_allocation -= n;

        return socket_ids;
    }

    /**
     * \brief Deallocate a socket.
     *
     * \param[in] socket_id The socket ID for the socket to deallocate.
     *
     * \pre the socket has been allocated
     */
    // NOLINTNEXTLINE(readability-function-size)
    void deallocate_socket( Network_Stack_Socket_Allocation_Key, Socket_ID socket_id ) noexcept
    {
        // #lizard forgives the length

        auto const socket = static_cast<std::uint_fast8_t>(
            to_underlying( socket_id ) >> Control_Byte::Bit::SOCKET );

        PICOLIBRARY_EXPECT(
            m_socket_status[ socket ] == Socket_Status::ALLOCATED, Generic_Error::LOGIC_ERROR );

        m_driver->write_sn_cr( socket_id, SN_CR::COMMAND_CLOSE );
        while ( m_driver->read_sn_cr( socket_id ) ) {} // while

        while ( m_driver->read_sn_sr( socket_id ) != SN_SR::STATUS_SOCK_CLOSED ) {} // while

        m_driver->write_sn_mr( socket_id, SN_MR::RESET );
        m_driver->write_sn_ir( socket_id, Socket_Interrupt::ALL );
        m_driver->write_sn_port( socket_id, SN_PORT::RESET );
        m_driver->write_sn_dhar( socket_id, SN_DHAR::RESET );
        m_driver->write_sn_dipr( socket_id, SN_DIPR::RESET );
        m_driver->write_sn_dport( socket_id, SN_DPORT::RESET );
        m_driver->write_sn_mssr( socket_id, SN_MSSR::RESET );
        m_driver->write_sn_tos( socket_id, SN_TOS::RESET );
        m_driver->write_sn_ttl( socket_id, SN_TTL::RESET );
        m_driver->write_sn_imr( socket_id, SN_IMR::RESET );
        m_driver->write_sn_frag( socket_id, SN_FRAG::RESET );
        m_driver->write_sn_kpalvtr( socket_id, SN_KPALVTR::RESET );

        m_socket_status[ socket ] = Socket_Status::AVAILABLE_FOR_ALLOCATION;
        ++m_sockets_available_for_allocation;

        m_tcp_server_is_detached[ socket ] = false;
    }

    /**
     * \brief Construct a TCP client socket.
     *
     * \pre a socket is available
     *
     * \return The constructed TCP client socket.
     */
    auto make_tcp_client() noexcept -> TCP_Client
    {
        return { *this, allocate_socket() };
    }

    /**
     * \brief Construct a TCP client socket that uses a specific socket.
     *
     * \param[in] socket_id The socket ID for the socket to use.
     *
     * \pre the requested socket is available for allocation
     *
     * \return The constructed TCP client socket.
     */
    auto make_tcp_client( Socket_ID socket_id ) noexcept -> TCP_Client
    {
        return { *this, allocate_socket( socket_id ) };
    }

    /**
     * \brief Detach a TCP server socket from the TCP acceptor socket it is associated
     *        with.
     *
     * \param[in] socket_id The socket ID for the TCP server socket to detach from the TCP
     *            acceptor socket it is associated with.
     */
    void detach_tcp_server( Network_Stack_TCP_Server_Detachment_Key, Socket_ID socket_id ) noexcept
    {
        auto const socket = static_cast<std::uint_fast8_t>(
            to_underlying( socket_id ) >> Control_Byte::Bit::SOCKET );

        m_tcp_server_is_detached[ socket ] = true;
    }

    /**
     * \brief Check if a TCP server socket has been detached from the TCP acceptor socket
     *        it is associated with.
     *
     * \param[in] socket_id The socket ID for the TCP server socket to check.
     *
     * \return true if the TCP server socket has been detached from the TCP acceptor
     *         socket it is associated with.
     * \return false if the TCP server socket has not been detached from the TCP acceptor
     *         socket it is associated with.
     */
    constexpr auto tcp_server_is_detached( Socket_ID socket_id ) const noexcept -> bool
    {
        auto const socket = static_cast<std::uint_fast8_t>(
            to_underlying( socket_id ) >> Control_Byte::Bit::SOCKET );

        return m_tcp_server_is_detached[ socket ];
    }

    /**
     * \brief Access the TCP port allocator.
     *
     * \return The TCP port allocator.
     */
    auto tcp_port_allocator( Network_Stack_TCP_Port_Allocator_Access_Key ) noexcept -> TCP_Port_Allocator &
    {
        return m_tcp_port_allocator;
    }

    /**
     * \brief Access the UDP port allocator.
     *
     * \return The UDP port allocator.
     */
    auto udp_port_allocator( Network_Stack_UDP_Port_Allocator_Access_Key ) noexcept -> UDP_Port_Allocator &
    {
        return m_udp_port_allocator;
    }

  private:
    /**
     * \brief Socket status.
     */
    enum class Socket_Status : std::uint_fast8_t {
        NOT_AVAILABLE_FOR_USE,    ///< Not available for use.
        ALLOCATED,                ///< Allocated.
        AVAILABLE_FOR_ALLOCATION, ///< Available for allocation.
    };

    /**
     * \brief The driver used to interact with the W5500.
     */
    Driver m_driver{};

    /**
     * \brief The fatal error that occurs if an operation fails due to the W5500 being
     *        nonresponsive.
     */
    Error_Code m_nonresponsive_device_error{};

    /**
     * \brief The TCP over IP port allocator.
     */
    TCP_Port_Allocator m_tcp_port_allocator{};

    /**
     * \brief The UDP over IP port allocator.
     */
    UDP_Port_Allocator m_udp_port_allocator{};

    /**
     * \brief The socket buffer size.
     */
    Socket_Buffer_Size m_socket_buffer_size{};

    /**
     * \brief The number of sockets the network stack is configured to support.
     */
    std::uint_fast8_t m_sockets{};

    /**
     * \brief The number of sockets that are available for allocation.
     */
    std::uint_fast8_t m_sockets_available_for_allocation{};

    /**
     * \brief The socket status.
     */
    Array<Socket_Status, SOCKETS> m_socket_status{};

    /**
     * \brief The TCP server socket is detached from the TCP acceptor socket it is
     *        associated with flag.
     */
    Array<bool, SOCKETS> m_tcp_server_is_detached{};

    /**
     * \brief Allocate a socket.
     *
     * \pre a socket is available
     *
     * \return The socket ID for the allocated socket.
     */
    constexpr auto allocate_socket() noexcept -> Socket_ID
    {
        for ( auto socket = std::uint_fast8_t{}; socket < m_sockets; ++socket ) {
            if ( m_socket_status[ socket ] == Socket_Status::AVAILABLE_FOR_ALLOCATION ) {
                m_socket_status[ socket ] = Socket_Status::ALLOCATED;
                --m_sockets_available_for_allocation;

                return static_cast<Socket_ID>( socket << Control_Byte::Bit::SOCKET );
            } // if
        }     // for

        PICOLIBRARY_EXPECTATION_NOT_MET( Generic_Error::NO_SOCKETS_AVAILABLE );
    }

    /**
     * \brief Allocate a specific socket.
     *
     * \param[in] socket_id The socket ID for the socket to allocate.
     *
     * \pre the requested socket is available for allocation
     *
     * \return socket_id
     */
    constexpr auto allocate_socket( Socket_ID socket_id ) noexcept -> Socket_ID
    {
        auto const socket = static_cast<std::uint_fast8_t>(
            to_underlying( socket_id ) >> Control_Byte::Bit::SOCKET );

        PICOLIBRARY_EXPECT(
            m_socket_status[ socket ] == Socket_Status::AVAILABLE_FOR_ALLOCATION,
            Generic_Error::LOGIC_ERROR );

        m_socket_status[ socket ] = Socket_Status::ALLOCATED;
        --m_sockets_available_for_allocation;

        return socket_id;
    }

    /**
     * \brief Allocate specific sockets.
     *
     * \tparam Iterator Socket IDs range iterator.
     *
     * \param[in] begin The beginning of the range of socket IDs for the sockets to
     *            allocate.
     * \param[in] end The end of the range of socket IDs for the sockets to allocate.
     *
     * \pre the requested sockets are available for allocation.
     *
     * \return The socket IDs for the allocated sockets.
     */
    template<typename Iterator>
    auto allocate_sockets( Iterator begin, Iterator end ) noexcept
        -> Fixed_Capacity_Vector<Socket_ID, SOCKETS>
    {
        auto socket_ids = Fixed_Capacity_Vector<Socket_ID, SOCKETS>{};

        ::picolibrary::for_each( begin, end, [ this, &socket_ids ]( auto socket_id ) noexcept {
            socket_ids.push_back( BYPASS_PRECONDITION_EXPECTATION_CHECKS, allocate_socket( socket_id ) );
        } );

        return socket_ids;
    }
};

} // namespace picolibrary::WIZnet::W5500::IP

#endif // PICOLIBRARY_WIZNET_W5500_IP_NETWORK_STACK_H

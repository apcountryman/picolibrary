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
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack interface.
 */

#ifndef PICOLIBRARY_WIZNET_W5500_IP_NETWORK_STACK_H
#define PICOLIBRARY_WIZNET_W5500_IP_NETWORK_STACK_H

#include <cstdint>
#include <utility>

#include "picolibrary/array.h"
#include "picolibrary/error.h"
#include "picolibrary/ip.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/mac_address.h"
#include "picolibrary/precondition.h"
#include "picolibrary/utility.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/ip/tcp.h"

namespace picolibrary::WIZnet::W5500::IP {

/**
 * \brief Network stack.
 *
 * \tparam Driver The type of driver used to interact with the W5500.
 * \tparam TCP_Port_Allocator The type of TCP over IP port allocator to use.
 */
template<typename Driver, typename TCP_Port_Allocator>
class Network_Stack {
  public:
    /**
     * \brief The type of TCP client socket that is used to interact with the network stack.
     */
    using TCP_Client = TCP::Client<Driver, Network_Stack>;

    /**
     * \brief Constructor.
     */
    constexpr Network_Stack() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] driver The driver used to interact with the W5500.
     * \param[in] nonresponsive_device_error The fatal error that occurs if an operation
     *            fails due to the W5500 being nonresponsive.
     * \param[in] tcp_port_allocator The TCP over IP port allocator to use.
     */
    constexpr Network_Stack( Driver & driver, Error_Code const & nonresponsive_device_error, TCP_Port_Allocator tcp_port_allocator ) noexcept
        :
        m_driver{ &driver },
        m_nonresponsive_device_error{ nonresponsive_device_error },
        m_tcp_port_allocator{ std::move( tcp_port_allocator ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Network_Stack( Network_Stack && source ) noexcept :
        m_driver{ source.m_driver },
        m_nonresponsive_device_error{ std::move( source.m_nonresponsive_device_error ) },
        m_tcp_port_allocator{ std::move( source.m_tcp_port_allocator ) },
        m_socket_buffer_size{ source.m_socket_buffer_size },
        m_sockets{ source.m_sockets },
        m_sockets_available_for_allocation{ source.m_sockets_available_for_allocation },
        m_socket_status{ std::move( source.m_socket_status ) }
    {
        source.m_driver                           = nullptr;
        source.m_socket_buffer_size               = Socket_Buffer_Size::_0_KiB;
        source.m_sockets                          = 0;
        source.m_sockets_available_for_allocation = 0;
        source.m_socket_status                    = {};
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
            m_driver                     = expression.m_driver;
            m_nonresponsive_device_error = std::move( expression.m_nonresponsive_device_error );
            m_tcp_port_allocator         = std::move( expression.m_tcp_port_allocator );
            m_socket_buffer_size         = expression.m_socket_buffer_size;
            m_sockets                    = expression.m_sockets;
            m_sockets_available_for_allocation = expression.m_sockets_available_for_allocation;
            m_socket_status                    = std::move( expression.m_socket_status );

            expression.m_driver                           = nullptr;
            expression.m_socket_buffer_size               = Socket_Buffer_Size::_0_KiB;
            expression.m_sockets                          = 0;
            expression.m_sockets_available_for_allocation = 0;
            expression.m_socket_status                    = {};
        } // if

        return *this;
    }

    auto operator=( Network_Stack const & ) = delete;

    /**
     * \brief Get the fatal error that occurs if an operation fails due to the W5500 being
     *        nonresponsive.
     *
     * \return The fatal error that occurs if an operation fails due to the W5500 being
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
        return m_driver->read_versionr() == VERSIONR::VERSION;
    }

    /**
     * \brief Configure the PHY.
     *
     * \param[in] phy_mode The desired PHY mode.
     */
    void configure_phy( PHY_Mode phy_mode ) noexcept
    {
        m_driver->write_phycfgr( to_underlying( phy_mode ) | PHYCFGR::Mask::RST );
        m_driver->write_phycfgr( to_underlying( phy_mode ) );
        m_driver->write_phycfgr( to_underlying( phy_mode ) | PHYCFGR::Mask::RST );
    }

    /**
     * \brief Get the PHY mode.
     *
     * \return The PHY mode.
     */
    auto phy_mode() const noexcept -> PHY_Mode
    {
        return static_cast<PHY_Mode>(
            m_driver->read_phycfgr() & ( PHYCFGR::Mask::OPMD | PHYCFGR::Mask::OPMDC ) );
    }

    /**
     * \brief Get the link status.
     *
     * \return The link status.
     */
    auto link_status() const noexcept -> Link_Status
    {
        return static_cast<Link_Status>( m_driver->read_phycfgr() & PHYCFGR::Mask::LNK );
    }

    /**
     * \brief Get the link mode.
     *
     * \return The link mode.
     */
    auto link_mode() const noexcept -> Link_Mode
    {
        return static_cast<Link_Mode>( m_driver->read_phycfgr() & PHYCFGR::Mask::DPX );
    }

    /**
     * \brief Get the link speed.
     *
     * \return The link speed.
     */
    auto link_speed() const noexcept -> Link_Speed
    {
        return static_cast<Link_Speed>( m_driver->read_phycfgr() & PHYCFGR::Mask::SPD );
    }

    /**
     * \brief Configure ping blocking.
     *
     * \param[in] ping_blocking_configuration The desired ping blocking configuration.
     */
    void configure_ping_blocking( Ping_Blocking ping_blocking_configuration ) noexcept
    {
        m_driver->write_mr( ( m_driver->read_mr() & ~MR::Mask::PB ) | to_underlying( ping_blocking_configuration ) );
    }

    /**
     * \brief Get the ping blocking configuration.
     *
     * \return The ping blocking configuration.
     */
    auto ping_blocking_configuration() const noexcept -> Ping_Blocking
    {
        return static_cast<Ping_Blocking>( m_driver->read_mr() & MR::Mask::PB );
    }

    /**
     * \brief Configure ARP forcing.
     *
     * \param[in] arp_forcing_configuration The desired ARP forcing configuration.
     */
    void configure_arp_forcing( ARP_Forcing arp_forcing_configuration ) noexcept
    {
        m_driver->write_mr( ( m_driver->read_mr() & ~MR::Mask::FARP ) | to_underlying( arp_forcing_configuration ) );
    }

    /**
     * \brief Get the ARP forcing configuration.
     *
     * \return The ARP forcing configuration.
     */
    auto arp_forcing_configuration() const noexcept -> ARP_Forcing
    {
        return static_cast<ARP_Forcing>( m_driver->read_mr() & MR::Mask::FARP );
    }

    /**
     * \brief Configure retransmission (RTR and RCR register values).
     *
     * \param[in] retry_time The desired retry time (RTR register value).
     * \param[in] retry_count The desired retry count (RCR register value).
     */
    void configure_retransmission( std::uint16_t retry_time, std::uint8_t retry_count ) noexcept
    {
        m_driver->write_rtr( retry_time );
        m_driver->write_rcr( retry_count );
    }

    /**
     * \brief Get the retry time (RTR register value).
     *
     * \return The retry time.
     */
    auto retry_time() const noexcept -> std::uint16_t
    {
        return m_driver->read_rtr();
    }

    /**
     * \brief Get the retry count (RCR register value).
     *
     * \return The retry count.
     */
    auto retry_count() const noexcept -> std::uint8_t
    {
        return m_driver->read_rcr();
    }

    /**
     * \brief Configure the socket buffer size.
     *
     * \pre the socket buffer size has not already been configured
     *
     * \param[in] socket_buffer_size The desired socket buffer size.
     *
     * \pre socket_buffer_size is 2 KiB, 4 KiB, 8 KiB, or 16 KiB
     */
    // NOLINTNEXTLINE(readability-function-size)
    void configure_socket_buffer_size( Socket_Buffer_Size socket_buffer_size ) noexcept
    {
        // #lizard forgives the length

        expect( m_sockets == 0, Generic_Error::LOGIC_ERROR );

        auto sockets = std::uint_fast8_t{ 0 };

        switch ( socket_buffer_size ) {
                // clang-format off

            case Socket_Buffer_Size::_2_KiB:  sockets = 16 /  2; break;
            case Socket_Buffer_Size::_4_KiB:  sockets = 16 /  4; break;
            case Socket_Buffer_Size::_8_KiB:  sockets = 16 /  8; break;
            case Socket_Buffer_Size::_16_KiB: sockets = 16 / 16; break;

                // clang-format on

            default: expect( Generic_Error::INVALID_ARGUMENT );
        } // switch

        for ( auto socket = std::uint_fast8_t{ 0 }; socket < sockets; ++socket ) {
            auto const socket_id = static_cast<Socket_ID>( socket << Control_Byte::Bit::SOCKET );

            m_driver->write_sn_rxbuf_size( socket_id, to_underlying( socket_buffer_size ) );
            m_driver->write_sn_txbuf_size( socket_id, to_underlying( socket_buffer_size ) );

            m_socket_status[ socket ] = Socket_Status::AVAILABLE_FOR_ALLOCATION;
        } // for

        for ( auto socket = sockets; socket < SOCKETS; ++socket ) {
            auto const socket_id = static_cast<Socket_ID>( socket << Control_Byte::Bit::SOCKET );

            m_driver->write_sn_rxbuf_size( socket_id, to_underlying( Socket_Buffer_Size::_0_KiB ) );
            m_driver->write_sn_txbuf_size( socket_id, to_underlying( Socket_Buffer_Size::_0_KiB ) );
        } // for

        m_socket_buffer_size               = socket_buffer_size;
        m_sockets                          = sockets;
        m_sockets_available_for_allocation = sockets;
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
     * \brief Configure the W5500's MAC address.
     *
     * \param[in] address The desired MAC address.
     */
    void configure_mac_address( MAC_Address const & address ) noexcept
    {
        m_driver->write_shar( address.as_byte_array() );
    }

    /**
     * \brief Get the W5500's MAC address.
     *
     * \return The W5500's MAC address.
     */
    auto mac_address() const noexcept -> MAC_Address
    {
        return m_driver->read_shar();
    }

    /**
     * \brief Configure the W5500's IPv4 address.
     *
     * \param[in] address The desired IPv4 address.
     */
    void configure_ipv4_address( IPv4::Address const & address ) noexcept
    {
        m_driver->write_sipr( address.as_byte_array() );
    }

    /**
     * \brief Get the W5500's IPv4 address.
     *
     * \return The W5500's IPv4 address.
     */
    auto ipv4_address() const noexcept -> IPv4::Address
    {
        return m_driver->read_sipr();
    }

    /**
     * \brief Configure the W5500's IPv4 gateway address.
     *
     * \param[in] address The desired IPv4 gateway address.
     */
    void configure_ipv4_gateway_address( IPv4::Address const & address ) noexcept
    {
        m_driver->write_gar( address.as_byte_array() );
    }

    /**
     * \brief Get the W5500's IPv4 gateway address.
     *
     * \return The W5500's IPv4 gateway address.
     */
    auto ipv4_gateway_address() const noexcept -> IPv4::Address
    {
        return m_driver->read_gar();
    }

    /**
     * \brief Configure the W5500's IPv4 subnet mask.
     *
     * \param[in] subnet_mask The desired IPv4 subnet mask.
     */
    void configure_ipv4_subnet_mask( IPv4::Address const & subnet_mask ) noexcept
    {
        m_driver->write_subr( subnet_mask.as_byte_array() );
    }

    /**
     * \brief Get the W5500's IPv4 subnet mask.
     *
     * \return The W5500's IPv4 subnet mask.
     */
    auto ipv4_subnet_mask() const noexcept -> IPv4::Address
    {
        return m_driver->read_subr();
    }

    /**
     * \brief Configure the W5500's interrupt assert wait time (INTLEVEL register value).
     *
     * \param[in] interrupt_assert_wait_time The desired interrupt assert wait time
     *            (INTLEVEL register value).
     */
    void configure_interrupt_assert_wait_time( std::uint16_t interrupt_assert_wait_time ) noexcept
    {
        m_driver->write_intlevel( interrupt_assert_wait_time );
    }

    /**
     * \brief Get the W5500's interrupt assert wait time (INTLEVEL register value).
     *
     * \return The W5500's interrupt assert wait time.
     */
    auto interrupt_assert_wait_time() const noexcept -> std::uint16_t
    {
        return m_driver->read_intlevel();
    }

    /**
     * \brief Enable interrupts.
     *
     * \param[in] mask The mask identifying the interrupts to enable.
     */
    void enable_interrupts( std::uint8_t mask ) noexcept
    {
        m_driver->write_imr( m_driver->read_imr() | mask );
    }

    /**
     * \brief Disable interrupts.
     *
     * \param[in] mask The mask identifying the interrupts to disable.
     */
    void disable_interrupts( std::uint8_t mask ) noexcept
    {
        m_driver->write_imr( m_driver->read_imr() & ~mask );
    }

    /**
     * \brief Disable all interrupts.
     */
    void disable_interrupts() noexcept
    {
        m_driver->write_imr( 0x00 );
    }

    /**
     * \brief Get a mask identifying the interrupts that are enabled.
     *
     * \return A mask identifying the interrupts that are enabled.
     */
    auto enabled_interrupts() const noexcept -> std::uint8_t
    {
        return m_driver->read_imr();
    }

    /**
     * \brief Get the interrupt context (IR register value).
     *
     * \return The interrupt context.
     */
    auto interrupt_context() const noexcept -> std::uint8_t
    {
        return m_driver->read_ir();
    }

    /**
     * \brief Clear interrupts.
     *
     * \param[in] mask The mask identifying the interrupts to clear.
     */
    void clear_interrupts( std::uint8_t mask ) noexcept
    {
        m_driver->write_ir( mask );
    }

    /**
     * \brief Enable socket interrupts.
     */
    void enable_socket_interrupts() noexcept
    {
        m_driver->write_simr( 0xFF );
    }

    /**
     * \brief Disable socket interrupts.
     */
    void disable_socket_interrupts() noexcept
    {
        m_driver->write_simr( 0x00 );
    }

    /**
     * \brief Check if socket interrupts are enabled.
     *
     * \return true if socket interrupts are enabled.
     * \return false if socket interrupts are not enabled.
     */
    auto socket_interrupts_are_enabled() const noexcept -> bool
    {
        return m_driver->read_simr();
    }

    /**
     * \brief Get the socket interrupt context (SIR register value).
     *
     * \return The socket interrupt context.
     */
    auto socket_interrupt_context() const noexcept -> std::uint8_t
    {
        return m_driver->read_sir();
    }

    /**
     * \brief Get the IPv4 address that has been reported to be unreachable.
     *
     * \return The IPv4 address that has been reported to be unreachable.
     */
    auto unreachable_ipv4_address() const noexcept -> IPv4::Address
    {
        return m_driver->read_uipr();
    }

    /**
     * \brief Get the port that has been reported to be unreachable.
     *
     * \return The port that has been reported to be unreachable.
     */
    auto unreachable_port() const noexcept -> ::picolibrary::IP::Port
    {
        return m_driver->read_uportr();
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
     * \brief Allocate a socket.
     *
     * \pre a socket is available
     *
     * \return The socket ID for the allocated socket.
     */
    constexpr auto allocate_socket() noexcept -> Socket_ID
    {
        for ( auto socket = std::uint_fast8_t{}; socket < sockets(); ++socket ) {
            if ( m_socket_status[ socket ] == Socket_Status::AVAILABLE_FOR_ALLOCATION ) {
                m_socket_status[ socket ] = Socket_Status::ALLOCATED;
                --m_sockets_available_for_allocation;

                return static_cast<Socket_ID>( socket << Control_Byte::Bit::SOCKET );
            } // if
        }     // for

        expect( Generic_Error::NO_SOCKETS_AVAILABLE );
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

        expect( m_socket_status[ socket ] == Socket_Status::AVAILABLE_FOR_ALLOCATION, Generic_Error::LOGIC_ERROR );

        m_socket_status[ socket ] = Socket_Status::ALLOCATED;
        --m_sockets_available_for_allocation;

        return socket_id;
    }

    /**
     * \brief Deallocate a socket.
     *
     * \param[in] socket_id The socket ID for the socket to deallocate.
     */
    constexpr void deallocate_socket( Socket_ID socket_id ) noexcept
    {
        m_socket_status[ to_underlying( socket_id ) >> Control_Byte::Bit::SOCKET ] = Socket_Status::AVAILABLE_FOR_ALLOCATION;
        ++m_sockets_available_for_allocation;
    }

    /**
     * \brief Access the TCP over IP port allocator.
     *
     * \return The TCP over IP port allocator.
     */
    auto tcp_port_allocator() noexcept -> TCP_Port_Allocator &
    {
        return m_tcp_port_allocator;
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
        return { *m_driver, allocate_socket(), *this };
    }

    /**
     * \brief Construct a TCP client socket that uses a specific W5500 socket.
     *
     * \param[in] socket_id The socket ID for the W5500 socket to use.
     *
     * \pre the requested W5500 socket is available for allocation
     */
    auto make_tcp_client( Socket_ID socket_id ) noexcept -> TCP_Client
    {
        return { *m_driver, allocate_socket( socket_id ), *this };
    }

  private:
    /**
     * \brief Socket allocation state.
     */
    enum class Socket_Status : std::uint_fast8_t {
        NOT_AVAILABLE_FOR_USE,    ///< Not available for use.
        ALLOCATED,                ///< Allocated.
        AVAILABLE_FOR_ALLOCATION, ///< Available for allocation.
    };

    /**
     * \brief The driver used to interact with the W5500.
     */
    Driver * m_driver{};

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
     * \brief The socket buffer size.
     */
    Socket_Buffer_Size m_socket_buffer_size{ Socket_Buffer_Size::_0_KiB };

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
};

} // namespace picolibrary::WIZnet::W5500::IP

#endif // PICOLIBRARY_WIZNET_W5500_IP_NETWORK_STACK_H

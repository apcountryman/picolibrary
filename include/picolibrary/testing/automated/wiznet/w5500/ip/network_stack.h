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
 * \brief picolibrary::Testing::Automated::WIZnet::W5500::IP::Network_Stack interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_IP_NETWORK_STACK_H
#define PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_IP_NETWORK_STACK_H

#include <cstdint>

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/fixed_capacity_vector.h"
#include "picolibrary/ip.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/mac_address.h"
#include "picolibrary/testing/automated/wiznet/w5500.h"
#include "picolibrary/testing/automated/wiznet/w5500/ip.h"
#include "picolibrary/testing/automated/wiznet/w5500/ip/tcp.h"
#include "picolibrary/testing/automated/wiznet/w5500/ip/udp.h"
#include "picolibrary/wiznet/w5500.h"

namespace picolibrary::Testing::Automated::WIZnet::W5500::IP {

/**
 * \brief Mock network stack.
 */
class Mock_Network_Stack {
  public:
    using TCP_Client = TCP::Mock_Client::Handle;

    using TCP_Server = TCP::Mock_Server::Handle;

    using UDP_Socket = UDP::Mock_Socket::Handle;

    Mock_Network_Stack() = default;

    Mock_Network_Stack( Mock_Network_Stack && ) = delete;

    Mock_Network_Stack( Mock_Network_Stack const & ) = delete;

    ~Mock_Network_Stack() noexcept = default;

    auto operator=( Mock_Network_Stack && ) = delete;

    auto operator=( Mock_Network_Stack const & ) = delete;

    MOCK_METHOD(
        void,
        initialize,
        ( ::picolibrary::WIZnet::W5500::PHY_Mode,
          ::picolibrary::WIZnet::W5500::Ping_Blocking,
          ::picolibrary::WIZnet::W5500::ARP_Forcing,
          std::uint16_t,
          std::uint8_t,
          MAC_Address const &,
          ::picolibrary::IPv4::Address const &,
          ::picolibrary::IPv4::Address const &,
          ::picolibrary::IPv4::Address const &,
          std::uint16_t,
          ::picolibrary::WIZnet::W5500::Socket_Buffer_Size ) );

    MOCK_METHOD( Mock_Driver &, driver, ( std::uint_fast8_t ) );

    MOCK_METHOD( Error_Code, nonresponsive_device_error, (), ( const ) );

    MOCK_METHOD( bool, w5500_is_responsive, (), ( const ) );

    MOCK_METHOD( ::picolibrary::WIZnet::W5500::PHY_Mode, phy_mode, (), ( const ) );

    MOCK_METHOD( ::picolibrary::WIZnet::W5500::Link_Status, link_status, (), ( const ) );
    MOCK_METHOD( ::picolibrary::WIZnet::W5500::Link_Mode, link_mode, (), ( const ) );
    MOCK_METHOD( ::picolibrary::WIZnet::W5500::Link_Speed, link_speed, (), ( const ) );

    MOCK_METHOD( void, configure_ping_blocking, ( ::picolibrary::WIZnet::W5500::Ping_Blocking ) );
    MOCK_METHOD( ::picolibrary::WIZnet::W5500::Ping_Blocking, ping_blocking_configuration, (), ( const ) );

    MOCK_METHOD( ::picolibrary::WIZnet::W5500::ARP_Forcing, arp_forcing_configuration, (), ( const ) );

    MOCK_METHOD( std::uint16_t, retransmission_retry_time, (), ( const ) );
    MOCK_METHOD( std::uint8_t, retransmission_retry_count, (), ( const ) );

    MOCK_METHOD( MAC_Address, mac_address, (), ( const ) );

    MOCK_METHOD( ::picolibrary::IPv4::Address, ipv4_address, (), ( const ) );
    MOCK_METHOD( ::picolibrary::IPv4::Address, ipv4_gateway_address, (), ( const ) );
    MOCK_METHOD( ::picolibrary::IPv4::Address, ipv4_subnet_mask, (), ( const ) );

    MOCK_METHOD( std::uint16_t, interrupt_assert_wait_time, (), ( const ) );

    MOCK_METHOD( void, enable_interrupts, ( std::uint8_t ) );
    MOCK_METHOD( void, disable_interrupts, ( std::uint8_t ) );
    MOCK_METHOD( void, disable_interrupts, () );
    MOCK_METHOD( std::uint8_t, enabled_interrupts, (), ( const ) );

    MOCK_METHOD( std::uint8_t, interrupt_context, (), ( const ) );
    MOCK_METHOD( void, clear_interrupts, ( std::uint8_t ) );

    MOCK_METHOD( void, enable_socket_interrupts, () );
    MOCK_METHOD( void, disable_socket_interrupts, () );
    MOCK_METHOD( bool, socket_interrupts_are_enabled, (), ( const ) );

    MOCK_METHOD( std::uint8_t, socket_interrupt_context, (), ( const ) );

    MOCK_METHOD( ::picolibrary::IP::Endpoint, unreachable_endpoint, (), ( const ) );

    MOCK_METHOD( ::picolibrary::WIZnet::W5500::Socket_Buffer_Size, socket_buffer_size, (), ( const ) );

    MOCK_METHOD( std::uint_fast8_t, sockets, (), ( const ) );

    MOCK_METHOD( std::uint_fast8_t, sockets_available_for_allocation, (), ( const ) );
    MOCK_METHOD(
        (Fixed_Capacity_Vector<::picolibrary::WIZnet::W5500::Socket_ID, 8>),
        allocate_sockets,
        ( std::uint_fast8_t, std::uint_fast8_t ) );
    MOCK_METHOD( void, deallocate_socket, ( std::uint_fast8_t, ::picolibrary::WIZnet::W5500::Socket_ID ) );

    MOCK_METHOD( TCP_Client, make_tcp_client, () );
    MOCK_METHOD( TCP_Client, make_tcp_client, ( ::picolibrary::WIZnet::W5500::Socket_ID ) );

    MOCK_METHOD( TCP_Server, make_tcp_server, () );
    MOCK_METHOD(
        TCP_Server,
        make_tcp_server,
        ( (Fixed_Capacity_Vector<::picolibrary::WIZnet::W5500::Socket_ID, 8> const &)) );

    MOCK_METHOD( void, detach_tcp_server_connection_handler, ( std::uint_fast8_t, ::picolibrary::WIZnet::W5500::Socket_ID ) );
    MOCK_METHOD( bool, tcp_server_connection_handler_is_detached, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );

    MOCK_METHOD( Mock_Port_Allocator &, tcp_port_allocator, ( std::uint_fast8_t ) );

    MOCK_METHOD( UDP_Socket, make_udp_socket, () );
    MOCK_METHOD( UDP_Socket, make_udp_socket, ( ::picolibrary::WIZnet::W5500::Socket_ID ) );

    MOCK_METHOD( Mock_Port_Allocator &, udp_port_allocator, ( std::uint_fast8_t ) );
};

} // namespace picolibrary::Testing::Automated::WIZnet::W5500::IP

#endif // PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_IP_NETWORK_STACK_H

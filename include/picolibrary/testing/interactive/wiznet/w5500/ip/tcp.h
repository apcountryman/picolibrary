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
 * \brief picolibrary::Testing::Interactive::WIZnet::W5500::IP::TCP interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_WIZNET_W5500_IP_TCP_H
#define PICOLIBRARY_TESTING_INTERACTIVE_WIZNET_W5500_IP_TCP_H

#include <cstdint>
#include <utility>

#include "picolibrary/error.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/mac_address.h"
#include "picolibrary/precondition.h"
#include "picolibrary/rom.h"
#include "picolibrary/stream.h"
#include "picolibrary/testing/interactive/ip/tcp.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/ip.h"
#include "picolibrary/wiznet/w5500/ip/network_stack.h"

/**
 * \brief WIZnet W5500 Transmission Control Protocol (TCP) over IP interactive testing
 *        facilities.
 */
namespace picolibrary::Testing::Interactive::WIZnet::W5500::IP::TCP {

/**
 * \brief Client socket echo interactive test helper.
 *
 * \tparam Controller The type of controller used to communicate with the W5500.
 * \tparam Device_Selector The type of device selector used to select and deselect the
 *         W5500.
 *
 * \param[in] stream The output stream to write information to.
 * \param[in] controller The controller used to communicate with the W5500.
 * \param[in] configuration The controller clock and data exchange bit order configuration
 *            that meets the W5500's communication requirements.
 * \param[in] device_selector The device selector used to select and deselect the W5500.
 * \param[in] phy_mode The desired W5500 PHY mode.
 * \param[in] ping_blocking_configuration The desired W5500 ping blocking configuration.
 * \param[in] arp_forcing_configuration The desired W5500 ARP forcing configuration.
 * \param[in] retransmission_retry_time The desired W5500 retransmission retry time (RTR
 *            register value).
 * \param[in] retransmission_retry_count The desired W5500 retransmission retry count (RCR
 *            register value).
 * \param[in] socket_buffer_size The desired W5500 socket buffer size.
 * \param[in] mac_address The desired W5500 MAC address.
 * \param[in] ipv4_address The desired W5500 IPv4 address.
 * \param[in] ipv4_gateway_address The desired W5500 IPv4 gateway address.
 * \param[in] ipv4_subnet_mask The desired W5500 IPv4 subnet mask.
 * \param[in] no_delayed_ack_usage_configuration The desired client socket no delayed ACK
 *            usage configuration.
 * \param[in] maximum_segment_size The desired client socket maximum segment size.
 * \param[in] time_to_live The desired client socket IPv4 packet time to live field value.
 * \param[in] keepalive_period The desired client socket keepalive packet transmission
 *            period (SN_KPALVTR register value).
 * \param[in] local_endpoint The local endpoint to bind client sockets to.
 * \param[in] remote_endpoint The remote endpoint to connect to.
 */
template<typename Controller, typename Device_Selector>
// NOLINTNEXTLINE(readability-function-size)
[[noreturn]] void echo_client(
    Reliable_Output_Stream &                           stream,
    Controller                                         controller,
    typename Controller::Configuration                 configuration,
    Device_Selector                                    device_selector,
    ::picolibrary::WIZnet::W5500::PHY_Mode             phy_mode,
    ::picolibrary::WIZnet::W5500::Ping_Blocking        ping_blocking_configuration,
    ::picolibrary::WIZnet::W5500::ARP_Forcing          arp_forcing_configuration,
    std::uint16_t                                      retransmission_retry_time,
    std::uint8_t                                       retransmission_retry_count,
    ::picolibrary::WIZnet::W5500::Socket_Buffer_Size   socket_buffer_size,
    MAC_Address                                        mac_address,
    IPv4::Address                                      ipv4_address,
    IPv4::Address                                      ipv4_gateway_address,
    IPv4::Address                                      ipv4_subnet_mask,
    ::picolibrary::WIZnet::W5500::No_Delayed_ACK_Usage no_delayed_ack_usage_configuration,
    std::uint16_t                                      maximum_segment_size,
    std::uint8_t                                       time_to_live,
    std::uint8_t                                       keepalive_period,
    ::picolibrary::IP::TCP::Endpoint const &           local_endpoint,
    ::picolibrary::IP::TCP::Endpoint const &           remote_endpoint ) noexcept
{
    // #lizard forgives the length
    // #lizard forgives the parameter count

    controller.initialize();

    auto w5500 = ::picolibrary::WIZnet::W5500::Driver{ controller, configuration, std::move( device_selector ) };

    w5500.initialize();

    auto network_stack = ::picolibrary::WIZnet::W5500::IP::Network_Stack{
        w5500,
        Generic_Error::NONRESPONSIVE_DEVICE,
        ::picolibrary::WIZnet::W5500::IP::TCP_UDP_Port_Allocator_Without_Ephemeral_Port_Allocation{
            ::picolibrary::WIZnet::W5500::Socket_Protocol::TCP }
    };

    expect( network_stack.w5500_is_responsive(), network_stack.nonresponsive_device_error() );

    network_stack.configure_phy( phy_mode );

    network_stack.configure_ping_blocking( ping_blocking_configuration );

    network_stack.configure_arp_forcing( arp_forcing_configuration );

    network_stack.configure_retransmission( retransmission_retry_time, retransmission_retry_count );

    network_stack.configure_socket_buffer_size( socket_buffer_size );

    network_stack.configure_mac_address( mac_address );

    network_stack.configure_ipv4_address( ipv4_address );
    network_stack.configure_ipv4_gateway_address( ipv4_gateway_address );
    network_stack.configure_ipv4_subnet_mask( ipv4_subnet_mask );

    stream.put( PICOLIBRARY_ROM_STRING( "waiting for link to be established\n" ) );
    stream.flush();

    while ( network_stack.link_status() != ::picolibrary::WIZnet::W5500::Link_Status::UP ) {} // while

    auto const link_speed = network_stack.link_speed();
    auto const link_mode  = network_stack.link_mode();

    // clang-format off
    stream.print(
        PICOLIBRARY_ROM_STRING( "link established:" ),
        PICOLIBRARY_ROM_STRING( "\n    speed: " ), link_speed == ::picolibrary::WIZnet::W5500::Link_Speed::_10_MbPs  ? PICOLIBRARY_ROM_STRING( "10 Mb/s" )
                                                 : link_speed == ::picolibrary::WIZnet::W5500::Link_Speed::_100_MbPs ? PICOLIBRARY_ROM_STRING( "100 Mb/s" )
                                                                                                                     : PICOLIBRARY_ROM_STRING( "unknown" ),
        PICOLIBRARY_ROM_STRING( "\n    mode: " ), link_mode == ::picolibrary::WIZnet::W5500::Link_Mode::HALF_DUPLEX ? PICOLIBRARY_ROM_STRING( "half duplex" )
                                                : link_mode == ::picolibrary::WIZnet::W5500::Link_Mode::FULL_DUPLEX ? PICOLIBRARY_ROM_STRING( "full duplex" )
                                                                                                                    : PICOLIBRARY_ROM_STRING( "unknown" ),
        PICOLIBRARY_ROM_STRING( "\nMAC address: " ), mac_address,
        PICOLIBRARY_ROM_STRING( "\nIPv4 address: " ), ipv4_address,
        PICOLIBRARY_ROM_STRING( "\nIPv4 gateway address: " ), ipv4_gateway_address,
        PICOLIBRARY_ROM_STRING( "\nIPv4 subnet mask: " ), ipv4_subnet_mask,
        '\n'
    );
    // clang-format on
    stream.flush();

    ::picolibrary::Testing::Interactive::IP::TCP::echo_client(
        stream,
        network_stack,
        [ no_delayed_ack_usage_configuration, maximum_segment_size, time_to_live, keepalive_period ](
            auto & client ) noexcept {
            client.configure_no_delayed_ack_usage( no_delayed_ack_usage_configuration );

            client.configure_maximum_segment_size( maximum_segment_size );

            client.configure_time_to_live( time_to_live );

            client.configure_keepalive_period( keepalive_period );
        },
        local_endpoint,
        remote_endpoint );
}

} // namespace picolibrary::Testing::Interactive::WIZnet::W5500::IP::TCP

#endif // PICOLIBRARY_TESTING_INTERACTIVE_WIZNET_W5500_IP_TCP_H

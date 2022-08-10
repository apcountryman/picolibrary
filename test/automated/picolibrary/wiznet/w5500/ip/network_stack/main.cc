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
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack automated test program.
 */

#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/mac_address.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/ipv4.h"
#include "picolibrary/testing/automated/mac_address.h"
#include "picolibrary/testing/automated/random.h"
#include "picolibrary/testing/automated/wiznet/w5500.h"
#include "picolibrary/testing/automated/wiznet/w5500/ip.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/ip/network_stack.h"

namespace {

using ::picolibrary::MAC_Address;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::random;
using ::picolibrary::Testing::Automated::random_array;
using ::picolibrary::Testing::Automated::WIZnet::W5500::Mock_Driver;
using ::picolibrary::Testing::Automated::WIZnet::W5500::IP::Mock_Port_Allocator;
using ::picolibrary::WIZnet::W5500::ARP_Forcing;
using ::picolibrary::WIZnet::W5500::Link_Mode;
using ::picolibrary::WIZnet::W5500::Link_Speed;
using ::picolibrary::WIZnet::W5500::Link_Status;
using ::picolibrary::WIZnet::W5500::PHY_Mode;
using ::picolibrary::WIZnet::W5500::Ping_Blocking;
using ::picolibrary::WIZnet::W5500::Socket_Buffer_Size;
using ::picolibrary::WIZnet::W5500::Socket_ID;
using ::picolibrary::WIZnet::W5500::IP::Network_Stack;
using ::testing::InSequence;
using ::testing::Return;

using IPv4_Address = ::picolibrary::IPv4::Address;

} // namespace

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::Network_Stack( Driver &,
 *        picolibrary::Error_Code const &, TCP_Port_Allocator ) works properly.
 */
TEST( constructor, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const nonresponsive_device_error = random<Mock_Error>();

    auto const network_stack = Network_Stack{ driver,
                                              nonresponsive_device_error,
                                              tcp_port_allocator.handle() };

    EXPECT_EQ( network_stack.nonresponsive_device_error(), nonresponsive_device_error );
    EXPECT_EQ( network_stack.socket_buffer_size(), Socket_Buffer_Size::_0_KiB );
    EXPECT_EQ( network_stack.sockets(), 0 );
    EXPECT_EQ( network_stack.sockets_available_for_allocation(), 0 );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::w5500_is_responsive()
 *        works properly.
 */
TEST( w5500IsResponsive, worksProperly )
{
    struct {
        std::uint8_t versionr;
        bool         w5500_is_responsive;
    } const test_cases[]{
        // clang-format off

        { random<std::uint8_t>( 0x00, 0x03 ), false },
        {                             0x04,   true  },
        { random<std::uint8_t>( 0x05, 0xFF ), false },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver             = Mock_Driver{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto const network_stack = Network_Stack{ driver,
                                                  random<Mock_Error>(),
                                                  tcp_port_allocator.handle() };

        EXPECT_CALL( driver, read_versionr() ).WillOnce( Return( test_case.versionr ) );

        EXPECT_EQ( network_stack.w5500_is_responsive(), test_case.w5500_is_responsive );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::configure_phy() works
 *        properly.
 */
TEST( configurePHY, worksProperly )
{
    struct {
        PHY_Mode     phy_mode;
        std::uint8_t phycfgr_opmd_opmdc;
    } const test_cases[]{
        // clang-format off

        { PHY_Mode::CONFIGURED_BY_HARDWARE,                       0b0'0'000'0'0'0 },
        { PHY_Mode::POWER_DOWN,                                   0b0'1'110'0'0'0 },
        { PHY_Mode::_10BT_HALF_DUPLEX_AUTO_NEGOTIATION_DISABLED,  0b0'1'000'0'0'0 },
        { PHY_Mode::_10BT_FULL_DUPLEX_AUTO_NEGOTIATION_DISABLED,  0b0'1'001'0'0'0 },
        { PHY_Mode::_100BT_HALF_DUPLEX_AUTO_NEGOTIATION_DISABLED, 0b0'1'010'0'0'0 },
        { PHY_Mode::_100BT_FULL_DUPLEX_AUTO_NEGOTIATION_DISABLED, 0b0'1'011'0'0'0 },
        { PHY_Mode::_100BT_HALF_DUPLEX_AUTO_NEGOTIATION_ENABLED,  0b0'1'100'0'0'0 },
        { PHY_Mode::ALL_CAPABLE_AUTO_NEGOTIATION_ENABLED,         0b0'1'111'0'0'0 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const in_sequence = InSequence{};

        auto driver             = Mock_Driver{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto network_stack = Network_Stack{ driver,
                                            random<Mock_Error>(),
                                            tcp_port_allocator.handle() };

        EXPECT_CALL( driver, write_phycfgr( test_case.phycfgr_opmd_opmdc | 0b1'0'000'000 ) );
        EXPECT_CALL( driver, write_phycfgr( test_case.phycfgr_opmd_opmdc ) );
        EXPECT_CALL( driver, write_phycfgr( test_case.phycfgr_opmd_opmdc | 0b1'0'000'000 ) );

        network_stack.configure_phy( test_case.phy_mode );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::phy_mode() works properly.
 */
TEST( phyMode, worksProperly )
{
    struct {
        std::uint8_t phycfgr_opmd_opmdc;
        PHY_Mode     phy_mode;
    } const test_cases[]{
        // clang-format off

        { 0b0'0'000'0'0'0, PHY_Mode::CONFIGURED_BY_HARDWARE                       },
        { 0b0'1'110'0'0'0, PHY_Mode::POWER_DOWN                                   },
        { 0b0'1'000'0'0'0, PHY_Mode::_10BT_HALF_DUPLEX_AUTO_NEGOTIATION_DISABLED  },
        { 0b0'1'001'0'0'0, PHY_Mode::_10BT_FULL_DUPLEX_AUTO_NEGOTIATION_DISABLED  },
        { 0b0'1'010'0'0'0, PHY_Mode::_100BT_HALF_DUPLEX_AUTO_NEGOTIATION_DISABLED },
        { 0b0'1'011'0'0'0, PHY_Mode::_100BT_FULL_DUPLEX_AUTO_NEGOTIATION_DISABLED },
        { 0b0'1'100'0'0'0, PHY_Mode::_100BT_HALF_DUPLEX_AUTO_NEGOTIATION_ENABLED  },
        { 0b0'1'111'0'0'0, PHY_Mode::ALL_CAPABLE_AUTO_NEGOTIATION_ENABLED         },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver             = Mock_Driver{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto const network_stack = Network_Stack{ driver,
                                                  random<Mock_Error>(),
                                                  tcp_port_allocator.handle() };

        EXPECT_CALL( driver, read_phycfgr() )
            .WillOnce( Return( ( random<std::uint8_t>() & 0b1'0'000'1'1'1 ) | test_case.phycfgr_opmd_opmdc ) );

        EXPECT_EQ( network_stack.phy_mode(), test_case.phy_mode );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::link_status() works
 *        properly.
 */
TEST( linkStatus, worksProperly )
{
    struct {
        std::uint8_t phycfgr_lnk;
        Link_Status  link_status;
    } const test_cases[]{
        // clang-format off

        { 0b0'0'000'0'0'0, Link_Status::DOWN },
        { 0b0'0'000'0'0'1, Link_Status::UP   },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver             = Mock_Driver{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto const network_stack = Network_Stack{ driver,
                                                  random<Mock_Error>(),
                                                  tcp_port_allocator.handle() };

        EXPECT_CALL( driver, read_phycfgr() )
            .WillOnce( Return( ( random<std::uint8_t>() & 0b1'1'111'1'1'0 ) | test_case.phycfgr_lnk ) );

        EXPECT_EQ( network_stack.link_status(), test_case.link_status );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::link_mode() works
 *        properly.
 */
TEST( linkMode, worksProperly )
{
    struct {
        std::uint8_t phycfgr_dpx;
        Link_Mode    link_mode;
    } const test_cases[]{
        // clang-format off

        { 0b0'0'000'0'0'0, Link_Mode::HALF_DUPLEX },
        { 0b0'0'000'1'0'0, Link_Mode::FULL_DUPLEX },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver             = Mock_Driver{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto const network_stack = Network_Stack{ driver,
                                                  random<Mock_Error>(),
                                                  tcp_port_allocator.handle() };

        EXPECT_CALL( driver, read_phycfgr() )
            .WillOnce( Return( ( random<std::uint8_t>() & 0b1'1'111'0'1'1 ) | test_case.phycfgr_dpx ) );

        EXPECT_EQ( network_stack.link_mode(), test_case.link_mode );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::link_speed() works
 *        properly.
 */
TEST( linkSpeed, worksProperly )
{
    struct {
        std::uint8_t phycfgr_lnk;
        Link_Speed   link_speed;
    } const test_cases[]{
        // clang-format off

        { 0b0'0'000'0'0'0, Link_Speed::_10_MbPs  },
        { 0b0'0'000'0'1'0, Link_Speed::_100_MbPs },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver             = Mock_Driver{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto const network_stack = Network_Stack{ driver,
                                                  random<Mock_Error>(),
                                                  tcp_port_allocator.handle() };

        EXPECT_CALL( driver, read_phycfgr() )
            .WillOnce( Return( ( random<std::uint8_t>() & 0b1'1'111'1'0'1 ) | test_case.phycfgr_lnk ) );

        EXPECT_EQ( network_stack.link_speed(), test_case.link_speed );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::configure_ping_blocking()
 *        works properly.
 */
TEST( configurePingBlocking, worksProperly )
{
    struct {
        Ping_Blocking ping_blocking_configuration;
        std::uint8_t  mr_pb;
    } const test_cases[]{
        // clang-format off

        { Ping_Blocking::DISABLED, 0b0'0'0'0'0'0'0'0 },
        { Ping_Blocking::ENABLED,  0b0'0'0'1'0'0'0'0 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const in_sequence = InSequence{};

        auto driver             = Mock_Driver{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto network_stack = Network_Stack{ driver,
                                            random<Mock_Error>(),
                                            tcp_port_allocator.handle() };

        auto const mr = random<std::uint8_t>();

        EXPECT_CALL( driver, read_mr() ).WillOnce( Return( mr ) );
        EXPECT_CALL( driver, write_mr( ( mr & 0b1'1'1'0'1'1'1'1 ) | test_case.mr_pb ) );

        network_stack.configure_ping_blocking( test_case.ping_blocking_configuration );
    } // for
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::Network_Stack::ping_blocking_configuration()
 *        works properly.
 */
TEST( pingBlockingConfiguration, worksProperly )
{
    struct {
        std::uint8_t  mr_pb;
        Ping_Blocking ping_blocking_configuration;
    } const test_cases[]{
        // clang-format off

        { 0b0'0'0'0'0'0'0'0, Ping_Blocking::DISABLED },
        { 0b0'0'0'1'0'0'0'0, Ping_Blocking::ENABLED  },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver             = Mock_Driver{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto const network_stack = Network_Stack{ driver,
                                                  random<Mock_Error>(),
                                                  tcp_port_allocator.handle() };

        EXPECT_CALL( driver, read_mr() )
            .WillOnce( Return( ( random<std::uint8_t>() & 0b1'1'1'0'1'1'1'1 ) | test_case.mr_pb ) );

        EXPECT_EQ( network_stack.ping_blocking_configuration(), test_case.ping_blocking_configuration );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::configure_arp_forcing()
 *        works properly.
 */
TEST( configureARPForcing, worksProperly )
{
    struct {
        ARP_Forcing  arp_forcing_configuration;
        std::uint8_t mr_farp;
    } const test_cases[]{
        // clang-format off

        { ARP_Forcing::DISABLED, 0b0'0'0'0'0'0'0'0 },
        { ARP_Forcing::ENABLED,  0b0'0'0'0'0'0'1'0 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const in_sequence = InSequence{};

        auto driver             = Mock_Driver{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto network_stack = Network_Stack{ driver,
                                            random<Mock_Error>(),
                                            tcp_port_allocator.handle() };

        auto const mr = random<std::uint8_t>();

        EXPECT_CALL( driver, read_mr() ).WillOnce( Return( mr ) );
        EXPECT_CALL( driver, write_mr( ( mr & 0b1'1'1'1'1'1'0'1 ) | test_case.mr_farp ) );

        network_stack.configure_arp_forcing( test_case.arp_forcing_configuration );
    } // for
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::Network_Stack::arp_forcing_configuration() works
 *        properly.
 */
TEST( arpForcingConfiguration, worksProperly )
{
    struct {
        std::uint8_t mr_farp;
        ARP_Forcing  arp_forcing_configuration;
    } const test_cases[]{
        // clang-format off

        { 0b0'0'0'0'0'0'0'0, ARP_Forcing::DISABLED },
        { 0b0'0'0'0'0'0'1'0, ARP_Forcing::ENABLED  },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver             = Mock_Driver{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto const network_stack = Network_Stack{ driver,
                                                  random<Mock_Error>(),
                                                  tcp_port_allocator.handle() };

        EXPECT_CALL( driver, read_mr() )
            .WillOnce( Return( ( random<std::uint8_t>() & 0b1'1'1'1'1'1'0'1 ) | test_case.mr_farp ) );

        EXPECT_EQ( network_stack.arp_forcing_configuration(), test_case.arp_forcing_configuration );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::configure_retransmission()
 *        works properly.
 */
TEST( configureRetransmission, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>(), tcp_port_allocator.handle() };

    auto const retry_time  = random<std::uint16_t>();
    auto const retry_count = random<std::uint8_t>();

    EXPECT_CALL( driver, write_rtr( retry_time ) );
    EXPECT_CALL( driver, write_rcr( retry_count ) );

    network_stack.configure_retransmission( retry_time, retry_count );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::retry_time() works
 *        properly.
 */
TEST( retryTime, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              random<Mock_Error>(),
                                              tcp_port_allocator.handle() };

    auto const rtr = random<std::uint16_t>();

    EXPECT_CALL( driver, read_rtr() ).WillOnce( Return( rtr ) );

    EXPECT_EQ( network_stack.retry_time(), rtr );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::retry_count() works
 *        properly.
 */
TEST( retryCount, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              random<Mock_Error>(),
                                              tcp_port_allocator.handle() };

    auto const rcr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_rcr() ).WillOnce( Return( rcr ) );

    EXPECT_EQ( network_stack.retry_count(), rcr );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::Network_Stack::configure_socket_buffer_size()
 *        works properly.
 */
TEST( configureSocketBufferSize, worksProperly )
{
    Socket_ID const socket_id[]{
        // clang-format off

        Socket_ID::_0,
        Socket_ID::_1,
        Socket_ID::_2,
        Socket_ID::_3,
        Socket_ID::_4,
        Socket_ID::_5,
        Socket_ID::_6,
        Socket_ID::_7,

        // clang-format on
    };

    struct {
        Socket_Buffer_Size socket_buffer_size;
        std::uint8_t       sn_xxbuf_size;
        std::uint_fast8_t  sockets;
    } const test_cases[]{
        // clang-format off

        { Socket_Buffer_Size::_2_KiB,  2,  8 },
        { Socket_Buffer_Size::_4_KiB,  4,  4 },
        { Socket_Buffer_Size::_8_KiB,  8,  2 },
        { Socket_Buffer_Size::_16_KiB, 16, 1 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const in_sequence = InSequence{};

        auto driver             = Mock_Driver{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto network_stack = Network_Stack{ driver,
                                            random<Mock_Error>(),
                                            tcp_port_allocator.handle() };

        for ( auto socket = std::uint_fast8_t{ 0 }; socket < test_case.sockets; ++socket ) {
            EXPECT_CALL( driver, write_sn_rxbuf_size( socket_id[ socket ], test_case.sn_xxbuf_size ) );
            EXPECT_CALL( driver, write_sn_txbuf_size( socket_id[ socket ], test_case.sn_xxbuf_size ) );
        } // for

        for ( auto socket = test_case.sockets; socket < 8; ++socket ) {
            EXPECT_CALL( driver, write_sn_rxbuf_size( socket_id[ socket ], 0 ) );
            EXPECT_CALL( driver, write_sn_txbuf_size( socket_id[ socket ], 0 ) );
        } // for

        network_stack.configure_socket_buffer_size( test_case.socket_buffer_size );

        EXPECT_EQ( network_stack.socket_buffer_size(), test_case.socket_buffer_size );
        EXPECT_EQ( network_stack.sockets(), test_case.sockets );
        EXPECT_EQ( network_stack.sockets_available_for_allocation(), test_case.sockets );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::configure_mac_address()
 *        works properly.
 */
TEST( configureMACAddress, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>(), tcp_port_allocator.handle() };

    auto const address = random<MAC_Address>();

    EXPECT_CALL( driver, write_shar( address.as_byte_array() ) );

    network_stack.configure_mac_address( address );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::configure_mac_address()
 *        works properly.
 */
TEST( macAddress, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              random<Mock_Error>(),
                                              tcp_port_allocator.handle() };

    auto const shar = random_array<std::uint8_t, 6>();

    EXPECT_CALL( driver, read_shar() ).WillOnce( Return( shar ) );

    EXPECT_EQ( network_stack.mac_address().as_byte_array(), shar );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::configure_ipv4_address()
 *        works properly.
 */
TEST( configureIPv4Address, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>(), tcp_port_allocator.handle() };

    auto const address = random<IPv4_Address>();

    EXPECT_CALL( driver, write_sipr( address.as_byte_array() ) );

    network_stack.configure_ipv4_address( address );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::ipv4_address() works
 *        properly.
 */
TEST( ipv4Address, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              random<Mock_Error>(),
                                              tcp_port_allocator.handle() };

    auto const sipr = random_array<std::uint8_t, 4>();

    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( sipr ) );

    EXPECT_EQ( network_stack.ipv4_address().as_byte_array(), sipr );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::Network_Stack::configure_gateway_ipv4_address()
 *        works properly.
 */
TEST( configureGatewayIPv4Address, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>(), tcp_port_allocator.handle() };

    auto const address = random<IPv4_Address>();

    EXPECT_CALL( driver, write_gar( address.as_byte_array() ) );

    network_stack.configure_gateway_ipv4_address( address );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::gateway_ipv4_address()
 *        works properly.
 */
TEST( gatewayIPv4Address, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              random<Mock_Error>(),
                                              tcp_port_allocator.handle() };

    auto const gar = random_array<std::uint8_t, 4>();

    EXPECT_CALL( driver, read_gar() ).WillOnce( Return( gar ) );

    EXPECT_EQ( network_stack.gateway_ipv4_address().as_byte_array(), gar );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::Network_Stack::configure_ipv4_subnet_mask()
 *        works properly.
 */
TEST( configureIPv4SubnetMask, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>(), tcp_port_allocator.handle() };

    auto const subnet_mask = random<IPv4_Address>();

    EXPECT_CALL( driver, write_subr( subnet_mask.as_byte_array() ) );

    network_stack.configure_ipv4_subnet_mask( subnet_mask );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::ipv4_subnet_mask() works
 *        properly.
 */
TEST( ipv4SubnetMask, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              random<Mock_Error>(),
                                              tcp_port_allocator.handle() };

    auto const subr = random_array<std::uint8_t, 4>();

    EXPECT_CALL( driver, read_subr() ).WillOnce( Return( subr ) );

    EXPECT_EQ( network_stack.ipv4_subnet_mask().as_byte_array(), subr );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::Network_Stack::configure_interrupt_assert_wait_time()
 *        works properly.
 */
TEST( configureInterruptAssertWaitTime, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>(), tcp_port_allocator.handle() };

    auto const interrupt_assert_wait_time = random<std::uint16_t>();

    EXPECT_CALL( driver, write_intlevel( interrupt_assert_wait_time ) );

    network_stack.configure_interrupt_assert_wait_time( interrupt_assert_wait_time );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::Network_Stack::interrupt_assert_wait_time()
 *        works properly.
 */
TEST( interruptAssertWaitTime, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              random<Mock_Error>(),
                                              tcp_port_allocator.handle() };

    auto const intlevel = random<std::uint16_t>();

    EXPECT_CALL( driver, read_intlevel() ).WillOnce( Return( intlevel ) );

    EXPECT_EQ( network_stack.interrupt_assert_wait_time(), intlevel );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::enable_interrupts() works
 *        properly.
 */
TEST( enableInterrupts, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>(), tcp_port_allocator.handle() };

    auto const imr  = random<std::uint8_t>();
    auto const mask = random<std::uint8_t>();

    EXPECT_CALL( driver, read_imr() ).WillOnce( Return( imr ) );
    EXPECT_CALL( driver, write_imr( imr | mask ) );

    network_stack.enable_interrupts( mask );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::disable_interrupts(
 *        std::uint8_t ) works properly.
 */
TEST( disableInterrupts, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>(), tcp_port_allocator.handle() };

    auto const imr  = random<std::uint8_t>();
    auto const mask = random<std::uint8_t>();

    EXPECT_CALL( driver, read_imr() ).WillOnce( Return( imr ) );
    EXPECT_CALL( driver, write_imr( imr & ~mask ) );

    network_stack.disable_interrupts( mask );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::disable_interrupts() works
 *        properly.
 */
TEST( disableAllInterrupts, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>(), tcp_port_allocator.handle() };

    EXPECT_CALL( driver, write_imr( 0x00 ) );

    network_stack.disable_interrupts();
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::enabled_interrupts() works
 *        properly.
 */
TEST( enabledInterrupts, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              random<Mock_Error>(),
                                              tcp_port_allocator.handle() };

    auto const imr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_imr() ).WillOnce( Return( imr ) );

    EXPECT_EQ( network_stack.enabled_interrupts(), imr );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::interrupt_context() works
 *        properly.
 */
TEST( interruptContext, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              random<Mock_Error>(),
                                              tcp_port_allocator.handle() };

    auto const ir = random<std::uint8_t>();

    EXPECT_CALL( driver, read_ir() ).WillOnce( Return( ir ) );

    EXPECT_EQ( network_stack.interrupt_context(), ir );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::clear_interrupts() works
 *        properly.
 */
TEST( clearInterrupts, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>(), tcp_port_allocator.handle() };

    auto const mask = random<std::uint8_t>();

    EXPECT_CALL( driver, write_ir( mask ) );

    network_stack.clear_interrupts( mask );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::enable_socket_interrupts()
 *        works properly.
 */
TEST( enableSocketInterrupts, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>(), tcp_port_allocator.handle() };

    EXPECT_CALL( driver, write_simr( 0xFF ) );

    network_stack.enable_socket_interrupts();
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::Network_Stack::disable_socket_interrupts() works
 *        properly.
 */
TEST( disableSocketInterrupts, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>(), tcp_port_allocator.handle() };

    EXPECT_CALL( driver, write_simr( 0x00 ) );

    network_stack.disable_socket_interrupts();
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::Network_Stack::socket_interrupts_are_enabled()
 *        works properly.
 */
TEST( socketInterruptsAreEnabled, worksProperly )
{
    struct {
        std::uint8_t simr;
        bool         socket_interrupts_are_enabled;
    } const test_cases[]{
        // clang-format off

        {                       0x00,   false },
        { random<std::uint8_t>( 0x01 ), true  },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver             = Mock_Driver{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto const network_stack = Network_Stack{ driver,
                                                  random<Mock_Error>(),
                                                  tcp_port_allocator.handle() };

        EXPECT_CALL( driver, read_simr() ).WillOnce( Return( test_case.simr ) );

        EXPECT_EQ( network_stack.socket_interrupts_are_enabled(), test_case.socket_interrupts_are_enabled );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::socket_interrupt_context()
 *        works properly.
 */
TEST( socketInterruptContext, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              random<Mock_Error>(),
                                              tcp_port_allocator.handle() };

    auto const sir = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sir() ).WillOnce( Return( sir ) );

    EXPECT_EQ( network_stack.socket_interrupt_context(), sir );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::unreachable_ipv4_address()
 *        works properly.
 */
TEST( unreachableIPv4Address, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              random<Mock_Error>(),
                                              tcp_port_allocator.handle() };

    auto const uipr = random_array<std::uint8_t, 4>();

    EXPECT_CALL( driver, read_uipr() ).WillOnce( Return( uipr ) );

    EXPECT_EQ( network_stack.unreachable_ipv4_address().as_byte_array(), uipr );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::unreachable_port() works
 *        properly.
 */
TEST( unreachablePort, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              random<Mock_Error>(),
                                              tcp_port_allocator.handle() };

    auto const uportr = random<std::uint16_t>();

    EXPECT_CALL( driver, read_uportr() ).WillOnce( Return( uportr ) );

    EXPECT_EQ( network_stack.unreachable_port().as_unsigned_integer(), uportr );
}

/**
 * \brief Execute the picolibrary::WIZnet::W5500::IP::Network_Stack automated tests.
 *
 * \param[in] argc The number of arguments to pass to testing::InitGoogleMock().
 * \param[in] argc The array of arguments to pass to testing::InitGoogleMock().
 *
 * \return See Google Test's RUN_ALL_TESTS().
 */
int main( int argc, char * argv[] )
{
    ::testing::InitGoogleMock( &argc, argv );

    return RUN_ALL_TESTS();
}

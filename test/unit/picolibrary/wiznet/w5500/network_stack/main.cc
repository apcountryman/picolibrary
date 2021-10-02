/**
 * picolibrary
 *
 * Copyright 2020-2021, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::WIZnet::W5500::Network_Stack unit test program.
 */

#include <algorithm>
#include <cstddef>
#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/fixed_size_array.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/mac_address.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/ip.h"
#include "picolibrary/testing/unit/ipv4.h"
#include "picolibrary/testing/unit/mac_address.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/wiznet/w5500.h"
#include "picolibrary/void.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/network_stack.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Fixed_Size_Array;
using ::picolibrary::Generic_Error;
using ::picolibrary::MAC_Address;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::WIZnet::W5500::Mock_Driver;
using ::picolibrary::WIZnet::W5500::ARP_Forcing;
using ::picolibrary::WIZnet::W5500::Buffer_Size;
using ::picolibrary::WIZnet::W5500::Link_Mode;
using ::picolibrary::WIZnet::W5500::Link_Speed;
using ::picolibrary::WIZnet::W5500::Link_Status;
using ::picolibrary::WIZnet::W5500::Network_Stack;
using ::picolibrary::WIZnet::W5500::PHY_Mode;
using ::picolibrary::WIZnet::W5500::Ping_Blocking;
using ::picolibrary::WIZnet::W5500::Socket_ID;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

using IPv4_Address = ::picolibrary::IPv4::Address;
using TCP_Port     = ::picolibrary::IP::TCP::Port;

template<typename T, std::size_t N>
auto random_fixed_size_array()
{
    Fixed_Size_Array<T, N> array;

    std::generate( array.begin(), array.end(), []() { return random<T>(); } );

    return array;
}

} // namespace

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::ping_w5500() properly handles
 *        a VERSIONR register read error.
 */
TEST( pingW5500, versionrReadError )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_versionr() ).WillOnce( Return( error ) );

    auto const result = network_stack.ping_w5500();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::ping_w5500() properly handles
 *        an incorrect chip version.
 */
TEST( pingW5500, incorrectChipVersion )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    EXPECT_CALL( driver, read_versionr() )
        .WillOnce( Return(
            random<bool>() ? random<std::uint8_t>( 0x00, 0x03 ) : random<std::uint8_t>( 0x05 ) ) );

    auto const result = network_stack.ping_w5500();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::NONRESPONSIVE_DEVICE );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::ping_w5500() works properly.
 */
TEST( pingW5500, worksProperly )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    EXPECT_CALL( driver, read_versionr() ).WillOnce( Return( static_cast<std::uint8_t>( 0x04 ) ) );

    EXPECT_FALSE( network_stack.ping_w5500().is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_phy() properly
 *        handles a PHYCFGR register write error when writing the PHY configuration.
 */
TEST( configurePHY, phycfgrWriteErrorPHYConfiguration )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_phycfgr( _ ) ).WillOnce( Return( error ) );

    auto const result = network_stack.configure_phy( random<PHY_Mode>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_phy() properly
 *        handles a PHYCFGR register write error when entering PHY reset.
 */
TEST( configurePHY, phycfgrWriteErrorEnterPHYReset )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_phycfgr( _ ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) )
        .WillOnce( Return( error ) );

    auto const result = network_stack.configure_phy( random<PHY_Mode>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_phy() properly
 *        handles a PHYCFGR register write error when exiting PHY reset.
 */
TEST( configurePHY, phycfgrWriteErrorExitPHYReset )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_phycfgr( _ ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) )
        .WillOnce( Return( error ) );

    auto const result = network_stack.configure_phy( random<PHY_Mode>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_phy() works
 *        properly.
 */
TEST( configurePHY, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const phy_mode = random<PHY_Mode>();

    EXPECT_CALL( driver, write_phycfgr( static_cast<std::uint8_t>( phy_mode ) | 0b1'0'000'0'0'0 ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_phycfgr( static_cast<std::uint8_t>( phy_mode ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_phycfgr( static_cast<std::uint8_t>( phy_mode ) | 0b1'0'000'0'0'0 ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( network_stack.configure_phy( phy_mode ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::phy_mode() properly handles a
 *        PHYCFGR register read error.
 */
TEST( phyMode, phycfgrReadError )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_phycfgr() ).WillOnce( Return( error ) );

    auto const result = network_stack.phy_mode();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::phy_mode() works properly.
 */
TEST( phyMode, worksProperly )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const phycfgr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_phycfgr() ).WillOnce( Return( phycfgr ) );

    auto const result = network_stack.phy_mode();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), static_cast<PHY_Mode>( phycfgr & 0b0'1'111'0'0'0 ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::link_status() properly handles
 *        a PHYCFGR register read error.
 */
TEST( linkStatus, phycfgrReadError )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_phycfgr() ).WillOnce( Return( error ) );

    auto const result = network_stack.link_status();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::link_status() works properly.
 */
TEST( linkStatus, worksProperly )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const phycfgr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_phycfgr() ).WillOnce( Return( phycfgr ) );

    auto const result = network_stack.link_status();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), static_cast<Link_Status>( phycfgr & 0b0'0'000'0'0'1 ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::link_mode() properly handles a
 *        PHYCFGR register read error.
 */
TEST( linkMode, phycfgrReadError )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_phycfgr() ).WillOnce( Return( error ) );

    auto const result = network_stack.link_mode();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::link_mode() works properly.
 */
TEST( linkMode, worksProperly )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const phycfgr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_phycfgr() ).WillOnce( Return( phycfgr ) );

    auto const result = network_stack.link_mode();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), static_cast<Link_Mode>( phycfgr & 0b0'0'000'1'0'0 ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::link_speed() properly handles
 *        a PHYCFGR register read error.
 */
TEST( linkSpeed, phycfgrReadError )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_phycfgr() ).WillOnce( Return( error ) );

    auto const result = network_stack.link_speed();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::link_speed() works properly.
 */
TEST( linkSpeed, worksProperly )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const phycfgr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_phycfgr() ).WillOnce( Return( phycfgr ) );

    auto const result = network_stack.link_speed();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), static_cast<Link_Speed>( phycfgr & 0b0'0'000'0'1'0 ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_ping_blocking()
 *        properly handles an MR register read error.
 */
TEST( configurePingBlocking, mrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_mr() ).WillOnce( Return( error ) );

    auto const result = network_stack.configure_ping_blocking( random<Ping_Blocking>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_ping_blocking()
 *        properly handles an MR register write error.
 */
TEST( configurePingBlocking, mrWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_mr() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_mr( _ ) ).WillOnce( Return( error ) );

    auto const result = network_stack.configure_ping_blocking( random<Ping_Blocking>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_ping_blocking()
 *        works properly.
 */
TEST( configurePingBlocking, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const mr                          = random<std::uint8_t>();
    auto const ping_blocking_configuration = random<Ping_Blocking>();

    EXPECT_CALL( driver, read_mr() ).WillOnce( Return( mr ) );
    EXPECT_CALL( driver, write_mr( ( mr & 0b1'1'1'0'1'1'1'1 ) | static_cast<std::uint8_t>( ping_blocking_configuration ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( network_stack.configure_ping_blocking( ping_blocking_configuration ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::ping_blocking_configuration()
 *        properly handles an MR register read error.
 */
TEST( pingBlockingConfiguration, mrReadError )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_mr() ).WillOnce( Return( error ) );

    auto const result = network_stack.ping_blocking_configuration();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::ping_blocking_configuration()
 *        works properly.
 */
TEST( pingBlockingConfiguration, worksProperly )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const mr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_mr() ).WillOnce( Return( mr ) );

    auto const result = network_stack.ping_blocking_configuration();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), static_cast<Ping_Blocking>( mr & 0b0'0'0'1'0'0'0'0 ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_arp_forcing()
 *        properly handles an MR register read error.
 */
TEST( configureARPForcing, mrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_mr() ).WillOnce( Return( error ) );

    auto const result = network_stack.configure_arp_forcing( random<ARP_Forcing>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_arp_forcing()
 *        properly handles an MR register write error.
 */
TEST( configureARPForcing, mrWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_mr() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_mr( _ ) ).WillOnce( Return( error ) );

    auto const result = network_stack.configure_arp_forcing( random<ARP_Forcing>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_arp_forcing() works
 *        properly.
 */
TEST( configureARPForcing, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const mr                        = random<std::uint8_t>();
    auto const arp_forcing_configuration = random<ARP_Forcing>();

    EXPECT_CALL( driver, read_mr() ).WillOnce( Return( mr ) );
    EXPECT_CALL( driver, write_mr( ( mr & 0b1'1'1'1'1'1'0'1 ) | static_cast<std::uint8_t>( arp_forcing_configuration ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( network_stack.configure_arp_forcing( arp_forcing_configuration ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::arp_forcing_configuration()
 *        properly handles an MR register read error.
 */
TEST( arpForcingConfiguration, mrReadError )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_mr() ).WillOnce( Return( error ) );

    auto const result = network_stack.arp_forcing_configuration();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::arp_forcing_configuration()
 *        works properly.
 */
TEST( arpForcingConfiguration, worksProperly )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const mr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_mr() ).WillOnce( Return( mr ) );

    auto const result = network_stack.arp_forcing_configuration();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), static_cast<ARP_Forcing>( mr & 0b0'0'0'0'0'0'1'0 ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_retransmission()
 *        properly handles an RTR register write error.
 */
TEST( configureRetransmission, rtrWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_rtr( _ ) ).WillOnce( Return( error ) );

    auto const result = network_stack.configure_retransmission(
        random<std::uint16_t>(), random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_retransmission()
 *        properly handles an RCR register write error.
 */
TEST( configureRetransmission, rcrWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_rtr( _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_rcr( _ ) ).WillOnce( Return( error ) );

    auto const result = network_stack.configure_retransmission(
        random<std::uint16_t>(), random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_retransmission()
 *        works properly.
 */
TEST( configureRetransmission, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const retry_time  = random<std::uint16_t>();
    auto const retry_count = random<std::uint8_t>();

    EXPECT_CALL( driver, write_rtr( retry_time ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_rcr( retry_count ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( network_stack.configure_retransmission( retry_time, retry_count ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::retry_time() properly handles
 *        an RTR register read error.
 */
TEST( retryTime, rtrReadError )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_rtr() ).WillOnce( Return( error ) );

    auto const result = network_stack.retry_time();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::retry_time() works properly.
 */
TEST( retryTime, worksProperly )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const rtr = random<std::uint16_t>();

    EXPECT_CALL( driver, read_rtr() ).WillOnce( Return( rtr ) );

    auto const result = network_stack.retry_time();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), rtr );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::retry_count() properly handles
 *        an RCR register read error.
 */
TEST( retryCount, rcrReadError )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_rcr() ).WillOnce( Return( error ) );

    auto const result = network_stack.retry_count();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::retry_count() works properly.
 */
TEST( retryCount, worksProperly )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const rcr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_rcr() ).WillOnce( Return( rcr ) );

    auto const result = network_stack.retry_count();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), rcr );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_socket_buffers()
 *        properly handles an insufficient socket buffer size.
 */
TEST( configureSocketBuffers, insufficientBufferSize )
{
    Buffer_Size const insufficient_buffer_sizes[]{
        Buffer_Size::_0_KIB,
        Buffer_Size::_1_KIB,
    };

    for ( auto const insufficient_buffer_size : insufficient_buffer_sizes ) {
        auto driver = Mock_Driver{};

        auto network_stack = Network_Stack{ driver };

        EXPECT_CALL( driver, write_sn_txbuf_size( _, _ ) ).Times( 0 );
        EXPECT_CALL( driver, write_sn_rxbuf_size( _, _ ) ).Times( 0 );

        auto const result = network_stack.configure_socket_buffers( insufficient_buffer_size );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::INVALID_ARGUMENT );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_socket_buffers()
 *        properly handles an invalid socket buffer size.
 */
TEST( configureSocketBuffers, invalidBufferSize )
{
    Buffer_Size const invalid_buffer_sizes[]{
        static_cast<Buffer_Size>( 3 ),
        static_cast<Buffer_Size>( random<std::uint8_t>( 5, 7 ) ),
        static_cast<Buffer_Size>( random<std::uint8_t>( 9, 15 ) ),
        static_cast<Buffer_Size>( random<std::uint8_t>( 17 ) ),
    };

    for ( auto const invalid_buffer_size : invalid_buffer_sizes ) {
        auto driver = Mock_Driver{};

        auto network_stack = Network_Stack{ driver };

        EXPECT_CALL( driver, write_sn_txbuf_size( _, _ ) ).Times( 0 );
        EXPECT_CALL( driver, write_sn_rxbuf_size( _, _ ) ).Times( 0 );

        auto const result = network_stack.configure_socket_buffers( invalid_buffer_size );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::INVALID_ARGUMENT );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_socket_buffers()
 *        properly handles an SN_RXBUF_SIZE register write error when configuring the
 *        socket buffer size of an available socket.
 */
TEST( configureSocketBuffers, writeSNRXBUFSIZEErrorAvailableSocket )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_rxbuf_size( _, _ ) ).WillOnce( Return( error ) );
    EXPECT_CALL( driver, write_sn_txbuf_size( _, _ ) ).Times( 0 );

    auto const result = network_stack.configure_socket_buffers(
        random<Buffer_Size>( Buffer_Size::_2_KIB ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_socket_buffers()
 *        properly handles an SN_TXBUF_SIZE register write error when configuring the
 *        socket buffer size of an available socket.
 */
TEST( configureSocketBuffers, writeSNTXBUFSIZEErrorAvailableSocket )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_rxbuf_size( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_txbuf_size( _, _ ) ).WillOnce( Return( error ) );

    auto const result = network_stack.configure_socket_buffers(
        random<Buffer_Size>( Buffer_Size::_2_KIB ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_socket_buffers()
 *        properly handles an SN_RXBUF_SIZE register write error when configuring the
 *        socket buffer size of an unused socket.
 */
TEST( configureSocketBuffers, writeSNRXBUFSIZEErrorUnusedSocket )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const buffer_size = random<Buffer_Size>( Buffer_Size::_4_KIB );

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_rxbuf_size( _, static_cast<std::uint8_t>( buffer_size ) ) )
        .WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_txbuf_size( _, static_cast<std::uint8_t>( buffer_size ) ) )
        .WillRepeatedly( Return( Result<Void, Error_Code>{} ) );

    EXPECT_CALL( driver, write_sn_rxbuf_size( _, static_cast<std::uint8_t>( Buffer_Size::_0_KIB ) ) )
        .WillOnce( Return( error ) );
    EXPECT_CALL( driver, write_sn_txbuf_size( _, static_cast<std::uint8_t>( Buffer_Size::_0_KIB ) ) )
        .Times( 0 );

    auto const result = network_stack.configure_socket_buffers( buffer_size );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_socket_buffers()
 *        properly handles an SN_TXBUF_SIZE register write error when configuring the
 *        socket buffer size of an unused socket.
 */
TEST( configureSocketBuffers, writeSNTXBUFSIZEErrorUnusedSocket )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const buffer_size = random<Buffer_Size>( Buffer_Size::_4_KIB );

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_rxbuf_size( _, static_cast<std::uint8_t>( buffer_size ) ) )
        .WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_txbuf_size( _, static_cast<std::uint8_t>( buffer_size ) ) )
        .WillRepeatedly( Return( Result<Void, Error_Code>{} ) );

    EXPECT_CALL( driver, write_sn_rxbuf_size( _, static_cast<std::uint8_t>( Buffer_Size::_0_KIB ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_txbuf_size( _, static_cast<std::uint8_t>( Buffer_Size::_0_KIB ) ) )
        .WillOnce( Return( error ) );

    auto const result = network_stack.configure_socket_buffers( buffer_size );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_socket_buffers()
 *        works properly.
 */
TEST( configureSocketBuffers, worksProperly )
{
    struct {
        Buffer_Size  buffer_size;
        std::uint8_t available_sockets;
    } const configurations[]{
        { Buffer_Size::_2_KIB, 8 },
        { Buffer_Size::_4_KIB, 4 },
        { Buffer_Size::_8_KIB, 2 },
        { Buffer_Size::_16_KIB, 1 },
    };

    for ( auto const configuration : configurations ) {
        auto const in_sequence = InSequence{};

        auto driver = Mock_Driver{};

        auto network_stack = Network_Stack{ driver };

        for ( auto socket = std::uint8_t{}; socket < configuration.available_sockets; ++socket ) {
            EXPECT_CALL(
                driver,
                write_sn_rxbuf_size(
                    static_cast<Socket_ID>( socket ),
                    static_cast<std::uint8_t>( configuration.buffer_size ) ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL(
                driver,
                write_sn_txbuf_size(
                    static_cast<Socket_ID>( socket ),
                    static_cast<std::uint8_t>( configuration.buffer_size ) ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
        } // for

        for ( auto socket = configuration.available_sockets; socket < 8; ++socket ) {
            EXPECT_CALL(
                driver,
                write_sn_rxbuf_size(
                    static_cast<Socket_ID>( socket ), static_cast<std::uint8_t>( Buffer_Size::_0_KIB ) ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL(
                driver,
                write_sn_txbuf_size(
                    static_cast<Socket_ID>( socket ), static_cast<std::uint8_t>( Buffer_Size::_0_KIB ) ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
        } // for

        EXPECT_FALSE( network_stack.configure_socket_buffers( configuration.buffer_size ).is_error() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::socket_buffer_size() properly
 *        handles an SN_RXBUF_SIZE register read error.
 */
TEST( socketBufferSize, snrxbufsizeReadError )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_rxbuf_size( _ ) ).WillOnce( Return( error ) );

    auto const result = network_stack.socket_buffer_size();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::socket_buffer_size() works
 *        properly.
 */
TEST( socketBufferSize, worksProperly )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const sn_rxbuf_size = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_rxbuf_size( Socket_ID::_0 ) ).WillOnce( Return( sn_rxbuf_size ) );

    auto const result = network_stack.socket_buffer_size();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), static_cast<Buffer_Size>( sn_rxbuf_size ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_mac_address()
 *        properly handles a SHAR register write error.
 */
TEST( configureMACAddress, sharWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_shar( _ ) ).WillOnce( Return( error ) );

    auto const result = network_stack.configure_mac_address( random<MAC_Address>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_mac_address() works
 *        properly.
 */
TEST( configureMACAddress, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const address = random<MAC_Address>();

    EXPECT_CALL( driver, write_shar( address.as_byte_array() ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( network_stack.configure_mac_address( address ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::mac_address() properly handles
 *        a SHAR register read error.
 */
TEST( macAddress, sharReadError )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_shar() ).WillOnce( Return( error ) );

    auto const result = network_stack.mac_address();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::mac_address() works properly.
 */
TEST( macAddress, worksProperly )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const shar = random_fixed_size_array<std::uint8_t, 6>();

    EXPECT_CALL( driver, read_shar() ).WillOnce( Return( shar ) );

    auto const result = network_stack.mac_address();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value().as_byte_array(), shar );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_ip_address()
 *        properly handles a SIPR register write error.
 */
TEST( configureIPAddress, siprWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sipr( _ ) ).WillOnce( Return( error ) );

    auto const result = network_stack.configure_ip_address( random<IPv4_Address>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_ip_address() works
 *        properly.
 */
TEST( configureIPAddress, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const address = random<IPv4_Address>();

    EXPECT_CALL( driver, write_sipr( address.as_byte_array() ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( network_stack.configure_ip_address( address ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::ip_address() properly handles
 *        a SIPR register read error.
 */
TEST( ipAddress, siprReadError )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( error ) );

    auto const result = network_stack.ip_address();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::ip_address() works properly.
 */
TEST( ipAddress, worksProperly )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const sipr = random_fixed_size_array<std::uint8_t, 4>();

    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( sipr ) );

    auto const result = network_stack.ip_address();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value().as_byte_array(), sipr );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_gateway_ip_address()
 *        properly handles a GAR register write error.
 */
TEST( configureGatewayIPAddress, garWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_gar( _ ) ).WillOnce( Return( error ) );

    auto const result = network_stack.configure_gateway_ip_address( random<IPv4_Address>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_gateway_ip_address()
 *        works properly.
 */
TEST( configureGatewayIPAddress, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const address = random<IPv4_Address>();

    EXPECT_CALL( driver, write_gar( address.as_byte_array() ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( network_stack.configure_gateway_ip_address( address ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::gateway_ip_address() properly
 *        handles a GAR register read error.
 */
TEST( gatewayIPAddress, garReadError )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_gar() ).WillOnce( Return( error ) );

    auto const result = network_stack.gateway_ip_address();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::gateway_ip_address() works
 *        properly.
 */
TEST( gatewayIPAddress, worksProperly )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const gar = random_fixed_size_array<std::uint8_t, 4>();

    EXPECT_CALL( driver, read_gar() ).WillOnce( Return( gar ) );

    auto const result = network_stack.gateway_ip_address();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value().as_byte_array(), gar );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_subnet_mask()
 *        properly handles a SUBR register write error.
 */
TEST( configureSubnetMask, subrWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_subr( _ ) ).WillOnce( Return( error ) );

    auto const result = network_stack.configure_subnet_mask( random<IPv4_Address>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_subnet_mask() works
 *        properly.
 */
TEST( configureSubnetMask, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const subnet_mask = random<IPv4_Address>();

    EXPECT_CALL( driver, write_subr( subnet_mask.as_byte_array() ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( network_stack.configure_subnet_mask( subnet_mask ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::subnet_mask() properly handles
 *        a SUBR register read error.
 */
TEST( subnetMask, subrReadError )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_subr() ).WillOnce( Return( error ) );

    auto const result = network_stack.subnet_mask();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::subnet_mask() works properly.
 */
TEST( subnetMask, worksProperly )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const subr = random_fixed_size_array<std::uint8_t, 4>();

    EXPECT_CALL( driver, read_subr() ).WillOnce( Return( subr ) );

    auto const result = network_stack.subnet_mask();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value().as_byte_array(), subr );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::configure_interrupt_assert_wait_time()
 *        properly handles an INTLEVEL register write error.
 */
TEST( configureInterruptAssertWaitTime, intlevelWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_intlevel( _ ) ).WillOnce( Return( error ) );

    auto const result = network_stack.configure_interrupt_assert_wait_time( random<std::uint16_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::configure_interrupt_assert_wait_time()
 *        works properly.
 */
TEST( configureInterruptAssertWaitTime, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const interrupt_assert_wait_time = random<std::uint16_t>();

    EXPECT_CALL( driver, write_intlevel( interrupt_assert_wait_time ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE(
        network_stack.configure_interrupt_assert_wait_time( interrupt_assert_wait_time ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::interrupt_assert_wait_time()
 *        properly handles an INTLEVEL register read error.
 */
TEST( interruptAssertWaitTime, intlevelReadError )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_intlevel() ).WillOnce( Return( error ) );

    auto const result = network_stack.interrupt_assert_wait_time();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::interrupt_assert_wait_time()
 *        works properly.
 */
TEST( interruptAssertWaitTime, worksProperly )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const intlevel = random<std::uint16_t>();

    EXPECT_CALL( driver, read_intlevel() ).WillOnce( Return( intlevel ) );

    auto const result = network_stack.interrupt_assert_wait_time();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), intlevel );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::enable_interrupts() properly
 *        handles an IMR register read error.
 */
TEST( enableInterrupts, imrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_imr() ).WillOnce( Return( error ) );

    auto const result = network_stack.enable_interrupts( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::enable_interrupts() properly
 *        handles an IMR register write error.
 */
TEST( enableInterrupts, imrWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_imr() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_imr( _ ) ).WillOnce( Return( error ) );

    auto const result = network_stack.enable_interrupts( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::enable_interrupts() works
 *        properly.
 */
TEST( enableInterrupts, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const imr  = random<std::uint8_t>();
    auto const mask = random<std::uint8_t>();

    EXPECT_CALL( driver, read_imr() ).WillOnce( Return( imr ) );
    EXPECT_CALL( driver, write_imr( imr | mask ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( network_stack.enable_interrupts( mask ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::disable_interrupts(
 *        std::uint8_t ) properly handles an IMR register read error.
 */
TEST( disableInterrupts, imrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_imr() ).WillOnce( Return( error ) );

    auto const result = network_stack.disable_interrupts( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::disable_interrupts(
 *        std::uint8_t ) properly handles an IMR register write error.
 */
TEST( disableInterrupts, imrWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_imr() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_imr( _ ) ).WillOnce( Return( error ) );

    auto const result = network_stack.disable_interrupts( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::disable_interrupts(
 *        std::uint8_t ) works properly.
 */
TEST( disableInterrupts, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const imr  = random<std::uint8_t>();
    auto const mask = random<std::uint8_t>();

    EXPECT_CALL( driver, read_imr() ).WillOnce( Return( imr ) );
    EXPECT_CALL( driver, write_imr( imr & ~mask ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( network_stack.disable_interrupts( mask ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::disable_interrupts() properly
 *        handles an IMR register write error.
 */
TEST( disableAllInterrupts, imrWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_imr( _ ) ).WillOnce( Return( error ) );

    auto const result = network_stack.disable_interrupts();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::disable_interrupts() works
 *        properly.
 */
TEST( disableAllInterrupts, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    EXPECT_CALL( driver, write_imr( 0 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( network_stack.disable_interrupts().is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::enabled_interrupts() properly
 *        handles an IMR register read error.
 */
TEST( enabledInterrupts, imrReadError )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_imr() ).WillOnce( Return( error ) );

    auto const result = network_stack.enabled_interrupts();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::enabled_interrupts() works
 *        properly.
 */
TEST( enabledInterrupts, worksProperly )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const imr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_imr() ).WillOnce( Return( imr ) );

    auto const result = network_stack.enabled_interrupts();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), imr );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::interrupt_context() properly
 *        handles an IR register read error.
 */
TEST( interruptContext, irReadError )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_ir() ).WillOnce( Return( error ) );

    auto const result = network_stack.interrupt_context();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::interrupt_context() works
 *        properly.
 */
TEST( interruptContext, worksProperly )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const ir = random<std::uint8_t>();

    EXPECT_CALL( driver, read_ir() ).WillOnce( Return( ir ) );

    auto const result = network_stack.interrupt_context();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), ir );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::enable_socket_interrupts()
 *        properly handles a SIMR register write error.
 */
TEST( enableSocketInterrupts, simrWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_simr( _ ) ).WillOnce( Return( error ) );

    auto const result = network_stack.enable_socket_interrupts();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::enable_socket_interrupts()
 *        works properly.
 */
TEST( enableSocketInterrupts, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    EXPECT_CALL( driver, write_simr( 0xFF ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( network_stack.enable_socket_interrupts().is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::disable_socket_interrupts()
 *        properly handles a SIMR register write error.
 */
TEST( disableSocketInterrupts, simrWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_simr( _ ) ).WillOnce( Return( error ) );

    auto const result = network_stack.disable_socket_interrupts();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::disable_socket_interrupts()
 *        works properly.
 */
TEST( disableSocketInterrupts, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    EXPECT_CALL( driver, write_simr( 0x00 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( network_stack.disable_socket_interrupts().is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::enabled_socket_interrupts()
 *        properly handles a SIMR register read error.
 */
TEST( enabledSocketInterrupts, simrReadError )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_simr() ).WillOnce( Return( error ) );

    auto const result = network_stack.enabled_socket_interrupts();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::enabled_socket_interrupts()
 *        works properly.
 */
TEST( enabledSocketInterrupts, worksProperly )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const simr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_simr() ).WillOnce( Return( simr ) );

    auto const result = network_stack.enabled_socket_interrupts();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), simr );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::socket_interrupt_context()
 * properly handles a SIR register read error.
 */
TEST( socketInterruptContext, sirReadError )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sir() ).WillOnce( Return( error ) );

    auto const result = network_stack.socket_interrupt_context();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::socket_interrupt_context()
 *        works properly.
 */
TEST( socketInterruptContext, worksProperly )
{
    auto driver = Mock_Driver{};

    auto const network_stack = Network_Stack{ driver };

    auto const sir = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sir() ).WillOnce( Return( sir ) );

    auto const result = network_stack.socket_interrupt_context();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sir );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::service() works properly.
 */
TEST( service, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    EXPECT_FALSE( network_stack.service().is_error() );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::enable_tcp_ephemeral_port_allocation()
 *        properly handles TCP ephemeral port allocation already having been enabled.
 */
TEST( enableTCPEphemeralPortAllocation, alreadyEnabled )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const min = random<TCP_Port>( 1 );
    auto const max = random<TCP_Port>( min );

    EXPECT_FALSE( network_stack.enable_tcp_ephemeral_port_allocation( min, max ).is_error() );

    auto result = network_stack.enable_tcp_ephemeral_port_allocation( min, max );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::LOGIC_ERROR );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::enable_tcp_ephemeral_port_allocation()
 *        properly handles an invalid port range.
 */
TEST( enableTCPEphemeralPortAllocation, invalidPortRange )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const min = random<TCP_Port>( 1 );
    auto const max = random<TCP_Port>( 0, min.as_unsigned_integer() - 1 );

    auto const result = network_stack.enable_tcp_ephemeral_port_allocation( min, max );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::INVALID_ARGUMENT );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::enable_tcp_ephemeral_port_allocation()
 *        properly handles an invalid port range bound.
 */
TEST( enableTCPEphemeralPortAllocation, invalidPortRangeBound )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const result = network_stack.enable_tcp_ephemeral_port_allocation(
        0, random<TCP_Port>( 0 ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::INVALID_ARGUMENT );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::enable_tcp_ephemeral_port_allocation()
 *        works properly.
 */
TEST( enableTCPEphemeralPortAllocation, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const min = random<TCP_Port>( 1 );
    auto const max = random<TCP_Port>( min );

    EXPECT_FALSE( network_stack.enable_tcp_ephemeral_port_allocation( min, max ).is_error() );
}

/**
 * \brief Execute the picolibrary::WIZnet::W5500::Network_Stack unit tests.
 *
 * \param[in] argc The number of arguments to pass to testing::InitGoogleMock().
 * \param[in] argv The array  of arguments to pass to testing::InitGoogleMock().
 *
 * \return See Google Test's RUN_ALL_TESTS().
 */
int main( int argc, char * argv[] )
{
    ::testing::InitGoogleMock( &argc, argv );

    return RUN_ALL_TESTS();
}

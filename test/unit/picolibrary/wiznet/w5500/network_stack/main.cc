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

#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/mac_address.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/mac_address.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/wiznet/w5500.h"
#include "picolibrary/void.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/network_stack.h"

namespace {

using ::picolibrary::Error_Code;
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

} // namespace

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_phy() properly
 *        handles a PHYCFGR register write error when writing the PHY configuration.
 */
TEST( configurePhy, phycfgrWriteErrorPhyConfiguration )
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
TEST( configurePhy, phycfgrWrtieErrorEnterReset )
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
TEST( configurePhy, phycfgrWriteErrorExitReset )
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
TEST( configurePhy, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const phy_mode = random<PHY_Mode>();

    EXPECT_CALL( driver, write_phycfgr( static_cast<std::uint8_t>( phy_mode ) | 0b1'0'000'000 ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_phycfgr( static_cast<std::uint8_t>( phy_mode ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_phycfgr( static_cast<std::uint8_t>( phy_mode ) | 0b1'0'000'000 ) )
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

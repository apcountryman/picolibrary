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
 * \brief picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket unit test program.
 */

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/fixed_size_array.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/wiznet/w5500.h"
#include "picolibrary/void.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/network_stack.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Fixed_Size_Array;
using ::picolibrary::Generic_Error;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::picolibrary::Testing::Unit::WIZnet::W5500::Mock_Driver;
using ::picolibrary::WIZnet::W5500::Buffer_Size;
using ::picolibrary::WIZnet::W5500::Network_Stack;
using ::picolibrary::WIZnet::W5500::No_Delayed_ACK;
using ::picolibrary::WIZnet::W5500::Socket_ID;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

class Socket : public Network_Stack<Mock_Driver>::TCP_Socket {
  public:
    Socket( Network_Stack<Mock_Driver> & network_stack, Socket_ID socket_id ) noexcept :
        Network_Stack<Mock_Driver>::TCP_Socket{ network_stack, socket_id }
    {
    }
};

template<typename T, std::size_t N>
auto random_fixed_size_array()
{
    Fixed_Size_Array<T, N> array;

    std::generate( array.begin(), array.end(), []() { return random<T>(); } );

    return array;
}

} // namespace

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::socket_id() works
 *        properly.
 */
TEST( socketID, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto const socket = Socket{ network_stack, socket_id };

    EXPECT_EQ( socket.socket_id(), socket_id );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::socket_interrupt_mask()
 *        works properly.
 */
TEST( socketInterruptMask, worksProperly )
{
    struct {
        Socket_ID    socket_id;
        std::uint8_t socket_interrupt_mask;
    } const test_cases[]{
        // clang-format off

        { Socket_ID::_0, 0b0000'0001 },
        { Socket_ID::_1, 0b0000'0010 },
        { Socket_ID::_2, 0b0000'0100 },
        { Socket_ID::_3, 0b0000'1000 },
        { Socket_ID::_4, 0b0001'0000 },
        { Socket_ID::_5, 0b0010'0000 },
        { Socket_ID::_6, 0b0100'0000 },
        { Socket_ID::_7, 0b1000'0000 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver = Mock_Driver{};

        auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

        auto const socket = Socket{ network_stack, test_case.socket_id };

        EXPECT_EQ( socket.socket_interrupt_mask(), test_case.socket_interrupt_mask );
    } // for
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::enabled_interrupts()
 *        properly handles an SN_IMR register read error.
 */
TEST( enabledInterrupts, snimrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_imr( _ ) ).WillOnce( Return( error ) );

    auto const result = socket.enabled_interrupts();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::enabled_interrupts()
 *        works properly.
 */
TEST( enabledInterrupts, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto const socket = Socket{ network_stack, socket_id };

    auto const sn_imr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_imr( socket_id ) ).WillOnce( Return( sn_imr ) );

    auto const result = socket.enabled_interrupts();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_imr );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::interrupt_context()
 *        properly handles an SN_IR register read error.
 */
TEST( interruptContext, snirReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_ir( _ ) ).WillOnce( Return( error ) );

    auto const result = socket.interrupt_context();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::interrupt_context() works
 *        properly.
 */
TEST( interruptContext, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto const socket = Socket{ network_stack, socket_id };

    auto const sn_ir = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( sn_ir ) );

    auto const result = socket.interrupt_context();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_ir );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::no_delayed_ack_configuration()
 *        properly handles an SN_MR register read error.
 */
TEST( noDelayedACKConfiguration, mrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillOnce( Return( error ) );

    auto const result = socket.no_delayed_ack_configuration();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::no_delayed_ack_configuration()
 *        works properly.
 */
TEST( noDelayedACKConfiguration, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto const socket = Socket{ network_stack, socket_id };

    auto const sn_mr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_mr( socket_id ) ).WillOnce( Return( sn_mr ) );

    auto const result = socket.no_delayed_ack_configuration();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), static_cast<No_Delayed_ACK>( sn_mr & 0b0'0'1'0'0000 ) );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::maximum_segment_size()
 *        properly handles an SN_MSSR register read error.
 */
TEST( maximumSegmentSize, snmssrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_mssr( _ ) ).WillOnce( Return( error ) );

    auto const result = socket.maximum_segment_size();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::maximum_segment_size()
 *        works properly.
 */
TEST( maximumSegmentSize, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto const socket = Socket{ network_stack, socket_id };

    auto const sn_mssr = random<std::uint16_t>();

    EXPECT_CALL( driver, read_sn_mssr( socket_id ) ).WillOnce( Return( sn_mssr ) );

    auto const result = socket.maximum_segment_size();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_mssr );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::time_to_live()
 *        properly handles an SN_TTL register read error.
 */
TEST( timeToLive, snttlReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_ttl( _ ) ).WillOnce( Return( error ) );

    auto const result = socket.time_to_live();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::time_to_live()
 *        works properly.
 */
TEST( timeToLive, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto const socket = Socket{ network_stack, socket_id };

    auto const sn_ttl = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_ttl( socket_id ) ).WillOnce( Return( sn_ttl ) );

    auto const result = socket.time_to_live();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_ttl );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::keepalive_period()
 *        properly handles an SN_KPALVTR register read error.
 */
TEST( keepalivePeriod, snkpalvtrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_kpalvtr( _ ) ).WillOnce( Return( error ) );

    auto const result = socket.keepalive_period();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::keepalive_period()
 *        works properly.
 */
TEST( keepalivePeriod, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto const socket = Socket{ network_stack, socket_id };

    auto const sn_kpalvtr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_kpalvtr( socket_id ) ).WillOnce( Return( sn_kpalvtr ) );

    auto const result = socket.keepalive_period();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_kpalvtr );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::is_connected()
 *        properly handles an SN_SR register read error.
 */
TEST( isConnected, snsrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( error ) );

    auto const result = socket.is_connected();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::is_connected()
 *        properly handles a nonresponsive device error.
 */
TEST( isConnected, nonresponsiveDeviceError )
{
    struct {
        std::uint8_t sn_sr;
    } const test_cases[]{
        // clang-format off

        { random<std::uint8_t>( 0x01, 0x12 ) },
        { 0x19                               },
        { random<std::uint8_t>( 0x1E, 0xFF ) },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver = Mock_Driver{};

        auto const nonresponsive_device_error = random<Mock_Error>();

        auto network_stack = Network_Stack{ driver, nonresponsive_device_error };

        auto const socket = Socket{ network_stack, random<Socket_ID>() };

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );

        auto const result = socket.is_connected();

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), nonresponsive_device_error );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::is_connected()
 *        works properly.
 */
TEST( isConnected, worksProperly )
{
    struct {
        std::uint8_t sn_sr;
        bool         is_connected;
    } const test_cases[]{
        // clang-format off

        { 0x00, false },
        { 0x13, false },
        { 0x14, false },
        { 0x15, false },
        { 0x16, false },
        { 0x17, true  },
        { 0x18, false },
        { 0x1A, false },
        { 0x1B, false },
        { 0x1C, true  },
        { 0x1D, false },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver = Mock_Driver{};

        auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

        auto const socket_id = random<Socket_ID>();

        auto const socket = Socket{ network_stack, socket_id };

        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );

        auto const result = socket.is_connected();

        EXPECT_TRUE( result.is_value() );
        EXPECT_EQ( result.value(), test_case.is_connected );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::remote_endpoint()
 *        properly handles an SN_DIPR register read error.
 */
TEST( remoteEndpoint, sndiprReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_dipr( _ ) ).WillOnce( Return( error ) );

    auto const result = socket.remote_endpoint();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::remote_endpoint()
 *        properly handles an SN_DPORT register read error.
 */
TEST( remoteEndpoint, sndportReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_dipr( _ ) ).WillOnce( Return( random_fixed_size_array<std::uint8_t, 4>() ) );
    EXPECT_CALL( driver, read_sn_dport( _ ) ).WillOnce( Return( error ) );

    auto const result = socket.remote_endpoint();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::remote_endpoint()
 *        works properly.
 */
TEST( remoteEndpoint, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto const socket = Socket{ network_stack, socket_id };

    auto const dipr  = random_fixed_size_array<std::uint8_t, 4>();
    auto const dport = random<std::uint16_t>();

    EXPECT_CALL( driver, read_sn_dipr( socket_id ) ).WillOnce( Return( dipr ) );
    EXPECT_CALL( driver, read_sn_dport( socket_id ) ).WillOnce( Return( dport ) );

    auto const result = socket.remote_endpoint();

    EXPECT_TRUE( result.is_value() );
    EXPECT_TRUE( result.value().address().is_ipv4() );
    EXPECT_EQ( result.value().address().ipv4().as_byte_array(), dipr );
    EXPECT_EQ( result.value().port().as_unsigned_integer(), dport );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::local_endpoint()
 *        properly handles a SIPR register read error.
 */
TEST( localEndpoint, siprReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( error ) );

    auto const result = socket.local_endpoint();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::local_endpoint()
 *        properly handles an SN_PORT register read error.
 */
TEST( localEndpoint, snportReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( random_fixed_size_array<std::uint8_t, 4>() ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( error ) );

    auto const result = socket.local_endpoint();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::local_endpoint()
 *        works properly.
 */
TEST( localEndpoint, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto const socket = Socket{ network_stack, socket_id };

    auto const sipr = random_fixed_size_array<std::uint8_t, 4>();
    auto const port = random<std::uint16_t>();

    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( sipr ) );
    EXPECT_CALL( driver, read_sn_port( socket_id ) ).WillOnce( Return( port ) );

    auto const result = socket.local_endpoint();

    EXPECT_TRUE( result.is_value() );
    EXPECT_TRUE( result.value().address().is_ipv4() );
    EXPECT_EQ( result.value().address().ipv4().as_byte_array(), sipr );
    EXPECT_EQ( result.value().port().as_unsigned_integer(), port );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::outstanding()
 *        properly handles an SN_TX_FSR register read error.
 */
TEST( outstanding, sntxfsrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( error ) );

    auto const result = socket.outstanding();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::outstanding()
 *        properly handles a nonresponsive device error.
 */
TEST( outstanding, nonresponsiveDeviceError )
{
    {
        auto driver = Mock_Driver{};

        auto const nonresponsive_device_error = random<Mock_Error>();

        auto network_stack = Network_Stack{ driver, nonresponsive_device_error };

        auto const socket = Socket{ network_stack, random<Socket_ID>() };

        EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( random<std::uint16_t>( ( 2 * 1024 ) + 1 ) ) );

        auto const result = socket.outstanding();

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), nonresponsive_device_error );
    }

    {
        struct {
            Buffer_Size   buffer_size;
            std::uint16_t sn_tx_fsr_max;
        } const test_cases[]{
            { Buffer_Size::_2_KIB, 2 * 1024 },
            { Buffer_Size::_4_KIB, 4 * 1024 },
            { Buffer_Size::_8_KIB, 8 * 1024 },
            { Buffer_Size::_16_KIB, 16 * 1024 },
        };

        for ( auto const test_case : test_cases ) {
            auto driver = Mock_Driver{};

            auto const nonresponsive_device_error = random<Mock_Error>();

            auto network_stack = Network_Stack{ driver, nonresponsive_device_error };

            EXPECT_CALL( driver, write_sn_rxbuf_size( _, _ ) )
                .WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_txbuf_size( _, _ ) )
                .WillRepeatedly( Return( Result<Void, Error_Code>{} ) );

            EXPECT_FALSE( network_stack.configure_socket_buffers( test_case.buffer_size ).is_error() );

            auto const socket = Socket{ network_stack, random<Socket_ID>() };

            EXPECT_CALL( driver, read_sn_tx_fsr( _ ) )
                .WillOnce( Return( random<std::uint16_t>( test_case.sn_tx_fsr_max + 1 ) ) );

            auto const result = socket.outstanding();

            EXPECT_TRUE( result.is_error() );
            EXPECT_EQ( result.error(), nonresponsive_device_error );
        } // for
    }
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::outstanding()
 *        works properly.
 */
TEST( outstanding, worksProperly )
{
    {
        auto driver = Mock_Driver{};

        auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

        auto const socket_id = random<Socket_ID>();

        auto const socket = Socket{ network_stack, socket_id };

        auto const sn_tx_fsr = random<std::uint16_t>( 0, 2 * 1024 );

        EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( sn_tx_fsr ) );

        auto const result = socket.outstanding();

        EXPECT_TRUE( result.is_value() );
        EXPECT_EQ( result.value(), ( 2 * 1024 ) - sn_tx_fsr );
    }

    {
        struct {
            Buffer_Size   buffer_size;
            std::uint16_t sn_tx_fsr_max;
        } const test_cases[]{
            { Buffer_Size::_2_KIB, 2 * 1024 },
            { Buffer_Size::_4_KIB, 4 * 1024 },
            { Buffer_Size::_8_KIB, 8 * 1024 },
            { Buffer_Size::_16_KIB, 16 * 1024 },
        };

        for ( auto const test_case : test_cases ) {
            auto driver = Mock_Driver{};

            auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

            EXPECT_CALL( driver, write_sn_rxbuf_size( _, _ ) )
                .WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_txbuf_size( _, _ ) )
                .WillRepeatedly( Return( Result<Void, Error_Code>{} ) );

            EXPECT_FALSE( network_stack.configure_socket_buffers( test_case.buffer_size ).is_error() );

            auto const socket_id = random<Socket_ID>();

            auto const socket = Socket{ network_stack, socket_id };

            auto const sn_tx_fsr = random<std::uint16_t>( 0, test_case.sn_tx_fsr_max );

            EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( sn_tx_fsr ) );

            auto const result = socket.outstanding();

            EXPECT_TRUE( result.is_value() );
            EXPECT_EQ( result.value(), test_case.sn_tx_fsr_max - sn_tx_fsr );
        } // for
    }
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit()
 *        properly handles an SN_SR register read error.
 */
TEST( transmit, snsrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( error ) );

    auto const data   = random_container<std::vector<std::uint8_t>>();
    auto const result = socket.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit()
 *        properly handles a nonresponsive device error when reading the SN_SR register.
 */
TEST( transmit, nonresponsiveDeviceErrorSNSR )
{
    struct {
        std::uint8_t sn_sr;
    } const test_cases[]{
        // clang-format off

        { random<std::uint8_t>( 0x01, 0x12 ) },
        { 0x19                               },
        { random<std::uint8_t>( 0x1E, 0xFF ) },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver = Mock_Driver{};

        auto const nonresponsive_device_error = random<Mock_Error>();

        auto network_stack = Network_Stack{ driver, nonresponsive_device_error };

        auto socket = Socket{ network_stack, random<Socket_ID>() };

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );

        auto const data   = random_container<std::vector<std::uint8_t>>();
        auto const result = socket.transmit( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), nonresponsive_device_error );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit()
 *        properly handles the socket not being connected to a remote endpoint.
 */
TEST( transmit, notConnected )
{
    struct {
        std::uint8_t sn_sr;
    } const test_cases[]{
        // clang-format off

        { 0x00 },
        { 0x13 },
        { 0x14 },
        { 0x15 },
        { 0x16 },
        { 0x18 },
        { 0x1A },
        { 0x1B },
        { 0x1D },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver = Mock_Driver{};

        auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

        auto socket = Socket{ network_stack, random<Socket_ID>() };

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );

        auto const data   = random_container<std::vector<std::uint8_t>>();
        auto const result = socket.transmit( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::CONNECTION_LOST );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit()
 *        properly handles an SN_TX_FSR register read error.
 */
TEST( transmit, sntxfsrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x17 ) ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( error ) );

    auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
    auto const result = socket.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit()
 *        properly handles a nonresponsive device error when reading the SN_TX_FSR
 *        register.
 */
TEST( transmit, nonresponsiveDeviceErrorSNTXFSR )
{
    {
        auto driver = Mock_Driver{};

        auto const nonresponsive_device_error = random<Mock_Error>();

        auto network_stack = Network_Stack{ driver, nonresponsive_device_error };

        auto socket = Socket{ network_stack, random<Socket_ID>() };

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x17 ) ) );
        EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( random<std::uint16_t>( ( 2 * 1024 ) + 1 ) ) );

        auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
        auto const result = socket.transmit( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), nonresponsive_device_error );
    }

    {
        struct {
            Buffer_Size   buffer_size;
            std::uint16_t sn_tx_fsr_max;
        } const test_cases[]{
            { Buffer_Size::_2_KIB, 2 * 1024 },
            { Buffer_Size::_4_KIB, 4 * 1024 },
            { Buffer_Size::_8_KIB, 8 * 1024 },
            { Buffer_Size::_16_KIB, 16 * 1024 },
        };

        for ( auto const test_case : test_cases ) {
            auto driver = Mock_Driver{};

            auto const nonresponsive_device_error = random<Mock_Error>();

            auto network_stack = Network_Stack{ driver, nonresponsive_device_error };

            EXPECT_CALL( driver, write_sn_rxbuf_size( _, _ ) )
                .WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_txbuf_size( _, _ ) )
                .WillRepeatedly( Return( Result<Void, Error_Code>{} ) );

            EXPECT_FALSE( network_stack.configure_socket_buffers( test_case.buffer_size ).is_error() );

            auto socket = Socket{ network_stack, random<Socket_ID>() };

            EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x17 ) ) );
            EXPECT_CALL( driver, read_sn_tx_fsr( _ ) )
                .WillOnce( Return( random<std::uint16_t>( test_case.sn_tx_fsr_max + 1 ) ) );

            auto const data = random_container<std::vector<std::uint8_t>>(
                random<std::uint_fast8_t>( 1 ) );
            auto const result = socket.transmit( &*data.begin(), &*data.end() );

            EXPECT_TRUE( result.is_error() );
            EXPECT_EQ( result.error(), nonresponsive_device_error );
        }
    }
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit()
 *        properly handles a full transmit buffer.
 */
TEST( transmit, transmitBufferFull )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto socket = Socket{ network_stack, random<Socket_ID>() };

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x17 ) ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( static_cast<std::uint16_t>( 0 ) ) );

    auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
    auto const result = socket.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit()
 *        properly handles an SN_TX_WR register read error.
 */
TEST( transmit, sntxwrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x17 ) ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( random<std::uint16_t>( 1, 2 * 1024 ) ) );
    EXPECT_CALL( driver, read_sn_tx_wr( _ ) ).WillOnce( Return( error ) );

    auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
    auto const result = socket.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit()
 *        properly handles a transmit buffer write error.
 */
TEST( transmit, transmitBufferWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x17 ) ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( random<std::uint16_t>( 1, 2 * 1024 ) ) );
    EXPECT_CALL( driver, read_sn_tx_wr( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
    EXPECT_CALL( driver, write( _, _, _ ) ).WillOnce( Return( error ) );

    auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
    auto const result = socket.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit()
 *        properly handles an SN_TX_WR register write error.
 */
TEST( transmit, sntxwrWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x17 ) ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( random<std::uint16_t>( 1, 2 * 1024 ) ) );
    EXPECT_CALL( driver, read_sn_tx_wr( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
    EXPECT_CALL( driver, write( _, _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_tx_wr( _, _ ) ).WillOnce( Return( error ) );

    auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
    auto const result = socket.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit()
 *        properly handles an SN_CR register write error.
 */
TEST( transmit, sncrWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x17 ) ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( random<std::uint16_t>( 1, 2 * 1024 ) ) );
    EXPECT_CALL( driver, read_sn_tx_wr( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
    EXPECT_CALL( driver, write( _, _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_tx_wr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( error ) );

    auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
    auto const result = socket.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit()
 *        properly handles an SN_CR register read error.
 */
TEST( transmit, sncrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x17 ) ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( random<std::uint16_t>( 1, 2 * 1024 ) ) );
    EXPECT_CALL( driver, read_sn_tx_wr( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
    EXPECT_CALL( driver, write( _, _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_tx_wr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( error ) );

    auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
    auto const result = socket.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit()
 *        properly handles a nonresponsive device error when reading the SN_CR register.
 */
TEST( transmit, nonresponsiveDeviceErrorSNCR )
{
    struct {
        std::uint8_t sn_cr;
    } const test_cases[]{
        { random<std::uint8_t>( 0x01, 0x19 ) },
        { random<std::uint8_t>( 0x21, 0xFF ) },
    };

    for ( auto const test_case : test_cases ) {
        auto driver = Mock_Driver{};

        auto const nonresponsive_device_error = random<Mock_Error>();

        auto network_stack = Network_Stack{ driver, nonresponsive_device_error };

        auto socket = Socket{ network_stack, random<Socket_ID>() };

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x17 ) ) );
        EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( random<std::uint16_t>( 1, 2 * 1024 ) ) );
        EXPECT_CALL( driver, read_sn_tx_wr( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
        EXPECT_CALL( driver, write( _, _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, write_sn_tx_wr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( test_case.sn_cr ) );

        auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
        auto const result = socket.transmit( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), nonresponsive_device_error );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit()
 *        properly handles an SN_IR register read error.
 */
TEST( transmit, snirReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillRepeatedly( Return( static_cast<std::uint8_t>( 0x17 ) ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( random<std::uint16_t>( 1, 2 * 1024 ) ) );
    EXPECT_CALL( driver, read_sn_tx_wr( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
    EXPECT_CALL( driver, write( _, _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_tx_wr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x00 ) ) );
    EXPECT_CALL( driver, read_sn_ir( _ ) ).WillOnce( Return( error ) );

    {
        auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
        EXPECT_FALSE( socket.transmit( &*data.begin(), &*data.end() ).is_error() );
    }

    auto const data   = random_container<std::vector<std::uint8_t>>();
    auto const result = socket.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit()
 *        properly handles an in-progress transmission that is not complete.
 */
TEST( transmit, transmissionNotComplete )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto socket = Socket{ network_stack, random<Socket_ID>() };

    {
        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x17 ) ) );
        EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( random<std::uint16_t>( 1, 2 * 1024 ) ) );
        EXPECT_CALL( driver, read_sn_tx_wr( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
        EXPECT_CALL( driver, write( _, _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, write_sn_tx_wr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x00 ) ) );

        auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
        EXPECT_FALSE( socket.transmit( &*data.begin(), &*data.end() ).is_error() );
    }

    {
        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x17 ) ) );
        EXPECT_CALL( driver, read_sn_ir( _ ) )
            .WillOnce( Return( static_cast<std::uint8_t>( random<std::uint8_t>() & 0b111'0'1'1'1'1 ) ) );

        auto const data   = random_container<std::vector<std::uint8_t>>();
        auto const result = socket.transmit( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );
    }

    {
        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x17 ) ) );
        EXPECT_CALL( driver, read_sn_ir( _ ) )
            .WillOnce( Return( static_cast<std::uint8_t>( random<std::uint8_t>() & 0b111'0'1'1'1'1 ) ) );

        auto const data   = random_container<std::vector<std::uint8_t>>();
        auto const result = socket.transmit( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );
    }
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit()
 *        properly handles an SN_IR register write error.
 */
TEST( transmit, snirWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillRepeatedly( Return( static_cast<std::uint8_t>( 0x17 ) ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( random<std::uint16_t>( 1, 2 * 1024 ) ) );
    EXPECT_CALL( driver, read_sn_tx_wr( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
    EXPECT_CALL( driver, write( _, _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_tx_wr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x00 ) ) );
    EXPECT_CALL( driver, read_sn_ir( _ ) )
        .WillOnce( Return( static_cast<std::uint8_t>( random<std::uint8_t>() | 0b000'1'0'0'0'0 ) ) );
    EXPECT_CALL( driver, write_sn_ir( _, _ ) ).WillOnce( Return( error ) );

    {
        auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
        EXPECT_FALSE( socket.transmit( &*data.begin(), &*data.end() ).is_error() );
    }

    auto const data   = random_container<std::vector<std::uint8_t>>();
    auto const result = socket.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit() works
 *        properly.
 */
TEST( transmit, worksProperly )
{
    {
        auto const in_sequence = InSequence{};

        auto driver = Mock_Driver{};

        auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

        auto const socket_id = random<Socket_ID>();

        auto socket = Socket{ network_stack, socket_id };

        {
            EXPECT_CALL( driver, read_sn_sr( socket_id ) )
                .WillOnce( Return( static_cast<std::uint8_t>( random<bool>() ? 0x17 : 0x1C ) ) );

            auto const data   = std::vector<std::uint8_t>{};
            auto const result = socket.transmit( &*data.begin(), &*data.end() );

            EXPECT_TRUE( result.is_value() );
            EXPECT_EQ( result.value(), &*data.begin() );
        }

        {
            EXPECT_CALL( driver, read_sn_sr( socket_id ) )
                .WillOnce( Return( static_cast<std::uint8_t>( random<bool>() ? 0x17 : 0x1C ) ) );

            auto const data   = std::vector<std::uint8_t>{};
            auto const result = socket.transmit( &*data.begin(), &*data.end() );

            EXPECT_TRUE( result.is_value() );
            EXPECT_EQ( result.value(), &*data.begin() );
        }
    }

    {
        struct {
            Buffer_Size   buffer_size;
            std::uint16_t sn_tx_fsr_max;
        } const test_cases[]{
            { Buffer_Size::_2_KIB, 2 * 1024 },
            { Buffer_Size::_4_KIB, 4 * 1024 },
            { Buffer_Size::_8_KIB, 8 * 1024 },
            { Buffer_Size::_16_KIB, 16 * 1024 },
        };

        for ( auto const test_case : test_cases ) {
            auto driver = Mock_Driver{};

            auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

            EXPECT_CALL( driver, write_sn_rxbuf_size( _, _ ) )
                .WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_txbuf_size( _, _ ) )
                .WillRepeatedly( Return( Result<Void, Error_Code>{} ) );

            EXPECT_FALSE( network_stack.configure_socket_buffers( test_case.buffer_size ).is_error() );

            auto const in_sequence = InSequence{};

            auto const socket_id = random<Socket_ID>();

            auto socket = Socket{ network_stack, socket_id };

            {
                auto const data = random_container<std::vector<std::uint8_t>>(
                    random<std::uint16_t>( 1, test_case.sn_tx_fsr_max ) );
                auto const sn_tx_fsr = random<std::uint16_t>( data.size(), test_case.sn_tx_fsr_max );
                auto const sn_tx_wr = random<std::uint16_t>();

                EXPECT_CALL( driver, read_sn_sr( socket_id ) )
                    .WillOnce( Return( static_cast<std::uint8_t>( random<bool>() ? 0x17 : 0x1C ) ) );
                EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( sn_tx_fsr ) );
                EXPECT_CALL( driver, read_sn_tx_wr( socket_id ) ).WillOnce( Return( sn_tx_wr ) );
                EXPECT_CALL( driver, write( socket_id, sn_tx_wr, data ) )
                    .WillOnce( Return( Result<Void, Error_Code>{} ) );
                EXPECT_CALL( driver, write_sn_tx_wr( socket_id, sn_tx_wr + data.size() ) )
                    .WillOnce( Return( Result<Void, Error_Code>{} ) );
                EXPECT_CALL( driver, write_sn_cr( socket_id, 0x20 ) )
                    .WillOnce( Return( Result<Void, Error_Code>{} ) );
                EXPECT_CALL( driver, read_sn_cr( socket_id ) )
                    .WillOnce( Return( static_cast<std::uint8_t>( 0x20 ) ) );
                EXPECT_CALL( driver, read_sn_cr( socket_id ) )
                    .WillOnce( Return( static_cast<std::uint8_t>( 0x00 ) ) );

                auto const result = socket.transmit( &*data.begin(), &*data.end() );

                EXPECT_TRUE( result.is_value() );
                EXPECT_EQ( result.value(), &*data.end() );
            }

            {
                EXPECT_CALL( driver, read_sn_sr( socket_id ) )
                    .WillOnce( Return( static_cast<std::uint8_t>( random<bool>() ? 0x17 : 0x1C ) ) );
                EXPECT_CALL( driver, read_sn_ir( socket_id ) )
                    .WillOnce( Return( static_cast<std::uint8_t>( random<std::uint8_t>() | 0b000'1'0'0'0'0 ) ) );
                EXPECT_CALL( driver, write_sn_ir( socket_id, 0b000'1'0'0'0'0 ) )
                    .WillOnce( Return( Result<Void, Error_Code>{} ) );

                auto const data   = std::vector<std::uint8_t>{};
                auto const result = socket.transmit( &*data.begin(), &*data.end() );

                EXPECT_TRUE( result.is_value() );
                EXPECT_EQ( result.value(), &*data.begin() );
            }

            {
                EXPECT_CALL( driver, read_sn_sr( socket_id ) )
                    .WillOnce( Return( static_cast<std::uint8_t>( random<bool>() ? 0x17 : 0x1C ) ) );

                auto const data   = std::vector<std::uint8_t>{};
                auto const result = socket.transmit( &*data.begin(), &*data.end() );

                EXPECT_TRUE( result.is_value() );
                EXPECT_EQ( result.value(), &*data.begin() );
            }
        } // for
    }

    {
        struct {
            Buffer_Size   buffer_size;
            std::uint16_t sn_tx_fsr_max;
        } const test_cases[]{
            { Buffer_Size::_2_KIB, 2 * 1024 },
            { Buffer_Size::_4_KIB, 4 * 1024 },
            { Buffer_Size::_8_KIB, 8 * 1024 },
            { Buffer_Size::_16_KIB, 16 * 1024 },
        };

        for ( auto const test_case : test_cases ) {
            auto driver = Mock_Driver{};

            auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

            EXPECT_CALL( driver, write_sn_rxbuf_size( _, _ ) )
                .WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_txbuf_size( _, _ ) )
                .WillRepeatedly( Return( Result<Void, Error_Code>{} ) );

            EXPECT_FALSE( network_stack.configure_socket_buffers( test_case.buffer_size ).is_error() );

            auto const in_sequence = InSequence{};

            auto const socket_id = random<Socket_ID>();

            auto socket = Socket{ network_stack, socket_id };

            {
                auto const sn_tx_fsr = random<std::uint16_t>( 1, test_case.sn_tx_fsr_max );
                auto const data      = random_container<std::vector<std::uint8_t>>(
                    random<std::uint16_t>( sn_tx_fsr + 1 ) );
                auto const sn_tx_wr = random<std::uint16_t>();

                EXPECT_CALL( driver, read_sn_sr( socket_id ) )
                    .WillOnce( Return( static_cast<std::uint8_t>( random<bool>() ? 0x17 : 0x1C ) ) );
                EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( sn_tx_fsr ) );
                EXPECT_CALL( driver, read_sn_tx_wr( socket_id ) ).WillOnce( Return( sn_tx_wr ) );
                EXPECT_CALL(
                    driver,
                    write( socket_id, sn_tx_wr, std::vector<std::uint8_t>{ data.begin(), data.begin() + sn_tx_fsr } ) )
                    .WillOnce( Return( Result<Void, Error_Code>{} ) );
                EXPECT_CALL( driver, write_sn_tx_wr( socket_id, sn_tx_wr + sn_tx_fsr ) )
                    .WillOnce( Return( Result<Void, Error_Code>{} ) );
                EXPECT_CALL( driver, write_sn_cr( socket_id, 0x20 ) )
                    .WillOnce( Return( Result<Void, Error_Code>{} ) );
                EXPECT_CALL( driver, read_sn_cr( socket_id ) )
                    .WillOnce( Return( static_cast<std::uint8_t>( 0x20 ) ) );
                EXPECT_CALL( driver, read_sn_cr( socket_id ) )
                    .WillOnce( Return( static_cast<std::uint8_t>( 0x00 ) ) );

                auto const result = socket.transmit( &*data.begin(), &*data.end() );

                EXPECT_TRUE( result.is_value() );
                EXPECT_EQ( result.value(), &*( data.begin() + sn_tx_fsr ) );
            }

            {
                EXPECT_CALL( driver, read_sn_sr( socket_id ) )
                    .WillOnce( Return( static_cast<std::uint8_t>( random<bool>() ? 0x17 : 0x1C ) ) );
                EXPECT_CALL( driver, read_sn_ir( socket_id ) )
                    .WillOnce( Return( static_cast<std::uint8_t>( random<std::uint8_t>() | 0b000'1'0'0'0'0 ) ) );
                EXPECT_CALL( driver, write_sn_ir( socket_id, 0b000'1'0'0'0'0 ) )
                    .WillOnce( Return( Result<Void, Error_Code>{} ) );

                auto const data   = std::vector<std::uint8_t>{};
                auto const result = socket.transmit( &*data.begin(), &*data.end() );

                EXPECT_TRUE( result.is_value() );
                EXPECT_EQ( result.value(), &*data.begin() );
            }

            {
                EXPECT_CALL( driver, read_sn_sr( socket_id ) )
                    .WillOnce( Return( static_cast<std::uint8_t>( random<bool>() ? 0x17 : 0x1C ) ) );

                auto const data   = std::vector<std::uint8_t>{};
                auto const result = socket.transmit( &*data.begin(), &*data.end() );

                EXPECT_TRUE( result.is_value() );
                EXPECT_EQ( result.value(), &*data.begin() );
            }
        } // for
    }
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit_keepalive()
 *        properly handles an SN_SR register read error.
 */
TEST( transmitKeepalive, snsrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( error ) );

    auto const result = socket.transmit_keepalive();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit_keepalive()
 *        properly handles a nonresponsive device error when reading the SN_SR register.
 */
TEST( transmitKeepalive, nonresponsiveDeviceErrorSNSR )
{
    struct {
        std::uint8_t sn_sr;
    } const test_cases[]{
        // clang-format off

        { random<std::uint8_t>( 0x01, 0x12 ) },
        { 0x19                               },
        { random<std::uint8_t>( 0x1E, 0xFF ) },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver = Mock_Driver{};

        auto const nonresponsive_device_error = random<Mock_Error>();

        auto network_stack = Network_Stack{ driver, nonresponsive_device_error };

        auto socket = Socket{ network_stack, random<Socket_ID>() };

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );

        auto const result = socket.transmit_keepalive();

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), nonresponsive_device_error );
    } // for
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit_keepalive()
 *        properly handles the socket not being connected to a remote endpoint.
 */
TEST( transmitKeepalive, notConnected )
{
    struct {
        std::uint8_t sn_sr;
    } const test_cases[]{
        // clang-format off

        { 0x00 },
        { 0x13 },
        { 0x14 },
        { 0x15 },
        { 0x16 },
        { 0x18 },
        { 0x1A },
        { 0x1B },
        { 0x1D },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver = Mock_Driver{};

        auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

        auto socket = Socket{ network_stack, random<Socket_ID>() };

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );

        auto const result = socket.transmit_keepalive();

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::CONNECTION_LOST );
    } // for
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit_keepalive()
 *        properly handles an SN_CR register write error.
 */
TEST( transmitKeepalive, sncrWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x17 ) ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = socket.transmit_keepalive();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit_keepalive()
 *        properly handles an SN_CR register read error.
 */
TEST( transmitKeepalive, sncrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x17 ) ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( error ) );

    auto const result = socket.transmit_keepalive();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit_keepalive()
 *        properly handles a nonresponsive device error when reading the SN_CR register.
 */
TEST( transmitKeepalive, nonresponsiveDeviceErrorSNCR )
{
    struct {
        std::uint8_t sn_cr;
    } const test_cases[]{
        { random<std::uint8_t>( 0x01, 0x21 ) },
        { random<std::uint8_t>( 0x23, 0xFF ) },
    };

    for ( auto const test_case : test_cases ) {
        auto driver = Mock_Driver{};

        auto const nonresponsive_device_error = random<Mock_Error>();

        auto network_stack = Network_Stack{ driver, nonresponsive_device_error };

        auto socket = Socket{ network_stack, random<Socket_ID>() };

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x17 ) ) );
        EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( test_case.sn_cr ) );

        auto const result = socket.transmit_keepalive();

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), nonresponsive_device_error );
    } // for
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::transmit_keepalive()
 *        works properly.
 */
TEST( transmitKeepalive, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto socket = Socket{ network_stack, socket_id };

    EXPECT_CALL( driver, read_sn_sr( socket_id ) )
        .WillOnce( Return( static_cast<std::uint8_t>( random<bool>() ? 0x17 : 0x1C ) ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x22 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x22 ) ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( static_cast<std::uint8_t>( 0x00 ) ) );

    EXPECT_FALSE( socket.transmit_keepalive().is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::available()
 *        properly handles an SN_RX_RSR register read error.
 */
TEST( available, snrxrsrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_rx_rsr( _ ) ).WillOnce( Return( error ) );

    auto const result = socket.available();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::available()
 *        properly handles a nonresponsive device error.
 */
TEST( available, nonresponsiveDeviceError )
{
    {
        auto driver = Mock_Driver{};

        auto const nonresponsive_device_error = random<Mock_Error>();

        auto network_stack = Network_Stack{ driver, nonresponsive_device_error };

        auto const socket = Socket{ network_stack, random<Socket_ID>() };

        EXPECT_CALL( driver, read_sn_rx_rsr( _ ) ).WillOnce( Return( random<std::uint16_t>( ( 2 * 1024 ) + 1 ) ) );

        auto const result = socket.available();

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), nonresponsive_device_error );
    }

    {
        struct {
            Buffer_Size   buffer_size;
            std::uint16_t sn_rx_rsr_max;
        } const test_cases[]{
            { Buffer_Size::_2_KIB, 2 * 1024 },
            { Buffer_Size::_4_KIB, 4 * 1024 },
            { Buffer_Size::_8_KIB, 8 * 1024 },
            { Buffer_Size::_16_KIB, 16 * 1024 },
        };

        for ( auto const test_case : test_cases ) {
            auto driver = Mock_Driver{};

            auto const nonresponsive_device_error = random<Mock_Error>();

            auto network_stack = Network_Stack{ driver, nonresponsive_device_error };

            EXPECT_CALL( driver, write_sn_rxbuf_size( _, _ ) )
                .WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_txbuf_size( _, _ ) )
                .WillRepeatedly( Return( Result<Void, Error_Code>{} ) );

            EXPECT_FALSE( network_stack.configure_socket_buffers( test_case.buffer_size ).is_error() );

            auto const socket = Socket{ network_stack, random<Socket_ID>() };

            EXPECT_CALL( driver, read_sn_rx_rsr( _ ) )
                .WillOnce( Return( random<std::uint16_t>( test_case.sn_rx_rsr_max + 1 ) ) );

            auto const result = socket.available();

            EXPECT_TRUE( result.is_error() );
            EXPECT_EQ( result.error(), nonresponsive_device_error );
        } // for
    }
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::available() works
 *        properly.
 */
TEST( available, worksProperly )
{
    {
        auto driver = Mock_Driver{};

        auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

        auto const socket_id = random<Socket_ID>();

        auto const socket = Socket{ network_stack, socket_id };

        auto const sn_rx_rsr = random<std::uint16_t>( 0, 2 * 1024 );

        EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) ).WillOnce( Return( sn_rx_rsr ) );

        auto const result = socket.available();

        EXPECT_TRUE( result.is_value() );
        EXPECT_EQ( result.value(), sn_rx_rsr );
    }

    {
        struct {
            Buffer_Size   buffer_size;
            std::uint16_t sn_rx_rsr_max;
        } const test_cases[]{
            { Buffer_Size::_2_KIB, 2 * 1024 },
            { Buffer_Size::_4_KIB, 4 * 1024 },
            { Buffer_Size::_8_KIB, 8 * 1024 },
            { Buffer_Size::_16_KIB, 16 * 1024 },
        };

        for ( auto const test_case : test_cases ) {
            auto driver = Mock_Driver{};

            auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

            EXPECT_CALL( driver, write_sn_rxbuf_size( _, _ ) )
                .WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_txbuf_size( _, _ ) )
                .WillRepeatedly( Return( Result<Void, Error_Code>{} ) );

            EXPECT_FALSE( network_stack.configure_socket_buffers( test_case.buffer_size ).is_error() );

            auto const socket_id = random<Socket_ID>();

            auto const socket = Socket{ network_stack, socket_id };

            auto const sn_rx_rsr = random<std::uint16_t>( 0, test_case.sn_rx_rsr_max );

            EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) ).WillOnce( Return( sn_rx_rsr ) );

            auto const result = socket.available();

            EXPECT_TRUE( result.is_value() );
            EXPECT_EQ( result.value(), sn_rx_rsr );
        } // for
    }
}

/**
 * \brief Execute the picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket unit tests.
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

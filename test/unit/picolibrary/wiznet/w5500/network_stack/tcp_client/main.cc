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
 * \brief picolibrary::WIZnet::W5500::Network_Stack::TCP_Client unit test program.
 */

#include <cstdint>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/ip/tcp.h"
#include "picolibrary/testing/unit/ipv4.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/wiznet/w5500.h"
#include "picolibrary/void.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/network_stack.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Generic_Error;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::IP::TCP::Port;
using ::picolibrary::IPv4::Address;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::WIZnet::W5500::Mock_Driver;
using ::picolibrary::WIZnet::W5500::Buffer_Size;
using ::picolibrary::WIZnet::W5500::Network_Stack;
using ::picolibrary::WIZnet::W5500::No_Delayed_ACK;
using ::picolibrary::WIZnet::W5500::Socket_ID;
using ::testing::_;
using ::testing::AllOf;
using ::testing::Ge;
using ::testing::InSequence;
using ::testing::Le;
using ::testing::Return;

using Client = Network_Stack<Mock_Driver>::TCP_Client;

auto random_unique_addresses()
{
    auto const a = random<Address::Unsigned_Integer>();
    auto const b = random<Address::Unsigned_Integer>();

    return std::pair<Address, Address>{ a, b != a ? b : b ^ random<Address::Unsigned_Integer>( 1 ) };
}

} // namespace

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::TCP_Client( Driver
 *        &, picolibrary::WIZnet::W5500::Socket_ID ) works properly.
 */
TEST( constructor, worksProperly )
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

        auto const client = Client{ driver, test_case.socket_id, network_stack };

        EXPECT_EQ( client.socket_id(), test_case.socket_id );
        EXPECT_EQ( client.socket_interrupt_mask(), test_case.socket_interrupt_mask );
    } // for
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::enable_interrupts()
 *        properly handles an SN_IMR register read error.
 */
TEST( enableInterrupts, snimrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_imr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.enable_interrupts( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::enable_interrupts()
 *        properly handles an SN_IMR register write error.
 */
TEST( enableInterrupts, snimrWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_imr( _ ) ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_sn_imr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.enable_interrupts( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::enable_interrupts() works
 *        properly.
 */
TEST( enableInterrupts, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto client = Client{ driver, socket_id, network_stack };

    auto const sn_imr = random<std::uint8_t>();
    auto const mask   = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_imr( socket_id ) ).WillOnce( Return( sn_imr ) );
    EXPECT_CALL( driver, write_sn_imr( socket_id, sn_imr | mask ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( client.enable_interrupts( mask ).is_error() );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::disable_interrupts(
 *        std::uint8_t ) properly handles an SN_IMR register read error.
 */
TEST( disableInterrupts, snimrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_imr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.disable_interrupts( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::disable_interrupts(
 *        std::uint8_t ) properly handles an SN_IMR register write error.
 */
TEST( disableInterrupts, snimrWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_imr( _ ) ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_sn_imr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.disable_interrupts( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::disable_interrupts(
 *        std::uint8_t ) works properly.
 */
TEST( disableInterrupts, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto client = Client{ driver, socket_id, network_stack };

    auto const sn_imr = random<std::uint8_t>();
    auto const mask   = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_imr( socket_id ) ).WillOnce( Return( sn_imr ) );
    EXPECT_CALL( driver, write_sn_imr( socket_id, sn_imr & ~mask ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( client.disable_interrupts( mask ).is_error() );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::disable_interrupts()
 *        properly handles an SN_IMR register write error.
 */
TEST( disableAllInterrupts, snimrWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_imr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.disable_interrupts();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::disable_interrupts()
 *        works properly.
 */
TEST( disableAllInterrupts, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto client = Client{ driver, socket_id, network_stack };

    EXPECT_CALL( driver, write_sn_imr( socket_id, 0 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( client.disable_interrupts().is_error() );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::enabled_interrupts()
 *        properly handles an SN_IMR register read error.
 */
TEST( enabledInterrupts, snimrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_imr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.enabled_interrupts();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::enabled_interrupts()
 *        works properly.
 */
TEST( enabledInterrupts, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto const client = Client{ driver, socket_id, network_stack };

    auto const sn_imr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_imr( socket_id ) ).WillOnce( Return( sn_imr ) );

    auto const result = client.enabled_interrupts();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_imr );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::interrupt_context()
 *        properly handles an SN_IR register read error.
 */
TEST( interruptContext, snirReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_ir( _ ) ).WillOnce( Return( error ) );

    auto const result = client.interrupt_context();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::interrupt_context() works
 *        properly.
 */
TEST( interruptContext, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto const client = Client{ driver, socket_id, network_stack };

    auto const sn_ir = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( sn_ir ) );

    auto const result = client.interrupt_context();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_ir );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::configure_no_delayed_ack()
 *        properly handles an SN_MR register read error.
 */
TEST( configureNoDelayedAck, snmrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.configure_no_delayed_ack( random<No_Delayed_ACK>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::configure_no_delayed_ack()
 *        properly handles an SN_MR register write error.
 */
TEST( configureNoDelayedAck, snmrWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_sn_mr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.configure_no_delayed_ack( random<No_Delayed_ACK>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::configure_no_delayed_ack()
 *        works properly.
 */
TEST( configureNoDelayedAck, worksProperly )
{
    struct {
        No_Delayed_ACK no_delayed_ack_configuration;
        std::uint8_t   sn_mr_nd;
    } const test_cases[]{
        // clang-format off

        { No_Delayed_ACK::DISABLED, 0b0'0'0'0'0000 },
        { No_Delayed_ACK::ENABLED,  0b0'0'1'0'0000 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const in_sequence = InSequence{};

        auto driver = Mock_Driver{};

        auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ driver, socket_id, network_stack };

        auto const sn_mr = random<std::uint8_t>();

        EXPECT_CALL( driver, read_sn_mr( socket_id ) ).WillOnce( Return( sn_mr ) );
        EXPECT_CALL(
            driver, write_sn_mr( socket_id, ( sn_mr & 0b1'1'0'1'1111 ) | test_case.sn_mr_nd ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );

        EXPECT_FALSE(
            client.configure_no_delayed_ack( test_case.no_delayed_ack_configuration ).is_error() );
    } // for
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::no_delayed_ack_configuration()
 *        properly handles an SN_MR register read error.
 */
TEST( noDelayedAckConfiguration, snmrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.no_delayed_ack_configuration();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::no_delayed_ack_configuration()
 *        works properly.
 */
TEST( noDelayedAckConfiguration, worksProperly )
{
    struct {
        std::uint8_t   sn_mr_nd;
        No_Delayed_ACK no_delayed_ack_configuration;
    } const test_cases[]{
        // clang-format off

        { 0b0'0'0'0'0000, No_Delayed_ACK::DISABLED },
        { 0b0'0'1'0'0000, No_Delayed_ACK::ENABLED  },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver = Mock_Driver{};

        auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

        auto const socket_id = random<Socket_ID>();

        auto const client = Client{ driver, socket_id, network_stack };

        auto const sn_mr = static_cast<std::uint8_t>(
            ( random<std::uint8_t>() & 0b1'1'0'1'1111 ) | test_case.sn_mr_nd );

        EXPECT_CALL( driver, read_sn_mr( socket_id ) ).WillOnce( Return( sn_mr ) );

        auto const result = client.no_delayed_ack_configuration();

        EXPECT_TRUE( result.is_value() );
        EXPECT_EQ( result.value(), test_case.no_delayed_ack_configuration );
    } // for
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::configure_maximum_segment_size()
 *        properly handles an SN_MSSR register write error.
 */
TEST( configureMaximumSegmentSize, snmssrWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_mssr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.configure_maximum_segment_size( random<std::uint16_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::configure_maximum_segment_size()
 *        works properly.
 */
TEST( configureMaximumSegmentSize, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto client = Client{ driver, socket_id, network_stack };

    auto const maximum_segment_size = random<std::uint16_t>();

    EXPECT_CALL( driver, write_sn_mssr( socket_id, maximum_segment_size ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( client.configure_maximum_segment_size( maximum_segment_size ).is_error() );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::maximum_segment_size()
 *        properly handles an SN_MSSR register read error.
 */
TEST( maximumSegmentSize, snmssrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_mssr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.maximum_segment_size();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::maximum_segment_size()
 *        works properly.
 */
TEST( maximumSegmentSize, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto const client = Client{ driver, socket_id, network_stack };

    auto const sn_mssr = random<std::uint16_t>();

    EXPECT_CALL( driver, read_sn_mssr( socket_id ) ).WillOnce( Return( sn_mssr ) );

    auto const result = client.maximum_segment_size();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_mssr );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::configure_time_to_live()
 *        properly handles an SN_TTL register write error.
 */
TEST( configureTimeToLive, snttlWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_ttl( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.configure_time_to_live( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::configure_time_to_live()
 *        works properly.
 */
TEST( configureTimeToLive, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto client = Client{ driver, socket_id, network_stack };

    auto const time_to_live = random<std::uint8_t>();

    EXPECT_CALL( driver, write_sn_ttl( socket_id, time_to_live ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( client.configure_time_to_live( time_to_live ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::time_to_live()
 *        properly handles an SN_TTL register read error.
 */
TEST( timeToLive, snttlReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_ttl( _ ) ).WillOnce( Return( error ) );

    auto const result = client.time_to_live();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::time_to_live()
 *        works properly.
 */
TEST( timeToLive, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto const client = Client{ driver, socket_id, network_stack };

    auto const sn_ttl = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_ttl( socket_id ) ).WillOnce( Return( sn_ttl ) );

    auto const result = client.time_to_live();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_ttl );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::configure_keepalive_period()
 *        properly handles an SN_KPALVTR register write error.
 */
TEST( configureKeepalivePeriod, snkpalvtrWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.configure_keepalive_period( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::configure_keepalive_period()
 *        works properly.
 */
TEST( configureKeepalivePeriod, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto client = Client{ driver, socket_id, network_stack };

    auto const keepalive_period = random<std::uint8_t>();

    EXPECT_CALL( driver, write_sn_kpalvtr( socket_id, keepalive_period ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( client.configure_keepalive_period( keepalive_period ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::keepalive_period()
 *        properly handles an SN_KPALVTR register read error.
 */
TEST( keepalivePeriod, snkpalvtrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_kpalvtr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.keepalive_period();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::keepalive_period()
 *        works properly.
 */
TEST( keepalivePeriod, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const socket_id = random<Socket_ID>();

    auto const client = Client{ driver, socket_id, network_stack };

    auto const sn_kpalvtr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_kpalvtr( socket_id ) ).WillOnce( Return( sn_kpalvtr ) );

    auto const result = client.keepalive_period();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_kpalvtr );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::bind() properly
 *        handles a SIPR register read error.
 */
TEST( bind, siprReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( error ) );

    auto result = client.bind( { random<Address>( 1 ), random<Port>() } );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::bind() properly
 *        handles an attempt to bind the socket to an invalid endpoint.
 */
TEST( bind, invalidEndpoint )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const [ address, endpoint_address ] = random_unique_addresses();

    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( address.as_byte_array() ) );

    auto result = client.bind( { endpoint_address, random<Port>() } );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::INVALID_ARGUMENT );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::bind() properly
 *        handles ephemeral port allocation not being enabled.
 */
TEST( bind, ephemeralPortAllocationNotEnabled )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const result = client.bind( Port{} );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::EPHEMERAL_PORTS_EXHAUSTED );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::bind() properly
 *        handles an SN_MR register read error.
 */
TEST( bind, snmrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillOnce( Return( error ) );

    auto result = client.bind( random<Port>( 1 ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::bind() properly
 *        handles an SN_PORT register read error.
 */
TEST( bind, snportReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( error ) );

    auto result = client.bind( random<Port>( 1 ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::bind() properly
 *        handles ephemeral port exhaustion.
 */
TEST( bind, ephemeralPortsExhausted )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto const ephemeral_port = random<Port>( 1 );

    EXPECT_FALSE( network_stack
                      .enable_tcp_ephemeral_port_allocation( ephemeral_port, ephemeral_port )
                      .is_error() );

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    EXPECT_CALL( driver, read_sn_mr( _ ) )
        .WillRepeatedly( Return( static_cast<std::uint8_t>(
            ( random<std::uint8_t>() & 0b1'1'1'1'0000 ) | 0b0001 ) ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillRepeatedly( Return( ephemeral_port.as_unsigned_integer() ) );

    auto const result = client.bind( Port{} );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::EPHEMERAL_PORTS_EXHAUSTED );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::bind() properly
 *        handles the requested endpoint being in use.
 */
TEST( bind, endpointInUse )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto port = random<Port>( 1 );

    EXPECT_CALL( driver, read_sn_mr( _ ) )
        .WillRepeatedly( Return( static_cast<std::uint8_t>(
            ( random<std::uint8_t>() & 0b1'1'1'1'0000 ) | 0b0001 ) ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillRepeatedly( Return( port.as_unsigned_integer() ) );

    auto const result = client.bind( port );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::ENDPOINT_IN_USE );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::bind() properly
 *        handles an SN_PORT register write error.
 */
TEST( bind, snportWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillRepeatedly( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillRepeatedly( Return( static_cast<std::uint16_t>( 0 ) ) );
    EXPECT_CALL( driver, write_sn_port( _, _ ) ).WillOnce( Return( error ) );

    auto result = client.bind( random<Port>( 1 ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::bind() properly
 *        handles an attempt to bind a socket that has already been bound.
 */
TEST( bind, alreadyBound )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillRepeatedly( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillRepeatedly( Return( static_cast<std::uint16_t>( 0 ) ) );
    EXPECT_CALL( driver, write_sn_port( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( client.bind( random<Port>( 1 ) ).is_error() );

    auto result = client.bind( random<Port>( 1 ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::LOGIC_ERROR );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::bind() works
 *        properly.
 */
TEST( bind, worksProperly )
{
    auto const generate_sn_port = []( std::uint8_t sn_mr, Port port ) {
        auto const sn_port = random<std::uint16_t>();

        return ( sn_mr & 0b0'0'0'0'1111 ) == 0b0001 and sn_port == port.as_unsigned_integer()
                   ? static_cast<std::uint16_t>( sn_port ^ random<std::uint16_t>() )
                   : sn_port;
    };

    {
        auto const in_sequence = InSequence{};

        auto driver = Mock_Driver{};

        auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

        auto const min = random<Port>( 1 );
        auto const max = random<Port>( min );

        EXPECT_FALSE( network_stack.enable_tcp_ephemeral_port_allocation( min, max ).is_error() );

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ driver, socket_id, network_stack };

        auto const port = random<Port>( min, max );
        for ( auto socket = std::uint8_t{}; socket < 8; ++socket ) {
            auto const sn_mr = random<std::uint8_t>();

            auto const sn_port = generate_sn_port( sn_mr, port );

            EXPECT_CALL( driver, read_sn_mr( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_mr ) );
            EXPECT_CALL( driver, read_sn_port( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_port ) );
        } // for

        EXPECT_CALL(
            driver,
            write_sn_port(
                socket_id, AllOf( Ge( min.as_unsigned_integer() ), Le( max.as_unsigned_integer() ) ) ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );

        EXPECT_FALSE( client.bind().is_error() );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver = Mock_Driver{};

        auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

        auto const min = random<Port>( 1 );
        auto const max = random<Port>( min );

        EXPECT_FALSE( network_stack.enable_tcp_ephemeral_port_allocation( min, max ).is_error() );

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ driver, socket_id, network_stack };

        auto const port = random<Port>( min, max );
        for ( auto socket = std::uint8_t{}; socket < 8; ++socket ) {
            auto const sn_mr = random<std::uint8_t>();

            auto const sn_port = generate_sn_port( sn_mr, port );

            EXPECT_CALL( driver, read_sn_mr( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_mr ) );
            EXPECT_CALL( driver, read_sn_port( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_port ) );
        } // for

        EXPECT_CALL(
            driver,
            write_sn_port(
                socket_id, AllOf( Ge( min.as_unsigned_integer() ), Le( max.as_unsigned_integer() ) ) ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );

        EXPECT_FALSE( client.bind( Port{} ).is_error() );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver = Mock_Driver{};

        auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ driver, socket_id, network_stack };

        auto const port = random<Port>( 1 );

        for ( auto socket = std::uint8_t{}; socket < 8; ++socket ) {
            auto const sn_mr = random<std::uint8_t>();

            auto const sn_port = generate_sn_port( sn_mr, port );

            EXPECT_CALL( driver, read_sn_mr( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_mr ) );
            EXPECT_CALL( driver, read_sn_port( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_port ) );
        } // for

        EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );

        EXPECT_FALSE( client.bind( port ).is_error() );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver = Mock_Driver{};

        auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

        auto const min = random<Port>( 1 );
        auto const max = random<Port>( min );

        EXPECT_FALSE( network_stack.enable_tcp_ephemeral_port_allocation( min, max ).is_error() );

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ driver, socket_id, network_stack };

        auto const address = random<Address>( 1 );

        EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( address.as_byte_array() ) );

        auto const port = random<Port>( min, max );
        for ( auto socket = std::uint8_t{}; socket < 8; ++socket ) {
            auto const sn_mr = random<std::uint8_t>();

            auto const sn_port = generate_sn_port( sn_mr, port );

            EXPECT_CALL( driver, read_sn_mr( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_mr ) );
            EXPECT_CALL( driver, read_sn_port( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_port ) );
        } // for

        EXPECT_CALL(
            driver,
            write_sn_port(
                socket_id, AllOf( Ge( min.as_unsigned_integer() ), Le( max.as_unsigned_integer() ) ) ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );

        EXPECT_FALSE( client.bind( { address, Port{} } ).is_error() );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver = Mock_Driver{};

        auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ driver, socket_id, network_stack };

        auto const address = random<Address>( 1 );
        auto const port    = random<Port>( 1 );

        EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( address.as_byte_array() ) );

        for ( auto socket = std::uint8_t{}; socket < 8; ++socket ) {
            auto const sn_mr = random<std::uint8_t>();

            auto const sn_port = generate_sn_port( sn_mr, port );

            EXPECT_CALL( driver, read_sn_mr( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_mr ) );
            EXPECT_CALL( driver, read_sn_port( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_port ) );
        } // for

        EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );

        EXPECT_FALSE( client.bind( { address, port } ).is_error() );
    }

    {
        struct {
            Buffer_Size       buffer_size;
            std::uint_fast8_t available_sockets;
        } const test_cases[]{
            // clang-format off

            { Buffer_Size::_2_KIB, 8 },
            { Buffer_Size::_4_KIB, 4 },
            { Buffer_Size::_8_KIB, 2 },
            { Buffer_Size::_16_KIB, 1 },
        };

        for ( auto const test_case : test_cases ) {
            auto driver = Mock_Driver{};

            auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

            EXPECT_CALL( driver, write_sn_rxbuf_size( _, _ ) ).WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_txbuf_size( _, _ ) ).WillRepeatedly( Return( Result<Void, Error_Code>{} ) );

            EXPECT_FALSE( network_stack.configure_socket_buffers( test_case.buffer_size ).is_error() );

            auto const min = random<Port>( 1 );
            auto const max = random<Port>( min );

            EXPECT_FALSE( network_stack.enable_tcp_ephemeral_port_allocation( min, max ).is_error() );

            auto const socket_id = random<Socket_ID>();

            auto const in_sequence = InSequence{};

            auto client = Client{ driver, socket_id, network_stack };

            auto const port = random<Port>( min, max );
            for ( auto socket = std::uint8_t{}; socket < test_case.available_sockets; ++socket ) {
                auto const sn_mr = random<std::uint8_t>();

                auto const sn_port = generate_sn_port( sn_mr, port );

                EXPECT_CALL( driver, read_sn_mr( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_mr ) );
                EXPECT_CALL( driver, read_sn_port( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_port ) );
            } // for

            EXPECT_CALL( driver, write_sn_port( socket_id, AllOf( Ge( min.as_unsigned_integer() ), Le( max.as_unsigned_integer() ) ) ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

            EXPECT_FALSE( client.bind().is_error() );
        } // for
    }

    {
        struct {
            Buffer_Size       buffer_size;
            std::uint_fast8_t available_sockets;
        } const test_cases[]{
            // clang-format off

            { Buffer_Size::_2_KIB, 8 },
            { Buffer_Size::_4_KIB, 4 },
            { Buffer_Size::_8_KIB, 2 },
            { Buffer_Size::_16_KIB, 1 },
        };

        for ( auto const test_case : test_cases ) {
            auto driver = Mock_Driver{};

            auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

            EXPECT_CALL( driver, write_sn_rxbuf_size( _, _ ) ).WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_txbuf_size( _, _ ) ).WillRepeatedly( Return( Result<Void, Error_Code>{} ) );

            EXPECT_FALSE( network_stack.configure_socket_buffers( test_case.buffer_size ).is_error() );

            auto const min = random<Port>( 1 );
            auto const max = random<Port>( min );

            EXPECT_FALSE( network_stack.enable_tcp_ephemeral_port_allocation( min, max ).is_error() );

            auto const socket_id = random<Socket_ID>();

            auto const in_sequence = InSequence{};

            auto client = Client{ driver, socket_id, network_stack };

            auto const port = random<Port>( min, max );
            for ( auto socket = std::uint8_t{}; socket < test_case.available_sockets; ++socket ) {
                auto const sn_mr = random<std::uint8_t>();

                auto const sn_port = generate_sn_port( sn_mr, port );

                EXPECT_CALL( driver, read_sn_mr( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_mr ) );
                EXPECT_CALL( driver, read_sn_port( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_port ) );
            } // for

            EXPECT_CALL( driver, write_sn_port( socket_id, AllOf( Ge( min.as_unsigned_integer() ), Le( max.as_unsigned_integer() ) ) ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

            EXPECT_FALSE( client.bind( Port{} ).is_error() );
        } // for
    }

    {
        struct {
            Buffer_Size       buffer_size;
            std::uint_fast8_t available_sockets;
        } const test_cases[]{
            // clang-format off

            { Buffer_Size::_2_KIB, 8 },
            { Buffer_Size::_4_KIB, 4 },
            { Buffer_Size::_8_KIB, 2 },
            { Buffer_Size::_16_KIB, 1 },
        };

        for ( auto const test_case : test_cases ) {
            auto driver = Mock_Driver{};

            auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

            EXPECT_CALL( driver, write_sn_rxbuf_size( _, _ ) ).WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_txbuf_size( _, _ ) ).WillRepeatedly( Return( Result<Void, Error_Code>{} ) );

            EXPECT_FALSE( network_stack.configure_socket_buffers( test_case.buffer_size ).is_error() );

            auto const socket_id = random<Socket_ID>();

            auto const in_sequence = InSequence{};

            auto client = Client{ driver, socket_id, network_stack };

            auto const port = random<Port>( 1 );

            for ( auto socket = std::uint8_t{}; socket < test_case.available_sockets; ++socket ) {
                auto const sn_mr = random<std::uint8_t>();

                auto const sn_port = generate_sn_port( sn_mr, port );

                EXPECT_CALL( driver, read_sn_mr( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_mr ) );
                EXPECT_CALL( driver, read_sn_port( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_port ) );
            } // for

            EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

            EXPECT_FALSE( client.bind( port ).is_error() );
        } // for
    }

    {
        struct {
            Buffer_Size       buffer_size;
            std::uint_fast8_t available_sockets;
        } const test_cases[]{
            // clang-format off

            { Buffer_Size::_2_KIB, 8 },
            { Buffer_Size::_4_KIB, 4 },
            { Buffer_Size::_8_KIB, 2 },
            { Buffer_Size::_16_KIB, 1 },
        };

        for ( auto const test_case : test_cases ) {
            auto driver = Mock_Driver{};

            auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

            EXPECT_CALL( driver, write_sn_rxbuf_size( _, _ ) ).WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_txbuf_size( _, _ ) ).WillRepeatedly( Return( Result<Void, Error_Code>{} ) );

            EXPECT_FALSE( network_stack.configure_socket_buffers( test_case.buffer_size ).is_error() );

            auto const min = random<Port>( 1 );
            auto const max = random<Port>( min );

            EXPECT_FALSE( network_stack.enable_tcp_ephemeral_port_allocation( min, max ).is_error() );

            auto const socket_id = random<Socket_ID>();

            auto const in_sequence = InSequence{};

            auto client = Client{ driver, socket_id, network_stack };

            auto const address = random<Address>( 1 );

            EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( address.as_byte_array() ) );

            auto const port = random<Port>( min, max );
            for ( auto socket = std::uint8_t{}; socket < test_case.available_sockets; ++socket ) {
                auto const sn_mr = random<std::uint8_t>();

                auto const sn_port = generate_sn_port( sn_mr, port );

                EXPECT_CALL( driver, read_sn_mr( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_mr ) );
                EXPECT_CALL( driver, read_sn_port( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_port ) );
            } // for

            EXPECT_CALL( driver, write_sn_port( socket_id, AllOf( Ge( min.as_unsigned_integer() ), Le( max.as_unsigned_integer() ) ) ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

            EXPECT_FALSE( client.bind( { address, Port{} } ).is_error() );
        } // for
    }

    {
        struct {
            Buffer_Size       buffer_size;
            std::uint_fast8_t available_sockets;
        } const test_cases[]{
            // clang-format off

            { Buffer_Size::_2_KIB, 8 },
            { Buffer_Size::_4_KIB, 4 },
            { Buffer_Size::_8_KIB, 2 },
            { Buffer_Size::_16_KIB, 1 },
        };

        for ( auto const test_case : test_cases ) {
            auto driver = Mock_Driver{};

            auto network_stack = Network_Stack{ driver, random<Mock_Error>() };

            EXPECT_CALL( driver, write_sn_rxbuf_size( _, _ ) ).WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_txbuf_size( _, _ ) ).WillRepeatedly( Return( Result<Void, Error_Code>{} ) );

            EXPECT_FALSE( network_stack.configure_socket_buffers( test_case.buffer_size ).is_error() );

            auto const socket_id = random<Socket_ID>();

            auto const in_sequence = InSequence{};

            auto client = Client{ driver, socket_id, network_stack };

            auto const address = random<Address>( 1 );
            auto const port = random<Port>( 1 );

            EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( address.as_byte_array() ) );

            for ( auto socket = std::uint8_t{}; socket < test_case.available_sockets; ++socket ) {
                auto const sn_mr = random<std::uint8_t>();

                auto const sn_port = generate_sn_port( sn_mr, port );

                EXPECT_CALL( driver, read_sn_mr( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_mr ) );
                EXPECT_CALL( driver, read_sn_port( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_port ) );
            } // for

            EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

            EXPECT_FALSE( client.bind( { address, port } ).is_error() );
        } // for
    }
}

/**
 * \brief Execute the picolibrary::WIZnet::W5500::Network_Stack::TCP_Client unit tests.
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

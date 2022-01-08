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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Acceptor unit test program.
 */

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/array.h"
#include "picolibrary/error.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/ip/tcp.h"
#include "picolibrary/testing/unit/ipv4.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/wiznet/w5500.h"
#include "picolibrary/testing/unit/wiznet/w5500/ip/network_stack.h"
#include "picolibrary/vector.h"
#include "picolibrary/void.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/ip/tcp.h"

namespace {

using ::picolibrary::Array;
using ::picolibrary::Error_Code;
using ::picolibrary::Fixed_Capacity_Vector;
using ::picolibrary::Generic_Error;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::IP::TCP::Port;
using ::picolibrary::IPv4::Address;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::pseudo_random_number_generator;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::WIZnet::W5500::Mock_Driver;
using ::picolibrary::Testing::Unit::WIZnet::W5500::IP::Mock_Network_Stack;
using ::picolibrary::WIZnet::W5500::No_Delayed_ACK;
using ::picolibrary::WIZnet::W5500::Socket_ID;
using ::testing::_;
using ::testing::A;
using ::testing::AllOf;
using ::testing::AnyNumber;
using ::testing::Ge;
using ::testing::InSequence;
using ::testing::Le;
using ::testing::Return;

using Acceptor = ::picolibrary::WIZnet::W5500::IP::TCP::Acceptor<Mock_Driver, Mock_Network_Stack>;
using State = Acceptor::State;

auto random_unique_socket_ids( std::uint_fast8_t backlog = random<std::uint_fast8_t>( 1, 8 ), std::uint_fast8_t sockets = 8 )
{
    if ( sockets > 8 ) {
        throw std::invalid_argument{ "sockets > 8" };
    } // if

    if ( backlog > sockets ) {
        throw std::invalid_argument{ "backlog > sockets" };
    } // if

    auto socket_ids = Array<Socket_ID, 8>{
        Socket_ID::_0, Socket_ID::_1, Socket_ID::_2, Socket_ID::_3,
        Socket_ID::_4, Socket_ID::_5, Socket_ID::_6, Socket_ID::_7,
    };

    std::shuffle( socket_ids.begin(), socket_ids.begin() + sockets, pseudo_random_number_generator() );

    return std::vector<Socket_ID>{ socket_ids.begin(), socket_ids.begin() + backlog };
}

auto sorted_socket_ids( std::vector<Socket_ID> socket_ids )
{
    std::sort( socket_ids.begin(), socket_ids.end() );

    return socket_ids;
}

auto sorted_socket_ids( Fixed_Capacity_Vector<Socket_ID, 8> const & socket_ids )
{
    return sorted_socket_ids( std::vector<Socket_ID>{ socket_ids.begin(), socket_ids.end() } );
}

auto socket_interrupt_mask( std::vector<Socket_ID> const & socket_ids )
{
    auto mask = std::uint8_t{};

    for ( auto const socket_id : socket_ids ) {
        mask |= 1 << ( static_cast<std::uint_fast8_t>( socket_id ) >> 5 );
    } // for

    return mask;
}

auto random_unique_addresses()
{
    auto const a = random<Address::Unsigned_Integer>();
    auto const b = random<Address::Unsigned_Integer>();

    return std::pair<Address, Address>{ a, b != a ? b : b ^ random<Address::Unsigned_Integer>( 1 ) };
}

} // namespace

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::Acceptor() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const acceptor = Acceptor{};

    EXPECT_EQ( acceptor.state(), State::UNINITIALIZED );
    EXPECT_TRUE( acceptor.socket_ids().empty() );
    EXPECT_EQ( acceptor.backlog(), 0 );
    EXPECT_EQ( acceptor.socket_interrupt_mask(), 0 );
    EXPECT_EQ( acceptor.maximum_segment_size(), 0x0000 );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::Acceptor( Driver &,
 *        picolibrary::WIZnet::W5500::Socket_ID, Network_Stack & ) works properly.
 */
TEST( constructorSocketID, worksProperly )
{
    struct {
        Socket_ID    socket_id;
        std::uint8_t socket_interrupt_mask;
    } const test_cases[]{
        // clang-format off

        { Socket_ID::_0, 0b00000001 },
        { Socket_ID::_1, 0b00000010 },
        { Socket_ID::_2, 0b00000100 },
        { Socket_ID::_3, 0b00001000 },
        { Socket_ID::_4, 0b00010000 },
        { Socket_ID::_5, 0b00100000 },
        { Socket_ID::_6, 0b01000000 },
        { Socket_ID::_7, 0b10000000 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const acceptor = Acceptor{ driver, test_case.socket_id, network_stack };

        EXPECT_EQ( acceptor.state(), State::INITIALIZED );
        EXPECT_EQ( acceptor.socket_ids().size(), 1 );
        EXPECT_EQ( acceptor.socket_ids().front(), test_case.socket_id );
        EXPECT_EQ( acceptor.backlog(), 1 );
        EXPECT_EQ( acceptor.socket_interrupt_mask(), test_case.socket_interrupt_mask );
        EXPECT_EQ( acceptor.maximum_segment_size(), 0x0000 );

        EXPECT_CALL( network_stack, deallocate_socket( test_case.socket_id ) );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::Acceptor( Driver &,
 *        picolibrary::WIZnet::W5500::Socket_ID const *,
 *        picolibrary::WIZnet::W5500::Socket_ID const *, Network_Stack & ) works properly.
 */
TEST( constructorSocketIDs, worksProperly )
{
    struct {
        std::uint_fast8_t backlog;
    } const test_cases[]{
        // clang-format off

        { 1 },
        { 2 },
        { 3 },
        { 4 },
        { 5 },
        { 6 },
        { 7 },
        { 8 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_ids = random_unique_socket_ids( test_case.backlog );

        auto const acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

        EXPECT_EQ( acceptor.state(), State::INITIALIZED );
        EXPECT_EQ( sorted_socket_ids( acceptor.socket_ids() ), sorted_socket_ids( socket_ids ) );
        EXPECT_EQ( acceptor.backlog(), test_case.backlog );
        EXPECT_EQ( acceptor.socket_interrupt_mask(), socket_interrupt_mask( socket_ids ) );
        EXPECT_EQ( acceptor.maximum_segment_size(), 0x0000 );

        for ( auto const socket_id : socket_ids ) {
            EXPECT_CALL( network_stack, deallocate_socket( socket_id ) );
        } // for
    }     // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::enable_interrupts()
 *        properly handles an SN_IMR register read error.
 */
TEST( enableInterrupts, snimrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_imr( _ ) ).WillOnce( Return( error ) );

    auto const result = acceptor.enable_interrupts( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::enable_interrupts()
 *        properly handles an SN_IMR register write error.
 */
TEST( enableInterrupts, snimrWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_imr( _ ) ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_sn_imr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = acceptor.enable_interrupts( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::enable_interrupts() works
 *        properly.
 */
TEST( enableInterrupts, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const sn_imr = random<std::uint8_t>();
    auto const mask   = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_imr( socket_ids.front() ) ).WillOnce( Return( sn_imr ) );
    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( driver, write_sn_imr( socket_id, sn_imr | mask ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
    } // for

    EXPECT_FALSE( acceptor.enable_interrupts( mask ).is_error() );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::disable_interrupts(
 *        std::uint8_t ) properly handles an SN_IMR register read error.
 */
TEST( disableInterrupts, snimrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_imr( _ ) ).WillOnce( Return( error ) );

    auto const result = acceptor.disable_interrupts( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::disable_interrupts(
 *        std::uint8_t ) properly handles an SN_IMR register write error.
 */
TEST( disableInterrupts, snimrWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_imr( _ ) ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_sn_imr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = acceptor.disable_interrupts( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::disable_interrupts(
 *        std::uint8_t ) works properly.
 */
TEST( disableInterrupts, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const sn_imr = random<std::uint8_t>();
    auto const mask   = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_imr( socket_ids[ 0 ] ) ).WillOnce( Return( sn_imr ) );
    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( driver, write_sn_imr( socket_id, sn_imr & ~mask ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
    } // for

    EXPECT_FALSE( acceptor.disable_interrupts( mask ).is_error() );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::disable_interrupts()
 *        properly handles an SN_IMR register write error.
 */
TEST( disableAllInterrupts, snimrWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_imr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = acceptor.disable_interrupts();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::disable_interrupts() works
 *        properly.
 */
TEST( disableAllInterrupts, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( driver, write_sn_imr( socket_id, 0x00 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    } // for

    EXPECT_FALSE( acceptor.disable_interrupts().is_error() );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::enabled_interrupts()
 *        properly handles an SN_IMR register read error.
 */
TEST( enabledInterrupts, snimrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto const acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_imr( _ ) ).WillOnce( Return( error ) );

    auto const result = acceptor.enabled_interrupts();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::enabled_interrupts() works
 *        properly.
 */
TEST( enabledInterrupts, worksProperly )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto const acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const sn_imr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_imr( socket_ids.front() ) ).WillOnce( Return( sn_imr ) );

    auto const result = acceptor.enabled_interrupts();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_imr );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::interrupt_context()
 *        properly handles an SN_IR register read error.
 */
TEST( interruptContext, snirReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto const acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_ir( _ ) ).WillOnce( Return( error ) );

    auto const result = acceptor.interrupt_context();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::interrupt_context() works
 *        properly.
 */
TEST( interruptContext, worksProperly )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto const acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto interrupt_context = std::uint8_t{};

    for ( auto const socket_id : socket_ids ) {
        auto const sn_ir = random<std::uint8_t>();

        EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( sn_ir ) );

        interrupt_context |= sn_ir;
    } // for

    auto const result = acceptor.interrupt_context();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), interrupt_context );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::configure_no_delayed_ack()
 *        properly handles an SN_MR register write error.
 */
TEST( configureNoDelayedAck, snmrWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_mr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = acceptor.configure_no_delayed_ack( random<No_Delayed_ACK>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::configure_no_delayed_ack()
 *        works properly.
 */
TEST( configureNoDelayedAck, worksProperly )
{
    struct {
        No_Delayed_ACK no_delayed_ack_configuration;
        std::uint8_t   sn_mr;
    } const test_cases[]{
        // clang-format off

        { No_Delayed_ACK::DISABLED, 0b0'0'0'0'0001 },
        { No_Delayed_ACK::ENABLED,  0b0'0'1'0'0001 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const in_sequence = InSequence{};

        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_ids = random_unique_socket_ids();

        auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

        for ( auto const socket_id : socket_ids ) {
            EXPECT_CALL( driver, write_sn_mr( socket_id, test_case.sn_mr ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
        } // for

        EXPECT_FALSE(
            acceptor.configure_no_delayed_ack( test_case.no_delayed_ack_configuration ).is_error() );

        EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Acceptor::no_delayed_ack_configuration()
 *        properly handles an SN_MR register read error.
 */
TEST( noDelayedAckConfiguration, snimrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto const acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillOnce( Return( error ) );

    auto const result = acceptor.no_delayed_ack_configuration();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Acceptor::no_delayed_ack_configuration()
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
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_ids = random_unique_socket_ids();

        auto const acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

        EXPECT_CALL( driver, read_sn_mr( socket_ids.front() ) )
            .WillOnce( Return( static_cast<std::uint8_t>(
                ( random<std::uint8_t>() & 0b1'1'0'1'1111 ) | test_case.sn_mr_nd ) ) );

        auto const result = acceptor.no_delayed_ack_configuration();

        EXPECT_TRUE( result.is_value() );
        EXPECT_EQ( result.value(), test_case.no_delayed_ack_configuration );

        EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Acceptor::configure_maximum_segment_size()
 *        properly handles an SN_MSSR register write error.
 */
TEST( configureMaximumSegmentSize, snmssrWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_mssr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = acceptor.configure_maximum_segment_size( random<std::uint16_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( acceptor.maximum_segment_size(), 0x0000 );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Acceptor::configure_maximum_segment_size()
 *        works properly.
 */
TEST( configureMaximumSegmentSize, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const maximum_segment_size = random<std::uint16_t>();

    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( driver, write_sn_mssr( socket_id, maximum_segment_size ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
    } // for

    EXPECT_FALSE( acceptor.configure_maximum_segment_size( maximum_segment_size ).is_error() );

    EXPECT_EQ( acceptor.maximum_segment_size(), maximum_segment_size );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::configure_time_to_live()
 *        properly handles an SN_TTL register write error.
 */
TEST( configureTimeToLive, snttlWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_ttl( _, _ ) ).WillOnce( Return( error ) );

    auto const result = acceptor.configure_time_to_live( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::configure_time_to_live()
 *        works properly.
 */
TEST( configureTimeToLive, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const time_to_live = random<std::uint8_t>();

    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( driver, write_sn_ttl( socket_id, time_to_live ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
    } // for

    EXPECT_FALSE( acceptor.configure_time_to_live( time_to_live ).is_error() );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::time_to_live() properly
 *        handles an SN_IMR register read error.
 */
TEST( timeToLive, snttlReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto const acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_ttl( _ ) ).WillOnce( Return( error ) );

    auto const result = acceptor.time_to_live();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::time_to_live() works
 *        properly.
 */
TEST( timeToLive, worksProperly )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto const acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const sn_ttl = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_ttl( socket_ids.front() ) ).WillOnce( Return( sn_ttl ) );

    auto const result = acceptor.time_to_live();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_ttl );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Acceptor::configure_keepalive_period()
 *        properly handles an SN_KPALVTR register write error.
 */
TEST( configureKeepalivePeriod, snkpalvtrWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = acceptor.configure_keepalive_period( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Acceptor::configure_keepalive_period()
 *        works properly.
 */
TEST( configureKeepalivePeriod, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const keepalive_period = random<std::uint8_t>();

    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( driver, write_sn_kpalvtr( socket_id, keepalive_period ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
    } // for

    EXPECT_FALSE( acceptor.configure_keepalive_period( keepalive_period ).is_error() );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::keepalive_period()
 *        properly handles an SN_IMR register read error.
 */
TEST( keepalivePeriod, snkpalvtrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto const acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_kpalvtr( _ ) ).WillOnce( Return( error ) );

    auto const result = acceptor.keepalive_period();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::keepalive_period() works
 *        properly.
 */
TEST( keepalivePeriod, worksProperly )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto const acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const sn_kpalvtr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_kpalvtr( socket_ids.front() ) ).WillOnce( Return( sn_kpalvtr ) );

    auto const result = acceptor.keepalive_period();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_kpalvtr );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::bind() properly handles an
 *        attempt to bind a socket that is not in a state that allows it to be bound.
 */
TEST( bind, invalidState )
{
    struct {
        State state;
    } const test_cases[]{
        // clang-format off

        { State::UNINITIALIZED },
        { State::BOUND         },
        { State::LISTENING     },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_ids = random_unique_socket_ids();

        auto acceptor = Acceptor{ test_case.state, driver, socket_ids.begin(), socket_ids.end(), network_stack };

        auto const result = acceptor.bind();

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::LOGIC_ERROR );

        EXPECT_EQ( acceptor.state(), test_case.state );

        EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::bind() properly handles a
 *        SIPR register read error.
 */
TEST( bind, siprReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( error ) );

    auto const result = acceptor.bind( { random<Address>( 1 ), random<Port>() } );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( acceptor.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::bind() properly handles an
 *        attempt to bind the socket to an invalid endpoint.
 */
TEST( bind, invalidEndpoint )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const [ address, endpoint_address ] = random_unique_addresses();

    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( address.as_byte_array() ) );

    auto const result = acceptor.bind( { endpoint_address, random<Port>() } );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::INVALID_ARGUMENT );

    EXPECT_EQ( acceptor.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::bind() properly handles
 *        ephemeral port allocation not being enabled.
 */
TEST( bind, ephemeralPortAllocationNotEnabled )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    EXPECT_CALL( network_stack, tcp_ephemeral_port_allocation_enabled() ).WillOnce( Return( false ) );

    auto const result = acceptor.bind();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::EPHEMERAL_PORTS_EXHAUSTED );

    EXPECT_EQ( acceptor.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::bind() properly handles an
 *        SN_MR register read error.
 */
TEST( bind, snmrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( network_stack, sockets() ).WillOnce( Return( random<std::uint_fast8_t>( 1, 8 ) ) );
    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillOnce( Return( error ) );

    auto const result = acceptor.bind( random<Port>( 1 ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( acceptor.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::bind() properly handles an
 *        SN_PORT register read error.
 */
TEST( bind, snportReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( network_stack, sockets() ).WillOnce( Return( random<std::uint_fast8_t>( 1, 8 ) ) );
    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillRepeatedly( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( error ) );

    auto const result = acceptor.bind( random<Port>( 1 ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( acceptor.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::bind() properly handles
 *        ephemeral port exhaustion.
 */
TEST( bind, ephemeralPortsExhausted )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const ephemeral_port = random<Port>( 1 );

    EXPECT_CALL( network_stack, tcp_ephemeral_port_allocation_enabled() ).WillOnce( Return( true ) );
    EXPECT_CALL( network_stack, sockets() ).WillOnce( Return( random<std::uint_fast8_t>( 1, 8 ) ) );
    EXPECT_CALL( driver, read_sn_mr( _ ) )
        .WillRepeatedly( Return( static_cast<std::uint8_t>(
            ( random<std::uint8_t>() & 0b1'1'1'1'0000 ) | 0b0001 ) ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillRepeatedly( Return( ephemeral_port.as_unsigned_integer() ) );
    EXPECT_CALL( network_stack, tcp_ephemeral_port_min() ).WillOnce( Return( ephemeral_port ) );
    EXPECT_CALL( network_stack, tcp_ephemeral_port_max() ).WillOnce( Return( ephemeral_port ) );

    auto const result = acceptor.bind();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::EPHEMERAL_PORTS_EXHAUSTED );

    EXPECT_EQ( acceptor.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::bind() properly handles
 *        the requested endpoint being in use.
 */
TEST( bind, endpointInUse )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const port = random<Port>( 1 );

    EXPECT_CALL( network_stack, sockets() ).WillOnce( Return( random<std::uint_fast8_t>( 1, 8 ) ) );
    EXPECT_CALL( driver, read_sn_mr( _ ) )
        .WillRepeatedly( Return( static_cast<std::uint8_t>(
            ( random<std::uint8_t>() & 0b1'1'1'1'0000 ) | 0b0001 ) ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillRepeatedly( Return( port.as_unsigned_integer() ) );

    auto const result = acceptor.bind( port );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::ENDPOINT_IN_USE );

    EXPECT_EQ( acceptor.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::bind() properly handles an
 *        SN_PORT register write error.
 */
TEST( bind, snportWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( network_stack, sockets() ).WillOnce( Return( random<std::uint_fast8_t>( 1, 8 ) ) );
    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillRepeatedly( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillRepeatedly( Return( std::uint16_t{ 0 } ) );
    EXPECT_CALL( driver, write_sn_port( _, _ ) ).WillOnce( Return( error ) );

    auto const result = acceptor.bind( random<Port>( 1 ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( acceptor.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::bind() properly handles an
 *        SN_CR register write error.
 */
TEST( bind, sncrWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( network_stack, sockets() ).WillOnce( Return( random<std::uint_fast8_t>( 1, 8 ) ) );
    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillRepeatedly( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillRepeatedly( Return( std::uint16_t{ 0 } ) );
    EXPECT_CALL( driver, write_sn_port( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = acceptor.bind( random<Port>( 1 ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( acceptor.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::bind() properly handles an
 *        SN_CR register read error.
 */
TEST( bind, sncrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( network_stack, sockets() ).WillOnce( Return( random<std::uint_fast8_t>( 1, 8 ) ) );
    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillRepeatedly( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillRepeatedly( Return( std::uint16_t{ 0 } ) );
    EXPECT_CALL( driver, write_sn_port( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( error ) );

    auto const result = acceptor.bind( random<Port>( 1 ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( acceptor.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::bind() properly handles an
 *        SN_SR register read error.
 */
TEST( bind, snsrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_ids = random_unique_socket_ids();

    auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( network_stack, sockets() ).WillOnce( Return( random<std::uint_fast8_t>( 1, 8 ) ) );
    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillRepeatedly( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillRepeatedly( Return( std::uint16_t{ 0 } ) );
    EXPECT_CALL( driver, write_sn_port( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );
    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( error ) );

    auto const result = acceptor.bind( random<Port>( 1 ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( acceptor.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::bind() properly handles an
 *        invalid SN_SR register value.
 */
TEST( bind, invalidSNSRValue )
{
    struct {
        std::uint8_t sn_sr;
    } const test_cases[]{
        // clang-format off

        { random<std::uint8_t>( 0x01, 0x12 ) },
        { random<std::uint8_t>( 0x14       ) },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_ids = random_unique_socket_ids();

        auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

        auto const error = random<Mock_Error>();

        EXPECT_CALL( network_stack, sockets() ).WillOnce( Return( random<std::uint_fast8_t>( 1, 8 ) ) );
        EXPECT_CALL( driver, read_sn_mr( _ ) ).WillRepeatedly( Return( random<std::uint8_t>() ) );
        EXPECT_CALL( driver, read_sn_port( _ ) ).WillRepeatedly( Return( std::uint16_t{ 0 } ) );
        EXPECT_CALL( driver, write_sn_port( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );
        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );
        EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( error ) );

        auto const result = acceptor.bind( random<Port>( 1 ) );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );

        EXPECT_EQ( acceptor.state(), State::INITIALIZED );

        EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Acceptor::bind() works properly.
 */
TEST( bind, worksProperly )
{
    auto const generate_sn_port = []( std::uint8_t sn_mr, Port reserved_port ) {
        auto const sn_port = random<std::uint16_t>();

        return ( sn_mr & 0b0'0'0'0'1111 ) == 0b0001 and sn_port == reserved_port.as_unsigned_integer()
                   ? static_cast<std::uint16_t>( sn_port ^ random<std::uint16_t>( 1 ) )
                   : sn_port;
    };

    {
        auto const in_sequence = InSequence{};

        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const sockets = random<std::uint_fast8_t>( 1, 8 );

        auto const socket_ids = random_unique_socket_ids(
            random<std::uint_fast8_t>( 1, sockets ), sockets );

        auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

        auto const ephemeral_port_min = random<Port>( 1 );
        auto const ephemeral_port_max = random<Port>( ephemeral_port_min );
        auto const reserved_ephemeral_port = random<Port>( ephemeral_port_min, ephemeral_port_max );

        EXPECT_CALL( network_stack, tcp_ephemeral_port_allocation_enabled() ).WillOnce( Return( true ) );
        EXPECT_CALL( network_stack, sockets() ).WillOnce( Return( sockets ) );
        for ( auto socket = std::uint_fast8_t{}; socket < sockets; ++socket ) {
            auto const sn_mr   = random<std::uint8_t>();
            auto const sn_port = generate_sn_port( sn_mr, reserved_ephemeral_port );

            EXPECT_CALL( driver, read_sn_mr( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_mr ) );
            EXPECT_CALL( driver, read_sn_port( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_port ) );
        } // for
        EXPECT_CALL( network_stack, tcp_ephemeral_port_min() ).WillOnce( Return( ephemeral_port_min ) );
        EXPECT_CALL( network_stack, tcp_ephemeral_port_max() ).WillOnce( Return( ephemeral_port_max ) );
        for ( auto const socket_id : socket_ids ) {
            EXPECT_CALL(
                driver,
                write_sn_port(
                    socket_id,
                    AllOf(
                        Ge( ephemeral_port_min.as_unsigned_integer() ),
                        Le( ephemeral_port_max.as_unsigned_integer() ) ) ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );
            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );
            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x13 } ) );
        } // for

        EXPECT_FALSE( acceptor.bind().is_error() );

        EXPECT_EQ( acceptor.state(), State::BOUND );

        EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const sockets = random<std::uint_fast8_t>( 1, 8 );

        auto const socket_ids = random_unique_socket_ids(
            random<std::uint_fast8_t>( 1, sockets ), sockets );

        auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

        auto const ephemeral_port_min = random<Port>( 1 );
        auto const ephemeral_port_max = random<Port>( ephemeral_port_min );
        auto const reserved_ephemeral_port = random<Port>( ephemeral_port_min, ephemeral_port_max );

        EXPECT_CALL( network_stack, tcp_ephemeral_port_allocation_enabled() ).WillOnce( Return( true ) );
        EXPECT_CALL( network_stack, sockets() ).WillOnce( Return( sockets ) );
        for ( auto socket = std::uint_fast8_t{}; socket < sockets; ++socket ) {
            auto const sn_mr   = random<std::uint8_t>();
            auto const sn_port = generate_sn_port( sn_mr, reserved_ephemeral_port );

            EXPECT_CALL( driver, read_sn_mr( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_mr ) );
            EXPECT_CALL( driver, read_sn_port( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_port ) );
        } // for
        EXPECT_CALL( network_stack, tcp_ephemeral_port_min() ).WillOnce( Return( ephemeral_port_min ) );
        EXPECT_CALL( network_stack, tcp_ephemeral_port_max() ).WillOnce( Return( ephemeral_port_max ) );
        for ( auto const socket_id : socket_ids ) {
            EXPECT_CALL(
                driver,
                write_sn_port(
                    socket_id,
                    AllOf(
                        Ge( ephemeral_port_min.as_unsigned_integer() ),
                        Le( ephemeral_port_max.as_unsigned_integer() ) ) ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );
            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );
            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x13 } ) );
        } // for

        EXPECT_FALSE( acceptor.bind( Port{} ).is_error() );

        EXPECT_EQ( acceptor.state(), State::BOUND );

        EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const sockets = random<std::uint_fast8_t>( 1, 8 );

        auto const socket_ids = random_unique_socket_ids(
            random<std::uint_fast8_t>( 1, sockets ), sockets );

        auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

        auto const port = random<Port>( 1 );

        EXPECT_CALL( network_stack, sockets() ).WillOnce( Return( sockets ) );
        for ( auto socket = std::uint_fast8_t{}; socket < sockets; ++socket ) {
            auto const sn_mr   = random<std::uint8_t>();
            auto const sn_port = generate_sn_port( sn_mr, port );

            EXPECT_CALL( driver, read_sn_mr( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_mr ) );
            EXPECT_CALL( driver, read_sn_port( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_port ) );
        } // for
        for ( auto const socket_id : socket_ids ) {
            EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );
            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );
            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x13 } ) );
        } // for

        EXPECT_FALSE( acceptor.bind( port ).is_error() );

        EXPECT_EQ( acceptor.state(), State::BOUND );

        EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const sockets = random<std::uint_fast8_t>( 1, 8 );

        auto const socket_ids = random_unique_socket_ids(
            random<std::uint_fast8_t>( 1, sockets ), sockets );

        auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

        auto const address            = random<Address>( 1 );
        auto const ephemeral_port_min = random<Port>( 1 );
        auto const ephemeral_port_max = random<Port>( ephemeral_port_min );
        auto const reserved_ephemeral_port = random<Port>( ephemeral_port_min, ephemeral_port_max );

        EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( address.as_byte_array() ) );
        EXPECT_CALL( network_stack, tcp_ephemeral_port_allocation_enabled() ).WillOnce( Return( true ) );
        EXPECT_CALL( network_stack, sockets() ).WillOnce( Return( sockets ) );
        for ( auto socket = std::uint_fast8_t{}; socket < sockets; ++socket ) {
            auto const sn_mr   = random<std::uint8_t>();
            auto const sn_port = generate_sn_port( sn_mr, reserved_ephemeral_port );

            EXPECT_CALL( driver, read_sn_mr( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_mr ) );
            EXPECT_CALL( driver, read_sn_port( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_port ) );
        } // for
        EXPECT_CALL( network_stack, tcp_ephemeral_port_min() ).WillOnce( Return( ephemeral_port_min ) );
        EXPECT_CALL( network_stack, tcp_ephemeral_port_max() ).WillOnce( Return( ephemeral_port_max ) );
        for ( auto const socket_id : socket_ids ) {
            EXPECT_CALL(
                driver,
                write_sn_port(
                    socket_id,
                    AllOf(
                        Ge( ephemeral_port_min.as_unsigned_integer() ),
                        Le( ephemeral_port_max.as_unsigned_integer() ) ) ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );
            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );
            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x13 } ) );
        } // for

        EXPECT_FALSE( acceptor.bind( { address, Port{} } ).is_error() );

        EXPECT_EQ( acceptor.state(), State::BOUND );

        EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const sockets = random<std::uint_fast8_t>( 1, 8 );

        auto const socket_ids = random_unique_socket_ids(
            random<std::uint_fast8_t>( 1, sockets ), sockets );

        auto acceptor = Acceptor{ driver, socket_ids.begin(), socket_ids.end(), network_stack };

        auto const address = random<Address>( 1 );
        auto const port    = random<Port>( 1 );

        EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( address.as_byte_array() ) );
        EXPECT_CALL( network_stack, sockets() ).WillOnce( Return( sockets ) );
        for ( auto socket = std::uint_fast8_t{}; socket < sockets; ++socket ) {
            auto const sn_mr   = random<std::uint8_t>();
            auto const sn_port = generate_sn_port( sn_mr, port );

            EXPECT_CALL( driver, read_sn_mr( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_mr ) );
            EXPECT_CALL( driver, read_sn_port( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_port ) );
        } // for
        for ( auto const socket_id : socket_ids ) {
            EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );
            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );
            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x13 } ) );
        } // for

        EXPECT_FALSE( acceptor.bind( { address, port } ).is_error() );

        EXPECT_EQ( acceptor.state(), State::BOUND );

        EXPECT_CALL( network_stack, deallocate_socket( A<Socket_ID>() ) ).Times( AnyNumber() );
    }
}

/**
 * \brief Execute the picolibrary::WIZnet::W5500::IP::TCP::Acceptor unit tests.
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

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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client unit test program.
 */

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/fixed_size_array.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/ip/tcp.h"
#include "picolibrary/testing/unit/ipv4.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/wiznet/w5500.h"
#include "picolibrary/testing/unit/wiznet/w5500/ip/network_stack.h"
#include "picolibrary/void.h"
#include "picolibrary/wiznet/w5500/ip/tcp.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Fixed_Size_Array;
using ::picolibrary::Generic_Error;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::IP::TCP::Endpoint;
using ::picolibrary::IP::TCP::Port;
using ::picolibrary::IPv4::Address;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::picolibrary::Testing::Unit::WIZnet::W5500::Mock_Driver;
using ::picolibrary::Testing::Unit::WIZnet::W5500::IP::Mock_Network_Stack;
using ::picolibrary::WIZnet::W5500::No_Delayed_ACK;
using ::picolibrary::WIZnet::W5500::Socket_ID;
using ::picolibrary::WIZnet::W5500::IP::TCP::Client;
using ::testing::_;
using ::testing::AllOf;
using ::testing::AnyNumber;
using ::testing::Ge;
using ::testing::InSequence;
using ::testing::Le;
using ::testing::Return;

using State = Client<Mock_Driver, Mock_Network_Stack>::State;

auto random_unique_addresses()
{
    auto const a = random<Address::Unsigned_Integer>();
    auto const b = random<Address::Unsigned_Integer>();

    return std::pair<Address, Address>{ a, b != a ? b : b ^ random<Address::Unsigned_Integer>( 1 ) };
}

template<typename T, std::size_t N>
auto random_fixed_size_array()
{
    Fixed_Size_Array<T, N> array;

    std::generate( array.begin(), array.end(), []() { return random<T>(); } );

    return array;
}

} // namespace

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::Client( Driver &,
 *        picolibrary::WIZnet::W5500::Socket_ID, Network_Stack & ) works properly.
 */
TEST( constructor, worksProperly )
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

        auto const client = Client{ driver, test_case.socket_id, network_stack };

        EXPECT_EQ( client.state(), State::INITIALIZED );
        EXPECT_EQ( client.socket_id(), test_case.socket_id );
        EXPECT_EQ( client.socket_interrupt_mask(), test_case.socket_interrupt_mask );
        EXPECT_FALSE( client.is_transmitting() );

        EXPECT_CALL( network_stack, deallocate_socket( test_case.socket_id ) );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::enable_interrupts() properly
 *        handles an SN_IMR register read error.
 */
TEST( enableInterrupts, snimrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_imr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.enable_interrupts( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::enable_interrupts() properly
 *        handles an SN_IMR register write error.
 */
TEST( enableInterrupts, snimrWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_imr( _ ) ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_sn_imr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.enable_interrupts( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::enable_interrupts() works
 *        properly.
 */
TEST( enableInterrupts, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_id = random<Socket_ID>();

    auto client = Client{ driver, socket_id, network_stack };

    auto const sn_imr = random<std::uint8_t>();
    auto const mask   = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_imr( socket_id ) ).WillOnce( Return( sn_imr ) );
    EXPECT_CALL( driver, write_sn_imr( socket_id, sn_imr | mask ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( client.enable_interrupts( mask ).is_error() );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::disable_interrupts(
 *        std::uint8_t ) properly handles an SN_IMR register read error.
 */
TEST( disableInterrupts, snimrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_imr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.disable_interrupts( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::disable_interrupts(
 *        std::uint8_t ) properly handles an SN_IMR register write error.
 */
TEST( disableInterrupts, snimrWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_imr( _ ) ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_sn_imr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.disable_interrupts( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::disable_interrupts(
 *        std::uint8_t ) works properly.
 */
TEST( disableInterrupts, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_id = random<Socket_ID>();

    auto client = Client{ driver, socket_id, network_stack };

    auto const sn_imr = random<std::uint8_t>();
    auto const mask   = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_imr( socket_id ) ).WillOnce( Return( sn_imr ) );
    EXPECT_CALL( driver, write_sn_imr( socket_id, sn_imr & ~mask ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( client.disable_interrupts( mask ).is_error() );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::disable_interrupts()
 *        properly handles an SN_IMR register write error.
 */
TEST( disableAllInterrupts, snimrWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_imr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.disable_interrupts();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::disable_interrupts() works
 *        properly.
 */
TEST( disableAllInterrupts, worksProperly )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_id = random<Socket_ID>();

    auto client = Client{ driver, socket_id, network_stack };

    EXPECT_CALL( driver, write_sn_imr( socket_id, 0x00 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( client.disable_interrupts().is_error() );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::enabled_interrupts()
 *        properly handles an SN_IMR read error.
 */
TEST( enabledInterrupts, snimrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_imr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.enabled_interrupts();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::enabled_interrupts() works
 *        properly.
 */
TEST( enabledInterrupts, worksProperly )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_id = random<Socket_ID>();

    auto const client = Client{ driver, socket_id, network_stack };

    auto const sn_imr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_imr( socket_id ) ).WillOnce( Return( sn_imr ) );

    auto const result = client.enabled_interrupts();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_imr );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::interrupt_context() properly
 *        handles an SN_IR read error.
 */
TEST( interruptContext, snirReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_ir( _ ) ).WillOnce( Return( error ) );

    auto const result = client.interrupt_context();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::interrupt_context() works
 *        properly.
 */
TEST( interruptContext, worksProperly )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_id = random<Socket_ID>();

    auto const client = Client{ driver, socket_id, network_stack };

    auto const sn_ir = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( sn_ir ) );

    auto const result = client.interrupt_context();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_ir );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::clear_interrupts() properly
 *        handles an SN_IR register write error.
 */
TEST( clearInterrupts, snirWriteError )
{
    {
        struct {
            bool is_transmitting;
        } const test_cases[]{
            { false },
            { true },
        };

        for ( auto const test_case : test_cases ) {
            auto driver        = Mock_Driver{};
            auto network_stack = Mock_Network_Stack{};

            auto client = Client{
                State::INITIALIZED, driver, random<Socket_ID>(), network_stack, test_case.is_transmitting
            };

            auto const error = random<Mock_Error>();

            EXPECT_CALL( driver, write_sn_ir( _, _ ) ).WillOnce( Return( error ) );

            auto const result = client.clear_interrupts();

            EXPECT_TRUE( result.is_error() );
            EXPECT_EQ( result.error(), error );

            EXPECT_EQ( client.is_transmitting(), test_case.is_transmitting );

            EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
        } // for
    }

    {
        struct {
            bool is_transmitting;
        } const test_cases[]{
            { false },
            { true },
        };

        for ( auto const test_case : test_cases ) {
            auto driver        = Mock_Driver{};
            auto network_stack = Mock_Network_Stack{};

            auto client = Client{
                State::INITIALIZED, driver, random<Socket_ID>(), network_stack, test_case.is_transmitting
            };

            auto const error = random<Mock_Error>();

            EXPECT_CALL( driver, write_sn_ir( _, _ ) ).WillOnce( Return( error ) );

            auto const result = client.clear_interrupts( random<std::uint8_t>() );

            EXPECT_TRUE( result.is_error() );
            EXPECT_EQ( result.error(), error );

            EXPECT_EQ( client.is_transmitting(), test_case.is_transmitting );

            EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
        } // for
    }
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::clear_interrupts() works
 *        properly.
 */
TEST( clearInterrupts, worksProperly )
{
    {
        struct {
            bool is_transmitting;
        } const test_cases[]{
            { false },
            { true },
        };

        for ( auto const test_case : test_cases ) {
            auto driver        = Mock_Driver{};
            auto network_stack = Mock_Network_Stack{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{
                State::INITIALIZED, driver, socket_id, network_stack, test_case.is_transmitting
            };

            EXPECT_CALL( driver, write_sn_ir( socket_id, 0b000'1'1'1'1'1 ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );

            EXPECT_FALSE( client.clear_interrupts().is_error() );

            EXPECT_FALSE( client.is_transmitting() );

            EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
        } // for
    }

    {
        struct {
            bool is_transmitting;
        } const test_cases[]{
            { false },
            { true },
        };

        for ( auto const test_case : test_cases ) {
            auto driver        = Mock_Driver{};
            auto network_stack = Mock_Network_Stack{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{
                State::INITIALIZED, driver, socket_id, network_stack, test_case.is_transmitting
            };

            auto const mask = random<std::uint8_t>();

            EXPECT_CALL( driver, write_sn_ir( socket_id, mask ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

            EXPECT_FALSE( client.clear_interrupts( mask ).is_error() );

            EXPECT_EQ( client.is_transmitting(), test_case.is_transmitting and not( mask & 0b000'1'0'0'0'0 ) );

            EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
        } // for
    }
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::configure_no_delayed_ack()
 *        properly handles an SN_MR register read error.
 */
TEST( configureNoDelayedAck, snmrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.configure_no_delayed_ack( random<No_Delayed_ACK>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::configure_no_delayed_ack()
 *        properly handles an SN_MR register write error.
 */
TEST( configureNoDelayedAck, snmrWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_sn_mr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.configure_no_delayed_ack( random<No_Delayed_ACK>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::configure_no_delayed_ack()
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

        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ driver, socket_id, network_stack };

        auto const sn_mr = random<std::uint8_t>();

        EXPECT_CALL( driver, read_sn_mr( socket_id ) ).WillOnce( Return( sn_mr ) );
        EXPECT_CALL(
            driver, write_sn_mr( socket_id, ( sn_mr & 0b1'1'0'1'1111 ) | test_case.sn_mr_nd ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );

        EXPECT_FALSE(
            client.configure_no_delayed_ack( test_case.no_delayed_ack_configuration ).is_error() );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Client::no_delayed_ack_configuration()
 *        properly handles an SN_MR register read error.
 */
TEST( noDelayedAckConfiguration, snmrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.no_delayed_ack_configuration();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Client::no_delayed_ack_configuration()
 *        works properly.
 */
TEST( noDelayedAckConfiguration, worksProperly )
{
    struct {
        std::uint8_t   sn_mr_nd;
        No_Delayed_ACK no_delayed_ack_configuration;
    } const test_cases[]{
        { 0b0'0'0'0'0000, No_Delayed_ACK::DISABLED },
        { 0b0'0'1'0'0000, No_Delayed_ACK::ENABLED },
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ driver, socket_id, network_stack };

        EXPECT_CALL( driver, read_sn_mr( socket_id ) )
            .WillOnce( Return( static_cast<std::uint8_t>(
                ( random<std::uint8_t>() & 0b1'1'0'1'1111 ) | test_case.sn_mr_nd ) ) );

        auto const result = client.no_delayed_ack_configuration();

        EXPECT_TRUE( result.is_value() );
        EXPECT_EQ( result.value(), test_case.no_delayed_ack_configuration );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Client::configure_maximum_segment_size()
 *        properly handles an SN_MSSR register write error.
 */
TEST( configureMaximumSegmentSize, mssrWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_mssr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.configure_maximum_segment_size( random<std::uint16_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Client::configure_maximum_segment_size()
 *        works properly.
 */
TEST( configureMaximumSegmentSize, worksProperly )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_id = random<Socket_ID>();

    auto client = Client{ driver, socket_id, network_stack };

    auto const maximum_segment_size = random<std::uint16_t>();

    EXPECT_CALL( driver, write_sn_mssr( socket_id, maximum_segment_size ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( client.configure_maximum_segment_size( maximum_segment_size ).is_error() );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::maximum_segment_size()
 *        properly handles an SN_MSSR register read error.
 */
TEST( maximumSegmentSize, mssrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_mssr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.maximum_segment_size();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::maximum_segment_size() works
 *        properly.
 */
TEST( maximumSegmentSize, worksProperly )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_id = random<Socket_ID>();

    auto const client = Client{ driver, socket_id, network_stack };

    auto const sn_mssr = random<std::uint16_t>();

    EXPECT_CALL( driver, read_sn_mssr( socket_id ) ).WillOnce( Return( sn_mssr ) );

    auto const result = client.maximum_segment_size();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_mssr );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::configure_time_to_live()
 *        properly handles an SN_TTL register write error.
 */
TEST( configureTimeToLive, ttlWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_ttl( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.configure_time_to_live( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::configure_time_to_live()
 *        works properly.
 */
TEST( configureTimeToLive, worksProperly )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_id = random<Socket_ID>();

    auto client = Client{ driver, socket_id, network_stack };

    auto const time_to_live = random<std::uint8_t>();

    EXPECT_CALL( driver, write_sn_ttl( socket_id, time_to_live ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( client.configure_time_to_live( time_to_live ).is_error() );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::time_to_live() properly
 *        handles an SN_TTL register read error.
 */
TEST( timeToLive, ttlReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_ttl( _ ) ).WillOnce( Return( error ) );

    auto const result = client.time_to_live();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::time_to_live() works
 *        properly.
 */
TEST( timeToLive, worksProperly )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_id = random<Socket_ID>();

    auto const client = Client{ driver, socket_id, network_stack };

    auto const sn_ttl = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_ttl( socket_id ) ).WillOnce( Return( sn_ttl ) );

    auto const result = client.time_to_live();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_ttl );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::configure_keepalive_period()
 *        properly handles an SN_KPALVTR register write error.
 */
TEST( configureKeepalivePeriod, kpalvtrWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.configure_keepalive_period( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::configure_keepalive_period()
 *        works properly.
 */
TEST( configureKeepalivePeriod, worksProperly )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_id = random<Socket_ID>();

    auto client = Client{ driver, socket_id, network_stack };

    auto const keepalive_period = random<std::uint8_t>();

    EXPECT_CALL( driver, write_sn_kpalvtr( socket_id, keepalive_period ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( client.configure_keepalive_period( keepalive_period ).is_error() );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::keepalive_period() properly
 *        handles an SN_KPALVTR register read error.
 */
TEST( keepalivePeriod, kpalvtrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_kpalvtr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.keepalive_period();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::keepalive_period() works
 *        properly.
 */
TEST( keepalivePeriod, worksProperly )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_id = random<Socket_ID>();

    auto const client = Client{ driver, socket_id, network_stack };

    auto const sn_kpalvtr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_kpalvtr( socket_id ) ).WillOnce( Return( sn_kpalvtr ) );

    auto const result = client.keepalive_period();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_kpalvtr );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::bind() properly handles an
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
        { State::CONNECTING    },
        { State::CONNECTED     },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto client = Client{ test_case.state, driver, random<Socket_ID>(), network_stack };

        auto const result = client.bind();

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::LOGIC_ERROR );

        EXPECT_EQ( client.state(), test_case.state );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::bind() properly handles a
 *        SIPR register read error.
 */
TEST( bind, siprReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( error ) );

    auto const result = client.bind( { random<Address>( 1 ), random<Port>() } );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::bind() properly handles an
 *        attempt to bind the socket to an invalid endpoint.
 */
TEST( bind, invalidEndpoint )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const [ address, endpoint_address ] = random_unique_addresses();

    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( address.as_byte_array() ) );

    auto const result = client.bind( { endpoint_address, random<Port>() } );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::INVALID_ARGUMENT );

    EXPECT_EQ( client.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::bind() properly handles
 *        ephemeral port allocation not being enabled.
 */
TEST( bind, ephemeralPortAllocationNotEnabled )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    EXPECT_CALL( network_stack, tcp_ephemeral_port_allocation_enabled() ).WillOnce( Return( false ) );

    auto const result = client.bind();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::EPHEMERAL_PORTS_EXHAUSTED );

    EXPECT_EQ( client.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::bind() properly handles an
 *        SN_MR register read error.
 */
TEST( bind, snmrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( network_stack, available_sockets() ).WillOnce( Return( random<std::uint_fast8_t>( 1, 8 ) ) );
    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.bind( random<Port>( 1 ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::bind() properly handles an
 *        SN_PORT register read error.
 */
TEST( bind, snportReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( network_stack, available_sockets() ).WillOnce( Return( random<std::uint_fast8_t>( 1, 8 ) ) );
    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillRepeatedly( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( error ) );

    auto const result = client.bind( random<Port>( 1 ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::bind() properly handles
 *        ephemeral port exhaustion.
 */
TEST( bind, ephemeralPortsExhausted )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const ephemeral_port = random<Port>( 1 );

    EXPECT_CALL( network_stack, tcp_ephemeral_port_allocation_enabled() ).WillOnce( Return( true ) );
    EXPECT_CALL( network_stack, available_sockets() ).WillOnce( Return( random<std::uint_fast8_t>( 1, 8 ) ) );
    EXPECT_CALL( driver, read_sn_mr( _ ) )
        .WillRepeatedly( Return( static_cast<std::uint8_t>(
            ( random<std::uint8_t>() & 0b1'1'1'1'0000 ) | 0b0001 ) ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillRepeatedly( Return( ephemeral_port.as_unsigned_integer() ) );
    EXPECT_CALL( network_stack, tcp_ephemeral_port_min() ).WillOnce( Return( ephemeral_port ) );
    EXPECT_CALL( network_stack, tcp_ephemeral_port_max() ).WillOnce( Return( ephemeral_port ) );

    auto const result = client.bind();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::EPHEMERAL_PORTS_EXHAUSTED );

    EXPECT_EQ( client.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::bind() properly handles the
 *        requested endpoint being in use.
 */
TEST( bind, endpointInUse )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const port = random<Port>( 1 );

    EXPECT_CALL( network_stack, available_sockets() ).WillOnce( Return( random<std::uint_fast8_t>( 1, 8 ) ) );
    EXPECT_CALL( driver, read_sn_mr( _ ) )
        .WillRepeatedly( Return( static_cast<std::uint8_t>(
            ( random<std::uint8_t>() & 0b1'1'1'1'0000 ) | 0b0001 ) ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillRepeatedly( Return( port.as_unsigned_integer() ) );

    auto const result = client.bind( port );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::ENDPOINT_IN_USE );

    EXPECT_EQ( client.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::bind() properly handles an
 *        SN_PORT register write error.
 */
TEST( bind, snportWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( network_stack, available_sockets() ).WillOnce( Return( random<std::uint_fast8_t>( 1, 8 ) ) );
    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillRepeatedly( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillRepeatedly( Return( std::uint16_t{ 0 } ) );
    EXPECT_CALL( driver, write_sn_port( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.bind( random<Port>( 1 ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::bind() properly handles an
 *        SN_CR register write error.
 */
TEST( bind, sncrWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( network_stack, available_sockets() ).WillOnce( Return( random<std::uint_fast8_t>( 1, 8 ) ) );
    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillRepeatedly( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillRepeatedly( Return( std::uint16_t{ 0 } ) );
    EXPECT_CALL( driver, write_sn_port( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.bind( random<Port>( 1 ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::bind() properly handles an
 *        SN_CR register read error.
 */
TEST( bind, sncrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( network_stack, available_sockets() ).WillOnce( Return( random<std::uint_fast8_t>( 1, 8 ) ) );
    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillRepeatedly( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillRepeatedly( Return( std::uint16_t{ 0 } ) );
    EXPECT_CALL( driver, write_sn_port( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.bind( random<Port>( 1 ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::bind() properly handles an
 *        SN_SR register read error.
 */
TEST( bind, snsrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( network_stack, available_sockets() ).WillOnce( Return( random<std::uint_fast8_t>( 1, 8 ) ) );
    EXPECT_CALL( driver, read_sn_mr( _ ) ).WillRepeatedly( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillRepeatedly( Return( std::uint16_t{ 0 } ) );
    EXPECT_CALL( driver, write_sn_port( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );
    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.bind( random<Port>( 1 ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::INITIALIZED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::bind() properly handles an
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

        auto client = Client{ driver, random<Socket_ID>(), network_stack };

        auto const error = random<Mock_Error>();

        EXPECT_CALL( network_stack, available_sockets() ).WillOnce( Return( random<std::uint_fast8_t>( 1, 8 ) ) );
        EXPECT_CALL( driver, read_sn_mr( _ ) ).WillRepeatedly( Return( random<std::uint8_t>() ) );
        EXPECT_CALL( driver, read_sn_port( _ ) ).WillRepeatedly( Return( uint16_t{ 0 } ) );
        EXPECT_CALL( driver, write_sn_port( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );
        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );
        EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( error ) );

        auto const result = client.bind( random<Port>( 1 ) );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );

        EXPECT_EQ( client.state(), State::INITIALIZED );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::bind() works properly.
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

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ driver, socket_id, network_stack };

        auto const available_sockets  = random<std::uint_fast8_t>( 1, 8 );
        auto const ephemeral_port_min = random<Port>( 1 );
        auto const ephemeral_port_max = random<Port>( ephemeral_port_min );
        auto const reserved_ephemeral_port = random<Port>( ephemeral_port_min, ephemeral_port_max );

        EXPECT_CALL( network_stack, tcp_ephemeral_port_allocation_enabled() ).WillOnce( Return( true ) );
        EXPECT_CALL( network_stack, available_sockets() ).WillOnce( Return( available_sockets ) );
        for ( auto socket = std::uint_fast8_t{}; socket < available_sockets; ++socket ) {
            auto const sn_mr   = random<std::uint8_t>();
            auto const sn_port = generate_sn_port( sn_mr, reserved_ephemeral_port );

            EXPECT_CALL( driver, read_sn_mr( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_mr ) );
            EXPECT_CALL( driver, read_sn_port( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_port ) );
        } // for
        EXPECT_CALL( network_stack, tcp_ephemeral_port_min() ).WillOnce( Return( ephemeral_port_min ) );
        EXPECT_CALL( network_stack, tcp_ephemeral_port_max() ).WillOnce( Return( ephemeral_port_max ) );
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

        EXPECT_FALSE( client.bind().is_error() );

        EXPECT_EQ( client.state(), State::BOUND );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ driver, socket_id, network_stack };

        auto const available_sockets  = random<std::uint_fast8_t>( 1, 8 );
        auto const ephemeral_port_min = random<Port>( 1 );
        auto const ephemeral_port_max = random<Port>( ephemeral_port_min );
        auto const reserved_ephemeral_port = random<Port>( ephemeral_port_min, ephemeral_port_max );

        EXPECT_CALL( network_stack, tcp_ephemeral_port_allocation_enabled() ).WillOnce( Return( true ) );
        EXPECT_CALL( network_stack, available_sockets() ).WillOnce( Return( available_sockets ) );
        for ( auto socket = std::uint_fast8_t{}; socket < available_sockets; ++socket ) {
            auto const sn_mr   = random<std::uint8_t>();
            auto const sn_port = generate_sn_port( sn_mr, reserved_ephemeral_port );

            EXPECT_CALL( driver, read_sn_mr( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_mr ) );
            EXPECT_CALL( driver, read_sn_port( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_port ) );
        } // for
        EXPECT_CALL( network_stack, tcp_ephemeral_port_min() ).WillOnce( Return( ephemeral_port_min ) );
        EXPECT_CALL( network_stack, tcp_ephemeral_port_max() ).WillOnce( Return( ephemeral_port_max ) );
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

        EXPECT_FALSE( client.bind( Port{} ).is_error() );

        EXPECT_EQ( client.state(), State::BOUND );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ driver, socket_id, network_stack };

        auto const port              = random<Port>( 1 );
        auto const available_sockets = random<std::uint_fast8_t>( 1, 8 );

        EXPECT_CALL( network_stack, available_sockets() ).WillOnce( Return( available_sockets ) );
        for ( auto socket = std::uint_fast8_t{}; socket < available_sockets; ++socket ) {
            auto const sn_mr   = random<std::uint8_t>();
            auto const sn_port = generate_sn_port( sn_mr, port );

            EXPECT_CALL( driver, read_sn_mr( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_mr ) );
            EXPECT_CALL( driver, read_sn_port( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_port ) );
        } // for
        EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x13 } ) );

        EXPECT_FALSE( client.bind( port ).is_error() );

        EXPECT_EQ( client.state(), State::BOUND );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ driver, socket_id, network_stack };

        auto const address            = random<Address>( 1 );
        auto const available_sockets  = random<std::uint_fast8_t>( 1, 8 );
        auto const ephemeral_port_min = random<Port>( 1 );
        auto const ephemeral_port_max = random<Port>( ephemeral_port_min );
        auto const reserved_ephemeral_port = random<Port>( ephemeral_port_min, ephemeral_port_max );

        EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( address.as_byte_array() ) );
        EXPECT_CALL( network_stack, tcp_ephemeral_port_allocation_enabled() ).WillOnce( Return( true ) );
        EXPECT_CALL( network_stack, available_sockets() ).WillOnce( Return( available_sockets ) );
        for ( auto socket = std::uint_fast8_t{}; socket < available_sockets; ++socket ) {
            auto const sn_mr   = random<std::uint8_t>();
            auto const sn_port = generate_sn_port( sn_mr, reserved_ephemeral_port );

            EXPECT_CALL( driver, read_sn_mr( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_mr ) );
            EXPECT_CALL( driver, read_sn_port( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_port ) );
        } // for
        EXPECT_CALL( network_stack, tcp_ephemeral_port_min() ).WillOnce( Return( ephemeral_port_min ) );
        EXPECT_CALL( network_stack, tcp_ephemeral_port_max() ).WillOnce( Return( ephemeral_port_max ) );
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

        EXPECT_FALSE( client.bind( { address, Port{} } ).is_error() );

        EXPECT_EQ( client.state(), State::BOUND );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ driver, socket_id, network_stack };

        auto const address           = random<Address>( 1 );
        auto const port              = random<Port>( 1 );
        auto const available_sockets = random<std::uint_fast8_t>( 1, 8 );

        EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( address.as_byte_array() ) );
        EXPECT_CALL( network_stack, available_sockets() ).WillOnce( Return( available_sockets ) );
        for ( auto socket = std::uint_fast8_t{}; socket < available_sockets; ++socket ) {
            auto const sn_mr   = random<std::uint8_t>();
            auto const sn_port = generate_sn_port( sn_mr, port );

            EXPECT_CALL( driver, read_sn_mr( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_mr ) );
            EXPECT_CALL( driver, read_sn_port( static_cast<Socket_ID>( socket << 5 ) ) ).WillOnce( Return( sn_port ) );
        } // for
        EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x13 } ) );

        EXPECT_FALSE( client.bind( { address, port } ).is_error() );

        EXPECT_EQ( client.state(), State::BOUND );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    }
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::connect() properly handles
 *        an attempt to connect to a remote endpoint when the socket is not in a state
 *        that allows it to connect to a remote endpoint.
 */
TEST( connect, invalidState )
{
    struct {
        State state;
    } const test_cases[]{
        // clang-format off

        { State::UNINITIALIZED },
        { State::INITIALIZED   },
        { State::CONNECTED     },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto client = Client{ test_case.state, driver, random<Socket_ID>(), network_stack };

        auto const result = client.connect( { random<Address>( 1 ), random<Port>( 1 ) } );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::LOGIC_ERROR );

        EXPECT_EQ( client.state(), test_case.state );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::connect() properly handles
 *        an attempt to connect to an invalid endpoint
 */
TEST( connect, invalidEndpoint )
{
    struct {
        Endpoint endpoint;
    } const test_cases[]{
        // clang-format off

        { {                                         } },
        { { Address{},            random<Port>( 1 ) } },
        { { random<Address>( 1 ), Port{}            } },
        { { Address{},            Port{}            } },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto client = Client{ State::BOUND, driver, random<Socket_ID>(), network_stack };

        auto const result = client.connect( test_case.endpoint );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::INVALID_ARGUMENT );

        EXPECT_EQ( client.state(), State::BOUND );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::connect() properly handles
 *        an SN_DIPR register write error.
 */
TEST( connect, sndiprWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::BOUND, driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_dipr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.connect( { random<Address>( 1 ), random<Port>( 1 ) } );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::BOUND );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::connect() properly handles
 *        an SN_DPORT register write error.
 */
TEST( connect, sndportWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::BOUND, driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_dipr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_dport( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.connect( { random<Address>( 1 ), random<Port>( 1 ) } );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::BOUND );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::connect() properly handles
 *        an SN_CR register write error.
 */
TEST( connect, sncrWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::BOUND, driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_dipr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_dport( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.connect( { random<Address>( 1 ), random<Port>( 1 ) } );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::BOUND );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::connect() properly handles
 *        an SN_CR register read error.
 */
TEST( connect, sncrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::BOUND, driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_sn_dipr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_dport( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.connect( { random<Address>( 1 ), random<Port>( 1 ) } );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::BOUND );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::connect() properly handles
 *        an SN_SR register read error.
 */
TEST( connect, snsrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTING, driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.connect( { random<Address>( 1 ), random<Port>( 1 ) } );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTING );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::connect() properly handles
 *        a connection timeout.
 */
TEST( connect, connectionTimeout )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTING, driver, random<Socket_ID>(), network_stack };

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );

    auto const result = client.connect( { random<Address>( 1 ), random<Port>( 1 ) } );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::OPERATION_TIMEOUT );

    EXPECT_EQ( client.state(), State::CONNECTING );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::connect() properly handles
 *        an invalid SN_SR register value.
 */
TEST( connect, invalidSNSRValue )
{
    struct {
        std::uint8_t sn_sr;
    } const test_cases[]{
        // clang-format off

        { random<std::uint8_t>( 0x01, 0x12 ) },
        {                       0x14         },
        {                       0x16         },
        { random<std::uint8_t>( 0x18, 0x1B ) },
        { random<std::uint8_t>( 0x1D       ) },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto client = Client{ State::CONNECTING, driver, random<Socket_ID>(), network_stack };

        auto const error = random<Mock_Error>();

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );
        EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( error ) );

        auto const result = client.connect( { random<Address>( 1 ), random<Port>( 1 ) } );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );

        EXPECT_EQ( client.state(), State::CONNECTING );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::connect() works properly.
 */
TEST( connect, worksProperly )
{
    {
        auto const in_sequence = InSequence{};

        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ State::BOUND, driver, socket_id, network_stack };

        auto const endpoint = Endpoint{ random<Address>( 1 ), random<Port>( 1 ) };

        EXPECT_CALL( driver, write_sn_dipr( socket_id, endpoint.address().ipv4().as_byte_array() ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, write_sn_dport( socket_id, endpoint.port().as_unsigned_integer() ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, write_sn_cr( socket_id, 0x04 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );

        auto const result = client.connect( endpoint );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

        EXPECT_EQ( client.state(), State::CONNECTING );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    }

    {
        struct {
            std::uint8_t sn_sr;
        } const test_cases[]{
            { 0x13 },
            { 0x15 },
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto driver        = Mock_Driver{};
            auto network_stack = Mock_Network_Stack{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{ State::CONNECTING, driver, socket_id, network_stack };

            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );

            auto const result = client.connect( { random<Address>( 1 ), random<Port>( 1 ) } );

            EXPECT_TRUE( result.is_error() );
            EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

            EXPECT_EQ( client.state(), State::CONNECTING );

            EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
        } // for
    }

    {
        struct {
            std::uint8_t sn_sr;
        } const test_cases[]{
            { 0x17 },
            { 0x1C },
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto driver        = Mock_Driver{};
            auto network_stack = Mock_Network_Stack{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{ State::CONNECTING, driver, socket_id, network_stack };

            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );

            EXPECT_FALSE( client.connect( { random<Address>( 1 ), random<Port>( 1 ) } ).is_error() );

            EXPECT_EQ( client.state(), State::CONNECTED );

            EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
        } // for
    }
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::is_connected() properly
 *        handles an SN_SR register read error.
 */
TEST( isConnected, snsrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.is_connected();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::is_connected() properly
 *        handles an invalid SN_SR register value.
 */
TEST( isConnected, invalidSNSRValue )
{
    struct {
        std::uint8_t sn_sr;
    } const test_cases[]{
        // clang-format off

        { random<std::uint8_t>( 0x01, 0x12 ) },
        {                       0x19         },
        { random<std::uint8_t>( 0x1E       ) },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const client = Client{ driver, random<Socket_ID>(), network_stack };

        auto const error = random<Mock_Error>();

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );
        EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( error ) );

        auto const result = client.is_connected();

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::is_connected() works
 *        properly.
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
        { 0x1C, false },
        { 0x1D, false },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_id = random<Socket_ID>();

        auto const client = Client{ driver, socket_id, network_stack };

        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );

        auto const result = client.is_connected();

        EXPECT_TRUE( result.is_value() );
        EXPECT_EQ( result.value(), test_case.is_connected );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::remote_endpoint() properly
 *        handles an SN_DIPR register read error.
 */
TEST( remoteEndpoint, sndiprReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_dipr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.remote_endpoint();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::remote_endpoint() properly
 *        handles an SN_DPORT register read error.
 */
TEST( remoteEndpoint, sndportReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_dipr( _ ) ).WillOnce( Return( random_fixed_size_array<std::uint8_t, 4>() ) );
    EXPECT_CALL( driver, read_sn_dport( _ ) ).WillOnce( Return( error ) );

    auto const result = client.remote_endpoint();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::remote_endpoint() works
 *        properly.
 */
TEST( remoteEndpoint, worksProperly )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_id = random<Socket_ID>();

    auto const client = Client{ driver, socket_id, network_stack };

    auto const sn_dipr  = random_fixed_size_array<std::uint8_t, 4>();
    auto const sn_dport = random<std::uint16_t>();

    EXPECT_CALL( driver, read_sn_dipr( socket_id ) ).WillOnce( Return( sn_dipr ) );
    EXPECT_CALL( driver, read_sn_dport( socket_id ) ).WillOnce( Return( sn_dport ) );

    auto const result = client.remote_endpoint();

    EXPECT_TRUE( result.is_value() );
    EXPECT_TRUE( result.value().address().is_ipv4() );
    EXPECT_EQ( result.value().address().ipv4().as_byte_array(), sn_dipr );
    EXPECT_EQ( result.value().port().as_unsigned_integer(), sn_dport );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::local_endpoint() properly
 *        handles a SIPR register read error.
 */
TEST( localEndpoint, siprReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( error ) );

    auto const result = client.local_endpoint();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::local_endpoint() properly
 *        handles an SN_PORT register read error.
 */
TEST( localEndpoint, snportReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( random_fixed_size_array<std::uint8_t, 4>() ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( error ) );

    auto const result = client.local_endpoint();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::local_endpoint() works
 *        properly.
 */
TEST( localEndpoint, worksProperly )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_id = random<Socket_ID>();

    auto const client = Client{ driver, socket_id, network_stack };

    auto const sipr    = random_fixed_size_array<std::uint8_t, 4>();
    auto const sn_port = random<std::uint16_t>();

    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( sipr ) );
    EXPECT_CALL( driver, read_sn_port( socket_id ) ).WillOnce( Return( sn_port ) );

    auto const result = client.local_endpoint();

    EXPECT_TRUE( result.is_value() );
    EXPECT_TRUE( result.value().address().is_ipv4() );
    EXPECT_EQ( result.value().address().ipv4().as_byte_array(), sipr );
    EXPECT_EQ( result.value().port().as_unsigned_integer(), sn_port );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::outstanding() properly
 *        handles an SN_TXBUF_SIZE register read error.
 */
TEST( outstanding, sntxbufsizeReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_txbuf_size( _ ) ).WillOnce( Return( error ) );

    auto const result = client.outstanding();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::outstanding() properly
 *        handles an invalid SN_TXBUF_SIZE register value.
 */
TEST( outstanding, invalidSNTXBUFSIZEValue )
{
    struct {
        std::uint8_t sn_txbuf_size;
    } const test_cases[]{
        // clang-format off

        {                        0       },
        {                        3       },
        { random<std::uint8_t>(  5,  7 ) },
        { random<std::uint8_t>(  9, 15 ) },
        { random<std::uint8_t>( 17     ) },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const client = Client{ driver, random<Socket_ID>(), network_stack };

        auto const error = random<Mock_Error>();

        EXPECT_CALL( driver, read_sn_txbuf_size( _ ) ).WillOnce( Return( test_case.sn_txbuf_size ) );
        EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( error ) );

        auto const result = client.outstanding();

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::outstanding() properly
 *        handles an SN_TX_FSR register read error.
 */
TEST( outstanding, sntxfsrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_txbuf_size( _ ) )
        .WillOnce( Return( static_cast<std::uint8_t>( 1 << random<std::uint_fast8_t>( 0, 4 ) ) ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.outstanding();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::outstanding() properly
 *        handles an invalid SN_TX_FSR register value.
 */
TEST( outstanding, invalidSNTXFSRValue )
{
    struct {
        std::uint8_t sn_txbuf_size;
    } const test_cases[]{
        // clang-format off

        {  1 },
        {  2 },
        {  4 },
        {  8 },
        { 16 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const client = Client{ driver, random<Socket_ID>(), network_stack };

        auto const error = random<Mock_Error>();

        EXPECT_CALL( driver, read_sn_txbuf_size( _ ) ).WillOnce( Return( test_case.sn_txbuf_size ) );
        EXPECT_CALL( driver, read_sn_tx_fsr( _ ) )
            .WillOnce( Return( random<std::uint16_t>( ( test_case.sn_txbuf_size * 1024 ) + 1 ) ) );
        EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( error ) );

        auto const result = client.outstanding();

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::outstanding() works
 *        properly.
 */
TEST( outstanding, worksProperly )
{
    struct {
        std::uint8_t sn_txbuf_size;
    } const test_cases[]{
        // clang-format off

        {  1 },
        {  2 },
        {  4 },
        {  8 },
        { 16 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_id = random<Socket_ID>();

        auto const client = Client{ driver, socket_id, network_stack };

        auto const sn_tx_fsr = random<std::uint16_t>( 0, test_case.sn_txbuf_size * 1024 );

        EXPECT_CALL( driver, read_sn_txbuf_size( socket_id ) ).WillOnce( Return( test_case.sn_txbuf_size ) );
        EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( sn_tx_fsr ) );

        auto const result = client.outstanding();

        EXPECT_TRUE( result.is_value() );
        EXPECT_EQ( result.value(), ( test_case.sn_txbuf_size * 1024 ) - sn_tx_fsr );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() properly handles
 *        an attempt to transmit data when the socket is not in a state that allows it to
 *        transmit data.
 */
TEST( transmit, invalidState )
{
    struct {
        State state;
    } const test_cases[]{
        // clang-format off

        { State::UNINITIALIZED },
        { State::INITIALIZED   },
        { State::BOUND         },
        { State::CONNECTING,   },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const is_transmitting = random<bool>();

        auto client = Client{ test_case.state, driver, random<Socket_ID>(), network_stack, is_transmitting };

        auto const data   = random_container<std::vector<std::uint8_t>>();
        auto const result = client.transmit( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::NOT_CONNECTED );

        EXPECT_EQ( client.state(), test_case.state );
        EXPECT_EQ( client.is_transmitting(), is_transmitting );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() properly handles
 *        an SN_SR register read error.
 */
TEST( transmit, snsrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const is_transmitting = random<bool>();

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack, is_transmitting };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( error ) );

    auto const data   = random_container<std::vector<std::uint8_t>>();
    auto const result = client.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );
    EXPECT_EQ( client.is_transmitting(), is_transmitting );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() properly handles
 *        an invalid SN_SR register value.
 */
TEST( transmit, invalidSNSRValue )
{
    struct {
        std::uint8_t sn_sr;
    } const test_cases[]{
        // clang-format off

        { random<std::uint8_t>( 0x01, 0x16 ) },
        {                       0x19         },
        { random<std::uint8_t>( 0x1E       ) },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const is_transmitting = random<bool>();

        auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack, is_transmitting };

        auto const error = random<Mock_Error>();

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );
        EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( error ) );

        auto const data   = random_container<std::vector<std::uint8_t>>();
        auto const result = client.transmit( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );

        EXPECT_EQ( client.state(), State::CONNECTED );
        EXPECT_EQ( client.is_transmitting(), is_transmitting );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() properly handles
 *        connection loss.
 */
TEST( transmit, connectionLost )
{
    struct {
        std::uint8_t sn_sr;
    } const test_cases[]{
        // clang-format off

        { 0x00 },
        { 0x18 },
        { 0x1A },
        { 0x1B },
        { 0x1C },
        { 0x1D },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const is_transmitting = random<bool>();

        auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack, is_transmitting };

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );

        auto const data   = random_container<std::vector<std::uint8_t>>();
        auto const result = client.transmit( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::NOT_CONNECTED );

        EXPECT_EQ( client.state(), State::CONNECTED );
        EXPECT_EQ( client.is_transmitting(), is_transmitting );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() properly handles
 *        an SN_IR register read error.
 */
TEST( transmit, snirReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack, true };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
    EXPECT_CALL( driver, read_sn_ir( _ ) ).WillOnce( Return( error ) );

    auto const data   = random_container<std::vector<std::uint8_t>>();
    auto const result = client.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );
    EXPECT_TRUE( client.is_transmitting() );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() properly handles
 *        an in-progress transmission that is not complete.
 */
TEST( transmit, transmissionNotComplete )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack, true };

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
    EXPECT_CALL( driver, read_sn_ir( _ ) )
        .WillOnce( Return( static_cast<std::uint8_t>( random<std::uint8_t>() & 0b111'0'1'1'1'1 ) ) );

    auto const data   = random_container<std::vector<std::uint8_t>>();
    auto const result = client.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

    EXPECT_EQ( client.state(), State::CONNECTED );
    EXPECT_TRUE( client.is_transmitting() );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() properly handles
 *        an SN_IR register write error.
 */
TEST( transmit, snirWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack, true };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
    EXPECT_CALL( driver, read_sn_ir( _ ) )
        .WillOnce( Return( static_cast<std::uint8_t>( random<std::uint8_t>() | 0b000'1'0'0'0'0 ) ) );
    EXPECT_CALL( driver, write_sn_ir( _, _ ) ).WillOnce( Return( error ) );

    auto const data   = random_container<std::vector<std::uint8_t>>();
    auto const result = client.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );
    EXPECT_TRUE( client.is_transmitting() );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() properly handles
 *        an SN_TXBUF_SIZE register read error.
 */
TEST( transmit, sntxbufsizeReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack, false };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
    EXPECT_CALL( driver, read_sn_txbuf_size( _ ) ).WillOnce( Return( error ) );

    auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
    auto const result = client.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );
    EXPECT_FALSE( client.is_transmitting() );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() properly handles
 *        an invalid SN_TXBUF_SIZE register value.
 */
TEST( transmit, invalidSNTXBUFSIZEValue )
{
    struct {
        std::uint8_t sn_txbuf_size;
    } const test_cases[]{
        // clang-format off

        {                        0       },
        {                        3       },
        { random<std::uint8_t>(  5,  7 ) },
        { random<std::uint8_t>(  9, 15 ) },
        { random<std::uint8_t>( 17     ) },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack, false };

        auto const error = random<Mock_Error>();

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
        EXPECT_CALL( driver, read_sn_txbuf_size( _ ) ).WillOnce( Return( test_case.sn_txbuf_size ) );
        EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( error ) );

        auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
        auto const result = client.transmit( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );

        EXPECT_EQ( client.state(), State::CONNECTED );
        EXPECT_FALSE( client.is_transmitting() );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() properly handles
 *        an SN_TX_FSR register read error.
 */
TEST( transmit, sntxfsrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack, false };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
    EXPECT_CALL( driver, read_sn_txbuf_size( _ ) )
        .WillOnce( Return( static_cast<std::uint8_t>( 1 << random<std::uint_fast8_t>( 0, 4 ) ) ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( error ) );

    auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
    auto const result = client.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );
    EXPECT_FALSE( client.is_transmitting() );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() properly handles
 *        an invalid SN_TX_FSR register value.
 */
TEST( transmit, invalidSNTXFSRValue )
{
    struct {
        std::uint8_t sn_txbuf_size;
    } const test_cases[]{
        // clang-format off

        {  1 },
        {  2 },
        {  4 },
        {  8 },
        { 16 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack, false };

        auto const error = random<Mock_Error>();

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
        EXPECT_CALL( driver, read_sn_txbuf_size( _ ) ).WillOnce( Return( test_case.sn_txbuf_size ) );
        EXPECT_CALL( driver, read_sn_tx_fsr( _ ) )
            .WillOnce( Return( random<std::uint16_t>( ( test_case.sn_txbuf_size * 1024 ) + 1 ) ) );
        EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( error ) );

        auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
        auto const result = client.transmit( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );

        EXPECT_EQ( client.state(), State::CONNECTED );
        EXPECT_FALSE( client.is_transmitting() );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() properly handles
 *        a full transmit buffer.
 */
TEST( transmit, transmitBufferFull )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack, false };

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
    EXPECT_CALL( driver, read_sn_txbuf_size( _ ) )
        .WillOnce( Return( static_cast<std::uint8_t>( 1 << random<std::uint_fast8_t>( 0, 4 ) ) ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( uint16_t{ 0 } ) );

    auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
    auto const result = client.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

    EXPECT_EQ( client.state(), State::CONNECTED );
    EXPECT_FALSE( client.is_transmitting() );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() properly handles
 *        an SN_TX_WR register read error.
 */
TEST( transmit, sntxwrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack, false };

    auto const sn_txbuf_size = static_cast<std::uint8_t>( 1 << random<std::uint_fast8_t>( 0, 4 ) );
    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
    EXPECT_CALL( driver, read_sn_txbuf_size( _ ) ).WillOnce( Return( sn_txbuf_size ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( random<std::uint16_t>( 1, sn_txbuf_size * 1024 ) ) );
    EXPECT_CALL( driver, read_sn_tx_wr( _ ) ).WillOnce( Return( error ) );

    auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
    auto const result = client.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );
    EXPECT_FALSE( client.is_transmitting() );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() properly handles
 *        a transmit buffer write error.
 */
TEST( transmit, transmitBufferWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack, false };

    auto const sn_txbuf_size = static_cast<std::uint8_t>( 1 << random<std::uint_fast8_t>( 0, 4 ) );
    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
    EXPECT_CALL( driver, read_sn_txbuf_size( _ ) ).WillOnce( Return( sn_txbuf_size ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( random<std::uint16_t>( 1, sn_txbuf_size * 1024 ) ) );
    EXPECT_CALL( driver, read_sn_tx_wr( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
    EXPECT_CALL( driver, write( _, _, _ ) ).WillOnce( Return( error ) );

    auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
    auto const result = client.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );
    EXPECT_FALSE( client.is_transmitting() );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() properly handles
 *        an SN_RX_WR register write error.
 */
TEST( transmit, sntxwrWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack, false };

    auto const sn_txbuf_size = static_cast<std::uint8_t>( 1 << random<std::uint_fast8_t>( 0, 4 ) );
    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
    EXPECT_CALL( driver, read_sn_txbuf_size( _ ) ).WillOnce( Return( sn_txbuf_size ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( random<std::uint16_t>( 1, sn_txbuf_size * 1024 ) ) );
    EXPECT_CALL( driver, read_sn_tx_wr( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
    EXPECT_CALL( driver, write( _, _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_tx_wr( _, _ ) ).WillOnce( Return( error ) );

    auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
    auto const result = client.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );
    EXPECT_FALSE( client.is_transmitting() );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() properly handles
 *        an SN_CR register write error.
 */
TEST( transmit, sncrWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack, false };

    auto const sn_txbuf_size = static_cast<std::uint8_t>( 1 << random<std::uint_fast8_t>( 0, 4 ) );
    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
    EXPECT_CALL( driver, read_sn_txbuf_size( _ ) ).WillOnce( Return( sn_txbuf_size ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( random<std::uint16_t>( 1, sn_txbuf_size * 1024 ) ) );
    EXPECT_CALL( driver, read_sn_tx_wr( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
    EXPECT_CALL( driver, write( _, _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_tx_wr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( error ) );

    auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
    auto const result = client.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );
    EXPECT_FALSE( client.is_transmitting() );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() properly handles
 *        an SN_CR register read error.
 */
TEST( transmit, sncrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack, false };

    auto const sn_txbuf_size = static_cast<std::uint8_t>( 1 << random<std::uint_fast8_t>( 0, 4 ) );
    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
    EXPECT_CALL( driver, read_sn_txbuf_size( _ ) ).WillOnce( Return( sn_txbuf_size ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( random<std::uint16_t>( 1, sn_txbuf_size * 1024 ) ) );
    EXPECT_CALL( driver, read_sn_tx_wr( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
    EXPECT_CALL( driver, write( _, _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_tx_wr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( error ) );

    auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
    auto const result = client.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );
    EXPECT_FALSE( client.is_transmitting() );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() works properly.
 */
TEST( transmit, worksProperly )
{
    {
        auto const in_sequence = InSequence{};

        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ State::CONNECTED, driver, socket_id, network_stack, false };

        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );

        auto const data   = std::vector<std::uint8_t>{};
        auto const result = client.transmit( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_value() );
        EXPECT_EQ( result.value(), &*data.begin() );

        EXPECT_EQ( client.state(), State::CONNECTED );
        EXPECT_FALSE( client.is_transmitting() );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ State::CONNECTED, driver, socket_id, network_stack, true };

        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
        EXPECT_CALL( driver, read_sn_ir( socket_id ) )
            .WillOnce( Return( static_cast<std::uint8_t>( random<std::uint8_t>() | 0b000'1'0'0'0'0 ) ) );
        EXPECT_CALL( driver, write_sn_ir( socket_id, 0b000'1'0'0'0'0 ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );

        auto const data   = std::vector<std::uint8_t>{};
        auto const result = client.transmit( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_value() );
        EXPECT_EQ( result.value(), &*data.begin() );

        EXPECT_EQ( client.state(), State::CONNECTED );
        EXPECT_FALSE( client.is_transmitting() );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    }

    {
        struct {
            std::uint8_t sn_txbuf_size;
        } const test_cases[]{
            // clang-format off

            {  1 },
            {  2 },
            {  4 },
            {  8 },
            { 16 },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto driver        = Mock_Driver{};
            auto network_stack = Mock_Network_Stack{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{ State::CONNECTED, driver, socket_id, network_stack, false };

            auto const sn_tx_fsr_max = static_cast<std::uint16_t>( test_case.sn_txbuf_size * 1024 );
            auto const data = random_container<std::vector<std::uint8_t>>(
                random<std::uint16_t>( 1, sn_tx_fsr_max ) );
            auto const sn_tx_fsr = random<std::uint16_t>( data.size(), sn_tx_fsr_max );
            auto const sn_tx_wr  = random<std::uint16_t>();

            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
            EXPECT_CALL( driver, read_sn_txbuf_size( socket_id ) )
                .WillOnce( Return( test_case.sn_txbuf_size ) );
            EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( sn_tx_fsr ) );
            EXPECT_CALL( driver, read_sn_tx_wr( socket_id ) ).WillOnce( Return( sn_tx_wr ) );
            EXPECT_CALL( driver, write( socket_id, sn_tx_wr, data ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_tx_wr( socket_id, sn_tx_wr + data.size() ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_cr( socket_id, 0x20 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );

            auto const result = client.transmit( &*data.begin(), &*data.end() );

            EXPECT_TRUE( result.is_value() );
            EXPECT_EQ( result.value(), &*data.end() );

            EXPECT_EQ( client.state(), State::CONNECTED );
            EXPECT_TRUE( client.is_transmitting() );

            EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
        } // for
    }

    {
        struct {
            std::uint8_t sn_txbuf_size;
        } const test_cases[]{
            // clang-format off

            {  1 },
            {  2 },
            {  4 },
            {  8 },
            { 16 },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto driver        = Mock_Driver{};
            auto network_stack = Mock_Network_Stack{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{ State::CONNECTED, driver, socket_id, network_stack, true };

            auto const sn_tx_fsr_max = static_cast<std::uint16_t>( test_case.sn_txbuf_size * 1024 );
            auto const data = random_container<std::vector<std::uint8_t>>(
                random<std::uint16_t>( 1, sn_tx_fsr_max ) );
            auto const sn_tx_fsr = random<std::uint16_t>( data.size(), sn_tx_fsr_max );
            auto const sn_tx_wr  = random<std::uint16_t>();

            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
            EXPECT_CALL( driver, read_sn_ir( socket_id ) )
                .WillOnce( Return( static_cast<std::uint8_t>( random<std::uint8_t>() | 0b000'1'0'0'0'0 ) ) );
            EXPECT_CALL( driver, write_sn_ir( socket_id, 0b000'1'0'0'0'0 ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, read_sn_txbuf_size( socket_id ) )
                .WillOnce( Return( test_case.sn_txbuf_size ) );
            EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( sn_tx_fsr ) );
            EXPECT_CALL( driver, read_sn_tx_wr( socket_id ) ).WillOnce( Return( sn_tx_wr ) );
            EXPECT_CALL( driver, write( socket_id, sn_tx_wr, data ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_tx_wr( socket_id, sn_tx_wr + data.size() ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_cr( socket_id, 0x20 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );

            auto const result = client.transmit( &*data.begin(), &*data.end() );

            EXPECT_TRUE( result.is_value() );
            EXPECT_EQ( result.value(), &*data.end() );

            EXPECT_EQ( client.state(), State::CONNECTED );
            EXPECT_TRUE( client.is_transmitting() );

            EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
        } // for
    }

    {
        struct {
            std::uint8_t sn_txbuf_size;
        } const test_cases[]{
            // clang-format off

            {  1 },
            {  2 },
            {  4 },
            {  8 },
            { 16 },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto driver        = Mock_Driver{};
            auto network_stack = Mock_Network_Stack{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{ State::CONNECTED, driver, socket_id, network_stack, false };

            auto const sn_tx_fsr = random<std::uint16_t>( 1, test_case.sn_txbuf_size * 1024 );
            auto const data      = random_container<std::vector<std::uint8_t>>(
                random<std::uint16_t>( sn_tx_fsr + 1 ) );
            auto const sn_tx_wr = random<std::uint16_t>();

            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
            EXPECT_CALL( driver, read_sn_txbuf_size( socket_id ) )
                .WillOnce( Return( test_case.sn_txbuf_size ) );
            EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( sn_tx_fsr ) );
            EXPECT_CALL( driver, read_sn_tx_wr( socket_id ) ).WillOnce( Return( sn_tx_wr ) );
            EXPECT_CALL(
                driver,
                write( socket_id, sn_tx_wr, std::vector<std::uint8_t>{ data.begin(), data.begin() + sn_tx_fsr } ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_tx_wr( socket_id, sn_tx_wr + sn_tx_fsr ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_cr( socket_id, 0x20 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );

            auto const result = client.transmit( &*data.begin(), &*data.end() );

            EXPECT_TRUE( result.is_value() );
            EXPECT_EQ( result.value(), &*( data.begin() + sn_tx_fsr ) );

            EXPECT_EQ( client.state(), State::CONNECTED );
            EXPECT_TRUE( client.is_transmitting() );

            EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
        } // for
    }

    {
        struct {
            std::uint8_t sn_txbuf_size;
        } const test_cases[]{
            // clang-format off

            {  1 },
            {  2 },
            {  4 },
            {  8 },
            { 16 },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto driver        = Mock_Driver{};
            auto network_stack = Mock_Network_Stack{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{ State::CONNECTED, driver, socket_id, network_stack, true };

            auto const sn_tx_fsr = random<std::uint16_t>( 1, test_case.sn_txbuf_size * 1024 );
            auto const data      = random_container<std::vector<std::uint8_t>>(
                random<std::uint16_t>( sn_tx_fsr + 1 ) );
            auto const sn_tx_wr = random<std::uint16_t>();

            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
            EXPECT_CALL( driver, read_sn_ir( socket_id ) )
                .WillOnce( Return( static_cast<std::uint8_t>( random<std::uint8_t>() | 0b000'1'0'0'0'0 ) ) );
            EXPECT_CALL( driver, write_sn_ir( socket_id, 0b000'1'0'0'0'0 ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, read_sn_txbuf_size( socket_id ) )
                .WillOnce( Return( test_case.sn_txbuf_size ) );
            EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( sn_tx_fsr ) );
            EXPECT_CALL( driver, read_sn_tx_wr( socket_id ) ).WillOnce( Return( sn_tx_wr ) );
            EXPECT_CALL(
                driver,
                write( socket_id, sn_tx_wr, std::vector<std::uint8_t>{ data.begin(), data.begin() + sn_tx_fsr } ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_tx_wr( socket_id, sn_tx_wr + sn_tx_fsr ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_cr( socket_id, 0x20 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );

            auto const result = client.transmit( &*data.begin(), &*data.end() );

            EXPECT_TRUE( result.is_value() );
            EXPECT_EQ( result.value(), &*( data.begin() + sn_tx_fsr ) );

            EXPECT_EQ( client.state(), State::CONNECTED );
            EXPECT_TRUE( client.is_transmitting() );

            EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
        } // for
    }
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit_keepalive()
 *        properly handles an attempt to transmit data when the socket is not in a state
 *        that allows it to transmit data.
 */
TEST( transmitKeepalive, invalidState )
{
    struct {
        State state;
    } const test_cases[]{
        // clang-format off

        { State::UNINITIALIZED },
        { State::INITIALIZED   },
        { State::BOUND         },
        { State::CONNECTING,   },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto client = Client{ test_case.state, driver, random<Socket_ID>(), network_stack };

        auto const result = client.transmit_keepalive();

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::NOT_CONNECTED );

        EXPECT_EQ( client.state(), test_case.state );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit_keepalive()
 *        properly handles an SN_SR register read error.
 */
TEST( transmitKeepalive, snsrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.transmit_keepalive();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit_keepalive()
 *        properly handles an invalid SN_SR register value.
 */
TEST( transmitKeepalive, invalidSNSRValue )
{
    struct {
        std::uint8_t sn_sr;
    } const test_cases[]{
        // clang-format off

        { random<std::uint8_t>( 0x01, 0x16 ) },
        {                       0x19         },
        { random<std::uint8_t>( 0x1E       ) },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

        auto const error = random<Mock_Error>();

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );
        EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( error ) );

        auto const result = client.transmit_keepalive();

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );

        EXPECT_EQ( client.state(), State::CONNECTED );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit_keepalive()
 *        properly handles connection loss.
 */
TEST( transmitKeepalive, connectionLost )
{
    struct {
        std::uint8_t sn_sr;
    } const test_cases[]{
        // clang-format off

        { 0x00 },
        { 0x18 },
        { 0x1A },
        { 0x1B },
        { 0x1C },
        { 0x1D },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );

        auto const result = client.transmit_keepalive();

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::NOT_CONNECTED );

        EXPECT_EQ( client.state(), State::CONNECTED );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit_keepalive()
 *        properly handles an SN_CR register write error.
 */
TEST( transmitKeepalive, sncrWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.transmit_keepalive();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit_keepalive()
 *        properly handles an SN_CR register read error.
 */
TEST( transmitKeepalive, sncrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.transmit_keepalive();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit_keepalive() works
 *        properly.
 */
TEST( transmitKeepalive, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const socket_id = random<Socket_ID>();

    auto client = Client{ State::CONNECTED, driver, socket_id, network_stack };

    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x17 } ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x22 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );

    EXPECT_FALSE( client.transmit_keepalive().is_error() );

    EXPECT_EQ( client.state(), State::CONNECTED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::available() properly handles
 *        an SN_RXBUF_SIZE register read error.
 */
TEST( available, snrxbufsizeReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_rxbuf_size( _ ) ).WillOnce( Return( error ) );

    auto const result = client.available();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::available() properly handles
 *        an invalid SN_RXBUF_SIZE register value.
 */
TEST( available, invalidSNRXBUFSIZEValue )
{
    struct {
        std::uint8_t sn_rxbuf_size;
    } const test_cases[]{
        // clang-format off

        {                        0       },
        {                        3       },
        { random<std::uint8_t>(  5,  7 ) },
        { random<std::uint8_t>(  9, 15 ) },
        { random<std::uint8_t>( 17     ) },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const client = Client{ driver, random<Socket_ID>(), network_stack };

        auto const error = random<Mock_Error>();

        EXPECT_CALL( driver, read_sn_rxbuf_size( _ ) ).WillOnce( Return( test_case.sn_rxbuf_size ) );
        EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( error ) );

        auto const result = client.available();

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::available() properly handles
 *        an SN_RX_RSR register read error.
 */
TEST( available, snrxrsrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_rxbuf_size( _ ) )
        .WillOnce( Return( static_cast<std::uint8_t>( 1 << random<std::uint_fast8_t>( 0, 4 ) ) ) );
    EXPECT_CALL( driver, read_sn_rx_rsr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.available();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::available() properly handles
 *        an invalid SN_RX_RSR register value.
 */
TEST( available, invalidSNRXRSRValue )
{
    struct {
        std::uint8_t sn_rxbuf_size;
    } const test_cases[]{
        // clang-format off

        {  1 },
        {  2 },
        {  4 },
        {  8 },
        { 16 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const client = Client{ driver, random<Socket_ID>(), network_stack };

        auto const error = random<Mock_Error>();

        EXPECT_CALL( driver, read_sn_rxbuf_size( _ ) ).WillOnce( Return( test_case.sn_rxbuf_size ) );
        EXPECT_CALL( driver, read_sn_rx_rsr( _ ) )
            .WillOnce( Return( random<std::uint16_t>( ( test_case.sn_rxbuf_size * 1024 ) + 1 ) ) );
        EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( error ) );

        auto const result = client.available();

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::available() works properly.
 */
TEST( available, worksProperly )
{
    struct {
        std::uint8_t sn_rxbuf_size;
    } const test_cases[]{
        // clang-format off

        {  1 },
        {  2 },
        {  4 },
        {  8 },
        { 16 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_id = random<Socket_ID>();

        auto const client = Client{ driver, socket_id, network_stack };

        auto const sn_rx_rsr = random<std::uint16_t>( 0, test_case.sn_rxbuf_size * 1024 );

        EXPECT_CALL( driver, read_sn_rxbuf_size( socket_id ) ).WillOnce( Return( test_case.sn_rxbuf_size ) );
        EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) ).WillOnce( Return( sn_rx_rsr ) );

        auto const result = client.available();

        EXPECT_TRUE( result.is_value() );
        EXPECT_EQ( result.value(), sn_rx_rsr );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() properly handles
 *        an attempt to receive data when the socket is not in a state that allows it to
 *        receive data.
 */
TEST( receive, invalidState )
{
    struct {
        State state;
    } const test_cases[]{
        // clang-format off

        { State::UNINITIALIZED },
        { State::INITIALIZED   },
        { State::BOUND         },
        { State::CONNECTING    },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto client = Client{ test_case.state, driver, random<Socket_ID>(), network_stack };

        auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>() );
        auto const result = client.receive( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::NOT_CONNECTED );

        EXPECT_EQ( client.state(), test_case.state );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() properly handles
 *        an SN_SR register read error.
 */
TEST( receive, snsrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( error ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>() );
    auto const result = client.receive( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() properly handles
 *        an invalid SN_SR register value.
 */
TEST( receive, invalidSNSRValue )
{
    struct {
        std::uint8_t sn_sr;
    } const test_cases[]{
        // clang-format off

        { random<std::uint8_t>( 0x01, 0x16 ) },
        {                       0x19         },
        { random<std::uint8_t>( 0x1E       ) },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

        auto const error = random<Mock_Error>();

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );
        EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( error ) );

        auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>() );
        auto const result = client.receive( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );

        EXPECT_EQ( client.state(), State::CONNECTED );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() properly handles
 *        connection loss and graceful shutdown.
 */
TEST( receive, connectionLostGracefulShutdown )
{
    struct {
        std::uint8_t  sn_sr;
        Generic_Error error;
    } const test_cases[]{
        // clang-format off

        { 0x00, Generic_Error::NOT_CONNECTED },
        { 0x18, Generic_Error::WOULD_BLOCK   },
        { 0x1A, Generic_Error::WOULD_BLOCK   },
        { 0x1B, Generic_Error::WOULD_BLOCK   },
        { 0x1D, Generic_Error::WOULD_BLOCK   },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );

        auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>() );
        auto const result = client.receive( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), test_case.error );

        EXPECT_EQ( client.state(), State::CONNECTED );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() properly handles
 *        an SN_RXBUF_SIZE register read error.
 */
TEST( receive, snrxbufsizeReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) )
        .WillOnce( Return( static_cast<std::uint8_t>( random<bool>() ? 0x17 : 0x1C ) ) );
    EXPECT_CALL( driver, read_sn_rxbuf_size( _ ) ).WillOnce( Return( error ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>() );
    auto const result = client.receive( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() properly handles
 *        an invalid SN_RXBUF_SIZE register value.
 */
TEST( receive, invalidSNRXBUFSIZEValue )
{
    struct {
        std::uint8_t sn_rxbuf_size;
    } const test_cases[]{
        // clang-format off

        {                        0       },
        {                        3       },
        { random<std::uint8_t>(  5,  7 ) },
        { random<std::uint8_t>(  9, 15 ) },
        { random<std::uint8_t>( 17     ) },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

        auto const error = random<Mock_Error>();

        EXPECT_CALL( driver, read_sn_sr( _ ) )
            .WillOnce( Return( static_cast<std::uint8_t>( random<bool>() ? 0x17 : 0x1C ) ) );
        EXPECT_CALL( driver, read_sn_rxbuf_size( _ ) ).WillOnce( Return( test_case.sn_rxbuf_size ) );
        EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( error ) );

        auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>() );
        auto const result = client.receive( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );

        EXPECT_EQ( client.state(), State::CONNECTED );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() properly handles
 *        an SN_RX_RSR register read error.
 */
TEST( receive, snrxrsrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) )
        .WillOnce( Return( static_cast<std::uint8_t>( random<bool>() ? 0x17 : 0x1C ) ) );
    EXPECT_CALL( driver, read_sn_rxbuf_size( _ ) )
        .WillOnce( Return( static_cast<std::uint8_t>( 1 << random<std::uint_fast8_t>( 0, 4 ) ) ) );
    EXPECT_CALL( driver, read_sn_rx_rsr( _ ) ).WillOnce( Return( error ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>() );
    auto const result = client.receive( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() properly handles
 *        an invalid SN_RX_RSR register value.
 */
TEST( receive, invalidSNRXRSRValue )
{
    struct {
        std::uint8_t sn_rxbuf_size;
    } const test_cases[]{
        // clang-format off

        {  1 },
        {  2 },
        {  4 },
        {  8 },
        { 16 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

        auto const error = random<Mock_Error>();

        EXPECT_CALL( driver, read_sn_sr( _ ) )
            .WillOnce( Return( static_cast<std::uint8_t>( random<bool>() ? 0x17 : 0x1C ) ) );
        EXPECT_CALL( driver, read_sn_rxbuf_size( _ ) ).WillOnce( Return( test_case.sn_rxbuf_size ) );
        EXPECT_CALL( driver, read_sn_rx_rsr( _ ) )
            .WillOnce( Return( random<std::uint16_t>( ( test_case.sn_rxbuf_size * 1024 ) + 1 ) ) );
        EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( error ) );

        auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>() );
        auto const result = client.receive( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );

        EXPECT_EQ( client.state(), State::CONNECTED );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() properly handles
 *        an empty receive buffer.
 */
TEST( receive, receiveBufferEmpty )
{
    struct {
        std::uint8_t  sn_sr;
        Generic_Error error;
    } const test_cases[]{
        // clang-format off

        { 0x17, Generic_Error::WOULD_BLOCK   },
        { 0x1C, Generic_Error::NOT_CONNECTED },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );
        EXPECT_CALL( driver, read_sn_rxbuf_size( _ ) )
            .WillOnce( Return( static_cast<std::uint8_t>( 1 << random<std::uint_fast8_t>( 0, 4 ) ) ) );
        EXPECT_CALL( driver, read_sn_rx_rsr( _ ) ).WillOnce( Return( uint16_t{ 0 } ) );

        auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>() );
        auto const result = client.receive( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), test_case.error );

        EXPECT_EQ( client.state(), State::CONNECTED );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() properly handles
 *        an SN_RX_RD register read error.
 */
TEST( receive, snrxrdReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

    auto const sn_rxbuf_size = static_cast<std::uint8_t>( 1 << random<std::uint_fast8_t>( 0, 4 ) );
    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) )
        .WillOnce( Return( static_cast<std::uint8_t>( random<bool>() ? 0x17 : 0x1C ) ) );
    EXPECT_CALL( driver, read_sn_rxbuf_size( _ ) ).WillOnce( Return( sn_rxbuf_size ) );
    EXPECT_CALL( driver, read_sn_rx_rsr( _ ) ).WillOnce( Return( random<std::uint16_t>( 1, sn_rxbuf_size * 1024 ) ) );
    EXPECT_CALL( driver, read_sn_rx_rd( _ ) ).WillOnce( Return( error ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = client.receive( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() properly handles a
 *        receive buffer read error.
 */
TEST( receive, receiveBufferReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

    auto const sn_rxbuf_size = static_cast<std::uint8_t>( 1 << random<std::uint_fast8_t>( 0, 4 ) );
    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) )
        .WillOnce( Return( static_cast<std::uint8_t>( random<bool>() ? 0x17 : 0x1C ) ) );
    EXPECT_CALL( driver, read_sn_rxbuf_size( _ ) ).WillOnce( Return( sn_rxbuf_size ) );
    EXPECT_CALL( driver, read_sn_rx_rsr( _ ) ).WillOnce( Return( random<std::uint16_t>( 1, sn_rxbuf_size * 1024 ) ) );
    EXPECT_CALL( driver, read_sn_rx_rd( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
    EXPECT_CALL( driver, read( _, _, _ ) ).WillOnce( Return( error ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = client.receive( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() properly handles
 *        an SN_RX_RD register write error.
 */
TEST( receive, snrxrdWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

    auto data = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );

    auto const sn_rxbuf_size = static_cast<std::uint8_t>( 1 << random<std::uint_fast8_t>( 0, 4 ) );
    auto const sn_rx_rsr = random<std::uint16_t>( 1, sn_rxbuf_size * 1024 );
    auto const error     = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) )
        .WillOnce( Return( static_cast<std::uint8_t>( random<bool>() ? 0x17 : 0x1C ) ) );
    EXPECT_CALL( driver, read_sn_rxbuf_size( _ ) ).WillOnce( Return( sn_rxbuf_size ) );
    EXPECT_CALL( driver, read_sn_rx_rsr( _ ) ).WillOnce( Return( sn_rx_rsr ) );
    EXPECT_CALL( driver, read_sn_rx_rd( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
    EXPECT_CALL( driver, read( _, _, _ ) )
        .WillOnce( Return( random_container<std::vector<std::uint8_t>>(
            std::min( static_cast<std::uint16_t>( data.size() ), sn_rx_rsr ) ) ) );
    EXPECT_CALL( driver, write_sn_rx_rd( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.receive( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() properly handles
 *        an SN_CR register write error.
 */
TEST( receive, sncrWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

    auto data = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );

    auto const sn_rxbuf_size = static_cast<std::uint8_t>( 1 << random<std::uint_fast8_t>( 0, 4 ) );
    auto const sn_rx_rsr = random<std::uint16_t>( 1, sn_rxbuf_size * 1024 );
    auto const error     = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) )
        .WillOnce( Return( static_cast<std::uint8_t>( random<bool>() ? 0x17 : 0x1C ) ) );
    EXPECT_CALL( driver, read_sn_rxbuf_size( _ ) ).WillOnce( Return( sn_rxbuf_size ) );
    EXPECT_CALL( driver, read_sn_rx_rsr( _ ) ).WillOnce( Return( sn_rx_rsr ) );
    EXPECT_CALL( driver, read_sn_rx_rd( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
    EXPECT_CALL( driver, read( _, _, _ ) )
        .WillOnce( Return( random_container<std::vector<std::uint8_t>>(
            std::min( static_cast<std::uint16_t>( data.size() ), sn_rx_rsr ) ) ) );
    EXPECT_CALL( driver, write_sn_rx_rd( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.receive( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() properly handles
 *        an SN_CR register read error.
 */
TEST( receive, sncrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

    auto data = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );

    auto const sn_rxbuf_size = static_cast<std::uint8_t>( 1 << random<std::uint_fast8_t>( 0, 4 ) );
    auto const sn_rx_rsr = random<std::uint16_t>( 1, sn_rxbuf_size * 1024 );
    auto const error     = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) )
        .WillOnce( Return( static_cast<std::uint8_t>( random<bool>() ? 0x17 : 0x1C ) ) );
    EXPECT_CALL( driver, read_sn_rxbuf_size( _ ) ).WillOnce( Return( sn_rxbuf_size ) );
    EXPECT_CALL( driver, read_sn_rx_rsr( _ ) ).WillOnce( Return( sn_rx_rsr ) );
    EXPECT_CALL( driver, read_sn_rx_rd( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
    EXPECT_CALL( driver, read( _, _, _ ) )
        .WillOnce( Return( random_container<std::vector<std::uint8_t>>(
            std::min( static_cast<std::uint16_t>( data.size() ), sn_rx_rsr ) ) ) );
    EXPECT_CALL( driver, write_sn_rx_rd( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.receive( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() works properly.
 */
TEST( receive, worksProperly )
{
    {
        struct {
            std::uint8_t sn_sr;
            std::uint8_t sn_rxbuf_size;
        } const test_cases[]{
            // clang-format off

            { 0x17,  1 },
            { 0x17,  2 },
            { 0x17,  4 },
            { 0x17,  8 },
            { 0x17, 16 },

            { 0x1C,  1 },
            { 0x1C,  2 },
            { 0x1C,  4 },
            { 0x1C,  8 },
            { 0x1C, 16 },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto driver        = Mock_Driver{};
            auto network_stack = Mock_Network_Stack{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{ State::CONNECTED, driver, socket_id, network_stack };

            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );
            EXPECT_CALL( driver, read_sn_rxbuf_size( socket_id ) )
                .WillOnce( Return( test_case.sn_rxbuf_size ) );
            EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) )
                .WillOnce( Return( random<std::uint16_t>( 1, test_case.sn_rxbuf_size * 1024 ) ) );

            auto       data   = std::vector<std::uint8_t>{};
            auto const result = client.receive( &*data.begin(), &*data.end() );

            EXPECT_TRUE( result.is_value() );
            EXPECT_EQ( result.value(), &*data.begin() );

            EXPECT_EQ( client.state(), State::CONNECTED );

            EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
        } // for
    }

    {
        struct {
            std::uint8_t sn_sr;
            std::uint8_t sn_rxbuf_size;
        } const test_cases[]{
            // clang-format off

            { 0x17,  1 },
            { 0x17,  2 },
            { 0x17,  4 },
            { 0x17,  8 },
            { 0x17, 16 },

            { 0x1C,  1 },
            { 0x1C,  2 },
            { 0x1C,  4 },
            { 0x1C,  8 },
            { 0x1C, 16 },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto driver        = Mock_Driver{};
            auto network_stack = Mock_Network_Stack{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{ State::CONNECTED, driver, socket_id, network_stack };

            auto const sn_rx_rsr_max = static_cast<std::uint16_t>( test_case.sn_rxbuf_size * 1024 );
            auto data = std::vector<std::uint8_t>( random<std::uint16_t>( 1, sn_rx_rsr_max ) );
            auto const sn_rx_rsr = random<std::uint16_t>( data.size(), sn_rx_rsr_max );
            auto const sn_rx_rd  = random<std::uint16_t>();
            auto const data_expected = random_container<std::vector<std::uint8_t>>( data.size() );

            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );
            EXPECT_CALL( driver, read_sn_rxbuf_size( socket_id ) )
                .WillOnce( Return( test_case.sn_rxbuf_size ) );
            EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) ).WillOnce( Return( sn_rx_rsr ) );
            EXPECT_CALL( driver, read_sn_rx_rd( socket_id ) ).WillOnce( Return( sn_rx_rd ) );
            EXPECT_CALL( driver, read( socket_id, sn_rx_rd, _ ) ).WillOnce( Return( data_expected ) );
            EXPECT_CALL( driver, write_sn_rx_rd( socket_id, sn_rx_rd + data.size() ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_cr( socket_id, 0x40 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );

            auto const result = client.receive( &*data.begin(), &*data.end() );

            EXPECT_TRUE( result.is_value() );
            EXPECT_EQ( result.value(), &*data.end() );

            EXPECT_EQ( data, data_expected );

            EXPECT_EQ( client.state(), State::CONNECTED );

            EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
        } // for
    }

    {
        struct {
            std::uint8_t sn_sr;
            std::uint8_t sn_rxbuf_size;
        } const test_cases[]{
            // clang-format off

            { 0x17,  1 },
            { 0x17,  2 },
            { 0x17,  4 },
            { 0x17,  8 },
            { 0x17, 16 },

            { 0x1C,  1 },
            { 0x1C,  2 },
            { 0x1C,  4 },
            { 0x1C,  8 },
            { 0x1C, 16 },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto driver        = Mock_Driver{};
            auto network_stack = Mock_Network_Stack{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{ State::CONNECTED, driver, socket_id, network_stack };

            auto const sn_rx_rsr = random<std::uint16_t>( 1, test_case.sn_rxbuf_size * 1024 );
            auto const data_expected = random_container<std::vector<std::uint8_t>>( sn_rx_rsr );
            auto const sn_rx_rd = random<std::uint16_t>();

            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );
            EXPECT_CALL( driver, read_sn_rxbuf_size( socket_id ) )
                .WillOnce( Return( test_case.sn_rxbuf_size ) );
            EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) ).WillOnce( Return( sn_rx_rsr ) );
            EXPECT_CALL( driver, read_sn_rx_rd( socket_id ) ).WillOnce( Return( sn_rx_rd ) );
            EXPECT_CALL( driver, read( socket_id, sn_rx_rd, _ ) ).WillOnce( Return( data_expected ) );
            EXPECT_CALL( driver, write_sn_rx_rd( socket_id, sn_rx_rd + sn_rx_rsr ) )
                .WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, write_sn_cr( socket_id, 0x40 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );

            auto data = std::vector<std::uint8_t>( random<std::uint16_t>( sn_rx_rsr + 1 ) );
            auto const result = client.receive( &*data.begin(), &*data.end() );

            EXPECT_TRUE( result.is_value() );
            EXPECT_EQ( result.value(), &*( data.begin() + sn_rx_rsr ) );

            EXPECT_EQ( ( std::vector<std::uint8_t>{ &*data.begin(), result.value() } ), data_expected );

            EXPECT_EQ( client.state(), State::CONNECTED );

            EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
        } // for
    }
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::shutdown() properly handles
 *        an attempt to disable further data transmission and reception when the socket is
 *        not in a state that allows further data transmission and reception to be
 *        disabled.
 */
TEST( shutdown, invalidState )
{
    struct {
        State state;
    } const test_cases[]{
        // clang-format off

        { State::UNINITIALIZED },
        { State::INITIALIZED   },
        { State::BOUND         },
        { State::CONNECTING    },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto client = Client{ test_case.state, driver, random<Socket_ID>(), network_stack };

        auto const result = client.shutdown();

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::NOT_CONNECTED );

        EXPECT_EQ( client.state(), test_case.state );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::shutdown() properly handles
 *        an SN_SR register read error.
 */
TEST( shutdown, snsrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.shutdown();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::shutdown() properly handles
 *        an invalid SN_SR register value.
 */
TEST( shutdown, invalidSNSRValue )
{
    struct {
        std::uint8_t sn_sr;
    } const test_cases[]{
        // clang-format off

        { random<std::uint8_t>( 0x01, 0x16 ) },
        {                       0x19         },
        { random<std::uint8_t>( 0x1E       ) },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

        auto const error = random<Mock_Error>();

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );
        EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( error ) );

        auto const result = client.shutdown();

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );

        EXPECT_EQ( client.state(), State::CONNECTED );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::shutdown() properly handles
 *        an attempt to disable further data transmission and reception when the socket is
 *        not connected to a remote endpoint.
 */
TEST( shutdown, notConnected )
{
    struct {
        std::uint8_t sn_sr;
    } const test_cases[]{
        // clang-format off

        { 0x00 },
        { 0x18 },
        { 0x1A },
        { 0x1B },
        { 0x1D },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );

        auto const result = client.shutdown();

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::NOT_CONNECTED );

        EXPECT_EQ( client.state(), State::CONNECTED );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::shutdown() properly handles
 *        an SN_CR register write error.
 */
TEST( shutdown, sncrWriteError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( static_cast<uint8_t>( random<bool>() ? 0x17 : 0x1C ) ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( error ) );

    auto const result = client.shutdown();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::shutdown() properly handles
 *        an SN_CR register read error.
 */
TEST( shutdown, sncrReadError )
{
    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto client = Client{ State::CONNECTED, driver, random<Socket_ID>(), network_stack };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( static_cast<uint8_t>( random<bool>() ? 0x17 : 0x1C ) ) );
    EXPECT_CALL( driver, write_sn_cr( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( error ) );

    auto const result = client.shutdown();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_EQ( client.state(), State::CONNECTED );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::shutdown() works properly.
 */
TEST( shutdown, worksProperly )
{
    struct {
        std::uint8_t sn_sr;
    } const test_cases[]{
        // clang-format off

        { 0x17 },
        { 0x1C },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const in_sequence = InSequence{};

        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ State::CONNECTED, driver, socket_id, network_stack };

        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );
        EXPECT_CALL( driver, write_sn_cr( socket_id, 0x08 ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( std::uint8_t{ 0x00 } ) );

        EXPECT_FALSE( client.shutdown().is_error() );

        EXPECT_EQ( client.state(), State::CONNECTED );

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( AnyNumber() );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::close() works properly.
 */
TEST( close, worksProperly )
{
    {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto client = Client{ State::UNINITIALIZED, driver, random<Socket_ID>(), network_stack };

        EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( 0 );

        EXPECT_FALSE( client.close().is_error() );
    }

    {
        struct {
            State state;
        } const test_cases[]{
            // clang-format off

            { State::INITIALIZED },
            { State::BOUND       },
            { State::CONNECTING  },
            { State::CONNECTED   },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto driver        = Mock_Driver{};
            auto network_stack = Mock_Network_Stack{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{ test_case.state, driver, socket_id, network_stack };

            EXPECT_CALL( network_stack, deallocate_socket( socket_id ) );

            EXPECT_FALSE( client.close().is_error() );

            EXPECT_CALL( network_stack, deallocate_socket( _ ) ).Times( 0 );
        } // for
    }
}

/**
 * \brief Execute the picolibrary::WIZnet::W5500::IP::TCP::Client unit tests.
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

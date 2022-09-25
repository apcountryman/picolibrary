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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client automated test program.
 */

#include <cstdint>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/array.h"
#include "picolibrary/error.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/ip/tcp.h"
#include "picolibrary/testing/automated/ipv4.h"
#include "picolibrary/testing/automated/random.h"
#include "picolibrary/testing/automated/wiznet/w5500.h"
#include "picolibrary/testing/automated/wiznet/w5500/ip.h"
#include "picolibrary/testing/automated/wiznet/w5500/ip/network_stack.h"
#include "picolibrary/utility.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/ip/tcp.h"

namespace {

using ::picolibrary::Array;
using ::picolibrary::Generic_Error;
using ::picolibrary::to_underlying;
using ::picolibrary::IP::TCP::Endpoint;
using ::picolibrary::IP::TCP::Port;
using ::picolibrary::IPv4::Address;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::random;
using ::picolibrary::Testing::Automated::random_array;
using ::picolibrary::Testing::Automated::random_container;
using ::picolibrary::Testing::Automated::WIZnet::W5500::Mock_Driver;
using ::picolibrary::Testing::Automated::WIZnet::W5500::IP::Mock_Network_Stack;
using ::picolibrary::Testing::Automated::WIZnet::W5500::IP::Mock_Port_Allocator;
using ::picolibrary::WIZnet::W5500::Socket_Buffer_Size;
using ::picolibrary::WIZnet::W5500::Socket_ID;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Ref;
using ::testing::Return;
using ::testing::ReturnRef;

using Client = ::picolibrary::WIZnet::W5500::IP::TCP::Client<Mock_Driver, Mock_Network_Stack>;

} // namespace

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::Client() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const client = Client{};

    EXPECT_EQ( client.state(), Client::State::UNINITIALIZED );
}

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

        EXPECT_EQ( client.state(), Client::State::INITIALIZED );
        EXPECT_EQ( client.socket_id(), test_case.socket_id );
        EXPECT_EQ( client.socket_interrupt_mask(), test_case.socket_interrupt_mask );
        EXPECT_FALSE( client.is_transmitting() );

        EXPECT_CALL( driver, write_sn_mr( _, _ ) );
        EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
        EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
        EXPECT_CALL( driver, write_sn_imr( _, _ ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
        EXPECT_CALL( network_stack, deallocate_socket( _ ) );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::bind() works properly.
 */
TEST( bind, worksProperly )
{
    {
        auto const in_sequence = InSequence{};

        auto driver             = Mock_Driver{};
        auto network_stack      = Mock_Network_Stack{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ driver, socket_id, network_stack };

        auto const port = random<Port>( 1 );

        EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
        EXPECT_CALL( tcp_port_allocator, allocate( Ref( driver ), Port{} ) ).WillOnce( Return( port ) );
        EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) );
        EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x13 ) );

        client.bind();

        EXPECT_EQ( client.state(), Client::State::BOUND );

        EXPECT_CALL( driver, write_sn_cr( _, _ ) );
        EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, write_sn_ir( _, _ ) );
        EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
        EXPECT_CALL( driver, write_sn_port( _, _ ) );
        EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
        EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
        EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
        EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
        EXPECT_CALL( driver, write_sn_dport( _, _ ) );
        EXPECT_CALL( driver, write_sn_mr( _, _ ) );
        EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
        EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
        EXPECT_CALL( driver, write_sn_imr( _, _ ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
        EXPECT_CALL( network_stack, deallocate_socket( _ ) );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver             = Mock_Driver{};
        auto network_stack      = Mock_Network_Stack{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ driver, socket_id, network_stack };

        auto const port = random<Port>( 1 );

        EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
        EXPECT_CALL( tcp_port_allocator, allocate( Ref( driver ), Port{} ) ).WillOnce( Return( port ) );
        EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) );
        EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x13 ) );

        client.bind( Port{} );

        EXPECT_EQ( client.state(), Client::State::BOUND );

        EXPECT_CALL( driver, write_sn_cr( _, _ ) );
        EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, write_sn_ir( _, _ ) );
        EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
        EXPECT_CALL( driver, write_sn_port( _, _ ) );
        EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
        EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
        EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
        EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
        EXPECT_CALL( driver, write_sn_dport( _, _ ) );
        EXPECT_CALL( driver, write_sn_mr( _, _ ) );
        EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
        EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
        EXPECT_CALL( driver, write_sn_imr( _, _ ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
        EXPECT_CALL( network_stack, deallocate_socket( _ ) );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver             = Mock_Driver{};
        auto network_stack      = Mock_Network_Stack{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ driver, socket_id, network_stack };

        auto const port = random<Port>( 1 );

        EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
        EXPECT_CALL( tcp_port_allocator, allocate( Ref( driver ), port ) ).WillOnce( Return( port ) );
        EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) );
        EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x13 ) );

        client.bind( port );

        EXPECT_EQ( client.state(), Client::State::BOUND );

        EXPECT_CALL( driver, write_sn_cr( _, _ ) );
        EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, write_sn_ir( _, _ ) );
        EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
        EXPECT_CALL( driver, write_sn_port( _, _ ) );
        EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
        EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
        EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
        EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
        EXPECT_CALL( driver, write_sn_dport( _, _ ) );
        EXPECT_CALL( driver, write_sn_mr( _, _ ) );
        EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
        EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
        EXPECT_CALL( driver, write_sn_imr( _, _ ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
        EXPECT_CALL( network_stack, deallocate_socket( _ ) );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver             = Mock_Driver{};
        auto network_stack      = Mock_Network_Stack{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ driver, socket_id, network_stack };

        auto const address = random<Address>( 1 );
        auto const port    = random<Port>( 1 );

        EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( address.as_byte_array() ) );
        EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
        EXPECT_CALL( tcp_port_allocator, allocate( Ref( driver ), Port{} ) ).WillOnce( Return( port ) );
        EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) );
        EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x13 ) );

        client.bind( { address, {} } );

        EXPECT_EQ( client.state(), Client::State::BOUND );

        EXPECT_CALL( driver, write_sn_cr( _, _ ) );
        EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, write_sn_ir( _, _ ) );
        EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
        EXPECT_CALL( driver, write_sn_port( _, _ ) );
        EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
        EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
        EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
        EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
        EXPECT_CALL( driver, write_sn_dport( _, _ ) );
        EXPECT_CALL( driver, write_sn_mr( _, _ ) );
        EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
        EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
        EXPECT_CALL( driver, write_sn_imr( _, _ ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
        EXPECT_CALL( network_stack, deallocate_socket( _ ) );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver             = Mock_Driver{};
        auto network_stack      = Mock_Network_Stack{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ driver, socket_id, network_stack };

        auto const address = random<Address>( 1 );
        auto const port    = random<Port>( 1 );

        EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( address.as_byte_array() ) );
        EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
        EXPECT_CALL( tcp_port_allocator, allocate( Ref( driver ), port ) ).WillOnce( Return( port ) );
        EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) );
        EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x13 ) );

        client.bind( { address, port } );

        EXPECT_EQ( client.state(), Client::State::BOUND );

        EXPECT_CALL( driver, write_sn_cr( _, _ ) );
        EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, write_sn_ir( _, _ ) );
        EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
        EXPECT_CALL( driver, write_sn_port( _, _ ) );
        EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
        EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
        EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
        EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
        EXPECT_CALL( driver, write_sn_dport( _, _ ) );
        EXPECT_CALL( driver, write_sn_mr( _, _ ) );
        EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
        EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
        EXPECT_CALL( driver, write_sn_imr( _, _ ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
        EXPECT_CALL( network_stack, deallocate_socket( _ ) );
    }
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::connect() properly handles a
 *        connection timeout.
 */
TEST( connect, connectionTimeout )
{
    auto driver             = Mock_Driver{};
    auto network_stack      = Mock_Network_Stack{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTING, driver, random<Socket_ID>(), network_stack };

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );

    auto const result = client.connect( { random<Address>( 1 ), random<Port>( 1 ) } );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::OPERATION_TIMEOUT );

    EXPECT_EQ( client.state(), Client::State::CONNECTING );

    EXPECT_CALL( driver, write_sn_cr( _, _ ) );
    EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, write_sn_ir( _, _ ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
    EXPECT_CALL( driver, write_sn_port( _, _ ) );
    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
    EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
    EXPECT_CALL( driver, write_sn_dport( _, _ ) );
    EXPECT_CALL( driver, write_sn_mr( _, _ ) );
    EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
    EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
    EXPECT_CALL( driver, write_sn_imr( _, _ ) );
    EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::connect() works properly.
 */
TEST( connect, worksProperly )
{
    {
        auto const in_sequence = InSequence{};

        auto driver             = Mock_Driver{};
        auto network_stack      = Mock_Network_Stack{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ Client::State::BOUND, driver, socket_id, network_stack };

        auto const endpoint = Endpoint{ random<Address>( 1 ), random<Port>( 1 ) };

        EXPECT_CALL( driver, write_sn_dipr( socket_id, endpoint.address().ipv4().as_byte_array() ) );
        EXPECT_CALL( driver, write_sn_dport( socket_id, endpoint.port().as_unsigned_integer() ) );
        EXPECT_CALL( driver, write_sn_cr( socket_id, 0x04 ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

        auto const result = client.connect( endpoint );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

        EXPECT_EQ( client.state(), Client::State::CONNECTING );

        EXPECT_CALL( driver, write_sn_cr( _, _ ) );
        EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, write_sn_ir( _, _ ) );
        EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
        EXPECT_CALL( driver, write_sn_port( _, _ ) );
        EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
        EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
        EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
        EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
        EXPECT_CALL( driver, write_sn_dport( _, _ ) );
        EXPECT_CALL( driver, write_sn_mr( _, _ ) );
        EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
        EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
        EXPECT_CALL( driver, write_sn_imr( _, _ ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
        EXPECT_CALL( network_stack, deallocate_socket( _ ) );
    }

    {
        auto driver             = Mock_Driver{};
        auto network_stack      = Mock_Network_Stack{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ Client::State::CONNECTING, driver, socket_id, network_stack };

        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x15 ) );

        auto const result = client.connect( { random<Address>( 1 ), random<Port>( 1 ) } );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

        EXPECT_EQ( client.state(), Client::State::CONNECTING );

        EXPECT_CALL( driver, write_sn_cr( _, _ ) );
        EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, write_sn_ir( _, _ ) );
        EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
        EXPECT_CALL( driver, write_sn_port( _, _ ) );
        EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
        EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
        EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
        EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
        EXPECT_CALL( driver, write_sn_dport( _, _ ) );
        EXPECT_CALL( driver, write_sn_mr( _, _ ) );
        EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
        EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
        EXPECT_CALL( driver, write_sn_imr( _, _ ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
        EXPECT_CALL( network_stack, deallocate_socket( _ ) );
    }

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
            auto driver             = Mock_Driver{};
            auto network_stack      = Mock_Network_Stack{};
            auto tcp_port_allocator = Mock_Port_Allocator{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{ Client::State::CONNECTING, driver, socket_id, network_stack };

            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );

            EXPECT_FALSE( client.connect( { random<Address>( 1 ), random<Port>( 1 ) } ).is_error() );

            EXPECT_EQ( client.state(), Client::State::CONNECTED );

            EXPECT_CALL( driver, write_sn_cr( _, _ ) );
            EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
            EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
            EXPECT_CALL( driver, write_sn_ir( _, _ ) );
            EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
            EXPECT_CALL( driver, write_sn_port( _, _ ) );
            EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
            EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
            EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
            EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
            EXPECT_CALL( driver, write_sn_dport( _, _ ) );
            EXPECT_CALL( driver, write_sn_mr( _, _ ) );
            EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
            EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
            EXPECT_CALL( driver, write_sn_imr( _, _ ) );
            EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
            EXPECT_CALL( network_stack, deallocate_socket( _ ) );
        } // for
    }
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
        { 0x17, true  },
        { 0x1C, false },
        { 0x15, false },
        { 0x18, false },
        { 0x1A, false },
        { 0x1B, false },
        { 0x1D, false },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_id = random<Socket_ID>();

        auto const client = Client{ driver, socket_id, network_stack };

        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );

        EXPECT_EQ( client.is_connected(), test_case.is_connected );

        EXPECT_CALL( driver, write_sn_mr( _, _ ) );
        EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
        EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
        EXPECT_CALL( driver, write_sn_imr( _, _ ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
        EXPECT_CALL( network_stack, deallocate_socket( _ ) );
    } // for
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

    auto const sn_dipr  = random_array<std::uint8_t, 4>();
    auto const sn_dport = random<std::uint16_t>();

    EXPECT_CALL( driver, read_sn_dipr( socket_id ) ).WillOnce( Return( sn_dipr ) );
    EXPECT_CALL( driver, read_sn_dport( socket_id ) ).WillOnce( Return( sn_dport ) );

    auto const endpoint = client.remote_endpoint();

    EXPECT_TRUE( endpoint.address().is_ipv4() );
    EXPECT_EQ( endpoint.address().ipv4().as_byte_array(), sn_dipr );
    EXPECT_EQ( endpoint.port().as_unsigned_integer(), sn_dport );

    EXPECT_CALL( driver, write_sn_mr( _, _ ) );
    EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
    EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
    EXPECT_CALL( driver, write_sn_imr( _, _ ) );
    EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
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

    auto const sipr    = random_array<std::uint8_t, 4>();
    auto const sn_port = random<std::uint16_t>();

    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( sipr ) );
    EXPECT_CALL( driver, read_sn_port( socket_id ) ).WillOnce( Return( sn_port ) );

    auto const endpoint = client.local_endpoint();

    EXPECT_TRUE( endpoint.address().is_ipv4() );
    EXPECT_EQ( endpoint.address().ipv4().as_byte_array(), sipr );
    EXPECT_EQ( endpoint.port().as_unsigned_integer(), sn_port );

    EXPECT_CALL( driver, write_sn_mr( _, _ ) );
    EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
    EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
    EXPECT_CALL( driver, write_sn_imr( _, _ ) );
    EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::outstanding() works
 *        properly.
 */
TEST( outstanding, worksProperly )
{
    struct {
        Socket_Buffer_Size socket_buffer_size;
    } const test_cases[]{
        // clang-format off

        { Socket_Buffer_Size::_2_KiB  },
        { Socket_Buffer_Size::_4_KiB  },
        { Socket_Buffer_Size::_8_KiB  },
        { Socket_Buffer_Size::_16_KiB },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_id = random<Socket_ID>();

        auto const client = Client{ driver, socket_id, network_stack };

        auto const sn_tx_fsr = random<std::uint16_t>(
            0, to_underlying( test_case.socket_buffer_size ) * 1024 );

        EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
        EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( sn_tx_fsr ) );
        EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( random<Mock_Error>() ) );

        EXPECT_EQ( client.outstanding(), ( to_underlying( test_case.socket_buffer_size ) * 1024 ) - sn_tx_fsr );

        EXPECT_CALL( driver, write_sn_mr( _, _ ) );
        EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
        EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
        EXPECT_CALL( driver, write_sn_imr( _, _ ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
        EXPECT_CALL( network_stack, deallocate_socket( _ ) );
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
        { 0x1C },
        { 0x18 },
        { 0x1A },
        { 0x1B },
        { 0x1D },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver             = Mock_Driver{};
        auto network_stack      = Mock_Network_Stack{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto const is_transmitting = random<bool>();

        auto client = Client{ Client::State::CONNECTED, driver, random<Socket_ID>(), network_stack, is_transmitting };

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );

        auto const data   = random_container<std::vector<std::uint8_t>>();
        auto const result = client.transmit( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::NOT_CONNECTED );

        EXPECT_EQ( client.state(), Client::State::CONNECTED );
        EXPECT_EQ( client.is_transmitting(), is_transmitting );

        EXPECT_CALL( driver, write_sn_cr( _, _ ) );
        EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, write_sn_ir( _, _ ) );
        EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
        EXPECT_CALL( driver, write_sn_port( _, _ ) );
        EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
        EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
        EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
        EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
        EXPECT_CALL( driver, write_sn_dport( _, _ ) );
        EXPECT_CALL( driver, write_sn_mr( _, _ ) );
        EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
        EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
        EXPECT_CALL( driver, write_sn_imr( _, _ ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
        EXPECT_CALL( network_stack, deallocate_socket( _ ) );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() properly handles
 *        attempts to transmit data when an in progress transmission is not complete.
 */
TEST( transmit, transmissionNotComplete )
{
    auto driver             = Mock_Driver{};
    auto network_stack      = Mock_Network_Stack{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const socket_id = random<Socket_ID>();

    auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack, true };

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x17 ) );
    EXPECT_CALL( driver, read_sn_ir( _ ) ).WillOnce( Return( random<std::uint8_t>() & 0b111'0'1'1'1'1 ) );

    auto const data   = random_container<std::vector<std::uint8_t>>();
    auto const result = client.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );
    EXPECT_TRUE( client.is_transmitting() );

    EXPECT_CALL( driver, write_sn_cr( _, _ ) );
    EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, write_sn_ir( _, _ ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
    EXPECT_CALL( driver, write_sn_port( _, _ ) );
    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
    EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
    EXPECT_CALL( driver, write_sn_dport( _, _ ) );
    EXPECT_CALL( driver, write_sn_mr( _, _ ) );
    EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
    EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
    EXPECT_CALL( driver, write_sn_imr( _, _ ) );
    EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() properly handles
 *        a full transmit buffer.
 */
TEST( transmit, transmitBufferFull )
{
    auto driver             = Mock_Driver{};
    auto network_stack      = Mock_Network_Stack{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const socket_id = random<Socket_ID>();

    auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack, false };

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x17 ) );
    EXPECT_CALL( network_stack, socket_buffer_size() )
        .WillOnce( Return( static_cast<Socket_Buffer_Size>( 1 << random<std::uint_fast8_t>( 1, 4 ) ) ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( _ ) ).WillOnce( Return( 0 ) );
    EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( random<Mock_Error>() ) );

    auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1, 15 ) );
    auto const result = client.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );
    EXPECT_FALSE( client.is_transmitting() );

    EXPECT_CALL( driver, write_sn_cr( _, _ ) );
    EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, write_sn_ir( _, _ ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
    EXPECT_CALL( driver, write_sn_port( _, _ ) );
    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
    EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
    EXPECT_CALL( driver, write_sn_dport( _, _ ) );
    EXPECT_CALL( driver, write_sn_mr( _, _ ) );
    EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
    EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
    EXPECT_CALL( driver, write_sn_imr( _, _ ) );
    EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() works properly.
 */
TEST( transmit, worksProperly )
{
    {
        auto driver             = Mock_Driver{};
        auto network_stack      = Mock_Network_Stack{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack, false };

        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );

        auto const data   = std::vector<std::uint8_t>{};
        auto const result = client.transmit( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_value() );
        EXPECT_EQ( result.value(), &*data.end() );

        EXPECT_EQ( client.state(), Client::State::CONNECTED );
        EXPECT_FALSE( client.is_transmitting() );

        EXPECT_CALL( driver, write_sn_cr( _, _ ) );
        EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, write_sn_ir( _, _ ) );
        EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
        EXPECT_CALL( driver, write_sn_port( _, _ ) );
        EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
        EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
        EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
        EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
        EXPECT_CALL( driver, write_sn_dport( _, _ ) );
        EXPECT_CALL( driver, write_sn_mr( _, _ ) );
        EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
        EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
        EXPECT_CALL( driver, write_sn_imr( _, _ ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
        EXPECT_CALL( network_stack, deallocate_socket( _ ) );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver             = Mock_Driver{};
        auto network_stack      = Mock_Network_Stack{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack, true };

        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
        EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( random<std::uint8_t>() | 0b000'1'0'0'0'0 ) );
        EXPECT_CALL( driver, write_sn_ir( socket_id, 0b000'1'0'0'0'0 ) );

        auto const data   = std::vector<std::uint8_t>{};
        auto const result = client.transmit( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_value() );
        EXPECT_EQ( result.value(), &*data.end() );

        EXPECT_EQ( client.state(), Client::State::CONNECTED );
        EXPECT_FALSE( client.is_transmitting() );

        EXPECT_CALL( driver, write_sn_cr( _, _ ) );
        EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, write_sn_ir( _, _ ) );
        EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
        EXPECT_CALL( driver, write_sn_port( _, _ ) );
        EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
        EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
        EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
        EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
        EXPECT_CALL( driver, write_sn_dport( _, _ ) );
        EXPECT_CALL( driver, write_sn_mr( _, _ ) );
        EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
        EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
        EXPECT_CALL( driver, write_sn_imr( _, _ ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
        EXPECT_CALL( network_stack, deallocate_socket( _ ) );
    }

    {
        struct {
            Socket_Buffer_Size socket_buffer_size;
        } const test_cases[]{
            // clang-format off

            { Socket_Buffer_Size::_2_KiB  },
            { Socket_Buffer_Size::_4_KiB  },
            { Socket_Buffer_Size::_8_KiB  },
            { Socket_Buffer_Size::_16_KiB },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto driver             = Mock_Driver{};
            auto network_stack      = Mock_Network_Stack{};
            auto tcp_port_allocator = Mock_Port_Allocator{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack, false };

            auto const sn_tx_fsr_max = static_cast<std::uint16_t>(
                to_underlying( test_case.socket_buffer_size ) * 1024 );
            auto const data = random_container<std::vector<std::uint8_t>>(
                random<std::uint16_t>( 1, sn_tx_fsr_max ) );
            auto const sn_tx_fsr = random<std::uint16_t>( data.size(), sn_tx_fsr_max );
            auto const sn_tx_wr  = random<std::uint16_t>();

            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
            EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
            EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( sn_tx_fsr ) );
            EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( random<Mock_Error>() ) );
            EXPECT_CALL( driver, read_sn_tx_wr( socket_id ) ).WillOnce( Return( sn_tx_wr ) );
            EXPECT_CALL( driver, write_tx_buffer( socket_id, sn_tx_wr, data ) );
            EXPECT_CALL( driver, write_sn_tx_wr( socket_id, sn_tx_wr + data.size() ) );
            EXPECT_CALL( driver, write_sn_cr( socket_id, 0x20 ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

            auto const result = client.transmit( &*data.begin(), &*data.end() );

            EXPECT_TRUE( result.is_value() );
            EXPECT_EQ( result.value(), &*data.end() );

            EXPECT_EQ( client.state(), Client::State::CONNECTED );
            EXPECT_TRUE( client.is_transmitting() );

            EXPECT_CALL( driver, write_sn_cr( _, _ ) );
            EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
            EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
            EXPECT_CALL( driver, write_sn_ir( _, _ ) );
            EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
            EXPECT_CALL( driver, write_sn_port( _, _ ) );
            EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
            EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
            EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
            EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
            EXPECT_CALL( driver, write_sn_dport( _, _ ) );
            EXPECT_CALL( driver, write_sn_mr( _, _ ) );
            EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
            EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
            EXPECT_CALL( driver, write_sn_imr( _, _ ) );
            EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
            EXPECT_CALL( network_stack, deallocate_socket( _ ) );
        } // for
    }

    {
        struct {
            Socket_Buffer_Size socket_buffer_size;
        } const test_cases[]{
            // clang-format off

            { Socket_Buffer_Size::_2_KiB  },
            { Socket_Buffer_Size::_4_KiB  },
            { Socket_Buffer_Size::_8_KiB  },
            { Socket_Buffer_Size::_16_KiB },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto driver             = Mock_Driver{};
            auto network_stack      = Mock_Network_Stack{};
            auto tcp_port_allocator = Mock_Port_Allocator{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack, true };

            auto const sn_tx_fsr_max = static_cast<std::uint16_t>(
                to_underlying( test_case.socket_buffer_size ) * 1024 );
            auto const data = random_container<std::vector<std::uint8_t>>(
                random<std::uint16_t>( 1, sn_tx_fsr_max ) );
            auto const sn_tx_fsr = random<std::uint16_t>( data.size(), sn_tx_fsr_max );
            auto const sn_tx_wr  = random<std::uint16_t>();

            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
            EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( random<std::uint8_t>() | 0b000'1'0'0'0'0 ) );
            EXPECT_CALL( driver, write_sn_ir( socket_id, 0b000'1'0'0'0'0 ) );
            EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
            EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( sn_tx_fsr ) );
            EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( random<Mock_Error>() ) );
            EXPECT_CALL( driver, read_sn_tx_wr( socket_id ) ).WillOnce( Return( sn_tx_wr ) );
            EXPECT_CALL( driver, write_tx_buffer( socket_id, sn_tx_wr, data ) );
            EXPECT_CALL( driver, write_sn_tx_wr( socket_id, sn_tx_wr + data.size() ) );
            EXPECT_CALL( driver, write_sn_cr( socket_id, 0x20 ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

            auto const result = client.transmit( &*data.begin(), &*data.end() );

            EXPECT_TRUE( result.is_value() );
            EXPECT_EQ( result.value(), &*data.end() );

            EXPECT_EQ( client.state(), Client::State::CONNECTED );
            EXPECT_TRUE( client.is_transmitting() );

            EXPECT_CALL( driver, write_sn_cr( _, _ ) );
            EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
            EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
            EXPECT_CALL( driver, write_sn_ir( _, _ ) );
            EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
            EXPECT_CALL( driver, write_sn_port( _, _ ) );
            EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
            EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
            EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
            EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
            EXPECT_CALL( driver, write_sn_dport( _, _ ) );
            EXPECT_CALL( driver, write_sn_mr( _, _ ) );
            EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
            EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
            EXPECT_CALL( driver, write_sn_imr( _, _ ) );
            EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
            EXPECT_CALL( network_stack, deallocate_socket( _ ) );
        } // for
    }

    {
        struct {
            Socket_Buffer_Size socket_buffer_size;
        } const test_cases[]{
            // clang-format off

            { Socket_Buffer_Size::_2_KiB  },
            { Socket_Buffer_Size::_4_KiB  },
            { Socket_Buffer_Size::_8_KiB  },
            { Socket_Buffer_Size::_16_KiB },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto driver             = Mock_Driver{};
            auto network_stack      = Mock_Network_Stack{};
            auto tcp_port_allocator = Mock_Port_Allocator{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack, false };

            auto const sn_tx_fsr = random<std::uint16_t>(
                1, to_underlying( test_case.socket_buffer_size ) * 1024 );
            auto const data = random_container<std::vector<std::uint8_t>>(
                random<std::uint16_t>( sn_tx_fsr + 1 ) );
            auto const sn_tx_wr = random<std::uint16_t>();

            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
            EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
            EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( sn_tx_fsr ) );
            EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( random<Mock_Error>() ) );
            EXPECT_CALL( driver, read_sn_tx_wr( socket_id ) ).WillOnce( Return( sn_tx_wr ) );
            EXPECT_CALL(
                driver,
                write_tx_buffer(
                    socket_id, sn_tx_wr, std::vector<std::uint8_t>{ data.begin(), data.begin() + sn_tx_fsr } ) );
            EXPECT_CALL( driver, write_sn_tx_wr( socket_id, sn_tx_wr + sn_tx_fsr ) );
            EXPECT_CALL( driver, write_sn_cr( socket_id, 0x20 ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

            auto const result = client.transmit( &*data.begin(), &*data.end() );

            EXPECT_TRUE( result.is_value() );
            EXPECT_EQ( result.value(), &*data.begin() + sn_tx_fsr );

            EXPECT_EQ( client.state(), Client::State::CONNECTED );
            EXPECT_TRUE( client.is_transmitting() );

            EXPECT_CALL( driver, write_sn_cr( _, _ ) );
            EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
            EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
            EXPECT_CALL( driver, write_sn_ir( _, _ ) );
            EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
            EXPECT_CALL( driver, write_sn_port( _, _ ) );
            EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
            EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
            EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
            EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
            EXPECT_CALL( driver, write_sn_dport( _, _ ) );
            EXPECT_CALL( driver, write_sn_mr( _, _ ) );
            EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
            EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
            EXPECT_CALL( driver, write_sn_imr( _, _ ) );
            EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
            EXPECT_CALL( network_stack, deallocate_socket( _ ) );
        } // for
    }

    {
        struct {
            Socket_Buffer_Size socket_buffer_size;
        } const test_cases[]{
            // clang-format off

            { Socket_Buffer_Size::_2_KiB  },
            { Socket_Buffer_Size::_4_KiB  },
            { Socket_Buffer_Size::_8_KiB  },
            { Socket_Buffer_Size::_16_KiB },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto driver             = Mock_Driver{};
            auto network_stack      = Mock_Network_Stack{};
            auto tcp_port_allocator = Mock_Port_Allocator{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack, true };

            auto const sn_tx_fsr = random<std::uint16_t>(
                1, to_underlying( test_case.socket_buffer_size ) * 1024 );
            auto const data = random_container<std::vector<std::uint8_t>>(
                random<std::uint16_t>( sn_tx_fsr + 1 ) );
            auto const sn_tx_wr = random<std::uint16_t>();

            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
            EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( random<std::uint8_t>() | 0b000'1'0'0'0'0 ) );
            EXPECT_CALL( driver, write_sn_ir( socket_id, 0b000'1'0'0'0'0 ) );
            EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
            EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( sn_tx_fsr ) );
            EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( random<Mock_Error>() ) );
            EXPECT_CALL( driver, read_sn_tx_wr( socket_id ) ).WillOnce( Return( sn_tx_wr ) );
            EXPECT_CALL(
                driver,
                write_tx_buffer(
                    socket_id, sn_tx_wr, std::vector<std::uint8_t>{ data.begin(), data.begin() + sn_tx_fsr } ) );
            EXPECT_CALL( driver, write_sn_tx_wr( socket_id, sn_tx_wr + sn_tx_fsr ) );
            EXPECT_CALL( driver, write_sn_cr( socket_id, 0x20 ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

            auto const result = client.transmit( &*data.begin(), &*data.end() );

            EXPECT_TRUE( result.is_value() );
            EXPECT_EQ( result.value(), &*data.begin() + sn_tx_fsr );

            EXPECT_EQ( client.state(), Client::State::CONNECTED );
            EXPECT_TRUE( client.is_transmitting() );

            EXPECT_CALL( driver, write_sn_cr( _, _ ) );
            EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
            EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
            EXPECT_CALL( driver, write_sn_ir( _, _ ) );
            EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
            EXPECT_CALL( driver, write_sn_port( _, _ ) );
            EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
            EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
            EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
            EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
            EXPECT_CALL( driver, write_sn_dport( _, _ ) );
            EXPECT_CALL( driver, write_sn_mr( _, _ ) );
            EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
            EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
            EXPECT_CALL( driver, write_sn_imr( _, _ ) );
            EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
            EXPECT_CALL( network_stack, deallocate_socket( _ ) );
        } // for
    }
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
        { 0x1C },
        { 0x18 },
        { 0x1A },
        { 0x1B },
        { 0x1D },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver             = Mock_Driver{};
        auto network_stack      = Mock_Network_Stack{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto client = Client{ Client::State::CONNECTED, driver, random<Socket_ID>(), network_stack };

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );

        auto const result = client.transmit_keepalive();

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::NOT_CONNECTED );

        EXPECT_EQ( client.state(), Client::State::CONNECTED );

        EXPECT_CALL( driver, write_sn_cr( _, _ ) );
        EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, write_sn_ir( _, _ ) );
        EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
        EXPECT_CALL( driver, write_sn_port( _, _ ) );
        EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
        EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
        EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
        EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
        EXPECT_CALL( driver, write_sn_dport( _, _ ) );
        EXPECT_CALL( driver, write_sn_mr( _, _ ) );
        EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
        EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
        EXPECT_CALL( driver, write_sn_imr( _, _ ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
        EXPECT_CALL( network_stack, deallocate_socket( _ ) );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit_keepalive() works
 *        properly.
 */
TEST( transmitKeepalive, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver             = Mock_Driver{};
    auto network_stack      = Mock_Network_Stack{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const socket_id = random<Socket_ID>();

    auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack };

    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x22 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

    EXPECT_FALSE( client.transmit_keepalive().is_error() );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );

    EXPECT_CALL( driver, write_sn_cr( _, _ ) );
    EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, write_sn_ir( _, _ ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
    EXPECT_CALL( driver, write_sn_port( _, _ ) );
    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
    EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
    EXPECT_CALL( driver, write_sn_dport( _, _ ) );
    EXPECT_CALL( driver, write_sn_mr( _, _ ) );
    EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
    EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
    EXPECT_CALL( driver, write_sn_imr( _, _ ) );
    EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::available() works properly.
 */
TEST( available, worksProperly )
{
    struct {
        Socket_Buffer_Size socket_buffer_size;
    } const test_cases[]{
        // clang-format off

        { Socket_Buffer_Size::_2_KiB  },
        { Socket_Buffer_Size::_4_KiB  },
        { Socket_Buffer_Size::_8_KiB  },
        { Socket_Buffer_Size::_16_KiB },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_id = random<Socket_ID>();

        auto const client = Client{ driver, socket_id, network_stack };

        auto const sn_rx_rsr = random<std::uint16_t>(
            0, to_underlying( test_case.socket_buffer_size ) * 1024 );

        EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
        EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) ).WillOnce( Return( sn_rx_rsr ) );
        EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( random<Mock_Error>() ) );

        EXPECT_EQ( client.available(), sn_rx_rsr );

        EXPECT_CALL( driver, write_sn_mr( _, _ ) );
        EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
        EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
        EXPECT_CALL( driver, write_sn_imr( _, _ ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
        EXPECT_CALL( network_stack, deallocate_socket( _ ) );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() properly handles
 *        connection loss.
 */
TEST( receive, connectionLost )
{
    auto driver             = Mock_Driver{};
    auto network_stack      = Mock_Network_Stack{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTED, driver, random<Socket_ID>(), network_stack };

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1, 15 ) );
    auto const result = client.receive( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::NOT_CONNECTED );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );

    EXPECT_CALL( driver, write_sn_cr( _, _ ) );
    EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, write_sn_ir( _, _ ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
    EXPECT_CALL( driver, write_sn_port( _, _ ) );
    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
    EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
    EXPECT_CALL( driver, write_sn_dport( _, _ ) );
    EXPECT_CALL( driver, write_sn_mr( _, _ ) );
    EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
    EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
    EXPECT_CALL( driver, write_sn_imr( _, _ ) );
    EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() properly handles
 *        graceful shutdown.
 */
TEST( receive, gracefulShutdown )
{
    struct {
        std::uint8_t sn_sr;
    } const test_cases[]{
        // clang-format off

        { 0x18 },
        { 0x1A },
        { 0x1B },
        { 0x1D },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto driver             = Mock_Driver{};
        auto network_stack      = Mock_Network_Stack{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto client = Client{ Client::State::CONNECTED, driver, random<Socket_ID>(), network_stack };

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );

        auto       data = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 0, 15 ) );
        auto const result = client.receive( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

        EXPECT_EQ( client.state(), Client::State::CONNECTED );

        EXPECT_CALL( driver, write_sn_cr( _, _ ) );
        EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, write_sn_ir( _, _ ) );
        EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
        EXPECT_CALL( driver, write_sn_port( _, _ ) );
        EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
        EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
        EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
        EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
        EXPECT_CALL( driver, write_sn_dport( _, _ ) );
        EXPECT_CALL( driver, write_sn_mr( _, _ ) );
        EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
        EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
        EXPECT_CALL( driver, write_sn_imr( _, _ ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
        EXPECT_CALL( network_stack, deallocate_socket( _ ) );
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
        auto driver             = Mock_Driver{};
        auto network_stack      = Mock_Network_Stack{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto client = Client{ Client::State::CONNECTED, driver, random<Socket_ID>(), network_stack };

        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );
        EXPECT_CALL( network_stack, socket_buffer_size() )
            .WillOnce( Return(
                static_cast<Socket_Buffer_Size>( 1 << random<std::uint_fast8_t>( 1, 4 ) ) ) );
        EXPECT_CALL( driver, read_sn_rx_rsr( _ ) ).WillOnce( Return( 0 ) );
        EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( random<Mock_Error>() ) );

        auto       data = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 0, 15 ) );
        auto const result = client.receive( &*data.begin(), &*data.end() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), test_case.error );

        EXPECT_EQ( client.state(), Client::State::CONNECTED );

        EXPECT_CALL( driver, write_sn_cr( _, _ ) );
        EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, write_sn_ir( _, _ ) );
        EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
        EXPECT_CALL( driver, write_sn_port( _, _ ) );
        EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
        EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
        EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
        EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
        EXPECT_CALL( driver, write_sn_dport( _, _ ) );
        EXPECT_CALL( driver, write_sn_mr( _, _ ) );
        EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
        EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
        EXPECT_CALL( driver, write_sn_imr( _, _ ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
        EXPECT_CALL( network_stack, deallocate_socket( _ ) );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() works properly.
 */
TEST( receive, worksProperly )
{
    {
        struct {
            std::uint8_t       sn_sr;
            Socket_Buffer_Size socket_buffer_size;
        } const test_cases[]{
            // clang-format off

            { 0x17, Socket_Buffer_Size::_2_KiB  },
            { 0x17, Socket_Buffer_Size::_4_KiB  },
            { 0x17, Socket_Buffer_Size::_8_KiB  },
            { 0x17, Socket_Buffer_Size::_16_KiB },

            { 0x1C, Socket_Buffer_Size::_2_KiB  },
            { 0x1C, Socket_Buffer_Size::_4_KiB  },
            { 0x1C, Socket_Buffer_Size::_8_KiB  },
            { 0x1C, Socket_Buffer_Size::_16_KiB },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto driver             = Mock_Driver{};
            auto network_stack      = Mock_Network_Stack{};
            auto tcp_port_allocator = Mock_Port_Allocator{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack };

            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );
            EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
            EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) )
                .WillOnce( Return( random<std::uint16_t>(
                    1, to_underlying( test_case.socket_buffer_size ) * 1024 ) ) );
            EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( random<Mock_Error>() ) );

            auto       data   = std::vector<std::uint8_t>{};
            auto const result = client.receive( &*data.begin(), &*data.end() );

            EXPECT_TRUE( result.is_value() );
            EXPECT_EQ( result.value(), &*data.end() );

            EXPECT_EQ( client.state(), Client::State::CONNECTED );

            EXPECT_CALL( driver, write_sn_cr( _, _ ) );
            EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
            EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
            EXPECT_CALL( driver, write_sn_ir( _, _ ) );
            EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
            EXPECT_CALL( driver, write_sn_port( _, _ ) );
            EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
            EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
            EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
            EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
            EXPECT_CALL( driver, write_sn_dport( _, _ ) );
            EXPECT_CALL( driver, write_sn_mr( _, _ ) );
            EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
            EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
            EXPECT_CALL( driver, write_sn_imr( _, _ ) );
            EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
            EXPECT_CALL( network_stack, deallocate_socket( _ ) );
        } // for
    }

    {
        struct {
            std::uint8_t       sn_sr;
            Socket_Buffer_Size socket_buffer_size;
        } const test_cases[]{
            // clang-format off

            { 0x17, Socket_Buffer_Size::_2_KiB  },
            { 0x17, Socket_Buffer_Size::_4_KiB  },
            { 0x17, Socket_Buffer_Size::_8_KiB  },
            { 0x17, Socket_Buffer_Size::_16_KiB },

            { 0x1C, Socket_Buffer_Size::_2_KiB  },
            { 0x1C, Socket_Buffer_Size::_4_KiB  },
            { 0x1C, Socket_Buffer_Size::_8_KiB  },
            { 0x1C, Socket_Buffer_Size::_16_KiB },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto driver             = Mock_Driver{};
            auto network_stack      = Mock_Network_Stack{};
            auto tcp_port_allocator = Mock_Port_Allocator{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack };

            auto const sn_rx_rsr_max = static_cast<std::uint16_t>(
                to_underlying( test_case.socket_buffer_size ) * 1024 );
            auto data = std::vector<std::uint8_t>( random<std::uint16_t>( 1, sn_rx_rsr_max ) );
            auto const sn_rx_rsr = random<std::uint16_t>( data.size(), sn_rx_rsr_max );
            auto const sn_rx_rd  = random<std::uint16_t>();
            auto const data_expected = random_container<std::vector<std::uint8_t>>( data.size() );

            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );
            EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
            EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) ).WillOnce( Return( sn_rx_rsr ) );
            EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( random<Mock_Error>() ) );
            EXPECT_CALL( driver, read_sn_rx_rd( socket_id ) ).WillOnce( Return( sn_rx_rd ) );
            EXPECT_CALL( driver, read_rx_buffer( socket_id, sn_rx_rd, _ ) ).WillOnce( Return( data_expected ) );
            EXPECT_CALL( driver, write_sn_rx_rd( socket_id, sn_rx_rd + data.size() ) );
            EXPECT_CALL( driver, write_sn_cr( socket_id, 0x40 ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

            auto const result = client.receive( &*data.begin(), &*data.end() );

            EXPECT_TRUE( result.is_value() );
            EXPECT_EQ( result.value(), &*data.end() );

            EXPECT_EQ( data, data_expected );

            EXPECT_EQ( client.state(), Client::State::CONNECTED );

            EXPECT_CALL( driver, write_sn_cr( _, _ ) );
            EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
            EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
            EXPECT_CALL( driver, write_sn_ir( _, _ ) );
            EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
            EXPECT_CALL( driver, write_sn_port( _, _ ) );
            EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
            EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
            EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
            EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
            EXPECT_CALL( driver, write_sn_dport( _, _ ) );
            EXPECT_CALL( driver, write_sn_mr( _, _ ) );
            EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
            EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
            EXPECT_CALL( driver, write_sn_imr( _, _ ) );
            EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
            EXPECT_CALL( network_stack, deallocate_socket( _ ) );
        } // for
    }

    {
        struct {
            std::uint8_t       sn_sr;
            Socket_Buffer_Size socket_buffer_size;
        } const test_cases[]{
            // clang-format off

            { 0x17, Socket_Buffer_Size::_2_KiB  },
            { 0x17, Socket_Buffer_Size::_4_KiB  },
            { 0x17, Socket_Buffer_Size::_8_KiB  },
            { 0x17, Socket_Buffer_Size::_16_KiB },

            { 0x1C, Socket_Buffer_Size::_2_KiB  },
            { 0x1C, Socket_Buffer_Size::_4_KiB  },
            { 0x1C, Socket_Buffer_Size::_8_KiB  },
            { 0x1C, Socket_Buffer_Size::_16_KiB },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto driver             = Mock_Driver{};
            auto network_stack      = Mock_Network_Stack{};
            auto tcp_port_allocator = Mock_Port_Allocator{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack };

            auto const sn_rx_rsr = random<std::uint16_t>(
                1, to_underlying( test_case.socket_buffer_size ) * 1024 );
            auto const data_expected = random_container<std::vector<std::uint8_t>>( sn_rx_rsr );
            auto const sn_rx_rd = random<std::uint16_t>();

            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );
            EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
            EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) ).WillOnce( Return( sn_rx_rsr ) );
            EXPECT_CALL( network_stack, nonresponsive_device_error() ).WillOnce( Return( random<Mock_Error>() ) );
            EXPECT_CALL( driver, read_sn_rx_rd( socket_id ) ).WillOnce( Return( sn_rx_rd ) );
            EXPECT_CALL( driver, read_rx_buffer( socket_id, sn_rx_rd, _ ) ).WillOnce( Return( data_expected ) );
            EXPECT_CALL( driver, write_sn_rx_rd( socket_id, sn_rx_rd + sn_rx_rsr ) );
            EXPECT_CALL( driver, write_sn_cr( socket_id, 0x40 ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

            auto data = std::vector<std::uint8_t>( random<std::uint16_t>( sn_rx_rsr + 1 ) );
            auto const result = client.receive( &*data.begin(), &*data.end() );

            EXPECT_TRUE( result.is_value() );
            EXPECT_EQ( result.value(), &*data.begin() + sn_rx_rsr );

            EXPECT_EQ( ( std::vector<std::uint8_t>{ &*data.begin(), result.value() } ), data_expected );

            EXPECT_EQ( client.state(), Client::State::CONNECTED );

            EXPECT_CALL( driver, write_sn_cr( _, _ ) );
            EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
            EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
            EXPECT_CALL( driver, write_sn_ir( _, _ ) );
            EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
            EXPECT_CALL( driver, write_sn_port( _, _ ) );
            EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
            EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
            EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
            EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
            EXPECT_CALL( driver, write_sn_dport( _, _ ) );
            EXPECT_CALL( driver, write_sn_mr( _, _ ) );
            EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
            EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
            EXPECT_CALL( driver, write_sn_imr( _, _ ) );
            EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
            EXPECT_CALL( network_stack, deallocate_socket( _ ) );
        } // for
    }
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::shutdown() works properly.
 */
TEST( shutdown, worksProperly )
{
    // SOCK_CLOSED (0x00)
    {
        auto driver             = Mock_Driver{};
        auto network_stack      = Mock_Network_Stack{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack };

        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );

        client.shutdown();

        EXPECT_EQ( client.state(), Client::State::CONNECTED );

        EXPECT_CALL( driver, write_sn_cr( _, _ ) );
        EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, write_sn_ir( _, _ ) );
        EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
        EXPECT_CALL( driver, write_sn_port( _, _ ) );
        EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
        EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
        EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
        EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
        EXPECT_CALL( driver, write_sn_dport( _, _ ) );
        EXPECT_CALL( driver, write_sn_mr( _, _ ) );
        EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
        EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
        EXPECT_CALL( driver, write_sn_imr( _, _ ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
        EXPECT_CALL( network_stack, deallocate_socket( _ ) );
    }

    // SOCK_ESTABLISHED (0x17), SOCK_CLOSE_WAIT (0x1C)
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

            auto driver             = Mock_Driver{};
            auto network_stack      = Mock_Network_Stack{};
            auto tcp_port_allocator = Mock_Port_Allocator{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack };

            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );
            EXPECT_CALL( driver, write_sn_cr( socket_id, 0x08 ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

            client.shutdown();

            EXPECT_EQ( client.state(), Client::State::CONNECTED );

            EXPECT_CALL( driver, write_sn_cr( _, _ ) );
            EXPECT_CALL( driver, read_sn_cr( _ ) ).WillOnce( Return( 0x00 ) );
            EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );
            EXPECT_CALL( driver, write_sn_ir( _, _ ) );
            EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( random<std::uint16_t>() ) );
            EXPECT_CALL( driver, write_sn_port( _, _ ) );
            EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
            EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
            EXPECT_CALL( driver, write_sn_dhar( _, _ ) );
            EXPECT_CALL( driver, write_sn_dipr( _, _ ) );
            EXPECT_CALL( driver, write_sn_dport( _, _ ) );
            EXPECT_CALL( driver, write_sn_mr( _, _ ) );
            EXPECT_CALL( driver, write_sn_mssr( _, _ ) );
            EXPECT_CALL( driver, write_sn_ttl( _, _ ) );
            EXPECT_CALL( driver, write_sn_imr( _, _ ) );
            EXPECT_CALL( driver, write_sn_kpalvtr( _, _ ) );
            EXPECT_CALL( network_stack, deallocate_socket( _ ) );
        } // for
    }
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::close() works properly.
 */
TEST( close, worksProperly )
{
    {
        auto client = Client{};

        client.close();

        EXPECT_EQ( client.state(), Client::State::UNINITIALIZED );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver        = Mock_Driver{};
        auto network_stack = Mock_Network_Stack{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ driver, socket_id, network_stack };

        EXPECT_CALL( driver, write_sn_mr( socket_id, 0x00 ) );
        EXPECT_CALL( driver, write_sn_mssr( socket_id, 0x0000 ) );
        EXPECT_CALL( driver, write_sn_ttl( socket_id, 0x80 ) );
        EXPECT_CALL( driver, write_sn_imr( socket_id, 0xFF ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( socket_id, 0x00 ) );
        EXPECT_CALL( network_stack, deallocate_socket( socket_id ) );

        client.close();

        EXPECT_EQ( client.state(), Client::State::UNINITIALIZED );
    }

    {
        auto const in_sequence = InSequence{};

        auto driver             = Mock_Driver{};
        auto network_stack      = Mock_Network_Stack{};
        auto tcp_port_allocator = Mock_Port_Allocator{};

        auto const socket_id = random<Socket_ID>();

        auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack };

        auto const sn_port = random<std::uint16_t>();

        EXPECT_CALL( driver, write_sn_cr( socket_id, 0x10 ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, write_sn_ir( socket_id, 0b000'1'1'1'1'1 ) );
        EXPECT_CALL( driver, read_sn_port( socket_id ) ).WillOnce( Return( sn_port ) );
        EXPECT_CALL( driver, write_sn_port( socket_id, 0x0000 ) );
        EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
        EXPECT_CALL( tcp_port_allocator, deallocate( Port{ sn_port } ) );
        EXPECT_CALL(
            driver,
            write_sn_dhar( socket_id, Array<std::uint8_t, 6>{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } ) );
        EXPECT_CALL(
            driver, write_sn_dipr( socket_id, Array<std::uint8_t, 4>{ 0x00, 0x00, 0x00, 0x00 } ) );
        EXPECT_CALL( driver, write_sn_dport( socket_id, 0x0000 ) );
        EXPECT_CALL( driver, write_sn_mr( socket_id, 0x00 ) );
        EXPECT_CALL( driver, write_sn_mssr( socket_id, 0x0000 ) );
        EXPECT_CALL( driver, write_sn_ttl( socket_id, 0x80 ) );
        EXPECT_CALL( driver, write_sn_imr( socket_id, 0xFF ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( socket_id, 0x00 ) );
        EXPECT_CALL( network_stack, deallocate_socket( socket_id ) );

        client.close();

        EXPECT_EQ( client.state(), Client::State::UNINITIALIZED );
    }

    {
        struct {
            Client::State state;
            std::uint8_t  sn_sr;
        } const test_cases[]{
            // clang-format off

            { Client::State::BOUND,      0x13 },
            { Client::State::CONNECTING, 0x17 },
            { Client::State::CONNECTING, 0x1C },
            { Client::State::CONNECTING, 0x15 },
            { Client::State::CONNECTING, 0x18 },
            { Client::State::CONNECTING, 0x1A },
            { Client::State::CONNECTING, 0x1B },
            { Client::State::CONNECTING, 0x1D },
            { Client::State::CONNECTED,  0x17 },
            { Client::State::CONNECTED,  0x1C },
            { Client::State::CONNECTED,  0x15 },
            { Client::State::CONNECTED,  0x18 },
            { Client::State::CONNECTED,  0x1A },
            { Client::State::CONNECTED,  0x1B },
            { Client::State::CONNECTED,  0x1D },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto driver             = Mock_Driver{};
            auto network_stack      = Mock_Network_Stack{};
            auto tcp_port_allocator = Mock_Port_Allocator{};

            auto const socket_id = random<Socket_ID>();

            auto client = Client{ test_case.state, driver, socket_id, network_stack };

            auto const sn_port = random<std::uint16_t>();

            EXPECT_CALL( driver, write_sn_cr( socket_id, 0x10 ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( random<std::uint8_t>( 0x01 ) ) );
            EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );
            EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
            EXPECT_CALL( driver, write_sn_ir( socket_id, 0b000'1'1'1'1'1 ) );
            EXPECT_CALL( driver, read_sn_port( socket_id ) ).WillOnce( Return( sn_port ) );
            EXPECT_CALL( driver, write_sn_port( socket_id, 0x0000 ) );
            EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
            EXPECT_CALL( tcp_port_allocator, deallocate( Port{ sn_port } ) );
            EXPECT_CALL(
                driver,
                write_sn_dhar( socket_id, Array<std::uint8_t, 6>{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } ) );
            EXPECT_CALL(
                driver, write_sn_dipr( socket_id, Array<std::uint8_t, 4>{ 0x00, 0x00, 0x00, 0x00 } ) );
            EXPECT_CALL( driver, write_sn_dport( socket_id, 0x0000 ) );
            EXPECT_CALL( driver, write_sn_mr( socket_id, 0x00 ) );
            EXPECT_CALL( driver, write_sn_mssr( socket_id, 0x0000 ) );
            EXPECT_CALL( driver, write_sn_ttl( socket_id, 0x80 ) );
            EXPECT_CALL( driver, write_sn_imr( socket_id, 0xFF ) );
            EXPECT_CALL( driver, write_sn_kpalvtr( socket_id, 0x00 ) );
            EXPECT_CALL( network_stack, deallocate_socket( socket_id ) );

            client.close();

            EXPECT_EQ( client.state(), Client::State::UNINITIALIZED );
        } // for
    }
}

/**
 * \brief Execute the picolibrary::WIZnet::W5500::IP::TCP::Client automated tests.
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

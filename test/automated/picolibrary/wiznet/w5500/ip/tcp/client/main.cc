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

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/array.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/testing/automated/ip/tcp.h"
#include "picolibrary/testing/automated/ipv4.h"
#include "picolibrary/testing/automated/random.h"
#include "picolibrary/testing/automated/wiznet/w5500.h"
#include "picolibrary/testing/automated/wiznet/w5500/ip.h"
#include "picolibrary/testing/automated/wiznet/w5500/ip/network_stack.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/ip/tcp.h"

namespace {

using ::picolibrary::Array;
using ::picolibrary::IP::TCP::Port;
using ::picolibrary::IPv4::Address;
using ::picolibrary::Testing::Automated::random;
using ::picolibrary::Testing::Automated::WIZnet::W5500::Mock_Driver;
using ::picolibrary::Testing::Automated::WIZnet::W5500::IP::Mock_Network_Stack;
using ::picolibrary::Testing::Automated::WIZnet::W5500::IP::Mock_Port_Allocator;
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
        struct {
            Client::State state;
            std::uint8_t  sn_sr;
        } const test_cases[]{
            // clang-format off

            { Client::State::BOUND, 0x13 },

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

/**
 * picolibrary
 *
 * Copyright 2020-2023, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler automated test
 *        program.
 */

#include <bitset>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <limits>
#include <ostream>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/array.h"
#include "picolibrary/error.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/ip/tcp.h"
#include "picolibrary/testing/automated/wiznet/w5500.h"
#include "picolibrary/testing/automated/wiznet/w5500/ip.h"
#include "picolibrary/testing/automated/wiznet/w5500/ip/network_stack.h"
#include "picolibrary/testing/automated/wiznet/w5500/ip/tcp.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/ip/tcp.h"

namespace {

using ::picolibrary::Array;
using ::picolibrary::Generic_Error;
using ::picolibrary::IP::TCP::Port;
using ::picolibrary::Testing::Automated::WIZnet::W5500::Mock_Driver;
using ::picolibrary::Testing::Automated::WIZnet::W5500::IP::Mock_Network_Stack;
using ::picolibrary::Testing::Automated::WIZnet::W5500::IP::Mock_Port_Allocator;
using ::picolibrary::Testing::Automated::WIZnet::W5500::IP::TCP::Mock_Server;
using ::picolibrary::WIZnet::W5500::No_Delayed_ACK_Usage;
using ::picolibrary::WIZnet::W5500::Socket_Buffer_Size;
using ::picolibrary::WIZnet::W5500::Socket_ID;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::TestWithParam;
using ::testing::Values;
using ::testing::ValuesIn;

using Server_Connection_Handler =
    ::picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler<Mock_Network_Stack, Mock_Server>;

} // namespace

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::Server_Connection_Handler()
 *        works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const connection_handler = Server_Connection_Handler{};

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::UNINITIALIZED );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::~Server_Connection_Handler()
 *        works properly when the socket is in the
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::State::UNINITIALIZED
 *        state.
 */
TEST( destructor, worksProperlyUninitialized )
{
    auto const connection_handler = Server_Connection_Handler{};
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::~Server_Connection_Handler()
 *        works properly when the socket is in the
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::State::CONNECTED
 *        state and the socket has not been detached from the server socket it is
 *        associated with.
 */
TEST( destructor, worksProperlyConnectedNotDetached )
{
    auto const in_sequence = InSequence{};

    auto       network_stack = Mock_Network_Stack{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_1;

    auto const connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( socket_id ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, socket_id ) );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::~Server_Connection_Handler()
 *        works properly when the socket is in the
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::State::CONNECTED
 *        state, the socket has been detached from the server socket it is associated
 *        with, and the port is still in use.
 */
TEST( destructor, worksProperlyConnectedDetachedPortStillInUse )
{
    auto const in_sequence = InSequence{};

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_1;

    auto const connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    auto const sn_port = std::uint16_t{ 26387 };

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( socket_id ) ).WillOnce( Return( true ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( socket_id ) ).WillOnce( Return( sn_port ) );
    EXPECT_CALL( network_stack, sockets() ).WillOnce( Return( 8 ) );
    EXPECT_CALL( driver, read_sn_mr( Socket_ID::_0 ) ).WillOnce( Return( 0b1'1'0'0'0010 ) );
    EXPECT_CALL( driver, read_sn_mr( Socket_ID::_2 ) ).WillOnce( Return( 0b0'1'1'0'0001 ) );
    EXPECT_CALL( driver, read_sn_port( Socket_ID::_2 ) ).WillOnce( Return( 55818 ) );
    EXPECT_CALL( driver, read_sn_mr( Socket_ID::_3 ) ).WillOnce( Return( 0b0'1'0'0'0001 ) );
    EXPECT_CALL( driver, read_sn_port( Socket_ID::_3 ) ).WillOnce( Return( sn_port ) );
    EXPECT_CALL( driver, read_sn_mr( Socket_ID::_4 ) ).WillOnce( Return( 0b1'0'1'0'0111 ) );
    EXPECT_CALL( driver, read_sn_mr( Socket_ID::_5 ) ).WillOnce( Return( 0b0'1'0'0'0001 ) );
    EXPECT_CALL( driver, read_sn_port( Socket_ID::_5 ) ).WillOnce( Return( 2280 ) );
    EXPECT_CALL( driver, read_sn_mr( Socket_ID::_6 ) ).WillOnce( Return( 0b0'0'1'1'1100 ) );
    EXPECT_CALL( driver, read_sn_mr( Socket_ID::_7 ) ).WillOnce( Return( 0b0'1'1'1'0110 ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, socket_id ) );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::~Server_Connection_Handler()
 *        works properly when the socket is in the
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::State::CONNECTED
 *        state, the socket has been detached from the server socket it is associated
 *        with, and the port is not in use.
 */
TEST( destructor, worksProperlyConnectedDetachedPortNotInUse )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};
    auto       server             = Mock_Server{};
    auto const socket_id          = Socket_ID::_2;

    auto const connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    auto const sn_port = std::uint16_t{ 43787 };

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( socket_id ) ).WillOnce( Return( true ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( socket_id ) ).WillOnce( Return( sn_port ) );
    EXPECT_CALL( network_stack, sockets() ).WillOnce( Return( 4 ) );
    EXPECT_CALL( driver, read_sn_mr( Socket_ID::_0 ) ).WillOnce( Return( 0b0'0'1'1'0001 ) );
    EXPECT_CALL( driver, read_sn_port( Socket_ID::_0 ) ).WillOnce( Return( 20743 ) );
    EXPECT_CALL( driver, read_sn_mr( Socket_ID::_1 ) ).WillOnce( Return( 0b1'0'1'0'1101 ) );
    EXPECT_CALL( driver, read_sn_mr( Socket_ID::_3 ) ).WillOnce( Return( 0b0'0'0'0'0001 ) );
    EXPECT_CALL( driver, read_sn_port( Socket_ID::_3 ) ).WillOnce( Return( 13040 ) );
    EXPECT_CALL( network_stack, tcp_port_allocator( _ ) ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( Port{ sn_port } ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, socket_id ) );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::socket_id()
 *        works properly.
 */
TEST( socketId, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_1;

    auto const connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    EXPECT_EQ( connection_handler.socket_id(), socket_id );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::socket_interrupt_mask()
 *        test case.
 */
struct socketInterruptMask_Test_Case {
    /**
     * \brief The socket's hardware socket ID.
     */
    Socket_ID socket_id;

    /**
     * \brief The socket's socket interrupt mask.
     */
    std::uint8_t socket_interrupt_mask;
};

auto operator<<( std::ostream & stream, socketInterruptMask_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".socket_id = " << test_case.socket_id
                  << ", "
                  << ".socket_interrupt_mask = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.socket_interrupt_mask }
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::socket_interrupt_mask()
 *        test fixture.
 */
class socketInterruptMask : public TestWithParam<socketInterruptMask_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::socket_interrupt_mask()
 *        works properly.
 */
TEST_P( socketInterruptMask, worksProperly )
{
    auto const test_case = GetParam();

    auto network_stack = Mock_Network_Stack{};
    auto server        = Mock_Server{};

    auto const connection_handler = Server_Connection_Handler{ network_stack,
                                                               server,
                                                               test_case.socket_id };

    EXPECT_EQ( connection_handler.socket_interrupt_mask(), test_case.socket_interrupt_mask );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::socket_interrupt_mask()
 *        test cases.
 */
socketInterruptMask_Test_Case const socketInterruptMask_TEST_CASES[]{
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

INSTANTIATE_TEST_SUITE_P( testCases, socketInterruptMask, ValuesIn( socketInterruptMask_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::no_delayed_ack_usage_configuration()
 *        test case.
 */
struct noDelayedACKUsageConfiguration_Test_Case {
    /**
     * \brief The SN_MR register value.
     */
    std::uint8_t sn_mr;

    /**
     * \brief The no delayed ACK usage configuration.
     */
    No_Delayed_ACK_Usage no_delayed_ack_usage_configuration;
};

auto operator<<( std::ostream & stream, noDelayedACKUsageConfiguration_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".sn_mr = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.sn_mr }
                  << ", "
                  << ".no_delayed_ack_usage_configuration = " << test_case.no_delayed_ack_usage_configuration
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::no_delayed_ack_usage_configuration()
 *        test fixture.
 */
class noDelayedACKUsageConfiguration : public TestWithParam<noDelayedACKUsageConfiguration_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::no_delayed_ack_usage_configuration()
 *        works properly.
 */
TEST_P( noDelayedACKUsageConfiguration, worksProperly )
{
    auto const test_case = GetParam();

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_4;

    auto const connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_mr( socket_id ) ).WillOnce( Return( test_case.sn_mr ) );

    EXPECT_EQ( connection_handler.no_delayed_ack_usage_configuration(), test_case.no_delayed_ack_usage_configuration );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::no_delayed_ack_usage_configuration()
 *        test cases.
 */
noDelayedACKUsageConfiguration_Test_Case const noDelayedACKUsageConfiguration_TEST_CASES[]{
    // clang-format off

    { 0b0'1'0'1'1011, No_Delayed_ACK_Usage::DISABLED },
    { 0b0'1'1'1'1011, No_Delayed_ACK_Usage::ENABLED  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, noDelayedACKUsageConfiguration, ValuesIn( noDelayedACKUsageConfiguration_TEST_CASES ) );

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::maximum_segment_size()
 *        works properly.
 */
TEST( maximumSegmentSize, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_2;

    auto const connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    auto const sn_mssr = std::uint16_t{ 0x73D3 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_mssr( socket_id ) ).WillOnce( Return( sn_mssr ) );

    EXPECT_EQ( connection_handler.maximum_segment_size(), sn_mssr );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::time_to_live()
 *        works properly.
 */
TEST( timeToLive, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_0;

    auto const connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    auto const sn_ttl = std::uint8_t{ 0x88 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_ttl( socket_id ) ).WillOnce( Return( sn_ttl ) );

    EXPECT_EQ( connection_handler.time_to_live(), sn_ttl );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::keepalive_period()
 *        works properly.
 */
TEST( keepalivePeriod, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_0;

    auto const connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    auto const sn_kpalvtr = std::uint8_t{ 0x9C };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_kpalvtr( socket_id ) ).WillOnce( Return( sn_kpalvtr ) );

    EXPECT_EQ( connection_handler.keepalive_period(), sn_kpalvtr );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::enabled_interrupts()
 *        works properly.
 */
TEST( enabledInterrupts, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_1;

    auto const connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    auto const sn_imr = std::uint8_t{ 0b00100111 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_imr( socket_id ) ).WillOnce( Return( sn_imr ) );

    EXPECT_EQ( connection_handler.enabled_interrupts(), sn_imr );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::interrupt_context()
 *        works properly.
 */
TEST( interruptContext, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_5;

    auto const connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    auto const sn_ir = std::uint8_t{ 0b11010001 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( sn_ir ) );

    EXPECT_EQ( connection_handler.interrupt_context(), sn_ir );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::clear_interrupts()
 *        test case.
 */
struct clearInterrupts_Test_Case {
    /**
     * \brief Data transmission is in progress (initial).
     */
    bool is_transmitting_initial;

    /**
     * \brief The mask identifying the interrupts to clear.
     */
    std::uint8_t mask;

    /**
     * \brief Data transmission is in progress (final).
     */
    bool is_transmitting_final;
};

auto operator<<( std::ostream & stream, clearInterrupts_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".is_transmitting_initial = " << std::boolalpha << test_case.is_transmitting_initial
                  << ", "
                  << ".mask = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.mask }
                  << ", "
                  << ".is_transmitting_final = " << std::boolalpha << test_case.is_transmitting_final
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::clear_interrupts()
 *        test fixture.
 */
class clearInterrupts : public TestWithParam<clearInterrupts_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::clear_interrupts()
 *        works properly.
 */
TEST_P( clearInterrupts, worksProperly )
{
    auto const test_case = GetParam();

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_4;

    auto connection_handler = Server_Connection_Handler{
        network_stack, server, socket_id, test_case.is_transmitting_initial
    };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, write_sn_ir( socket_id, test_case.mask ) );

    connection_handler.clear_interrupts( test_case.mask );

    EXPECT_EQ( connection_handler.is_transmitting(), test_case.is_transmitting_final );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler_Connection_Handler::clear_interrupts()
 *        test cases.
 */
clearInterrupts_Test_Case const clearInterrupts_TEST_CASES[]{
    // clang-format off

    { false, 0b111'0'1'1'0'0, false },
    { false, 0b111'1'1'1'0'0, false },
    { true,  0b111'0'1'1'0'0, true  },
    { true,  0b111'1'1'1'0'0, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, clearInterrupts, ValuesIn( clearInterrupts_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::is_connected()
 *        test case.
 */
struct isConnected_Test_Case {
    /**
     * \brief The SN_SR register value.
     */
    std::uint8_t sn_sr;

    /**
     * \brief The socket is connected to a remote endpoint.
     */
    bool is_connected;
};

auto operator<<( std::ostream & stream, isConnected_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".sn_sr = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( test_case.sn_sr )
                  << ", "
                  << ".is_connected = " << std::boolalpha << test_case.is_connected
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::is_connected()
 *        test fixture.
 */
class isConnected : public TestWithParam<isConnected_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::is_connected()
 *        works properly.
 */
TEST_P( isConnected, worksProperly )
{
    auto const test_case = GetParam();

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_7;

    auto const connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );

    EXPECT_EQ( connection_handler.is_connected(), test_case.is_connected );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::is_connected()
 *        test cases.
 */
isConnected_Test_Case const isConnected_TEST_CASES[]{
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

INSTANTIATE_TEST_SUITE_P( testCases, isConnected, ValuesIn( isConnected_TEST_CASES ) );

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::remote_endpoint()
 *        works properly.
 */
TEST( remoteEndpoint, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_2;

    auto const connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    auto const sn_dipr  = Array<std::uint8_t, 4>{ 71, 135, 47, 193 };
    auto const sn_dport = std::uint16_t{ 12224 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_dipr( socket_id ) ).WillOnce( Return( sn_dipr ) );
    EXPECT_CALL( driver, read_sn_dport( socket_id ) ).WillOnce( Return( sn_dport ) );

    auto const endpoint = connection_handler.remote_endpoint();

    EXPECT_TRUE( endpoint.address().is_ipv4() );
    EXPECT_EQ( endpoint.address().ipv4().as_byte_array(), sn_dipr );
    EXPECT_EQ( endpoint.port().as_unsigned_integer(), sn_dport );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::local_endpoint()
 *        works properly.
 */
TEST( localEndpoint, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_2;

    auto const connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    auto const sipr    = Array<std::uint8_t, 4>{ 99, 105, 185, 107 };
    auto const sn_port = std::uint16_t{ 32306 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( sipr ) );
    EXPECT_CALL( driver, read_sn_port( socket_id ) ).WillOnce( Return( sn_port ) );

    auto const endpoint = connection_handler.local_endpoint();

    EXPECT_TRUE( endpoint.address().is_ipv4() );
    EXPECT_EQ( endpoint.address().ipv4().as_byte_array(), sipr );
    EXPECT_EQ( endpoint.port().as_unsigned_integer(), sn_port );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::outstanding()
 *        test case.
 */
struct outstanding_Test_Case {
    /**
     * \brief The socket buffer size.
     */
    Socket_Buffer_Size socket_buffer_size;

    /**
     * \brief The SN_TX_FSR register value.
     */
    std::uint16_t sn_tx_fsr;

    /**
     * \brief The amount of data that has yet to be transmitted to the remote endpoint.
     */
    std::uint16_t outstanding;
};

auto operator<<( std::ostream & stream, outstanding_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".socket_buffer_size = " << test_case.socket_buffer_size
                  << ", "
                  << ".sn_tx_fsr = " << test_case.sn_tx_fsr
                  << ", "
                  << ".outstanding = " << test_case.outstanding
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::outstanding()
 *        test fixture.
 */
class outstanding : public TestWithParam<outstanding_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::outstanding()
 *        works properly.
 */
TEST_P( outstanding, worksProperly )
{
    auto const test_case = GetParam();

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_7;

    auto const connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( test_case.sn_tx_fsr ) );

    EXPECT_EQ( connection_handler.outstanding(), test_case.outstanding );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::outstanding()
 *        test cases.
 */
outstanding_Test_Case const outstanding_TEST_CASES[]{
    // clang-format off

    { Socket_Buffer_Size::_2_KiB,    0, 2048 },
    { Socket_Buffer_Size::_2_KiB, 1061,  987 },
    { Socket_Buffer_Size::_2_KiB, 2048,    0 },

    { Socket_Buffer_Size::_4_KiB,    0, 4096 },
    { Socket_Buffer_Size::_4_KiB, 1514, 2582 },
    { Socket_Buffer_Size::_4_KiB, 4096,    0 },

    { Socket_Buffer_Size::_8_KiB,    0, 8192 },
    { Socket_Buffer_Size::_8_KiB,  600, 7592 },
    { Socket_Buffer_Size::_8_KiB, 8192,    0 },

    { Socket_Buffer_Size::_16_KiB,     0, 16384 },
    { Socket_Buffer_Size::_16_KiB,  9474,  6910 },
    { Socket_Buffer_Size::_16_KiB, 16384,     0 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, outstanding, ValuesIn( outstanding_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit()
 *        connection loss error handling test case.
 */
struct transmitErrorHandlingConnectionLoss_Test_Case {
    /**
     * \brief The SN_SR register value.
     */
    std::uint8_t sn_sr;

    /**
     * \brief Data transmission is in progress.
     */
    bool is_transmitting;
};

auto operator<<( std::ostream & stream, transmitErrorHandlingConnectionLoss_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".sn_sr = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( test_case.sn_sr )
                  << ", "
                  << ".is_transmitting = " << std::boolalpha << test_case.is_transmitting
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit()
 *        connection loss error handling test fixture.
 */
class transmitErrorHandlingConnectionLoss :
    public TestWithParam<transmitErrorHandlingConnectionLoss_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit()
 *        properly handles connection loss.
 */
TEST_P( transmitErrorHandlingConnectionLoss, connectionLoss )
{
    auto const test_case = GetParam();

    auto network_stack = Mock_Network_Stack{};
    auto driver        = Mock_Driver{};
    auto server        = Mock_Server{};

    auto connection_handler = Server_Connection_Handler{
        network_stack, server, Socket_ID::_5, test_case.is_transmitting
    };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );

    auto const data   = std::vector<std::uint8_t>{ 0x72, 0x5D };
    auto const result = connection_handler.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::NOT_CONNECTED );

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::CONNECTED );
    EXPECT_EQ( connection_handler.is_transmitting(), test_case.is_transmitting );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit()
 *        connection loss error handling test cases.
 */
transmitErrorHandlingConnectionLoss_Test_Case const transmitErrorHandlingConnectionLoss_TEST_CASES[]{
    // clang-format off

    { 0x00, false },
    { 0x00, true  },

    { 0x1C, false },
    { 0x1C, true  },

    { 0x18, false },
    { 0x18, true  },

    { 0x1A, false },
    { 0x1A, true  },

    { 0x1B, false },
    { 0x1B, true  },

    { 0x1D, false },
    { 0x1D, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, transmitErrorHandlingConnectionLoss, ValuesIn( transmitErrorHandlingConnectionLoss_TEST_CASES ) );

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit() works
 *        properly when an in progress transmission is not complete.
 */
TEST( transmit, worksProperlyInProgressTransmissionNotComplete )
{
    auto const in_sequence = InSequence{};

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_1;

    auto connection_handler = Server_Connection_Handler{ network_stack, server, socket_id, true };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
    EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( 0b001'0'1'1'0'1 ) );

    auto const data   = std::vector<std::uint8_t>{ 0x6D, 0x4A, 0xA0 };
    auto const result = connection_handler.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::CONNECTED );
    EXPECT_TRUE( connection_handler.is_transmitting() );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit() works
 *        properly when a transmission is not in progress and the data block is empty.
 */
TEST( transmit, worksProperlyTransmissionNotInProgressEmptyDataBlock )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_4;

    auto connection_handler = Server_Connection_Handler{ network_stack, server, socket_id, false };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );

    auto const data   = std::vector<std::uint8_t>{};
    auto const result = connection_handler.transmit( &*data.begin(), &*data.end() );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), &*data.end() );

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::CONNECTED );
    EXPECT_FALSE( connection_handler.is_transmitting() );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit() works
 *        properly when an in progress transmission is complete and the data block is
 *        empty.
 */
TEST( transmit, worksProperlyInProgressTransmissionCompleteEmptyDataBlock )
{
    auto const in_sequence = InSequence{};

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_4;

    auto connection_handler = Server_Connection_Handler{ network_stack, server, socket_id, true };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
    EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( 0b110'1'1'0'1'0 ) );
    EXPECT_CALL( driver, write_sn_ir( socket_id, 0b000'1'0'0'0'0 ) );

    auto const data   = std::vector<std::uint8_t>{};
    auto const result = connection_handler.transmit( &*data.begin(), &*data.end() );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), &*data.end() );

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::CONNECTED );
    EXPECT_FALSE( connection_handler.is_transmitting() );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit() works
 *        properly when a transmission is not in progress and the transmit buffer is full.
 */
TEST( transmit, worksProperlyTransmissionNotInProgressTransmitBufferFull )
{
    auto const in_sequence = InSequence{};

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_3;

    auto connection_handler = Server_Connection_Handler{ network_stack, server, socket_id, false };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( Socket_Buffer_Size::_8_KiB ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( 0 ) );

    auto const data   = std::vector<std::uint8_t>{ 0xCA, 0x67 };
    auto const result = connection_handler.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::CONNECTED );
    EXPECT_FALSE( connection_handler.is_transmitting() );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit() works
 *        properly when an in progress transmission is complete and the transmit buffer is
 *        full.
 */
TEST( transmit, worksProperlyInProgressTransmissionCompleteTransmitBufferFull )
{
    auto const in_sequence = InSequence{};

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_3;

    auto connection_handler = Server_Connection_Handler{ network_stack, server, socket_id, true };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
    EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( 0b011'1'1'0'1'1 ) );
    EXPECT_CALL( driver, write_sn_ir( socket_id, 0b000'1'0'0'0'0 ) );
    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( Socket_Buffer_Size::_8_KiB ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( 0 ) );

    auto const data   = std::vector<std::uint8_t>{ 0x68, 0x2B, 0x9E, 0x28 };
    auto const result = connection_handler.transmit( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::CONNECTED );
    EXPECT_FALSE( connection_handler.is_transmitting() );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit()
 *        transmit buffer capacity test case.
 */
struct transmitTransmitBufferCapacity_Test_Case {
    /**
     * \brief The socket buffer size.
     */
    Socket_Buffer_Size socket_buffer_size;

    /**
     * \brief The SN_TX_FSR register value.
     */
    std::uint16_t sn_tx_fsr;

    /**
     * \brief The SN_TX_WR register value (initial).
     */
    std::uint16_t sn_tx_wr_initial;

    /**
     * \brief The SN_TX_WR register value (final).
     */
    std::uint16_t sn_tx_wr_final;
};

auto operator<<( std::ostream & stream, transmitTransmitBufferCapacity_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".socket_buffer_size = " << test_case.socket_buffer_size
                  << ", "
                  << ".sn_tx_fsr = " << test_case.sn_tx_fsr
                  << ", "
                  << ".sn_tx_wr_initial = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint16_t>::digits / 4 ) << std::setfill( '0' ) << test_case.sn_tx_wr_initial
                  << ", "
                  << ".sn_tx_wr_final = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint16_t>::digits / 4 ) << std::setfill( '0' ) << test_case.sn_tx_wr_final
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit()
 *        sufficient transmit buffer capacity test fixture.
 */
class transmitSufficientTransmitBufferCapacity :
    public TestWithParam<transmitTransmitBufferCapacity_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit() works
 *        properly when a transmission is not in progress and there is sufficient transmit
 *        buffer capacity.
 */
TEST_P( transmitSufficientTransmitBufferCapacity, worksProperlyTransmissionNotInProgress )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_4;

    auto connection_handler = Server_Connection_Handler{ network_stack, server, socket_id, false };

    auto const data = std::vector<std::uint8_t>{ 0xA6, 0x94, 0x18, 0x2D };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( test_case.sn_tx_fsr ) );
    EXPECT_CALL( driver, read_sn_tx_wr( socket_id ) ).WillOnce( Return( test_case.sn_tx_wr_initial ) );
    EXPECT_CALL( driver, write_tx_buffer( socket_id, test_case.sn_tx_wr_initial, data ) );
    EXPECT_CALL( driver, write_sn_tx_wr( socket_id, test_case.sn_tx_wr_final ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x20 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x9A ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

    auto const result = connection_handler.transmit( &*data.begin(), &*data.end() );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), &*data.end() );

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::CONNECTED );
    EXPECT_TRUE( connection_handler.is_transmitting() );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit() works
 *        properly when an in progress transmission is complete and there is sufficient
 *        transmit buffer capacity.
 */
TEST_P( transmitSufficientTransmitBufferCapacity, worksProperlyInProgressTransmissionComplete )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_4;

    auto connection_handler = Server_Connection_Handler{ network_stack, server, socket_id, true };

    auto const data = std::vector<std::uint8_t>{ 0xA0, 0xA9, 0xC8, 0x3F };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
    EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( 0b010'1'0'0'1'0 ) );
    EXPECT_CALL( driver, write_sn_ir( socket_id, 0b000'1'0'0'0'0 ) );
    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( test_case.sn_tx_fsr ) );
    EXPECT_CALL( driver, read_sn_tx_wr( socket_id ) ).WillOnce( Return( test_case.sn_tx_wr_initial ) );
    EXPECT_CALL( driver, write_tx_buffer( socket_id, test_case.sn_tx_wr_initial, data ) );
    EXPECT_CALL( driver, write_sn_tx_wr( socket_id, test_case.sn_tx_wr_final ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x20 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x6F ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

    auto const result = connection_handler.transmit( &*data.begin(), &*data.end() );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), &*data.end() );

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::CONNECTED );
    EXPECT_TRUE( connection_handler.is_transmitting() );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit()
 *        sufficient transmit buffer capacity test cases.
 */
transmitTransmitBufferCapacity_Test_Case const transmitSufficientTransmitBufferCapacity_TEST_CASES[]{
    // clang-format off

    { Socket_Buffer_Size::_2_KiB,    4, 0x85FC, 0x8600 },
    { Socket_Buffer_Size::_2_KiB,    4, 0xFFFC, 0x0000 },
    { Socket_Buffer_Size::_2_KiB,  782, 0x4219, 0x421D },
    { Socket_Buffer_Size::_2_KiB,  782, 0xFFFE, 0x0002 },
    { Socket_Buffer_Size::_2_KiB, 2048, 0xC498, 0xC49C },
    { Socket_Buffer_Size::_2_KiB, 2048, 0xFFFF, 0x0003 },

    { Socket_Buffer_Size::_4_KiB,    4, 0xDF86, 0xDF8A },
    { Socket_Buffer_Size::_4_KiB,    4, 0xFFFD, 0x0001 },
    { Socket_Buffer_Size::_4_KiB,  882, 0x10BD, 0x10C1 },
    { Socket_Buffer_Size::_4_KiB,  882, 0xFFFF, 0x0003 },
    { Socket_Buffer_Size::_4_KiB, 4096, 0xB0ED, 0xB0F1 },
    { Socket_Buffer_Size::_4_KiB, 4096, 0xFFFD, 0x0001 },

    { Socket_Buffer_Size::_8_KiB,    4, 0x2A42, 0x2A46 },
    { Socket_Buffer_Size::_8_KiB,    4, 0xFFFD, 0x0001 },
    { Socket_Buffer_Size::_8_KiB, 7042, 0xD597, 0xD59B },
    { Socket_Buffer_Size::_8_KiB, 7042, 0xFFFF, 0x0003 },
    { Socket_Buffer_Size::_8_KiB, 8192, 0xDEF6, 0xDEFA },
    { Socket_Buffer_Size::_8_KiB, 8192, 0xFFFE, 0x0002 },

    { Socket_Buffer_Size::_16_KiB,     4, 0x3285, 0x3289 },
    { Socket_Buffer_Size::_16_KiB,     4, 0xFFFF, 0x0003 },
    { Socket_Buffer_Size::_16_KiB,  4710, 0xFFFE, 0x0002 },
    { Socket_Buffer_Size::_16_KiB,  4710, 0x1E05, 0x1E09 },
    { Socket_Buffer_Size::_16_KiB, 16384, 0x8B44, 0x8B48 },
    { Socket_Buffer_Size::_16_KiB, 16384, 0xFFFD, 0x0001 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, transmitSufficientTransmitBufferCapacity, ValuesIn( transmitSufficientTransmitBufferCapacity_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit()
 *        insufficient transmit buffer capacity test fixture.
 */
class transmitInsufficientTransmitBufferCapacity :
    public TestWithParam<transmitTransmitBufferCapacity_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit() works
 *        properly when a transmission is not in progress and there is insufficient
 *        transmit buffer capacity.
 */
TEST_P( transmitInsufficientTransmitBufferCapacity, worksProperlyTransmissionNotInProgress )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_4;

    auto connection_handler = Server_Connection_Handler{ network_stack, server, socket_id, false };

    auto const data = std::vector<std::uint8_t>{ 0x1C, 0x98, 0xAE, 0xBE };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( test_case.sn_tx_fsr ) );
    EXPECT_CALL( driver, read_sn_tx_wr( socket_id ) ).WillOnce( Return( test_case.sn_tx_wr_initial ) );
    EXPECT_CALL(
        driver,
        write_tx_buffer(
            socket_id,
            test_case.sn_tx_wr_initial,
            std::vector<std::uint8_t>{ data.begin(), data.begin() + test_case.sn_tx_fsr } ) );
    EXPECT_CALL( driver, write_sn_tx_wr( socket_id, test_case.sn_tx_wr_final ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x20 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0xB4 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

    auto const result = connection_handler.transmit( &*data.begin(), &*data.end() );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), &*data.begin() + test_case.sn_tx_fsr );

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::CONNECTED );
    EXPECT_TRUE( connection_handler.is_transmitting() );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit() works
 *        properly when an in progress transmission is complete and there is insufficient
 *        transmit buffer capacity.
 */
TEST_P( transmitInsufficientTransmitBufferCapacity, worksProperlyInProgressTransmissionComplete )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_4;

    auto connection_handler = Server_Connection_Handler{ network_stack, server, socket_id, true };

    auto const data = std::vector<std::uint8_t>{ 0x9A, 0x37, 0x71, 0xD9 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
    EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( 0b001'1'0'1'0'0 ) );
    EXPECT_CALL( driver, write_sn_ir( socket_id, 0b000'1'0'0'0'0 ) );
    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( test_case.sn_tx_fsr ) );
    EXPECT_CALL( driver, read_sn_tx_wr( socket_id ) ).WillOnce( Return( test_case.sn_tx_wr_initial ) );
    EXPECT_CALL(
        driver,
        write_tx_buffer(
            socket_id,
            test_case.sn_tx_wr_initial,
            std::vector<std::uint8_t>{ data.begin(), data.begin() + test_case.sn_tx_fsr } ) );
    EXPECT_CALL( driver, write_sn_tx_wr( socket_id, test_case.sn_tx_wr_final ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x20 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x1A ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

    auto const result = connection_handler.transmit( &*data.begin(), &*data.end() );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), &*data.begin() + test_case.sn_tx_fsr );

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::CONNECTED );
    EXPECT_TRUE( connection_handler.is_transmitting() );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit()
 *        insufficient transmit buffer capacity test cases.
 */
transmitTransmitBufferCapacity_Test_Case const transmitInsufficientTransmitBufferCapacity_TEST_CASES[]{
    // clang-format off

    { Socket_Buffer_Size::_2_KiB, 1, 0x9186, 0x9187 },
    { Socket_Buffer_Size::_2_KiB, 1, 0xFFFF, 0x0000 },
    { Socket_Buffer_Size::_2_KiB, 2, 0x6A34, 0x6A36 },
    { Socket_Buffer_Size::_2_KiB, 2, 0xFFFF, 0x0001 },
    { Socket_Buffer_Size::_2_KiB, 3, 0xC435, 0xC438 },
    { Socket_Buffer_Size::_2_KiB, 3, 0xFFFD, 0x0000 },

    { Socket_Buffer_Size::_4_KiB, 1, 0x8AE7, 0x8AE8 },
    { Socket_Buffer_Size::_4_KiB, 1, 0xFFFF, 0x0000 },
    { Socket_Buffer_Size::_4_KiB, 2, 0xFA4C, 0xFA4E },
    { Socket_Buffer_Size::_4_KiB, 2, 0xFFFF, 0x0001 },
    { Socket_Buffer_Size::_4_KiB, 3, 0xF2F2, 0xF2F5 },
    { Socket_Buffer_Size::_4_KiB, 3, 0xFFFD, 0x0000 },

    { Socket_Buffer_Size::_8_KiB, 1, 0xED2E, 0xED2F },
    { Socket_Buffer_Size::_8_KiB, 1, 0xFFFF, 0x0000 },
    { Socket_Buffer_Size::_8_KiB, 2, 0x18A4, 0x18A6 },
    { Socket_Buffer_Size::_8_KiB, 2, 0xFFFE, 0x0000 },
    { Socket_Buffer_Size::_8_KiB, 3, 0x1798, 0x179B },
    { Socket_Buffer_Size::_8_KiB, 3, 0xFFFE, 0x0001 },

    { Socket_Buffer_Size::_16_KiB, 1, 0x4B4E, 0x4B4F },
    { Socket_Buffer_Size::_16_KiB, 1, 0xFFFF, 0x0000 },
    { Socket_Buffer_Size::_16_KiB, 2, 0xB716, 0xB718 },
    { Socket_Buffer_Size::_16_KiB, 2, 0xFFFE, 0x0000 },
    { Socket_Buffer_Size::_16_KiB, 3, 0x7543, 0x7546 },
    { Socket_Buffer_Size::_16_KiB, 3, 0xFFFE, 0x0001 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, transmitInsufficientTransmitBufferCapacity, ValuesIn( transmitInsufficientTransmitBufferCapacity_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit_keepalive()
 *        connection loss error handling test fixture.
 */
class transmitKeepaliveErrorHandlingConnectionLoss : public TestWithParam<std::uint8_t> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit_keepalive()
 *        properly handles connection loss.
 */
TEST_P( transmitKeepaliveErrorHandlingConnectionLoss, connectionLoss )
{
    auto network_stack = Mock_Network_Stack{};
    auto driver        = Mock_Driver{};
    auto server        = Mock_Server{};

    auto connection_handler = Server_Connection_Handler{ network_stack, server, Socket_ID::_1 };

    auto const sn_sr = GetParam();

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( sn_sr ) );

    auto const result = connection_handler.transmit_keepalive();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::NOT_CONNECTED );

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::CONNECTED );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

INSTANTIATE_TEST_SUITE_P(
    testCases,
    transmitKeepaliveErrorHandlingConnectionLoss,
    Values<std::uint8_t>( 0x00, 0x1C, 0x18, 0x1A, 0x1B, 0x1D ) );

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit_keepalive()
 *        works properly.
 */
TEST( transmitKeepalive, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_2;

    auto connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x22 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x39 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

    EXPECT_FALSE( connection_handler.transmit_keepalive().is_error() );

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::CONNECTED );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::available() test
 *        case.
 */
struct available_Test_Case {
    /**
     * \brief The socket buffer size.
     */
    Socket_Buffer_Size socket_buffer_size;

    /**
     * \brief The SN_RX_RSR register value.
     */
    std::uint16_t sn_rx_rsr;
};

auto operator<<( std::ostream & stream, available_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".socket_buffer_size = " << test_case.socket_buffer_size
                  << ", "
                  << ".sn_rx_rsr = " << test_case.sn_rx_rsr
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::available() test
 *        fixture.
 */
class available : public TestWithParam<available_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::available()
 *        works properly.
 */
TEST_P( available, worksProperly )
{
    auto const test_case = GetParam();

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_4;

    auto const connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) ).WillOnce( Return( test_case.sn_rx_rsr ) );

    EXPECT_EQ( connection_handler.available(), test_case.sn_rx_rsr );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::available() test
 *        cases.
 */
available_Test_Case const available_TEST_CASES[]{
    // clang-format off

    { Socket_Buffer_Size::_2_KiB,    0 },
    { Socket_Buffer_Size::_2_KiB,  138 },
    { Socket_Buffer_Size::_2_KiB, 2048 },

    { Socket_Buffer_Size::_4_KiB,    0 },
    { Socket_Buffer_Size::_4_KiB, 2165 },
    { Socket_Buffer_Size::_4_KiB, 4096 },

    { Socket_Buffer_Size::_8_KiB,    0 },
    { Socket_Buffer_Size::_8_KiB, 6596 },
    { Socket_Buffer_Size::_8_KiB, 8192 },

    { Socket_Buffer_Size::_16_KiB,     0 },
    { Socket_Buffer_Size::_16_KiB, 11323 },
    { Socket_Buffer_Size::_16_KiB, 16384 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, available, ValuesIn( available_TEST_CASES ) );

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::receive()
 *        properly handles connection loss.
 */
TEST( receiveErrorHandling, connectionLoss )
{
    auto network_stack = Mock_Network_Stack{};
    auto driver        = Mock_Driver{};
    auto server        = Mock_Server{};

    auto connection_handler = Server_Connection_Handler{ network_stack, server, Socket_ID::_7 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );

    auto       data   = std::vector<std::uint8_t>( 3 );
    auto const result = connection_handler.receive( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::NOT_CONNECTED );

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::CONNECTED );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::receive()
 *        graceful shutdown test fixture.
 */
class receiveGracefulShutdown : public TestWithParam<std::uint8_t> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::receive()
 *        works properly during graceful shutdown.
 */
TEST_P( receiveGracefulShutdown, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_6;

    auto connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    auto const sn_sr = GetParam();

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( sn_sr ) );

    auto       data   = std::vector<std::uint8_t>( 2 );
    auto const result = connection_handler.receive( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::CONNECTED );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

INSTANTIATE_TEST_SUITE_P( testCases, receiveGracefulShutdown, Values<std::uint8_t>( 0x18, 0x1A, 0x1B, 0x1D ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::receive()
 *        receive buffer empty test
 *        case.
 */
struct receiveReceiveBufferEmpty_Test_Case {
    /**
     * \brief The SN_SR register value.
     */
    std::uint8_t sn_sr;

    /**
     * \brief The reported error.
     */
    Generic_Error error;
};

auto operator<<( std::ostream & stream, receiveReceiveBufferEmpty_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".sn_sr = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( test_case.sn_sr )
                  << ", "
                  << ".error = " << test_case.error
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::receive()
 *        receive buffer empty test fixture.
 */
class receiveReceiveBufferEmpty : public TestWithParam<receiveReceiveBufferEmpty_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::receive()
 *        works properly when the receive buffer is empty.
 */
TEST_P( receiveReceiveBufferEmpty, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_1;

    auto connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );
    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( Socket_Buffer_Size::_8_KiB ) );
    EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) ).WillOnce( Return( 0 ) );

    auto       data   = std::vector<std::uint8_t>( 5 );
    auto const result = connection_handler.receive( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), test_case.error );

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::CONNECTED );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::receive()
 *        receive buffer empty test cases.
 */
receiveReceiveBufferEmpty_Test_Case const receiveReceiveBufferEmpty_TEST_CASES[]{
    // clang-format off

    { 0x17, Generic_Error::WOULD_BLOCK   },
    { 0x1C, Generic_Error::NOT_CONNECTED },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, receiveReceiveBufferEmpty, ValuesIn( receiveReceiveBufferEmpty_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::receive() empty
 *        data block test case.
 */
struct receiveEmptyDataBlock_Test_Case {
    /**
     * \brief The SN_SR register value.
     */
    std::uint8_t sn_sr;

    /**
     * \brief The socket buffer size.
     */
    Socket_Buffer_Size socket_buffer_size;

    /**
     * \brief The SN_RX_RSR register value.
     */
    std::uint16_t sn_rx_rsr;
};

auto operator<<( std::ostream & stream, receiveEmptyDataBlock_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".sn_sr = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( test_case.sn_sr )
                  << ", "
                  << ".socket_buffer_size = " << test_case.socket_buffer_size
                  << ", "
                  << ".sn_rx_rsr = " << test_case.sn_rx_rsr
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::receive() empty
 *        data block test fixture.
 */
class receiveEmptyDataBlock : public TestWithParam<receiveEmptyDataBlock_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::receive()
 *        works properly when the data block is empty.
 */
TEST_P( receiveEmptyDataBlock, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_5;

    auto connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );
    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
    EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) ).WillOnce( Return( test_case.sn_rx_rsr ) );

    auto       data   = std::vector<std::uint8_t>{};
    auto const result = connection_handler.receive( &*data.begin(), &*data.end() );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), &*data.end() );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::receive() empty
 *        data block test cases.
 */
receiveEmptyDataBlock_Test_Case const receiveEmptyDataBlock_TEST_CASES[]{
    // clang-format off

    { 0x17, Socket_Buffer_Size::_2_KiB,    1 },
    { 0x17, Socket_Buffer_Size::_2_KiB,  773 },
    { 0x17, Socket_Buffer_Size::_2_KiB, 2048 },

    { 0x17, Socket_Buffer_Size::_4_KiB,    1 },
    { 0x17, Socket_Buffer_Size::_4_KiB, 2537 },
    { 0x17, Socket_Buffer_Size::_4_KiB, 4096 },

    { 0x17, Socket_Buffer_Size::_8_KiB,    1 },
    { 0x17, Socket_Buffer_Size::_8_KiB, 7045 },
    { 0x17, Socket_Buffer_Size::_8_KiB, 8192 },

    { 0x17, Socket_Buffer_Size::_16_KiB,     1 },
    { 0x17, Socket_Buffer_Size::_16_KiB,  7652 },
    { 0x17, Socket_Buffer_Size::_16_KiB, 16384 },

    { 0x1C, Socket_Buffer_Size::_2_KiB,    1 },
    { 0x1C, Socket_Buffer_Size::_2_KiB, 1084 },
    { 0x1C, Socket_Buffer_Size::_2_KiB, 2048 },

    { 0x1C, Socket_Buffer_Size::_4_KiB,    1 },
    { 0x1C, Socket_Buffer_Size::_4_KiB, 4074 },
    { 0x1C, Socket_Buffer_Size::_4_KiB, 4096 },

    { 0x1C, Socket_Buffer_Size::_8_KiB,    1 },
    { 0x1C, Socket_Buffer_Size::_8_KiB, 6486 },
    { 0x1C, Socket_Buffer_Size::_8_KiB, 8192 },

    { 0x1C, Socket_Buffer_Size::_16_KiB,     1 },
    { 0x1C, Socket_Buffer_Size::_16_KiB, 15600 },
    { 0x1C, Socket_Buffer_Size::_16_KiB, 16384 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, receiveEmptyDataBlock, ValuesIn( receiveEmptyDataBlock_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::receive()
 *        receive data test case.
 */
struct receiveData_Test_Case {
    /**
     * \brief The SN_SR register value.
     */
    std::uint8_t sn_sr;

    /**
     * \brief The socket buffer size.
     */
    Socket_Buffer_Size socket_buffer_size;

    /**
     * \brief The SN_RX_RSR register value.
     */
    std::uint16_t sn_rx_rsr;

    /**
     * \brief The SN_RX_RD register value (initial).
     */
    std::uint16_t sn_rx_rd_initial;

    /**
     * \brief The SN_RX_RD register value (final).
     */
    std::uint16_t sn_rx_rd_final;
};

auto operator<<( std::ostream & stream, receiveData_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".sn_sr = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( test_case.sn_sr )
                  << ", "
                  << ".socket_buffer_size = " << test_case.socket_buffer_size
                  << ", "
                  << ".sn_rx_rsr = " << test_case.sn_rx_rsr
                  << ", "
                  << ".sn_rx_rd_initial = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint16_t>::digits / 4 ) << std::setfill( '0' ) << test_case.sn_rx_rd_initial
                  << ", "
                  << ".sn_rx_rd_final = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint16_t>::digits / 4 ) << std::setfill( '0' ) << test_case.sn_rx_rd_final
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::receive()
 *        receive all data test fixture.
 */
class receiveAllData : public TestWithParam<receiveData_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::receive()
 *        works properly when all available data can be received.
 */
TEST_P( receiveAllData, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_0;

    auto connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    auto const data_expected = std::vector<std::uint8_t>{ 0x03, 0xA2, 0xAD };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );
    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
    EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) ).WillOnce( Return( test_case.sn_rx_rsr ) );
    EXPECT_CALL( driver, read_sn_rx_rd( socket_id ) ).WillOnce( Return( test_case.sn_rx_rd_initial ) );
    EXPECT_CALL( driver, read_rx_buffer( socket_id, test_case.sn_rx_rd_initial, _ ) )
        .WillOnce( Return( std::vector<std::uint8_t>{
            data_expected.begin(), data_expected.begin() + test_case.sn_rx_rsr } ) );
    EXPECT_CALL( driver, write_sn_rx_rd( socket_id, test_case.sn_rx_rd_final ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x40 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x60 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

    auto       data   = std::vector<std::uint8_t>( 4 );
    auto const result = connection_handler.receive( &*data.begin(), &*data.end() );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), &*data.begin() + test_case.sn_rx_rsr );

    EXPECT_EQ(
        ( std::vector<std::uint8_t>{ &*data.begin(), result.value() } ),
        ( std::vector<std::uint8_t>{ data_expected.begin(),
                                     data_expected.begin() + test_case.sn_rx_rsr } ) );

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::CONNECTED );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::receive()
 *        receive all data test cases.
 */
receiveData_Test_Case const receiveAllData_TEST_CASES[]{
    // clang-format off

    { 0x17, Socket_Buffer_Size::_2_KiB, 1, 0xCDF7, 0xCDF8 },
    { 0x17, Socket_Buffer_Size::_2_KiB, 1, 0xFFFF, 0x0000 },
    { 0x17, Socket_Buffer_Size::_2_KiB, 2, 0x3CF9, 0x3CFB },
    { 0x17, Socket_Buffer_Size::_2_KiB, 2, 0xFFFF, 0x0001 },
    { 0x17, Socket_Buffer_Size::_2_KiB, 3, 0x00F8, 0x00FB },
    { 0x17, Socket_Buffer_Size::_2_KiB, 3, 0xFFFD, 0x0000 },

    { 0x17, Socket_Buffer_Size::_4_KiB, 1, 0x8E06, 0x8E07 },
    { 0x17, Socket_Buffer_Size::_4_KiB, 1, 0xFFFF, 0x0000 },
    { 0x17, Socket_Buffer_Size::_4_KiB, 2, 0xA97E, 0xA980 },
    { 0x17, Socket_Buffer_Size::_4_KiB, 2, 0xFFFF, 0x0001 },
    { 0x17, Socket_Buffer_Size::_4_KiB, 3, 0x8EEE, 0x8EF1 },
    { 0x17, Socket_Buffer_Size::_4_KiB, 3, 0xFFFD, 0x0000 },

    { 0x17, Socket_Buffer_Size::_8_KiB, 1, 0xCB07, 0xCB08 },
    { 0x17, Socket_Buffer_Size::_8_KiB, 1, 0xFFFF, 0x0000 },
    { 0x17, Socket_Buffer_Size::_8_KiB, 2, 0x6E05, 0x6E07 },
    { 0x17, Socket_Buffer_Size::_8_KiB, 2, 0xFFFE, 0x0000 },
    { 0x17, Socket_Buffer_Size::_8_KiB, 3, 0x1D9D, 0x1DA0 },
    { 0x17, Socket_Buffer_Size::_8_KiB, 3, 0xFFFE, 0x0001 },

    { 0x17, Socket_Buffer_Size::_16_KiB, 1, 0xA48E, 0xA48F },
    { 0x17, Socket_Buffer_Size::_16_KiB, 1, 0xFFFF, 0x0000 },
    { 0x17, Socket_Buffer_Size::_16_KiB, 2, 0x13D1, 0x13D3 },
    { 0x17, Socket_Buffer_Size::_16_KiB, 2, 0xFFFE, 0x0000 },
    { 0x17, Socket_Buffer_Size::_16_KiB, 3, 0xE58A, 0xE58D },
    { 0x17, Socket_Buffer_Size::_16_KiB, 3, 0xFFFE, 0x0001 },

    { 0x1C, Socket_Buffer_Size::_2_KiB, 1, 0x83B1, 0x83B2 },
    { 0x1C, Socket_Buffer_Size::_2_KiB, 1, 0xFFFF, 0x0000 },
    { 0x1C, Socket_Buffer_Size::_2_KiB, 2, 0xDBDE, 0xDBE0 },
    { 0x1C, Socket_Buffer_Size::_2_KiB, 2, 0xFFFF, 0x0001 },
    { 0x1C, Socket_Buffer_Size::_2_KiB, 3, 0xF4C5, 0xF4C8 },
    { 0x1C, Socket_Buffer_Size::_2_KiB, 3, 0xFFFD, 0x0000 },

    { 0x1C, Socket_Buffer_Size::_4_KiB, 1, 0x10B7, 0x10B8 },
    { 0x1C, Socket_Buffer_Size::_4_KiB, 1, 0xFFFF, 0x0000 },
    { 0x1C, Socket_Buffer_Size::_4_KiB, 2, 0xFF3F, 0xFF41 },
    { 0x1C, Socket_Buffer_Size::_4_KiB, 2, 0xFFFF, 0x0001 },
    { 0x1C, Socket_Buffer_Size::_4_KiB, 3, 0x8B40, 0x8B43 },
    { 0x1C, Socket_Buffer_Size::_4_KiB, 3, 0xFFFD, 0x0000 },

    { 0x1C, Socket_Buffer_Size::_8_KiB, 1, 0x073E, 0x073F },
    { 0x1C, Socket_Buffer_Size::_8_KiB, 1, 0xFFFF, 0x0000 },
    { 0x1C, Socket_Buffer_Size::_8_KiB, 2, 0x346E, 0x3470 },
    { 0x1C, Socket_Buffer_Size::_8_KiB, 2, 0xFFFE, 0x0000 },
    { 0x1C, Socket_Buffer_Size::_8_KiB, 3, 0x3924, 0x3927 },
    { 0x1C, Socket_Buffer_Size::_8_KiB, 3, 0xFFFE, 0x0001 },

    { 0x1C, Socket_Buffer_Size::_16_KiB, 1, 0xA81A, 0xA81B },
    { 0x1C, Socket_Buffer_Size::_16_KiB, 1, 0xFFFF, 0x0000 },
    { 0x1C, Socket_Buffer_Size::_16_KiB, 2, 0xB774, 0xB776 },
    { 0x1C, Socket_Buffer_Size::_16_KiB, 2, 0xFFFE, 0x0000 },
    { 0x1C, Socket_Buffer_Size::_16_KiB, 3, 0xF4EF, 0xF4F2 },
    { 0x1C, Socket_Buffer_Size::_16_KiB, 3, 0xFFFE, 0x0001 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, receiveAllData, ValuesIn( receiveAllData_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::receive()
 *        receive some data test fixture.
 */
class receiveSomeData : public TestWithParam<receiveData_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::receive()
 *        works properly when only some of the available data can be received.
 */
TEST_P( receiveSomeData, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_1;

    auto connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    auto const data_expected = std::vector<std::uint8_t>{ 0x4B, 0x9F, 0x62, 0xB8 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );
    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
    EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) ).WillOnce( Return( test_case.sn_rx_rsr ) );
    EXPECT_CALL( driver, read_sn_rx_rd( socket_id ) ).WillOnce( Return( test_case.sn_rx_rd_initial ) );
    EXPECT_CALL( driver, read_rx_buffer( socket_id, test_case.sn_rx_rd_initial, _ ) ).WillOnce( Return( data_expected ) );
    EXPECT_CALL( driver, write_sn_rx_rd( socket_id, test_case.sn_rx_rd_final ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x40 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0xC9 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

    auto       data   = std::vector<std::uint8_t>( data_expected.size() );
    auto const result = connection_handler.receive( &*data.begin(), &*data.end() );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), &*data.end() );

    EXPECT_EQ( data, data_expected );

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::CONNECTED );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::receive()
 *        receive some data test cases.
 */
receiveData_Test_Case const receiveSomeData_TEST_CASES[]{
    // clang-format off

    { 0x17, Socket_Buffer_Size::_2_KiB,    5, 0x67F2, 0x67F6 },
    { 0x17, Socket_Buffer_Size::_2_KiB,    5, 0xFFFF, 0x0003 },
    { 0x17, Socket_Buffer_Size::_2_KiB,  613, 0x282D, 0x2831 },
    { 0x17, Socket_Buffer_Size::_2_KiB,  613, 0xFFFF, 0x0003 },
    { 0x17, Socket_Buffer_Size::_2_KiB, 2048, 0x27E5, 0x27E9 },
    { 0x17, Socket_Buffer_Size::_2_KiB, 2048, 0xFFFE, 0x0002 },

    { 0x17, Socket_Buffer_Size::_4_KiB,    5, 0x3FCE, 0x3FD2 },
    { 0x17, Socket_Buffer_Size::_4_KiB,    5, 0xFFFD, 0x0001 },
    { 0x17, Socket_Buffer_Size::_4_KiB, 3160, 0xD952, 0xD956 },
    { 0x17, Socket_Buffer_Size::_4_KiB, 3160, 0xFFFF, 0x0003 },
    { 0x17, Socket_Buffer_Size::_4_KiB, 4096, 0x3D64, 0x3D68 },
    { 0x17, Socket_Buffer_Size::_4_KiB, 4096, 0xFFFF, 0x0003 },

    { 0x17, Socket_Buffer_Size::_8_KiB,    5, 0xC72F, 0xc733 },
    { 0x17, Socket_Buffer_Size::_8_KiB,    5, 0xFFFF, 0x0003 },
    { 0x17, Socket_Buffer_Size::_8_KiB,  251, 0x2CD5, 0x2CD9 },
    { 0x17, Socket_Buffer_Size::_8_KiB,  251, 0xFFFD, 0x0001 },
    { 0x17, Socket_Buffer_Size::_8_KiB, 8192, 0x97B3, 0x97B7 },
    { 0x17, Socket_Buffer_Size::_8_KiB, 8192, 0xFFFE, 0x0002 },

    { 0x17, Socket_Buffer_Size::_16_KiB,     5, 0xA57E, 0xA582 },
    { 0x17, Socket_Buffer_Size::_16_KiB,     5, 0xFFFD, 0x0001 },
    { 0x17, Socket_Buffer_Size::_16_KiB, 14378, 0x14B5, 0x14B9 },
    { 0x17, Socket_Buffer_Size::_16_KiB, 14378, 0xFFFC, 0x0000 },
    { 0x17, Socket_Buffer_Size::_16_KiB, 16384, 0x4133, 0x4137 },
    { 0x17, Socket_Buffer_Size::_16_KiB, 16384, 0xFFFF, 0x0003 },

    { 0x1C, Socket_Buffer_Size::_2_KiB,    5, 0xD07D, 0xD081 },
    { 0x1C, Socket_Buffer_Size::_2_KiB,    5, 0xFFFF, 0x0003 },
    { 0x1C, Socket_Buffer_Size::_2_KiB,  405, 0xFA91, 0xFA95 },
    { 0x1C, Socket_Buffer_Size::_2_KiB,  405, 0xFFFF, 0x0003 },
    { 0x1C, Socket_Buffer_Size::_2_KiB, 2048, 0x1409, 0x140D },
    { 0x1C, Socket_Buffer_Size::_2_KiB, 2048, 0xFFFC, 0x0000 },

    { 0x1C, Socket_Buffer_Size::_4_KiB,    5, 0x9709, 0x970D },
    { 0x1C, Socket_Buffer_Size::_4_KiB,    5, 0xFFFD, 0x0001 },
    { 0x1C, Socket_Buffer_Size::_4_KiB, 2995, 0x2481, 0x2485 },
    { 0x1C, Socket_Buffer_Size::_4_KiB, 2995, 0xFFFE, 0x0002 },
    { 0x1C, Socket_Buffer_Size::_4_KiB, 4096, 0xE17D, 0xE181 },
    { 0x1C, Socket_Buffer_Size::_4_KiB, 4096, 0xFFFE, 0x0002 },

    { 0x1C, Socket_Buffer_Size::_8_KiB,    5, 0x537B, 0x537F },
    { 0x1C, Socket_Buffer_Size::_8_KiB,    5, 0xFFFE, 0x0002 },
    { 0x1C, Socket_Buffer_Size::_8_KiB, 3688, 0x7133, 0x7137 },
    { 0x1C, Socket_Buffer_Size::_8_KiB, 3688, 0xFFFD, 0x0001 },
    { 0x1C, Socket_Buffer_Size::_8_KiB, 8192, 0xA87E, 0xA882 },
    { 0x1C, Socket_Buffer_Size::_8_KiB, 8192, 0xFFFD, 0x0001 },

    { 0x1C, Socket_Buffer_Size::_16_KiB,     5, 0xBFB1, 0xBFB5 },
    { 0x1C, Socket_Buffer_Size::_16_KiB,     5, 0xFFFD, 0x0001 },
    { 0x1C, Socket_Buffer_Size::_16_KiB, 11027, 0xAB63, 0xAB67 },
    { 0x1C, Socket_Buffer_Size::_16_KiB, 11027, 0xFFFE, 0x0002 },
    { 0x1C, Socket_Buffer_Size::_16_KiB, 16384, 0x80FE, 0x8102 },
    { 0x1C, Socket_Buffer_Size::_16_KiB, 16384, 0xFFFC, 0x0000 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, receiveSomeData, ValuesIn( receiveSomeData_TEST_CASES ) );

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::shutdown() works
 *        properly when the connection has been lost.
 */
TEST( shutdownConnectionLost, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_7;

    auto connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );

    connection_handler.shutdown();

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::CONNECTED );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::shutdown()
 *        connection not lost test fixture.
 */
class shutdownConnectionNotLost : public TestWithParam<std::uint8_t> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::shutdown() works
 *        properly when the connection has not been lost.
 */
TEST_P( shutdownConnectionNotLost, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_2;

    auto connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    auto const sn_sr = GetParam();

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( sn_sr ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x08 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x28 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

    connection_handler.shutdown();

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::CONNECTED );

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( _ ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, _ ) );
}

INSTANTIATE_TEST_SUITE_P( testCases, shutdownConnectionNotLost, Values( 0x17, 0x1C ) );

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::close()
 *        works properly when the socket is in the
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::State::UNINITIALIZED
 *        state.
 */
TEST( close, worksProperlyUninitialized )
{
    auto connection_handler = Server_Connection_Handler{};

    connection_handler.close();

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::UNINITIALIZED );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::close()
 *        works properly when the socket is in the
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::State::CONNECTED
 *        state and the socket has not been detached from the server socket it is
 *        associated with.
 */
TEST( close, worksProperlyConnectedNotDetached )
{
    auto const in_sequence = InSequence{};

    auto       network_stack = Mock_Network_Stack{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_1;

    auto connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( socket_id ) ).WillOnce( Return( false ) );
    EXPECT_CALL( server, deallocate_socket( _, socket_id ) );

    connection_handler.close();

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::UNINITIALIZED );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::close()
 *        works properly when the socket is in the
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::State::CONNECTED
 *        state, the socket has been detached from the server socket it is associated
 *        with, and the port is still in use.
 */
TEST( close, worksProperlyConnectedDetachedPortStillInUse )
{
    auto const in_sequence = InSequence{};

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto       server        = Mock_Server{};
    auto const socket_id     = Socket_ID::_1;

    auto connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    auto const sn_port = std::uint16_t{ 26387 };

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( socket_id ) ).WillOnce( Return( true ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( socket_id ) ).WillOnce( Return( sn_port ) );
    EXPECT_CALL( network_stack, sockets() ).WillOnce( Return( 8 ) );
    EXPECT_CALL( driver, read_sn_mr( Socket_ID::_0 ) ).WillOnce( Return( 0b1'1'0'0'0010 ) );
    EXPECT_CALL( driver, read_sn_mr( Socket_ID::_2 ) ).WillOnce( Return( 0b0'1'1'0'0001 ) );
    EXPECT_CALL( driver, read_sn_port( Socket_ID::_2 ) ).WillOnce( Return( 55818 ) );
    EXPECT_CALL( driver, read_sn_mr( Socket_ID::_3 ) ).WillOnce( Return( 0b0'1'0'0'0001 ) );
    EXPECT_CALL( driver, read_sn_port( Socket_ID::_3 ) ).WillOnce( Return( sn_port ) );
    EXPECT_CALL( driver, read_sn_mr( Socket_ID::_4 ) ).WillOnce( Return( 0b1'0'1'0'0111 ) );
    EXPECT_CALL( driver, read_sn_mr( Socket_ID::_5 ) ).WillOnce( Return( 0b0'1'0'0'0001 ) );
    EXPECT_CALL( driver, read_sn_port( Socket_ID::_5 ) ).WillOnce( Return( 2280 ) );
    EXPECT_CALL( driver, read_sn_mr( Socket_ID::_6 ) ).WillOnce( Return( 0b0'0'1'1'1100 ) );
    EXPECT_CALL( driver, read_sn_mr( Socket_ID::_7 ) ).WillOnce( Return( 0b0'1'1'1'0110 ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, socket_id ) );

    connection_handler.close();

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::UNINITIALIZED );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::close()
 *        works properly when the socket is in the
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::State::CONNECTED
 *        state, the socket has been detached from the server socket it is associated
 *        with, and the port is not in use.
 */
TEST( close, worksProperlyConnectedDetachedPortNotInUse )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};
    auto       server             = Mock_Server{};
    auto const socket_id          = Socket_ID::_2;

    auto connection_handler = Server_Connection_Handler{ network_stack, server, socket_id };

    auto const sn_port = std::uint16_t{ 43787 };

    EXPECT_CALL( network_stack, tcp_server_connection_handler_is_detached( socket_id ) ).WillOnce( Return( true ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( socket_id ) ).WillOnce( Return( sn_port ) );
    EXPECT_CALL( network_stack, sockets() ).WillOnce( Return( 4 ) );
    EXPECT_CALL( driver, read_sn_mr( Socket_ID::_0 ) ).WillOnce( Return( 0b0'0'1'1'0001 ) );
    EXPECT_CALL( driver, read_sn_port( Socket_ID::_0 ) ).WillOnce( Return( 20743 ) );
    EXPECT_CALL( driver, read_sn_mr( Socket_ID::_1 ) ).WillOnce( Return( 0b1'0'1'0'1101 ) );
    EXPECT_CALL( driver, read_sn_mr( Socket_ID::_3 ) ).WillOnce( Return( 0b0'0'0'0'0001 ) );
    EXPECT_CALL( driver, read_sn_port( Socket_ID::_3 ) ).WillOnce( Return( 13040 ) );
    EXPECT_CALL( network_stack, tcp_port_allocator( _ ) ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( Port{ sn_port } ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, socket_id ) );

    connection_handler.close();

    EXPECT_EQ( connection_handler.state(), Server_Connection_Handler::State::UNINITIALIZED );
}

/**
 * \brief Execute the picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler
 *        automated tests.
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

/**
 * picolibrary
 *
 * Copyright 2020-2024, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server automated test program.
 */

#include <bitset>
#include <cstdint>
#include <limits>
#include <ostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/array.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/testing/automated/wiznet/w5500.h"
#include "picolibrary/testing/automated/wiznet/w5500/ip.h"
#include "picolibrary/testing/automated/wiznet/w5500/ip/network_stack.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/ip/tcp.h"

namespace {

using ::picolibrary::Array;
using ::picolibrary::IP::TCP::Port;
using ::picolibrary::IPv4::Address;
using ::picolibrary::Testing::Automated::WIZnet::W5500::Mock_Driver;
using ::picolibrary::Testing::Automated::WIZnet::W5500::IP::Mock_Network_Stack;
using ::picolibrary::Testing::Automated::WIZnet::W5500::IP::Mock_Port_Allocator;
using ::picolibrary::WIZnet::W5500::No_Delayed_ACK_Usage;
using ::picolibrary::WIZnet::W5500::Socket_ID;
using ::testing::_;
using ::testing::AnyNumber;
using ::testing::InSequence;
using ::testing::Ref;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::TestWithParam;
using ::testing::Values;
using ::testing::ValuesIn;

using Server = ::picolibrary::WIZnet::W5500::IP::TCP::Server<Mock_Network_Stack>;

} // namespace

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::Server() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const server = Server{};

    EXPECT_EQ( server.state(), Server::State::UNINITIALIZED );
    EXPECT_FALSE( server.is_listening() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::~Server() works properly
 *        when the socket is in the
 *        picolibrary::WIZnet::W5500::IP::TCP::Server::State::UNINITIALIZED state.
 */
TEST( destructor, worksProperlyUninitialized )
{
    auto const server = Server{};
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::~Server() works properly
 *        when the socket is in the
 *        picolibrary::WIZnet::W5500::IP::TCP::Server::State::INITIALIZED state.
 */
TEST( destructor, worksProperlyInitialized )
{
    auto       network_stack = Mock_Network_Stack{};
    auto const socket_ids = Server::Socket_IDs{ Socket_ID::_0, Socket_ID::_7, Socket_ID::_5 };

    auto const server = Server{ network_stack, socket_ids };

    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( network_stack, deallocate_socket( _, socket_id ) );
    } // for
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server::~Server() other states test
 *        fixture.
 */
class destructorOtherStates : public TestWithParam<Server::State> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::~Server() works properly
 *        when the socket is in states other than
 *        picolibrary::WIZnet::W5500::IP::TCP::Server::State::UNINITIALIZED, and
 *        picolibrary::WIZnet::W5500::IP::TCP::Server::State::INITIALIZED.
 */
TEST_P( destructorOtherStates, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};
    auto const socket_ids = Server::Socket_IDs{ Socket_ID::_0, Socket_ID::_7, Socket_ID::_5 };
    auto const state      = GetParam();

    auto const server = Server{ network_stack, socket_ids, state };

    auto const sn_port = std::uint16_t{ 13501 };

    EXPECT_CALL( network_stack, tcp_port_allocator( _ ) ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( socket_ids.front() ) ).WillOnce( Return( sn_port ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( Port{ sn_port } ) );
    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( network_stack, deallocate_socket( _, socket_id ) );
    } // for
}

INSTANTIATE_TEST_SUITE_P(
    testCases,
    destructorOtherStates,
    Values<Server::State>( Server::State::BOUND, Server::State::LISTENING ) );

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::socket_ids() works properly.
 */
TEST( socketIDs, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto const socket_ids = Server::Socket_IDs{ Socket_ID::_3, Socket_ID::_0, Socket_ID::_4, Socket_ID::_2 };

    auto const server = Server{ network_stack, socket_ids };

    EXPECT_EQ( server.socket_ids(), socket_ids );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server::socket_interrupt_mask() test case.
 */
struct socketInterruptMask_Test_Case {
    /**
     * \brief The socket's hardware socket IDs.
     */
    Server::Socket_IDs socket_ids;

    /**
     * \brief The socket's socket interrupt mask.
     */
    std::uint8_t socket_interrupt_mask;
};

auto operator<<( std::ostream & stream, socketInterruptMask_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    stream << "{ "
           << ".socket_ids = { ";

    for ( auto const socket_id : test_case.socket_ids ) {
        stream << socket_id << ", ";
    } // for

    return stream << "}"
                  << ", "
                  << ".socket_interrupt_mask = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.socket_interrupt_mask }
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server::socket_interrupt_mask() test
 *        fixture.
 */
class socketInterruptMask : public TestWithParam<socketInterruptMask_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::socket_interrupt_mask()
 *        works properly.
 */
TEST_P( socketInterruptMask, worksProperly )
{
    auto const test_case = GetParam();

    auto network_stack = Mock_Network_Stack{};

    auto const server = Server{ network_stack, test_case.socket_ids };

    EXPECT_EQ( server.socket_interrupt_mask(), test_case.socket_interrupt_mask );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server::socket_interrupt_mask() test cases.
 */
socketInterruptMask_Test_Case const socketInterruptMask_TEST_CASES[]{
    // clang-format off

    { { Socket_ID::_0 }, 0b00000001 },
    { { Socket_ID::_1 }, 0b00000010 },
    { { Socket_ID::_2 }, 0b00000100 },
    { { Socket_ID::_3 }, 0b00001000 },
    { { Socket_ID::_4 }, 0b00010000 },
    { { Socket_ID::_5 }, 0b00100000 },
    { { Socket_ID::_6 }, 0b01000000 },
    { { Socket_ID::_7 }, 0b10000000 },

    { { Socket_ID::_5, Socket_ID::_3, Socket_ID::_0, Socket_ID::_1 }, 0b00101011 },

    { { Socket_ID::_0, Socket_ID::_1, Socket_ID::_2, Socket_ID::_3, Socket_ID::_4, Socket_ID::_5, Socket_ID::_6, Socket_ID::_7 }, 0b11111111 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, socketInterruptMask, ValuesIn( socketInterruptMask_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server::configure_no_delayed_ack_usage()
 *        test case.
 */
struct configureNoDelayedACKUsage_Test_Case {
    /**
     * \brief The desired no delayed ACK usage configuration.
     */
    No_Delayed_ACK_Usage no_delayed_ack_usage_configuration;

    /**
     * \brief The SN_MR register value.
     */
    std::uint8_t sn_mr;
};

auto operator<<( std::ostream & stream, configureNoDelayedACKUsage_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".no_delayed_ack_usage_configuration = " << test_case.no_delayed_ack_usage_configuration
                  << ", "
                  << ".sn_mr = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.sn_mr }
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server::configure_no_delayed_ack_usage()
 *        test fixture.
 */
class configureNoDelayedACKUsage : public TestWithParam<configureNoDelayedACKUsage_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server::configure_no_delayed_ack_usage()
 *        works properly.
 */
TEST_P( configureNoDelayedACKUsage, worksProperly )
{
    auto const test_case = GetParam();

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_ids    = Server::Socket_IDs{ Socket_ID::_7, Socket_ID::_2 };

    auto server = Server{ network_stack, socket_ids };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( driver, write_sn_mr( socket_id, test_case.sn_mr ) );
    } // for

    server.configure_no_delayed_ack_usage( test_case.no_delayed_ack_usage_configuration );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server::configure_no_delayed_ack_usage()
 *        test cases.
 */
configureNoDelayedACKUsage_Test_Case const configureNoDelayedACKUsage_TEST_CASES[]{
    // clang-format off

    { No_Delayed_ACK_Usage::DISABLED, 0b0'0'0'0'0000 },
    { No_Delayed_ACK_Usage::ENABLED,  0b0'0'1'0'0000 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, configureNoDelayedACKUsage, ValuesIn( configureNoDelayedACKUsage_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server::no_delayed_ack_usage_configuration()
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server::no_delayed_ack_usage_configuration()
 *        test fixture.
 */
class noDelayedACKUsageConfiguration : public TestWithParam<noDelayedACKUsageConfiguration_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server::no_delayed_ack_usage_configuration()
 *        works properly.
 */
TEST_P( noDelayedACKUsageConfiguration, worksProperly )
{
    auto const test_case = GetParam();

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_ids = Server::Socket_IDs{ Socket_ID::_1, Socket_ID::_4, Socket_ID::_0, Socket_ID::_7 };

    auto const server = Server{ network_stack, socket_ids };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_mr( socket_ids.front() ) ).WillOnce( Return( test_case.sn_mr ) );

    EXPECT_EQ( server.no_delayed_ack_usage_configuration(), test_case.no_delayed_ack_usage_configuration );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server::no_delayed_ack_usage_configuration()
 *        test cases.
 */
noDelayedACKUsageConfiguration_Test_Case const noDelayedACKUsageConfiguration_TEST_CASES[]{
    // clang-format off

    { 0b1'0'0'0'1011, No_Delayed_ACK_Usage::DISABLED },
    { 0b1'0'1'0'1011, No_Delayed_ACK_Usage::ENABLED  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, noDelayedACKUsageConfiguration, ValuesIn( noDelayedACKUsageConfiguration_TEST_CASES ) );

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Server::configure_maximum_segment_size()
 *        works properly.
 */
TEST( configureMaximumSegmentSize, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_ids    = Server::Socket_IDs{ Socket_ID::_4, Socket_ID::_2 };

    auto server = Server{ network_stack, socket_ids };

    auto const maximum_segment_size = std::uint16_t{ 0x52B0 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( driver, write_sn_mssr( socket_id, maximum_segment_size ) );
    } // for

    server.configure_maximum_segment_size( maximum_segment_size );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::maximum_segment_size() works
 *        properly.
 */
TEST( maximumSegmentSize, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_ids = Server::Socket_IDs{ Socket_ID::_2, Socket_ID::_5, Socket_ID::_3, Socket_ID::_1 };

    auto const server = Server{ network_stack, socket_ids };

    auto const sn_mssr = std::uint16_t{ 0x9410 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_mssr( socket_ids.front() ) ).WillOnce( Return( sn_mssr ) );

    EXPECT_EQ( server.maximum_segment_size(), sn_mssr );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::configure_time_to_live()
 *        works properly.
 */
TEST( configureTimeToLive, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_ids    = Server::Socket_IDs{ Socket_ID::_1, Socket_ID::_7 };

    auto server = Server{ network_stack, socket_ids };

    auto const time_to_live = std::uint8_t{ 0xAE };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( driver, write_sn_ttl( socket_id, time_to_live ) );
    } // for

    server.configure_time_to_live( time_to_live );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::time_to_live() works
 *        properly.
 */
TEST( timeToLive, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_ids    = Server::Socket_IDs{ Socket_ID::_1, Socket_ID::_3 };

    auto const server = Server{ network_stack, socket_ids };

    auto const sn_ttl = std::uint8_t{ 0x3B };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_ttl( socket_ids.front() ) ).WillOnce( Return( sn_ttl ) );

    EXPECT_EQ( server.time_to_live(), sn_ttl );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::configure_keepalive_period()
 *        works properly.
 */
TEST( configureKeepalivePeriod, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_ids    = Server::Socket_IDs{ Socket_ID::_3, Socket_ID::_2 };

    auto server = Server{ network_stack, socket_ids };

    auto const keepalive_period = std::uint8_t{ 0x42 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( driver, write_sn_kpalvtr( socket_id, keepalive_period ) );
    } // for

    server.configure_keepalive_period( keepalive_period );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::keepalive_period() works
 *        properly.
 */
TEST( keepalivePeriod, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_ids = Server::Socket_IDs{ Socket_ID::_5, Socket_ID::_6, Socket_ID::_0, Socket_ID::_1 };

    auto const server = Server{ network_stack, socket_ids };

    auto const sn_kpalvtr = std::uint8_t{ 0x4D };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_kpalvtr( socket_ids.front() ) ).WillOnce( Return( sn_kpalvtr ) );

    EXPECT_EQ( server.keepalive_period(), sn_kpalvtr );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::enable_interrupts() works
 *        properly.
 */
TEST( enableInterrupts, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_ids    = Server::Socket_IDs{ Socket_ID::_2, Socket_ID::_0 };

    auto server = Server{ network_stack, socket_ids };

    auto const mask = std::uint8_t{ 0b0101'1111 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_imr( socket_ids.front() ) ).WillOnce( Return( 0b0011'0010 ) );
    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( driver, write_sn_imr( socket_id, 0b0111'1111 ) );
    } // for

    server.enable_interrupts( mask );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::disable_interrupts(
 *        std::uint8_t ) works properly.
 */
TEST( disableInterrupts, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_ids    = Server::Socket_IDs{ Socket_ID::_1, Socket_ID::_5 };

    auto server = Server{ network_stack, socket_ids };

    auto const mask = std::uint8_t{ 0b0101'0111 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_imr( socket_ids.front() ) ).WillOnce( Return( 0b0011'1010 ) );
    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( driver, write_sn_imr( socket_id, 0b0010'1000 ) );
    } // for

    server.disable_interrupts( mask );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::disable_interrupts() works
 *        properly.
 */
TEST( disableAllInterrupts, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_ids    = Server::Socket_IDs{ Socket_ID::_0, Socket_ID::_3 };

    auto server = Server{ network_stack, socket_ids };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( driver, write_sn_imr( socket_id, 0x00 ) );
    } // for

    server.disable_interrupts();

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::enabled_interrupts() works
 *        properly.
 */
TEST( enabledInterrupts, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_ids    = Server::Socket_IDs{ Socket_ID::_7, Socket_ID::_1 };

    auto const server = Server{ network_stack, socket_ids };

    auto const sn_imr{ 0b00100111 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_imr( socket_ids.front() ) ).WillOnce( Return( sn_imr ) );

    EXPECT_EQ( server.enabled_interrupts(), sn_imr );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::interrupt_context() works
 *        properly.
 */
TEST( interruptContext, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_ids    = Server::Socket_IDs{ Socket_ID::_6, Socket_ID::_0 };

    auto const server = Server{ network_stack, socket_ids };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_ir( socket_ids[ 0 ] ) ).WillOnce( Return( 0b0101'0100 ) );
    EXPECT_CALL( driver, read_sn_ir( socket_ids[ 1 ] ) ).WillOnce( Return( 0b0011'1100 ) );

    EXPECT_EQ( server.interrupt_context(), 0b0111'1100 );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::clear_interrupts() works
 *        properly.
 */
TEST( clearInterrupts, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_ids    = Server::Socket_IDs{ Socket_ID::_1, Socket_ID::_2 };

    auto server = Server{ network_stack, socket_ids };

    auto const mask = std::uint8_t{ 0b111'0'0'0'1'1 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( driver, write_sn_ir( socket_id, mask ) );
    } // for

    server.clear_interrupts( mask );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::bind() works properly when
 *        the default local endpoint is used.
 */
TEST( bind, worksProperlyDefaultEndpoint )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};
    auto const socket_ids         = Server::Socket_IDs{ Socket_ID::_4, Socket_ID::_0 };

    auto server = Server{ network_stack, socket_ids };

    auto const port = Port{ 51561 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( network_stack, tcp_port_allocator( _ ) ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( tcp_port_allocator, allocate( Ref( driver ), Port{} ) ).WillOnce( Return( port ) );
    EXPECT_CALL( driver, read_sn_mr( socket_ids.front() ) ).WillOnce( Return( 0b1'1'0'0'0100 ) );
    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) );
        EXPECT_CALL( driver, write_sn_mr( socket_id, 0b1'1'0'0'0001 ) );
        EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x87 ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0xCD ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x13 ) );
    } // for

    server.bind();

    EXPECT_EQ( server.state(), Server::State::BOUND );
    EXPECT_FALSE( server.is_listening() );

    EXPECT_CALL( network_stack, tcp_port_allocator( _ ) ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 43252 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::bind() works properly when
 *        binding to any address and an ephemeral port.
 */
TEST( bind, worksProperlyAnyAddressEphemeralPort )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};
    auto const socket_ids         = Server::Socket_IDs{ Socket_ID::_7, Socket_ID::_4 };

    auto server = Server{ network_stack, socket_ids };

    auto const port = Port{ 44597 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( network_stack, tcp_port_allocator( _ ) ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( tcp_port_allocator, allocate( Ref( driver ), Port{} ) ).WillOnce( Return( port ) );
    EXPECT_CALL( driver, read_sn_mr( socket_ids.front() ) ).WillOnce( Return( 0b0'1'1'0'1001 ) );
    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) );
        EXPECT_CALL( driver, write_sn_mr( socket_id, 0b0'1'1'0'0001 ) );
        EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x54 ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x1F ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x13 ) );
    } // for

    server.bind( {} );

    EXPECT_EQ( server.state(), Server::State::BOUND );
    EXPECT_FALSE( server.is_listening() );

    EXPECT_CALL( network_stack, tcp_port_allocator( _ ) ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 64048 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::bind() works properly when
 *        binding to any address and a non-ephemeral port.
 */
TEST( bind, worksProperlyAnyAddressNonEphemeralPort )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};
    auto const socket_ids         = Server::Socket_IDs{ Socket_ID::_3, Socket_ID::_4 };

    auto server = Server{ network_stack, socket_ids };

    auto const port = Port{ 60790 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( network_stack, tcp_port_allocator( _ ) ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( tcp_port_allocator, allocate( Ref( driver ), port ) ).WillOnce( Return( port ) );
    EXPECT_CALL( driver, read_sn_mr( socket_ids.front() ) ).WillOnce( Return( 0b0'0'0'1'1011 ) );
    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) );
        EXPECT_CALL( driver, write_sn_mr( socket_id, 0b0'0'0'1'0001 ) );
        EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x0A ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x34 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x13 ) );
    } // for

    server.bind( port );

    EXPECT_EQ( server.state(), Server::State::BOUND );
    EXPECT_FALSE( server.is_listening() );

    EXPECT_CALL( network_stack, tcp_port_allocator( _ ) ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 18929 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::bind() works properly when
 *        binding to a specific address and an ephemeral port.
 */
TEST( bind, worksProperlySpecificAddressEphemeralPort )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};
    auto const socket_ids         = Server::Socket_IDs{ Socket_ID::_6, Socket_ID::_5 };

    auto server = Server{ network_stack, socket_ids };

    auto const address = Address{ { 17, 233, 244, 75 } };
    auto const port    = Port{ 17923 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( address.as_byte_array() ) );
    EXPECT_CALL( network_stack, tcp_port_allocator( _ ) ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( tcp_port_allocator, allocate( Ref( driver ), Port{} ) ).WillOnce( Return( port ) );
    EXPECT_CALL( driver, read_sn_mr( socket_ids.front() ) ).WillOnce( Return( 0b1'1'0'0'0000 ) );
    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) );
        EXPECT_CALL( driver, write_sn_mr( socket_id, 0b1'1'0'0'0001 ) );
        EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x0A ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0xCB ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x13 ) );
    } // for

    server.bind( { address, {} } );

    EXPECT_EQ( server.state(), Server::State::BOUND );
    EXPECT_FALSE( server.is_listening() );

    EXPECT_CALL( network_stack, tcp_port_allocator( _ ) ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 16325 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::bind() works properly when
 *        binding to a specific address and a non-ephemeral port.
 */
TEST( bind, worksProperlySpecificAddressNonEphemeralPort )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};
    auto const socket_ids         = Server::Socket_IDs{ Socket_ID::_2, Socket_ID::_5 };

    auto server = Server{ network_stack, socket_ids };

    auto const address = Address{ { 93, 188, 26, 253 } };
    auto const port    = Port{ 9090 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( address.as_byte_array() ) );
    EXPECT_CALL( network_stack, tcp_port_allocator( _ ) ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( tcp_port_allocator, allocate( Ref( driver ), port ) ).WillOnce( Return( port ) );
    EXPECT_CALL( driver, read_sn_mr( socket_ids.front() ) ).WillOnce( Return( 0b1'0'1'0'0100 ) );
    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) );
        EXPECT_CALL( driver, write_sn_mr( socket_id, 0b1'0'1'0'0001 ) );
        EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0xC9 ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x96 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x13 ) );
    } // for

    server.bind( { address, port } );

    EXPECT_EQ( server.state(), Server::State::BOUND );
    EXPECT_FALSE( server.is_listening() );

    EXPECT_CALL( network_stack, tcp_port_allocator( _ ) ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 27289 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server::listen() backlog less than or equal
 *        to socket count test fixture.
 */
class listenBacklogLessThanOrEqualToSocketCount : public TestWithParam<std::uint_fast8_t> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::listen() works properly when
 *        backlog is less than or equal to the socket count.
 */
TEST_P( listenBacklogLessThanOrEqualToSocketCount, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};
    auto const socket_ids         = Server::Socket_IDs{ Socket_ID::_7, Socket_ID::_2 };

    auto server = Server{ network_stack, socket_ids, Server::State::BOUND };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( driver, write_sn_cr( socket_id, 0x02 ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0xF6 ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
    } // for

    auto const backlog = GetParam();

    server.listen( backlog );

    EXPECT_EQ( server.state(), Server::State::LISTENING );
    EXPECT_EQ( server.socket_ids(), socket_ids );
    EXPECT_TRUE( server.is_listening() );

    EXPECT_CALL( network_stack, tcp_port_allocator( _ ) ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 21057 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

INSTANTIATE_TEST_SUITE_P( testCases, listenBacklogLessThanOrEqualToSocketCount, Values<std::uint8_t>( 1, 2 ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server::listen() backlog greater than
 *        socket count test case.
 */
struct listenBacklogGreaterThanSocketCount_Test_Case {
    /**
     * \brief The maximum number of simultaneously connected clients.
     */
    std::uint_fast8_t backlog;

    /**
     * \brief The socket's hardware socket IDs (new).
     */
    Server::Socket_IDs socket_ids_new;

    /**
     * \brief The socket's hardware socket IDs (final).
     */
    Server::Socket_IDs socket_ids_final;
};

auto operator<<( std::ostream & stream, listenBacklogGreaterThanSocketCount_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    stream << "{ "
           << ".backlog = " << static_cast<std::uint_fast16_t>( test_case.backlog )
           << ", "
           << ".socket_ids_new = { ";

    for ( auto const socket_id : test_case.socket_ids_new ) {
        stream << socket_id << ", ";
    } // for

    stream << "}"
           << ", "
           << ".socket_ids_final = { ";

    for ( auto const socket_id : test_case.socket_ids_final ) {
        stream << socket_id << ", ";
    } // for

    return stream << "}"
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server::listen() backlog greater than
 *        socket count test fixture.
 */
class listenBacklogGreaterThanSocketCount :
    public TestWithParam<listenBacklogGreaterThanSocketCount_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::listen() works properly when
 *        backlog is greater than the socket count.
 */
TEST_P( listenBacklogGreaterThanSocketCount, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};
    auto const socket_ids_initial = Server::Socket_IDs{ Socket_ID::_3, Socket_ID::_7 };

    auto server = Server{ network_stack, socket_ids_initial, Server::State::BOUND };

    auto const sn_mr      = std::uint8_t{ 0x8B };
    auto const sn_port    = std::uint16_t{ 0x7B12 };
    auto const sn_mssr    = std::uint16_t{ 0xBFEA };
    auto const sn_ttl     = std::uint8_t{ 0x6C };
    auto const sn_imr     = std::uint8_t{ 0x53 };
    auto const sn_kpalvtr = std::uint8_t{ 0x62 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( network_stack, allocate_sockets( _, test_case.socket_ids_new.size() ) )
        .WillOnce( Return( test_case.socket_ids_new ) );
    EXPECT_CALL( driver, read_sn_mr( socket_ids_initial.front() ) ).WillOnce( Return( sn_mr ) );
    EXPECT_CALL( driver, read_sn_port( socket_ids_initial.front() ) ).WillOnce( Return( sn_port ) );
    EXPECT_CALL( driver, read_sn_mssr( socket_ids_initial.front() ) ).WillOnce( Return( sn_mssr ) );
    EXPECT_CALL( driver, read_sn_ttl( socket_ids_initial.front() ) ).WillOnce( Return( sn_ttl ) );
    EXPECT_CALL( driver, read_sn_imr( socket_ids_initial.front() ) ).WillOnce( Return( sn_imr ) );
    EXPECT_CALL( driver, read_sn_kpalvtr( socket_ids_initial.front() ) ).WillOnce( Return( sn_kpalvtr ) );
    for ( auto const socket_id : test_case.socket_ids_new ) {
        EXPECT_CALL( driver, write_sn_mr( socket_id, sn_mr ) );
        EXPECT_CALL( driver, write_sn_port( socket_id, sn_port ) );
        EXPECT_CALL( driver, write_sn_mssr( socket_id, sn_mssr ) );
        EXPECT_CALL( driver, write_sn_ttl( socket_id, sn_ttl ) );
        EXPECT_CALL( driver, write_sn_imr( socket_id, sn_imr ) );
        EXPECT_CALL( driver, write_sn_kpalvtr( socket_id, sn_kpalvtr ) );
        EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0xD6 ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x45 ) );
        EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x13 ) );
    } // for
    for ( auto const socket_id : test_case.socket_ids_final ) {
        EXPECT_CALL( driver, write_sn_cr( socket_id, 0x02 ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x6E ) );
        EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
    } // for

    server.listen( test_case.backlog );

    EXPECT_EQ( server.state(), Server::State::LISTENING );
    EXPECT_EQ( server.socket_ids(), test_case.socket_ids_final );
    EXPECT_TRUE( server.is_listening() );

    EXPECT_CALL( network_stack, tcp_port_allocator( _ ) ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 55732 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server::listen() backlog greater than
 *        socket count test cases.
 */
listenBacklogGreaterThanSocketCount_Test_Case const listenBacklogGreaterThanSocketCount_TEST_CASES[]{
    // clang-format off

    { 3, { Socket_ID::_4 }, { Socket_ID::_3, Socket_ID::_7, Socket_ID::_4 } },

    { 8, { Socket_ID::_4, Socket_ID::_0, Socket_ID::_6, Socket_ID::_5, Socket_ID::_1, Socket_ID::_2 }, { Socket_ID::_3, Socket_ID::_7, Socket_ID::_4, Socket_ID::_0, Socket_ID::_6, Socket_ID::_5, Socket_ID::_1, Socket_ID::_2 } },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, listenBacklogGreaterThanSocketCount, ValuesIn( listenBacklogGreaterThanSocketCount_TEST_CASES ) );

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::local_endpoint() works
 *        properly.
 */
TEST( localEndpoint, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_ids    = Server::Socket_IDs{ Socket_ID::_1, Socket_ID::_0 };

    auto const server = Server{ network_stack, socket_ids };

    auto const sipr    = Array<std::uint8_t, 4>{ 116, 166, 96, 6 };
    auto const sn_port = std::uint16_t{ 46774 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( sipr ) );
    EXPECT_CALL( driver, read_sn_port( socket_ids.front() ) ).WillOnce( Return( sn_port ) );

    auto const endpoint = server.local_endpoint();

    EXPECT_TRUE( endpoint.address().is_ipv4() );
    EXPECT_EQ( endpoint.address().ipv4().as_byte_array(), sipr );
    EXPECT_EQ( endpoint.port().as_unsigned_integer(), sn_port );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) ).Times( AnyNumber() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::close() works properly when
 *        the socket is in the
 *        picolibrary::WIZnet::W5500::IP::TCP::Server::State::UNINITIALIZED state.
 */
TEST( close, worksProperlyUninitialized )
{
    auto server = Server{};

    server.close();

    EXPECT_EQ( server.state(), Server::State::UNINITIALIZED );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::close() works properly when
 *        the socket is in the
 *        picolibrary::WIZnet::W5500::IP::TCP::Server::State::INITIALIZED state.
 */
TEST( close, worksProperlyInitialized )
{
    auto       network_stack = Mock_Network_Stack{};
    auto const socket_ids    = Server::Socket_IDs{ Socket_ID::_5, Socket_ID::_4 };

    auto server = Server{ network_stack, socket_ids };

    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( network_stack, deallocate_socket( _, socket_id ) );
    } // for

    server.close();

    EXPECT_EQ( server.state(), Server::State::UNINITIALIZED );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server::close() other states test fixture.
 */
class closeOtherStates : public TestWithParam<Server::State> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Server::close() works properly when
 *        the socket is in states other than
 *        picolibrary::WIZnet::W5500::IP::TCP::Server::State::UNINITIALIZED, and
 *        picolibrary::WIZnet::W5500::IP::TCP::Server::State::INITIALIZED.
 */
TEST_P( closeOtherStates, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};
    auto const socket_ids         = Server::Socket_IDs{ Socket_ID::_3, Socket_ID::_2 };
    auto const state              = GetParam();

    auto server = Server{ network_stack, socket_ids, state };

    auto const sn_port = std::uint16_t{ 5144 };

    EXPECT_CALL( network_stack, tcp_port_allocator( _ ) ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( socket_ids.front() ) ).WillOnce( Return( sn_port ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( Port{ sn_port } ) );
    for ( auto const socket_id : socket_ids ) {
        EXPECT_CALL( network_stack, deallocate_socket( _, socket_id ) );
    } // for

    server.close();

    EXPECT_EQ( server.state(), Server::State::UNINITIALIZED );
}

INSTANTIATE_TEST_SUITE_P( testCases, closeOtherStates, Values<Server::State>( Server::State::BOUND, Server::State::LISTENING ) );

/**
 * \brief Execute the picolibrary::WIZnet::W5500::IP::TCP::Server automated tests.
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

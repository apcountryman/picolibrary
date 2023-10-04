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
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket automated test program.
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
#include "picolibrary/ip/udp.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/testing/automated/ip/udp.h"
#include "picolibrary/testing/automated/wiznet/w5500.h"
#include "picolibrary/testing/automated/wiznet/w5500/ip.h"
#include "picolibrary/testing/automated/wiznet/w5500/ip/network_stack.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/ip/udp.h"

namespace {

using ::picolibrary::Array;
using ::picolibrary::Generic_Error;
using ::picolibrary::IP::UDP::Endpoint;
using ::picolibrary::IP::UDP::Port;
using ::picolibrary::IPv4::Address;
using ::picolibrary::Testing::Automated::WIZnet::W5500::Mock_Driver;
using ::picolibrary::Testing::Automated::WIZnet::W5500::IP::Mock_Network_Stack;
using ::picolibrary::Testing::Automated::WIZnet::W5500::IP::Mock_Port_Allocator;
using ::picolibrary::WIZnet::W5500::Broadcast_Blocking;
using ::picolibrary::WIZnet::W5500::Socket_Buffer_Size;
using ::picolibrary::WIZnet::W5500::Socket_ID;
using ::picolibrary::WIZnet::W5500::Unicast_Blocking;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Ref;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::TestWithParam;
using ::testing::Values;
using ::testing::ValuesIn;

using Socket = ::picolibrary::WIZnet::W5500::IP::UDP::Socket<Mock_Network_Stack>;

auto to_vector( Address address, Port port, std::uint16_t size ) -> std::vector<std::uint8_t>
{
    // clang-format off

    return {
        address.as_byte_array()[ 0 ],
        address.as_byte_array()[ 1 ],
        address.as_byte_array()[ 2 ],
        address.as_byte_array()[ 3 ],
        static_cast<std::uint8_t>( port.as_unsigned_integer() >> std::numeric_limits<std::uint8_t>::digits ),
        static_cast<std::uint8_t>( port.as_unsigned_integer() ),
        static_cast<std::uint8_t>( size >> std::numeric_limits<std::uint8_t>::digits ),
        static_cast<std::uint8_t>( size ),
    };

    // clang-format on
}

} // namespace

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::Socket() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const socket = Socket{};

    EXPECT_EQ( socket.state(), Socket::State::UNINITIALIZED );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::~Socket() works properly
 *        when the socket is in the
 *        picolibrary::WIZnet::W5500::IP::UDP::Socket::State::UNINITIALIZED state.
 */
TEST( destructor, worksProperlyUninitialized )
{
    auto const socket = Socket{};
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::~Socket() works properly
 *        when the socket is in the
 *        picolibrary::WIZnet::W5500::IP::UDP::Socket::State::INITIALIZED state.
 */
TEST( destructor, worksProperlyInitialized )
{
    auto       network_stack = Mock_Network_Stack{};
    auto const socket_id     = Socket_ID::_5;

    auto const socket = Socket{ network_stack, socket_id };

    EXPECT_CALL( network_stack, deallocate_socket( _, socket_id ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::~Socket() works properly
 *        when the socket is in the
 *        picolibrary::WIZnet::W5500::IP::UDP::Socket::State::BOUND state.
 */
TEST( destructor, worksProperlyBound )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       udp_port_allocator = Mock_Port_Allocator{};
    auto const socket_id          = Socket_ID::_4;

    auto const socket = Socket{ network_stack, socket_id, Socket::State::BOUND };

    auto const sn_port = std::uint16_t{ 19758 };

    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( socket_id ) ).WillOnce( Return( sn_port ) );
    EXPECT_CALL( udp_port_allocator, deallocate( Port{ sn_port } ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, socket_id ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::socket_id() works properly.
 */
TEST( socketId, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto const socket_id     = Socket_ID::_7;

    auto const socket = Socket{ network_stack, socket_id };

    EXPECT_EQ( socket.socket_id(), socket_id );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::socket_interrupt_mask() test case.
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
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::socket_interrupt_mask() test
 *        fixture.
 */
class socketInterruptMask : public TestWithParam<socketInterruptMask_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::socket_interrupt_mask()
 *        works properly.
 */
TEST_P( socketInterruptMask, worksProperly )
{
    auto const test_case = GetParam();

    auto network_stack = Mock_Network_Stack{};

    auto const socket = Socket{ network_stack, test_case.socket_id };

    EXPECT_EQ( socket.socket_interrupt_mask(), test_case.socket_interrupt_mask );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::socket_interrupt_mask() test cases.
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
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::configure_broadcast_blocking() test
 *        case.
 */
struct configureBroadcastBlocking_Test_Case {
    /**
     * \brief The SN_MR register value (initial).
     */
    std::uint8_t sn_mr_initial;

    /**
     * \brief The desired broadcast blocking configuration.
     */
    Broadcast_Blocking broadcast_blocking_configuration;

    /**
     * \brief The SN_MR register value (final).
     */
    std::uint8_t sn_mr_final;
};

auto operator<<( std::ostream & stream, configureBroadcastBlocking_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".sn_mr_initial = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.sn_mr_initial }
                  << ", "
                  << ".broadcast_blocking_configuration = " << test_case.broadcast_blocking_configuration
                  << ", "
                  << ".sn_mr_final = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.sn_mr_final }
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::configure_broadcast_blocking() test
 *        fixture.
 */
class configureBroadcastBlocking : public TestWithParam<configureBroadcastBlocking_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::UDP::Socket::configure_broadcast_blocking()
 *        works properly.
 */
TEST_P( configureBroadcastBlocking, worksProperly )
{
    auto const test_case = GetParam();

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_5;

    auto socket = Socket{ network_stack, socket_id };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_mr( socket_id ) ).WillOnce( Return( test_case.sn_mr_initial ) );
    EXPECT_CALL( driver, write_sn_mr( socket_id, test_case.sn_mr_final ) );

    socket.configure_broadcast_blocking( test_case.broadcast_blocking_configuration );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::configure_broadcast_blocking() test
 *        cases.
 */
configureBroadcastBlocking_Test_Case const configureBroadcastBlocking_TEST_CASES[]{
    // clang-format off

    { 0b0'0'1'1'0101, Broadcast_Blocking::DISABLED, 0b0'0'1'1'0101 },
    { 0b0'0'1'1'0101, Broadcast_Blocking::ENABLED,  0b0'1'1'1'0101 },
    { 0b0'1'1'0'1110, Broadcast_Blocking::DISABLED, 0b0'0'1'0'1110 },
    { 0b0'1'1'0'1110, Broadcast_Blocking::ENABLED,  0b0'1'1'0'1110 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, configureBroadcastBlocking, ValuesIn( configureBroadcastBlocking_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::broadcast_blocking_configuration()
 *        test case.
 */
struct broadcastBlockingConfiguration_Test_Case {
    /**
     * \brief The SN_MR register value.
     */
    std::uint8_t sn_mr;

    /**
     * \brief The desired broadcast blocking configuration.
     */
    Broadcast_Blocking broadcast_blocking_configuration;
};

auto operator<<( std::ostream & stream, broadcastBlockingConfiguration_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".sn_mr = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.sn_mr }
                  << ", "
                  << ".broadcast_blocking_configuration = " << test_case.broadcast_blocking_configuration
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::broadcast_blocking_configuration()
 *        test fixture.
 */
class broadcastBlockingConfiguration : public TestWithParam<broadcastBlockingConfiguration_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::UDP::Socket::broadcast_blocking_configuration()
 *        works properly.
 */
TEST_P( broadcastBlockingConfiguration, worksProperly )
{
    auto const test_case = GetParam();

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_2;

    auto const socket = Socket{ network_stack, socket_id };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_mr( socket_id ) ).WillOnce( Return( test_case.sn_mr ) );

    EXPECT_EQ( socket.broadcast_blocking_configuration(), test_case.broadcast_blocking_configuration );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::broadcast_blocking_configuration()
 *        test cases.
 */
broadcastBlockingConfiguration_Test_Case const broadcastBlockingConfiguration_TEST_CASES[]{
    // clang-format off

    { 0b1'0'0'1'1011, Broadcast_Blocking::DISABLED },
    { 0b1'1'0'1'1011, Broadcast_Blocking::ENABLED  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, broadcastBlockingConfiguration, ValuesIn( broadcastBlockingConfiguration_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::configure_unicast_blocking() test
 *        case.
 */
struct configureUnicastBlocking_Test_Case {
    /**
     * \brief The SN_MR register value (initial).
     */
    std::uint8_t sn_mr_initial;

    /**
     * \brief The desired unicast blocking configuration.
     */
    Unicast_Blocking unicast_blocking_configuration;

    /**
     * \brief The SN_MR register value (final).
     */
    std::uint8_t sn_mr_final;
};

auto operator<<( std::ostream & stream, configureUnicastBlocking_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".sn_mr_initial = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.sn_mr_initial }
                  << ", "
                  << ".unicast_blocking_configuration = " << test_case.unicast_blocking_configuration
                  << ", "
                  << ".sn_mr_final = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.sn_mr_final }
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::configure_unicast_blocking() test
 *        fixture.
 */
class configureUnicastBlocking : public TestWithParam<configureUnicastBlocking_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::configure_unicast_blocking()
 *        works properly.
 */
TEST_P( configureUnicastBlocking, worksProperly )
{
    auto const test_case = GetParam();

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_0;

    auto socket = Socket{ network_stack, socket_id };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_mr( socket_id ) ).WillOnce( Return( test_case.sn_mr_initial ) );
    EXPECT_CALL( driver, write_sn_mr( socket_id, test_case.sn_mr_final ) );

    socket.configure_unicast_blocking( test_case.unicast_blocking_configuration );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::configure_unicast_blocking() test
 *        cases.
 */
configureUnicastBlocking_Test_Case const configureUnicastBlocking_TEST_CASES[]{
    // clang-format off

    { 0b1'0'0'0'1000, Unicast_Blocking::DISABLED, 0b1'0'0'0'1000 },
    { 0b1'0'0'0'1000, Unicast_Blocking::ENABLED,  0b1'0'0'1'1000 },
    { 0b1'0'1'1'1101, Unicast_Blocking::DISABLED, 0b1'0'1'0'1101 },
    { 0b1'0'1'1'1101, Unicast_Blocking::ENABLED,  0b1'0'1'1'1101 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, configureUnicastBlocking, ValuesIn( configureUnicastBlocking_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::unicast_blocking_configuration()
 *        test case.
 */
struct unicastBlockingConfiguration_Test_Case {
    /**
     * \brief The SN_MR register value.
     */
    std::uint8_t sn_mr;

    /**
     * \brief The desired unicast blocking configuration.
     */
    Unicast_Blocking unicast_blocking_configuration;
};

auto operator<<( std::ostream & stream, unicastBlockingConfiguration_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".sn_mr = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.sn_mr }
                  << ", "
                  << ".unicast_blocking_configuration = " << test_case.unicast_blocking_configuration
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::unicast_blocking_configuration()
 *        test fixture.
 */
class unicastBlockingConfiguration : public TestWithParam<unicastBlockingConfiguration_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::UDP::Socket::unicast_blocking_configuration()
 *        works properly.
 */
TEST_P( unicastBlockingConfiguration, worksProperly )
{
    auto const test_case = GetParam();

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_2;

    auto const socket = Socket{ network_stack, socket_id };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_mr( socket_id ) ).WillOnce( Return( test_case.sn_mr ) );

    EXPECT_EQ( socket.unicast_blocking_configuration(), test_case.unicast_blocking_configuration );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::unicast_blocking_configuration()
 *        test cases.
 */
unicastBlockingConfiguration_Test_Case const unicastBlockingConfiguration_TEST_CASES[]{
    // clang-format off

    { 0b0'0'1'0'0100, Unicast_Blocking::DISABLED },
    { 0b0'0'1'1'0100, Unicast_Blocking::ENABLED  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, unicastBlockingConfiguration, ValuesIn( unicastBlockingConfiguration_TEST_CASES ) );

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::configure_time_to_live()
 *        works properly.
 */
TEST( configureTimeToLive, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_5;

    auto socket = Socket{ network_stack, socket_id };

    auto const time_to_live = std::uint8_t{ 0x68 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, write_sn_ttl( socket_id, time_to_live ) );

    socket.configure_time_to_live( time_to_live );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::time_to_live() works
 *        properly.
 */
TEST( timeToLive, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_6;

    auto const socket = Socket{ network_stack, socket_id };

    auto const sn_ttl = std::uint8_t{ 0x3C };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_ttl( socket_id ) ).WillOnce( Return( sn_ttl ) );

    EXPECT_EQ( socket.time_to_live(), sn_ttl );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::enable_interrupts() works
 *        properly.
 */
TEST( enableInterrupts, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_3;

    auto socket = Socket{ network_stack, socket_id };

    auto const mask = std::uint8_t{ 0b0101'0010 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_imr( socket_id ) ).WillOnce( Return( 0b0011'0110 ) );
    EXPECT_CALL( driver, write_sn_imr( socket_id, 0b0111'0110 ) );

    socket.enable_interrupts( mask );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::disable_interrupts(
 *        std::uint8_t ) works properly.
 */
TEST( disableInterrupts, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_1;

    auto socket = Socket{ network_stack, socket_id };

    auto const mask = std::uint8_t{ 0b0101'1000 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_imr( socket_id ) ).WillOnce( Return( 0b0011'1110 ) );
    EXPECT_CALL( driver, write_sn_imr( socket_id, 0b0010'0110 ) );

    socket.disable_interrupts( mask );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::disable_interrupts() works
 *        properly.
 */
TEST( disableAllInterrupts, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_7;

    auto socket = Socket{ network_stack, socket_id };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, write_sn_imr( socket_id, 0x00 ) );

    socket.disable_interrupts();

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::enabled_interrupts() works
 *        properly.
 */
TEST( enabledInterrupts, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_6;

    auto const socket = Socket{ network_stack, socket_id };

    auto const sn_imr = std::uint8_t{ 0b10110001 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_imr( socket_id ) ).WillOnce( Return( sn_imr ) );

    EXPECT_EQ( socket.enabled_interrupts(), sn_imr );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::interrupt_context() works
 *        properly.
 */
TEST( interruptContext, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_6;

    auto const socket = Socket{ network_stack, socket_id };

    auto const sn_ir = std::uint8_t{ 0b01110010 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( sn_ir ) );

    EXPECT_EQ( socket.interrupt_context(), sn_ir );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::clear_interrupts() test case.
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
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::clear_interrupts() test fixture.
 */
class clearInterrupts : public TestWithParam<clearInterrupts_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::clear_interrupts() works
 *        properly.
 */
TEST_P( clearInterrupts, worksProperly )
{
    auto const test_case = GetParam();

    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_4;

    auto socket = Socket{ network_stack, socket_id, Socket::State::INITIALIZED, test_case.is_transmitting_initial };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, write_sn_ir( socket_id, test_case.mask ) );

    socket.clear_interrupts( test_case.mask );

    EXPECT_EQ( socket.is_transmitting(), test_case.is_transmitting_final );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::clear_interrupts() test cases.
 */
clearInterrupts_Test_Case const clearInterrupts_TEST_CASES[]{
    // clang-format off

    { false, 0b001'0'0'1'0'1, false },
    { false, 0b001'0'1'1'0'1, false },
    { false, 0b001'1'0'1'0'1, false },
    { false, 0b001'1'1'1'0'1, false },
    { true,  0b001'0'0'1'0'1, true  },
    { true,  0b001'0'1'1'0'1, false },
    { true,  0b001'1'0'1'0'1, false },
    { true,  0b001'1'1'1'0'1, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, clearInterrupts, ValuesIn( clearInterrupts_TEST_CASES ) );

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::bind() works properly when
 *        the default local endpoint is used.
 */
TEST( bind, worksProperlyDefaultEndpoint )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       udp_port_allocator = Mock_Port_Allocator{};
    auto const socket_id          = Socket_ID::_6;

    auto socket = Socket{ network_stack, socket_id };

    auto const port = Port{ 44373 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( udp_port_allocator, allocate( Ref( driver ), Port{} ) ).WillOnce( Return( port ) );
    EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) );
    EXPECT_CALL( driver, read_sn_mr( socket_id ) ).WillOnce( Return( 0b0'0'1'0'0111 ) );
    EXPECT_CALL( driver, write_sn_mr( socket_id, 0b0'0'1'0'0010 ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0xC4 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x07 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x22 ) );

    socket.bind();

    EXPECT_EQ( socket.state(), Socket::State::BOUND );

    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 47906 ) );
    EXPECT_CALL( udp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::bind() works properly when
 *        binding to any address and an ephemeral port.
 */
TEST( bind, worksProperlyAnyAddressEphemeralPort )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       udp_port_allocator = Mock_Port_Allocator{};
    auto const socket_id          = Socket_ID::_1;

    auto socket = Socket{ network_stack, socket_id };

    auto const port = Port{ 1627 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( udp_port_allocator, allocate( Ref( driver ), Port{} ) ).WillOnce( Return( port ) );
    EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) );
    EXPECT_CALL( driver, read_sn_mr( socket_id ) ).WillOnce( Return( 0b1'1'0'1'1001 ) );
    EXPECT_CALL( driver, write_sn_mr( socket_id, 0b1'1'0'1'0010 ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0xB6 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x1E ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x22 ) );

    socket.bind( {} );

    EXPECT_EQ( socket.state(), Socket::State::BOUND );

    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 49082 ) );
    EXPECT_CALL( udp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::bind() works properly when
 *        binding to any address and a non-ephemeral port.
 */
TEST( bind, worksProperlyAnyAddressNonEphemeralPort )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       udp_port_allocator = Mock_Port_Allocator{};
    auto const socket_id          = Socket_ID::_3;

    auto socket = Socket{ network_stack, socket_id };

    auto const port = Port{ 1647 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( udp_port_allocator, allocate( Ref( driver ), port ) ).WillOnce( Return( port ) );
    EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) );
    EXPECT_CALL( driver, read_sn_mr( socket_id ) ).WillOnce( Return( 0b0'0'0'0'0110 ) );
    EXPECT_CALL( driver, write_sn_mr( socket_id, 0b0'0'0'0'0010 ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x30 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x53 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x22 ) );

    socket.bind( port );

    EXPECT_EQ( socket.state(), Socket::State::BOUND );

    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 28662 ) );
    EXPECT_CALL( udp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::bind() works properly when
 *        binding to a specific address and an ephemeral port.
 */
TEST( bind, worksProperlySpecificAddressEphemeralPort )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       udp_port_allocator = Mock_Port_Allocator{};
    auto const socket_id          = Socket_ID::_1;

    auto socket = Socket{ network_stack, socket_id };

    auto const address = Address{ { 29, 192, 225, 187 } };
    auto const port    = Port{ 25128 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( address.as_byte_array() ) );
    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( udp_port_allocator, allocate( Ref( driver ), Port{} ) ).WillOnce( Return( port ) );
    EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) );
    EXPECT_CALL( driver, read_sn_mr( socket_id ) ).WillOnce( Return( 0b1'0'1'0'0110 ) );
    EXPECT_CALL( driver, write_sn_mr( socket_id, 0b1'0'1'0'0010 ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x98 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x7C ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x22 ) );

    socket.bind( { address, {} } );

    EXPECT_EQ( socket.state(), Socket::State::BOUND );

    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 52719 ) );
    EXPECT_CALL( udp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::bind() works properly when
 *        binding to a specific address and a non-ephemeral port.
 */
TEST( bind, worksProperlySpecificAddressNonEphemeralPort )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       udp_port_allocator = Mock_Port_Allocator{};
    auto const socket_id          = Socket_ID::_2;

    auto socket = Socket{ network_stack, socket_id };

    auto const address = Address{ { 124, 185, 59, 120 } };
    auto const port    = Port{ 936 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( address.as_byte_array() ) );
    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( udp_port_allocator, allocate( Ref( driver ), port ) ).WillOnce( Return( port ) );
    EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) );
    EXPECT_CALL( driver, read_sn_mr( socket_id ) ).WillOnce( Return( 0b0'1'1'0'1'1'1'0 ) );
    EXPECT_CALL( driver, write_sn_mr( socket_id, 0b0'1'1'0'0010 ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x7F ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x30 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x22 ) );

    socket.bind( { address, port } );

    EXPECT_EQ( socket.state(), Socket::State::BOUND );

    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 47919 ) );
    EXPECT_CALL( udp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::local_endpoint() works
 *        properly.
 */
TEST( localEndpoint, worksProperly )
{
    auto       network_stack = Mock_Network_Stack{};
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_3;

    auto const socket = Socket{ network_stack, socket_id };

    auto const sipr    = Array<std::uint8_t, 4>{ 35, 34, 150, 16 };
    auto const sn_port = std::uint16_t{ 58154 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( sipr ) );
    EXPECT_CALL( driver, read_sn_port( socket_id ) ).WillOnce( Return( sn_port ) );

    auto const endpoint = socket.local_endpoint();

    EXPECT_TRUE( endpoint.address().is_ipv4() );
    EXPECT_EQ( endpoint.address().ipv4().as_byte_array(), sipr );
    EXPECT_EQ( endpoint.port().as_unsigned_integer(), sn_port );

    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::transmit() excessive message size
 *        error handling test case.
 */
struct transmitErrorHandlingExcessiveMessageSize_Test_Case {
    /**
     * \brief The socket buffer size.
     */
    Socket_Buffer_Size socket_buffer_size;

    /**
     * \brief Data transmission is in progress.
     */
    bool is_transmitting;

    /**
     * \brief The block of data to transmit in the datagram.
     */
    std::vector<std::uint8_t> data;
};

auto operator<<( std::ostream & stream, transmitErrorHandlingExcessiveMessageSize_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".socket_buffer_size = " << test_case.socket_buffer_size
                  << ", "
                  << ".is_transmitting = " << std::boolalpha << test_case.is_transmitting
                  << ", "
                  << ".data.size() = " << test_case.data.size()
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::transmit() excessive message size
 *        error handling test fixture.
 */
class transmitErrorHandlingExcessiveMessageSize :
    public TestWithParam<transmitErrorHandlingExcessiveMessageSize_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::transmit() properly handles
 *        a data block that is too large to transmit in a single datagram.
 */
TEST_P( transmitErrorHandlingExcessiveMessageSize, excessiveMessageSize )
{
    auto const test_case = GetParam();

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       udp_port_allocator = Mock_Port_Allocator{};
    auto const socket_id          = Socket_ID::_2;

    auto socket = Socket{ network_stack, socket_id, Socket::State::BOUND, test_case.is_transmitting };

    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );

    auto const result = socket.transmit(
        { Address{ { 169, 69, 94, 30 } }, 4160 },
        &*test_case.data.begin(),
        &*test_case.data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::EXCESSIVE_MESSAGE_SIZE );

    EXPECT_EQ( socket.state(), Socket::State::BOUND );
    EXPECT_EQ( socket.is_transmitting(), test_case.is_transmitting );

    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 60002 ) );
    EXPECT_CALL( udp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::transmit() excessive message size
 *        error handling test case.
 */
transmitErrorHandlingExcessiveMessageSize_Test_Case const transmitErrorHandlingExcessiveMessageSize_TEST_CASES[]{
    // clang-format off

    { Socket_Buffer_Size::_2_KiB,  false, std::vector<std::uint8_t>(  2049 ) },
    { Socket_Buffer_Size::_2_KiB,  true,  std::vector<std::uint8_t>(  2049 ) },
    { Socket_Buffer_Size::_4_KiB,  false, std::vector<std::uint8_t>(  4097 ) },
    { Socket_Buffer_Size::_4_KiB,  true,  std::vector<std::uint8_t>(  4097 ) },
    { Socket_Buffer_Size::_8_KiB,  false, std::vector<std::uint8_t>(  8193 ) },
    { Socket_Buffer_Size::_8_KiB,  true,  std::vector<std::uint8_t>(  8193 ) },
    { Socket_Buffer_Size::_16_KiB, false, std::vector<std::uint8_t>( 16385 ) },
    { Socket_Buffer_Size::_16_KiB, true,  std::vector<std::uint8_t>( 16385 ) },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, transmitErrorHandlingExcessiveMessageSize, ValuesIn( transmitErrorHandlingExcessiveMessageSize_TEST_CASES ) );

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::transmit() works properly
 *        when an in progress transmission is not complete.
 */
TEST( transmit, worksProperlyInProgressTransmissionNotComplete )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       udp_port_allocator = Mock_Port_Allocator{};
    auto const socket_id          = Socket_ID::_7;

    auto socket = Socket{ network_stack, socket_id, Socket::State::BOUND, true };

    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( Socket_Buffer_Size::_2_KiB ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( 0b000'0'0'1'0'1 ) );

    auto const data   = std::vector<std::uint8_t>{ 0xA7, 0x55, 0x34, 0x57 };
    auto const result = socket.transmit(
        { Address{ { 212, 123, 17, 36 } }, 22364 }, &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

    EXPECT_EQ( socket.state(), Socket::State::BOUND );
    EXPECT_TRUE( socket.is_transmitting() );

    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 27244 ) );
    EXPECT_CALL( udp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::transmit() insufficient transmit
 *        buffer capacity test fixture.
 */
class transmitInsufficientTransmitBufferCapacity : public TestWithParam<std::uint16_t> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::transmit() works properly
 *        when a transmission is not in progress and there is insufficient transmit buffer
 *        capacity.
 */
TEST_P( transmitInsufficientTransmitBufferCapacity, worksProperlyTransmissionNotInProgress )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       udp_port_allocator = Mock_Port_Allocator{};
    auto const socket_id          = Socket_ID::_0;

    auto socket = Socket{ network_stack, socket_id, Socket::State::BOUND };

    auto const sn_tx_fsr = GetParam();

    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( Socket_Buffer_Size::_8_KiB ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( sn_tx_fsr ) );

    auto const data   = std::vector<std::uint8_t>{ 0x83, 0xD2, 0x79, 0x52 };
    auto const result = socket.transmit(
        { Address{ { 21, 66, 133, 158 } }, 11969 }, &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

    EXPECT_EQ( socket.state(), Socket::State::BOUND );
    EXPECT_FALSE( socket.is_transmitting() );

    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 61054 ) );
    EXPECT_CALL( udp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::transmit() works properly
 *        when an in progress transmission is complete and there is insufficient transmit
 *        buffer capacity.
 */
TEST_P( transmitInsufficientTransmitBufferCapacity, worksProperlyInProgressTransmissionComplete )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       udp_port_allocator = Mock_Port_Allocator{};
    auto const socket_id          = Socket_ID::_3;

    auto socket = Socket{ network_stack, socket_id, Socket::State::BOUND, true };

    auto const sn_tx_fsr = GetParam();

    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( Socket_Buffer_Size::_4_KiB ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( 0b101'1'0'1'1'1 ) );
    EXPECT_CALL( driver, write_sn_ir( socket_id, 0b000'1'1'0'0'0 ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( sn_tx_fsr ) );

    auto const data   = std::vector<std::uint8_t>{ 0xBA, 0xF0, 0x03, 0xD0 };
    auto const result = socket.transmit(
        { Address{ { 180, 104, 3, 83 } }, 19276 }, &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

    EXPECT_EQ( socket.state(), Socket::State::BOUND );
    EXPECT_FALSE( socket.is_transmitting() );

    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 25283 ) );
    EXPECT_CALL( udp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

INSTANTIATE_TEST_SUITE_P( testCases, transmitInsufficientTransmitBufferCapacity, Values<std::uint16_t>( 0, 1, 3 ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::transmit() sufficient transmit
 *        buffer capacity test case.
 */
struct transmitSufficientTransmitBufferCapacity_Test_Case {
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

auto operator<<( std::ostream & stream, transmitSufficientTransmitBufferCapacity_Test_Case const & test_case )
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
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::transmit() sufficient transmit
 *        buffer capacity test fixture.
 */
class transmitSufficientTransmitBufferCapacity :
    public TestWithParam<transmitSufficientTransmitBufferCapacity_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::transmit() works properly
 *        when a transmission is not in progress and there is sufficient transmit buffer
 *        capacity.
 */
TEST_P( transmitSufficientTransmitBufferCapacity, worksProperlyTransmissionNotInProgress )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       udp_port_allocator = Mock_Port_Allocator{};
    auto const socket_id          = Socket_ID::_5;

    auto socket = Socket{ network_stack, socket_id, Socket::State::BOUND };

    auto const endpoint = Endpoint{ Address{ { 104, 232, 44, 135 } }, 46090 };
    auto const data     = std::vector<std::uint8_t>{ 0xCB, 0x0F, 0x96, 0xC6 };

    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( test_case.sn_tx_fsr ) );
    EXPECT_CALL( driver, write_sn_dipr( socket_id, endpoint.address().ipv4().as_byte_array() ) );
    EXPECT_CALL( driver, write_sn_dport( socket_id, endpoint.port().as_unsigned_integer() ) );
    EXPECT_CALL( driver, read_sn_tx_wr( socket_id ) ).WillOnce( Return( test_case.sn_tx_wr_initial ) );
    EXPECT_CALL( driver, write_tx_buffer( socket_id, test_case.sn_tx_wr_initial, data ) );
    EXPECT_CALL( driver, write_sn_tx_wr( socket_id, test_case.sn_tx_wr_final ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x20 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x33 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

    EXPECT_FALSE( socket.transmit( endpoint, &*data.begin(), &*data.end() ).is_error() );

    EXPECT_EQ( socket.state(), Socket::State::BOUND );
    EXPECT_TRUE( socket.is_transmitting() );

    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 64208 ) );
    EXPECT_CALL( udp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::transmit() works properly
 *        when an in progress transmission is complete and there is sufficient transmit
 *        buffer capacity.
 */
TEST_P( transmitSufficientTransmitBufferCapacity, worksProperlyInProgressTransmissionComplete )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       udp_port_allocator = Mock_Port_Allocator{};
    auto const socket_id          = Socket_ID::_1;

    auto socket = Socket{ network_stack, socket_id, Socket::State::BOUND, true };

    auto const endpoint = Endpoint{ Address{ { 123, 151, 28, 128 } }, 14986 };
    auto const data     = std::vector<std::uint8_t>{ 0x00, 0x11, 0xDB, 0xB5 };

    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( 0b100'0'1'0'1'0 ) );
    EXPECT_CALL( driver, write_sn_ir( socket_id, 0b000'1'1'0'0'0 ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( test_case.sn_tx_fsr ) );
    EXPECT_CALL( driver, write_sn_dipr( socket_id, endpoint.address().ipv4().as_byte_array() ) );
    EXPECT_CALL( driver, write_sn_dport( socket_id, endpoint.port().as_unsigned_integer() ) );
    EXPECT_CALL( driver, read_sn_tx_wr( socket_id ) ).WillOnce( Return( test_case.sn_tx_wr_initial ) );
    EXPECT_CALL( driver, write_tx_buffer( socket_id, test_case.sn_tx_wr_initial, data ) );
    EXPECT_CALL( driver, write_sn_tx_wr( socket_id, test_case.sn_tx_wr_final ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x20 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0xC0 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

    EXPECT_FALSE( socket.transmit( endpoint, &*data.begin(), &*data.end() ).is_error() );

    EXPECT_EQ( socket.state(), Socket::State::BOUND );
    EXPECT_TRUE( socket.is_transmitting() );

    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 62841 ) );
    EXPECT_CALL( udp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::transmit() sufficient transmit
 *        buffer capacity test cases.
 */
transmitSufficientTransmitBufferCapacity_Test_Case const transmitSufficientTransmitBufferCapacity_TEST_CASES[]{
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
 * \brief verify picolibrary::WIZnet::W5500::IP::UDP::Socket::receive() works properly
 *        when the receive buffer is empty.
 */
TEST( receive, worksProperlyReceiveBufferEmpty )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       udp_port_allocator = Mock_Port_Allocator{};
    auto const socket_id          = Socket_ID::_3;

    auto socket = Socket{ network_stack, socket_id, Socket::State::BOUND };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( Socket_Buffer_Size::_2_KiB ) );
    EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) ).WillOnce( Return( 0 ) );

    auto       data   = std::vector<std::uint8_t>( 4 );
    auto const result = socket.receive( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

    EXPECT_EQ( socket.state(), Socket::State::BOUND );

    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 59824 ) );
    EXPECT_CALL( udp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::receive() receive data test case.
 */
struct receiveData_Test_Case {
    /**
     * \brief The socket buffer size.
     */
    Socket_Buffer_Size socket_buffer_size;

    /**
     * \brief The SN_RX_RSR register value.
     */
    std::uint16_t sn_rx_rsr;

    /**
     * \brief The datagram info data size value.
     */
    std::uint16_t data_size;

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
                  << ".socket_buffer_size = " << test_case.socket_buffer_size
                  << ", "
                  << ".sn_rx_rsr = " << test_case.sn_rx_rsr
                  << ", "
                  << ".data_size = " << test_case.data_size
                  << ", "
                  << ".sn_rx_rd_initial = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint16_t>::digits / 4 ) << std::setfill( '0' ) << test_case.sn_rx_rd_initial
                  << ", "
                  << ".sn_rx_rd_final = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint16_t>::digits / 4 ) << std::setfill( '0' ) << test_case.sn_rx_rd_final
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::receive() receive all data test
 *        fixture.
 */
class receiveAllData : public TestWithParam<receiveData_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::receive() works properly
 *        when all datagram data can be read.
 */
TEST_P( receiveAllData, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       udp_port_allocator = Mock_Port_Allocator{};
    auto const socket_id          = Socket_ID::_6;

    auto socket = Socket{ network_stack, socket_id, Socket::State::BOUND };

    auto const source_address = Address{ { 141, 157, 28, 149 } };
    auto const source_port    = Port{ 35861 };
    auto const data_expected  = std::vector<std::uint8_t>{ 0x7A, 0xBF, 0xA1 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
    EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) ).WillOnce( Return( test_case.sn_rx_rsr ) );
    EXPECT_CALL( driver, read_sn_rx_rd( socket_id ) ).WillOnce( Return( test_case.sn_rx_rd_initial ) );
    EXPECT_CALL( driver, read_rx_buffer( socket_id, test_case.sn_rx_rd_initial, _ ) )
        .WillOnce( Return( to_vector( source_address, source_port, test_case.data_size ) ) );
    EXPECT_CALL( driver, read_rx_buffer( socket_id, test_case.sn_rx_rd_initial + 8, _ ) )
        .WillOnce( Return( std::vector<std::uint8_t>{
            data_expected.begin(), data_expected.begin() + test_case.data_size } ) );
    EXPECT_CALL( driver, write_sn_rx_rd( socket_id, test_case.sn_rx_rd_final ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x40 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x24 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

    auto       data   = std::vector<std::uint8_t>( 4 );
    auto const result = socket.receive( &*data.begin(), &*data.end() );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value().endpoint, ( Endpoint{ source_address, source_port } ) );
    EXPECT_EQ( result.value().end, &*data.begin() + test_case.data_size );

    EXPECT_EQ(
        ( std::vector<std::uint8_t>{ &*data.begin(), result.value().end } ),
        ( std::vector<std::uint8_t>{ data_expected.begin(),
                                     data_expected.begin() + test_case.data_size } ) );

    EXPECT_EQ( socket.state(), Socket::State::BOUND );

    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 10673 ) );
    EXPECT_CALL( udp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::receive() receive all data test
 *        fixture.
 */
receiveData_Test_Case const receiveAllData_TEST_CASES[]{
    // clang-format off

    { Socket_Buffer_Size::_2_KiB,    8, 0, 0x1325, 0x132D },
    { Socket_Buffer_Size::_2_KiB,    8, 0, 0xFFF8, 0x0000 },
    { Socket_Buffer_Size::_2_KiB, 1509, 0, 0x1E11, 0x1E19 },
    { Socket_Buffer_Size::_2_KiB, 1509, 0, 0xFFF8, 0x0000 },
    { Socket_Buffer_Size::_2_KiB, 2048, 0, 0x90CD, 0x90D5 },
    { Socket_Buffer_Size::_2_KiB, 2048, 0, 0xFFF9, 0x0001 },
    { Socket_Buffer_Size::_2_KiB,    9, 1, 0xF71F, 0xF728 },
    { Socket_Buffer_Size::_2_KiB,    9, 1, 0xFFFB, 0x0004 },
    { Socket_Buffer_Size::_2_KiB,  867, 1, 0xB292, 0xB29B },
    { Socket_Buffer_Size::_2_KiB,  867, 1, 0xFFF8, 0x0001 },
    { Socket_Buffer_Size::_2_KiB, 2048, 1, 0xDFC0, 0xDFC9 },
    { Socket_Buffer_Size::_2_KiB, 2048, 1, 0xFFFF, 0x0008 },
    { Socket_Buffer_Size::_2_KiB,   11, 3, 0x5310, 0x531B },
    { Socket_Buffer_Size::_2_KiB,   11, 3, 0xFFFA, 0x0005 },
    { Socket_Buffer_Size::_2_KiB, 1253, 3, 0x97AF, 0x97BA },
    { Socket_Buffer_Size::_2_KiB, 1253, 3, 0xFFF9, 0x0004 },
    { Socket_Buffer_Size::_2_KiB, 2048, 3, 0xEDB7, 0xEDC2 },
    { Socket_Buffer_Size::_2_KiB, 2048, 3, 0xFFFA, 0x0005 },

    { Socket_Buffer_Size::_4_KiB,    8, 0, 0xFDB1, 0xFDB9 },
    { Socket_Buffer_Size::_4_KiB,    8, 0, 0xFFF9, 0x0001 },
    { Socket_Buffer_Size::_4_KiB, 3319, 0, 0x393F, 0x3947 },
    { Socket_Buffer_Size::_4_KiB, 3319, 0, 0xFFFD, 0x0005 },
    { Socket_Buffer_Size::_4_KiB, 4096, 0, 0xA6ED, 0xA6F5 },
    { Socket_Buffer_Size::_4_KiB, 4096, 0, 0xFFFF, 0x0007 },
    { Socket_Buffer_Size::_4_KiB,    9, 1, 0x7C8C, 0x7C95 },
    { Socket_Buffer_Size::_4_KiB,    9, 1, 0xFFFF, 0x0008 },
    { Socket_Buffer_Size::_4_KiB, 2076, 1, 0xB192, 0xB19B },
    { Socket_Buffer_Size::_4_KiB, 2076, 1, 0xFFF7, 0x0000 },
    { Socket_Buffer_Size::_4_KiB, 4096, 1, 0xDCF7, 0xDD00 },
    { Socket_Buffer_Size::_4_KiB, 4096, 1, 0xFFFC, 0x0005 },
    { Socket_Buffer_Size::_4_KiB,   11, 3, 0x57DE, 0x57E9 },
    { Socket_Buffer_Size::_4_KiB,   11, 3, 0xFFF8, 0x0003 },
    { Socket_Buffer_Size::_4_KiB, 1644, 3, 0x1D52, 0x1D5D },
    { Socket_Buffer_Size::_4_KiB, 1644, 3, 0xFFFA, 0x0005 },
    { Socket_Buffer_Size::_4_KiB, 4096, 3, 0xA8D5, 0xA8E0 },
    { Socket_Buffer_Size::_4_KiB, 4096, 3, 0xFFF9, 0x0004 },

    { Socket_Buffer_Size::_8_KiB,    8, 0, 0x7552, 0x755A },
    { Socket_Buffer_Size::_8_KiB,    8, 0, 0xFFFB, 0x0003 },
    { Socket_Buffer_Size::_8_KiB, 4211, 0, 0x06CF, 0x06D7 },
    { Socket_Buffer_Size::_8_KiB, 4211, 0, 0xFFFE, 0x0006 },
    { Socket_Buffer_Size::_8_KiB, 8192, 0, 0xD4E9, 0xD4F1 },
    { Socket_Buffer_Size::_8_KiB, 8192, 0, 0xFFFF, 0x0007 },
    { Socket_Buffer_Size::_8_KiB,    9, 1, 0x1261, 0x126A },
    { Socket_Buffer_Size::_8_KiB,    9, 1, 0xFFF7, 0x0000 },
    { Socket_Buffer_Size::_8_KiB, 2750, 1, 0xC9DD, 0xC9E6 },
    { Socket_Buffer_Size::_8_KiB, 2750, 1, 0xFFFB, 0x0004 },
    { Socket_Buffer_Size::_8_KiB, 8192, 1, 0xDBF3, 0xDBFC },
    { Socket_Buffer_Size::_8_KiB, 8192, 1, 0xFFFD, 0x0006 },
    { Socket_Buffer_Size::_8_KiB,   11, 3, 0xEC34, 0xEC3F },
    { Socket_Buffer_Size::_8_KiB,   11, 3, 0xFFFB, 0x0006 },
    { Socket_Buffer_Size::_8_KiB, 8121, 3, 0x5405, 0x5410 },
    { Socket_Buffer_Size::_8_KiB, 8121, 3, 0xFFFA, 0x0005 },
    { Socket_Buffer_Size::_8_KiB, 8192, 3, 0x5B8A, 0x5B95 },
    { Socket_Buffer_Size::_8_KiB, 8192, 3, 0xFFFC, 0x0007 },

    { Socket_Buffer_Size::_16_KiB,     8, 0, 0x5B03, 0x5B0B },
    { Socket_Buffer_Size::_16_KiB,     8, 0, 0xFFFB, 0x0003 },
    { Socket_Buffer_Size::_16_KiB,  2993, 0, 0xB30C, 0xB314 },
    { Socket_Buffer_Size::_16_KiB,  2993, 0, 0xFFFE, 0x0006 },
    { Socket_Buffer_Size::_16_KiB, 16384, 0, 0x820A, 0x8212 },
    { Socket_Buffer_Size::_16_KiB, 16384, 0, 0xFFFA, 0x0002 },
    { Socket_Buffer_Size::_16_KiB,     9, 1, 0x3049, 0x3052 },
    { Socket_Buffer_Size::_16_KiB,     9, 1, 0xFFFC, 0x0005 },
    { Socket_Buffer_Size::_16_KiB, 15453, 1, 0x8A35, 0x8A3E },
    { Socket_Buffer_Size::_16_KiB, 15453, 1, 0xFFFE, 0x0007 },
    { Socket_Buffer_Size::_16_KiB, 16384, 1, 0xE38B, 0xE394 },
    { Socket_Buffer_Size::_16_KiB, 16384, 1, 0xFFF9, 0x0002 },
    { Socket_Buffer_Size::_16_KiB,    11, 3, 0xDCC3, 0xDCCE },
    { Socket_Buffer_Size::_16_KiB,    11, 3, 0xFFFF, 0x000A },
    { Socket_Buffer_Size::_16_KiB,  3102, 3, 0xCD80, 0xCD8B },
    { Socket_Buffer_Size::_16_KiB,  3102, 3, 0xFFF6, 0x0001 },
    { Socket_Buffer_Size::_16_KiB, 16384, 3, 0x70CA, 0x70D5 },
    { Socket_Buffer_Size::_16_KiB, 16384, 3, 0xFFF5, 0x0000 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, receiveAllData, ValuesIn( receiveAllData_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::receive() receive some data test
 *        fixture.
 */
class receiveSomeData : public TestWithParam<receiveData_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::receive() works properly
 *        when only some datagram data can be read.
 */
TEST_P( receiveSomeData, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       udp_port_allocator = Mock_Port_Allocator{};
    auto const socket_id          = Socket_ID::_7;

    auto socket = Socket{ network_stack, socket_id, Socket::State::BOUND };

    auto const source_address = Address{ { 5, 4, 137, 69 } };
    auto const source_port    = Port{ 28114 };
    auto const data_expected  = std::vector<std::uint8_t>{ 0x55, 0xB9, 0x4B, 0xA4 };

    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
    EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) ).WillOnce( Return( test_case.sn_rx_rsr ) );
    EXPECT_CALL( driver, read_sn_rx_rd( socket_id ) ).WillOnce( Return( test_case.sn_rx_rd_initial ) );
    EXPECT_CALL( driver, read_rx_buffer( socket_id, test_case.sn_rx_rd_initial, _ ) )
        .WillOnce( Return( to_vector( source_address, source_port, test_case.data_size ) ) );
    EXPECT_CALL( driver, read_rx_buffer( socket_id, test_case.sn_rx_rd_initial + 8, _ ) ).WillOnce( Return( data_expected ) );
    EXPECT_CALL( driver, write_sn_rx_rd( socket_id, test_case.sn_rx_rd_final ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x40 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0xEA ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

    auto       data   = std::vector<std::uint8_t>( data_expected.size() );
    auto const result = socket.receive( &*data.begin(), &*data.end() );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value().endpoint, ( Endpoint{ source_address, source_port } ) );
    EXPECT_EQ( result.value().end, &*data.end() );

    EXPECT_EQ( data, data_expected );

    EXPECT_EQ( socket.state(), Socket::State::BOUND );

    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 22503 ) );
    EXPECT_CALL( udp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::UDP::Socket::receive() receive some data test
 *        fixture.
 */
receiveData_Test_Case const receiveSomeData_TEST_CASES[]{
    // clang-format off

    { Socket_Buffer_Size::_2_KiB,   13,    5, 0x01C6, 0x01D3 },
    { Socket_Buffer_Size::_2_KiB,   13,    5, 0xFFFD, 0x000A },
    { Socket_Buffer_Size::_2_KiB, 1926,    5, 0xE080, 0xE08D },
    { Socket_Buffer_Size::_2_KiB, 1926,    5, 0xFFFB, 0x0008 },
    { Socket_Buffer_Size::_2_KiB, 2048,    5, 0x14F3, 0x1500 },
    { Socket_Buffer_Size::_2_KiB, 2048,    5, 0xFFFE, 0x000B },
    { Socket_Buffer_Size::_2_KiB, 1154, 1146, 0x18A4, 0x1D26 },
    { Socket_Buffer_Size::_2_KiB, 1154, 1146, 0xFE64, 0x02E6 },
    { Socket_Buffer_Size::_2_KiB, 1886, 1146, 0x8BD4, 0x9056 },
    { Socket_Buffer_Size::_2_KiB, 1886, 1146, 0xFE5C, 0x02DE },
    { Socket_Buffer_Size::_2_KiB, 2048, 1146, 0xA4A7, 0xA929 },
    { Socket_Buffer_Size::_2_KiB, 2048, 1146, 0xFEAB, 0x032D },
    { Socket_Buffer_Size::_2_KiB, 2048, 2040, 0x3687, 0x3E87 },
    { Socket_Buffer_Size::_2_KiB, 2048, 2040, 0xFEE8, 0x06E8 },

    { Socket_Buffer_Size::_4_KiB,   13,    5, 0xC11E, 0xC12B },
    { Socket_Buffer_Size::_4_KiB,   13,    5, 0xFFF3, 0x0000 },
    { Socket_Buffer_Size::_4_KiB, 2979,    5, 0x6AC5, 0x6AD2 },
    { Socket_Buffer_Size::_4_KiB, 2979,    5, 0xFFFD, 0x000A },
    { Socket_Buffer_Size::_4_KiB, 4096,    5, 0x8E97, 0x8EA4 },
    { Socket_Buffer_Size::_4_KiB, 4096,    5, 0xFFF7, 0x0004 },
    { Socket_Buffer_Size::_4_KiB, 1154, 1146, 0x2099, 0x251B },
    { Socket_Buffer_Size::_4_KiB, 1154, 1146, 0xFE5C, 0x02DE },
    { Socket_Buffer_Size::_4_KiB, 2370, 1146, 0x2EA3, 0x3325 },
    { Socket_Buffer_Size::_4_KiB, 2370, 1146, 0xFC32, 0x00B4 },
    { Socket_Buffer_Size::_4_KiB, 4096, 1146, 0xDFFE, 0xE480 },
    { Socket_Buffer_Size::_4_KiB, 4096, 1146, 0xFC38, 0x00BA },
    { Socket_Buffer_Size::_4_KiB, 4096, 4088, 0x41D9, 0x51D9 },
    { Socket_Buffer_Size::_4_KiB, 4096, 4088, 0xFA26, 0x0A26 },

    { Socket_Buffer_Size::_8_KiB,   13,    5, 0x3CF8, 0x3D05 },
    { Socket_Buffer_Size::_8_KiB,   13,    5, 0xFFF3, 0x0000 },
    { Socket_Buffer_Size::_8_KiB, 1401,    5, 0x20F0, 0x20FD },
    { Socket_Buffer_Size::_8_KiB, 1401,    5, 0xFFF4, 0x0001 },
    { Socket_Buffer_Size::_8_KiB, 8192,    5, 0x14B4, 0x14C1 },
    { Socket_Buffer_Size::_8_KiB, 8192,    5, 0xFFFA, 0x0007 },
    { Socket_Buffer_Size::_8_KiB, 1154, 1146, 0xAB46, 0xAFC8 },
    { Socket_Buffer_Size::_8_KiB, 1154, 1146, 0xFC4B, 0x00CD },
    { Socket_Buffer_Size::_8_KiB, 2884, 1146, 0xCC26, 0xD0A8 },
    { Socket_Buffer_Size::_8_KiB, 2884, 1146, 0xFB9C, 0x001E },
    { Socket_Buffer_Size::_8_KiB, 8192, 1146, 0xD6BE, 0xDB40 },
    { Socket_Buffer_Size::_8_KiB, 8192, 1146, 0xFE3A, 0x02BC },
    { Socket_Buffer_Size::_8_KiB, 8192, 8184, 0x6992, 0x8992 },
    { Socket_Buffer_Size::_8_KiB, 8192, 8184, 0xEF62, 0x0F62 },

    { Socket_Buffer_Size::_16_KiB,    13,     5, 0x3C31, 0x3C3E },
    { Socket_Buffer_Size::_16_KiB,    13,     5, 0xFFF6, 0x0003 },
    { Socket_Buffer_Size::_16_KiB,  4569,     5, 0x2A42, 0x2A4F },
    { Socket_Buffer_Size::_16_KiB,  4569,     5, 0xFFF6, 0x0003 },
    { Socket_Buffer_Size::_16_KiB, 16384,     5, 0xD605, 0xD612 },
    { Socket_Buffer_Size::_16_KiB, 16384,     5, 0xFFFB, 0x0008 },
    { Socket_Buffer_Size::_16_KiB,  1154,  1146, 0xE16D, 0xE5EF },
    { Socket_Buffer_Size::_16_KiB,  1154,  1146, 0xFFFA, 0x047C },
    { Socket_Buffer_Size::_16_KiB,  9814,  1146, 0x4F45, 0x53C7 },
    { Socket_Buffer_Size::_16_KiB,  9814,  1146, 0xFF54, 0x03D6 },
    { Socket_Buffer_Size::_16_KiB, 16384,  1146, 0x2FA8, 0x342A },
    { Socket_Buffer_Size::_16_KiB, 16384,  1146, 0xFE2C, 0x02AE },
    { Socket_Buffer_Size::_16_KiB, 16384, 16376, 0x2037, 0x6037 },
    { Socket_Buffer_Size::_16_KiB, 16384, 16376, 0xED6E, 0x2D6E },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, receiveSomeData, ValuesIn( receiveSomeData_TEST_CASES ) );

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::close() works properly when
 *        the socket is in the
 *        picolibrary::WIZnet::W5500::IP::UDP::Socket::State::UNINITIALIZED state.
 */
TEST( close, worksProperlyUninitialized )
{
    auto socket = Socket{};

    socket.close();
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::close() works properly when
 *        the socket is in the
 *        picolibrary::WIZnet::W5500::IP::UDP::Socket::State::INITIALIZED state.
 */
TEST( close, worksProperlyInitialized )
{
    auto       network_stack = Mock_Network_Stack{};
    auto const socket_id     = Socket_ID::_2;

    auto socket = Socket{ network_stack, socket_id };

    EXPECT_CALL( network_stack, deallocate_socket( _, socket_id ) );

    socket.close();
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::UDP::Socket::close() works properly when
 *        the socket is in the picolibrary::WIZnet::W5500::IP::UDP::Socket::State::BOUND
 *        state.
 */
TEST( close, worksProperlyBound )
{
    auto const in_sequence = InSequence{};

    auto       network_stack      = Mock_Network_Stack{};
    auto       driver             = Mock_Driver{};
    auto       udp_port_allocator = Mock_Port_Allocator{};
    auto const socket_id          = Socket_ID::_2;

    auto socket = Socket{ network_stack, socket_id, Socket::State::BOUND };

    auto const sn_port = std::uint16_t{ 32834 };

    EXPECT_CALL( network_stack, udp_port_allocator( _ ) ).WillOnce( ReturnRef( udp_port_allocator ) );
    EXPECT_CALL( network_stack, driver( _ ) ).WillOnce( ReturnRef( driver ) );
    EXPECT_CALL( driver, read_sn_port( socket_id ) ).WillOnce( Return( sn_port ) );
    EXPECT_CALL( udp_port_allocator, deallocate( Port{ sn_port } ) );
    EXPECT_CALL( network_stack, deallocate_socket( _, socket_id ) );

    socket.close();
}

/**
 * \brief Execute the picolibrary::WIZnet::W5500::IP::UDP::Socket automated tests.
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

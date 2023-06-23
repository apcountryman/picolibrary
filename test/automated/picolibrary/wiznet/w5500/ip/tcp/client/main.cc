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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client automated test program.
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
#include "picolibrary/ipv4.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/ip/tcp.h"
#include "picolibrary/testing/automated/wiznet/w5500.h"
#include "picolibrary/testing/automated/wiznet/w5500/ip.h"
#include "picolibrary/testing/automated/wiznet/w5500/ip/network_stack.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/ip/tcp.h"

namespace {

using ::picolibrary::Array;
using ::picolibrary::Generic_Error;
using ::picolibrary::IP::TCP::Endpoint;
using ::picolibrary::IP::TCP::Port;
using ::picolibrary::IPv4::Address;
using ::picolibrary::Testing::Automated::WIZnet::W5500::Mock_Driver;
using ::picolibrary::Testing::Automated::WIZnet::W5500::IP::Mock_Network_Stack;
using ::picolibrary::Testing::Automated::WIZnet::W5500::IP::Mock_Port_Allocator;
using ::picolibrary::WIZnet::W5500::No_Delayed_ACK_Usage;
using ::picolibrary::WIZnet::W5500::Socket_Buffer_Size;
using ::picolibrary::WIZnet::W5500::Socket_ID;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Ref;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::TestWithParam;
using ::testing::Values;
using ::testing::ValuesIn;

using Client = ::picolibrary::WIZnet::W5500::IP::TCP::Client<Mock_Driver, Mock_Network_Stack>;

} // namespace

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::Client() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const client = Client{};

    ASSERT_EQ( client.state(), Client::State::UNINITIALIZED );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::Client( Driver &,
 *        picolibrary::WIZnet::W5500::Socket_ID, Network_Stack & ) test case.
 */
struct constructor_Test_Case {
    /**
     * \brief The socket's socket ID.
     */
    Socket_ID socket_id;

    /**
     * \brief The socket's socket interrupt mask.
     */
    std::uint8_t socket_interrupt_mask;
};

auto operator<<( std::ostream & stream, constructor_Test_Case const & test_case ) -> std::ostream &
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::Client( Driver &,
 *        picolibrary::WIZnet::W5500::Socket_ID, Network_Stack & ) test fixture.
 */
class constructor : public TestWithParam<constructor_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::Client( Driver &,
 *        picolibrary::WIZnet::W5500::Socket_ID, Network_Stack & ) works properly.
 */
TEST_P( constructor, worksProperly )
{
    auto const test_case = GetParam();

    auto driver        = Mock_Driver{};
    auto network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, test_case.socket_id, network_stack };

    EXPECT_EQ( client.state(), Client::State::INITIALIZED );
    EXPECT_EQ( client.socket_id(), test_case.socket_id );
    EXPECT_EQ( client.socket_interrupt_mask(), test_case.socket_interrupt_mask );
    EXPECT_FALSE( client.is_transmitting() );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::Client( Driver &,
 *        picolibrary::WIZnet::W5500::Socket_ID, Network_Stack & ) test cases.
 */
constructor_Test_Case const constructor_TEST_CASES[]{
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

INSTANTIATE_TEST_SUITE_P( testCases, constructor, ValuesIn( constructor_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::configure_no_delayed_ack_usage()
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::configure_no_delayed_ack_usage()
 *        test fixture.
 */
class configureNoDelayedACKUsage : public TestWithParam<configureNoDelayedACKUsage_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Client::configure_no_delayed_ack_usage()
 *        works properly.
 */
TEST_P( configureNoDelayedACKUsage, worksProperly )
{
    auto const test_case = GetParam();

    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_4;
    auto       network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, socket_id, network_stack };

    EXPECT_CALL( driver, write_sn_mr( socket_id, test_case.sn_mr ) );

    client.configure_no_delayed_ack_usage( test_case.no_delayed_ack_usage_configuration );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::configure_no_delayed_ack_usage()
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::no_delayed_ack_usage_configuration()
 *        test case.
 */
struct noDelayedACKUsageConfiguration_Test_Case {
    /**
     * \brief The SN_MR register value.
     */
    std::uint8_t sn_mr;

    /**
     * \brief The socket's no delayed ACK usage configuration.
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::no_delayed_ack_usage_configuration()
 *        test fixture.
 */
class noDelayedACKUsageConfiguration : public TestWithParam<noDelayedACKUsageConfiguration_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::TCP::Client::no_delayed_ack_usage_configuration()
 *        works properly.
 */
TEST_P( noDelayedACKUsageConfiguration, worksProperly )
{
    auto const test_case = GetParam();

    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_5;
    auto       network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, socket_id, network_stack };

    EXPECT_CALL( driver, read_sn_mr( socket_id ) ).WillOnce( Return( test_case.sn_mr ) );

    ASSERT_EQ( client.no_delayed_ack_usage_configuration(), test_case.no_delayed_ack_usage_configuration );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::no_delayed_ack_usage_configuration()
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
 *        picolibrary::WIZnet::W5500::IP::TCP::Client::configure_maximum_segment_size()
 *        works properly.
 */
TEST( configureMaximumSegmentSize, worksProperly )
{
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_7;
    auto       network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, socket_id, network_stack };

    auto const maximum_segment_size = std::uint16_t{ 0xD57F };

    EXPECT_CALL( driver, write_sn_mssr( socket_id, maximum_segment_size ) );

    client.configure_maximum_segment_size( maximum_segment_size );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::maximum_segment_size() works
 *        properly.
 */
TEST( maximumSegmentSize, worksProperly )
{
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_2;
    auto       network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, socket_id, network_stack };

    auto const sn_mssr = std::uint16_t{ 0x73D3 };

    EXPECT_CALL( driver, read_sn_mssr( socket_id ) ).WillOnce( Return( sn_mssr ) );

    ASSERT_EQ( client.maximum_segment_size(), sn_mssr );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::configure_time_to_live()
 *        works properly.
 */
TEST( configureTimeToLive, worksProperly )
{
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_6;
    auto       network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, socket_id, network_stack };

    auto const time_to_live = std::uint8_t{ 0x82 };

    EXPECT_CALL( driver, write_sn_ttl( socket_id, time_to_live ) );

    client.configure_time_to_live( time_to_live );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::time_to_live() works
 *        properly.
 */
TEST( timeToLive, worksProperly )
{
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_0;
    auto       network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, socket_id, network_stack };

    auto const sn_ttl = std::uint8_t{ 0x88 };

    EXPECT_CALL( driver, read_sn_ttl( socket_id ) ).WillOnce( Return( sn_ttl ) );

    ASSERT_EQ( client.time_to_live(), sn_ttl );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::configure_keepalive_period()
 *        works properly.
 */
TEST( configureKeepalivePeriod, worksProperly )
{
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_5;
    auto       network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, socket_id, network_stack };

    auto const keepalive_period = std::uint8_t{ 0x32 };

    EXPECT_CALL( driver, write_sn_kpalvtr( socket_id, keepalive_period ) );

    client.configure_keepalive_period( keepalive_period );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::keepalive_period() works
 *        properly.
 */
TEST( keepalivePeriod, worksProperly )
{
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_0;
    auto       network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, socket_id, network_stack };

    auto const sn_kpalvtr = std::uint8_t{ 0x9C };

    EXPECT_CALL( driver, read_sn_kpalvtr( socket_id ) ).WillOnce( Return( sn_kpalvtr ) );

    ASSERT_EQ( client.keepalive_period(), sn_kpalvtr );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::enable_interrupts() works
 *        properly.
 */
TEST( enableInterrupts, worksProperly )
{
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_3;
    auto       network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, socket_id, network_stack };

    auto const mask = std::uint8_t{ 0b0101'1110 };

    EXPECT_CALL( driver, read_sn_imr( socket_id ) ).WillOnce( Return( 0b0011'1011 ) );
    EXPECT_CALL( driver, write_sn_imr( socket_id, 0b0111'1111 ) );

    client.enable_interrupts( mask );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::disable_interrupts(
 *        std::uint8_t ) works properly.
 */
TEST( disableInterrupts, worksProperly )
{
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_2;
    auto       network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, socket_id, network_stack };

    auto const mask = std::uint8_t{ 0b0101'1001 };

    EXPECT_CALL( driver, read_sn_imr( socket_id ) ).WillOnce( Return( 0b0011'1111 ) );
    EXPECT_CALL( driver, write_sn_imr( socket_id, 0b0010'0110 ) );

    client.disable_interrupts( mask );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::disable_interrupts() works
 *        properly.
 */
TEST( disableAllInterrupts, worksProperly )
{
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_1;
    auto       network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, socket_id, network_stack };

    EXPECT_CALL( driver, write_sn_imr( socket_id, 0x00 ) );

    client.disable_interrupts();

    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::enabled_interrupts() works
 *        properly.
 */
TEST( enabledInterrupts, worksProperly )
{
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_1;
    auto       network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, socket_id, network_stack };

    auto const sn_imr = std::uint8_t{ 0b00100111 };

    EXPECT_CALL( driver, read_sn_imr( socket_id ) ).WillOnce( Return( sn_imr ) );

    ASSERT_EQ( client.enabled_interrupts(), sn_imr );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::interrupt_context() works
 *        properly.
 */
TEST( interruptContext, worksProperly )
{
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_5;
    auto       network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, socket_id, network_stack };

    auto const sn_ir = std::uint8_t{ 0b11010001 };

    EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( sn_ir ) );

    ASSERT_EQ( client.interrupt_context(), sn_ir );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::clear_interrupts() test case.
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::clear_interrupts() test fixture.
 */
class clearInterrupts : public TestWithParam<clearInterrupts_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::clear_interrupts() works
 *        properly.
 */
TEST_P( clearInterrupts, worksProperly )
{
    auto const test_case = GetParam();

    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_4;
    auto       network_stack = Mock_Network_Stack{};

    auto client = Client{
        Client::State::INITIALIZED, driver, socket_id, network_stack, test_case.is_transmitting_initial
    };

    EXPECT_CALL( driver, write_sn_ir( socket_id, test_case.mask ) );

    client.clear_interrupts( test_case.mask );

    ASSERT_EQ( client.is_transmitting(), test_case.is_transmitting_final );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::clear_interrupts() test cases.
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
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::bind() works properly when
 *        the default local endpoint is used.
 */
TEST( bind, worksProperlyDefaultEndpoint )
{
    auto const in_sequence = InSequence{};

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_6;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ driver, socket_id, network_stack };

    auto const port = Port{ 10952 };

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( tcp_port_allocator, allocate( Ref( driver ), Port{} ) ).WillOnce( Return( port ) );
    EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) );
    EXPECT_CALL( driver, read_sn_mr( socket_id ) ).WillOnce( Return( 0b0'1'1'0'0010 ) );
    EXPECT_CALL( driver, write_sn_mr( socket_id, 0b0'1'1'0'0001 ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0xAE ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0xC0 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x13 ) );

    client.bind();

    ASSERT_EQ( client.state(), Client::State::BOUND );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 17570 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::bind() works properly when
 *        binding to any address and an ephemeral port.
 */
TEST( bind, worksProperlyAnyAddressEphemeralPort )
{
    auto const in_sequence = InSequence{};

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_3;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ driver, socket_id, network_stack };

    auto const port = Port{ 46683 };

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( tcp_port_allocator, allocate( Ref( driver ), Port{} ) ).WillOnce( Return( port ) );
    EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) );
    EXPECT_CALL( driver, read_sn_mr( socket_id ) ).WillOnce( Return( 0b0'0'1'1'1010 ) );
    EXPECT_CALL( driver, write_sn_mr( socket_id, 0b0'0'1'1'0001 ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x93 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x0E ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x13 ) );

    client.bind( {} );

    ASSERT_EQ( client.state(), Client::State::BOUND );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 41993 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::bind() works properly when
 *        binding to any address and a non-ephemeral port.
 */
TEST( bind, worksProperlyAnyAddressNonEphemeralPort )
{
    auto const in_sequence = InSequence{};

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_5;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ driver, socket_id, network_stack };

    auto const port = Port{ 37165 };

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( tcp_port_allocator, allocate( Ref( driver ), port ) ).WillOnce( Return( port ) );
    EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) );
    EXPECT_CALL( driver, read_sn_mr( socket_id ) ).WillOnce( Return( 0b0'1'1'0'0001 ) );
    EXPECT_CALL( driver, write_sn_mr( socket_id, 0b0'1'1'0'0001 ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0xF2 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x46 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x13 ) );

    client.bind( port );

    ASSERT_EQ( client.state(), Client::State::BOUND );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 23907 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::bind() works properly when
 *        binding to a specific address and an ephemeral port.
 */
TEST( bind, worksProperlySpecificAddressEphemeralPort )
{
    auto const in_sequence = InSequence{};

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_7;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ driver, socket_id, network_stack };

    auto const address = Address{ { 0, 68, 85, 59 } };
    auto const port    = Port{ 52468 };

    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( address.as_byte_array() ) );
    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( tcp_port_allocator, allocate( Ref( driver ), Port{} ) ).WillOnce( Return( port ) );
    EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) );
    EXPECT_CALL( driver, read_sn_mr( socket_id ) ).WillOnce( Return( 0b1'0'1'0'0011 ) );
    EXPECT_CALL( driver, write_sn_mr( socket_id, 0b1'0'1'0'0001 ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0xF3 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x6B ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x13 ) );

    client.bind( { address, {} } );

    ASSERT_EQ( client.state(), Client::State::BOUND );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 2431 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::bind() works properly when
 *        binding to a specific address and a non-ephemeral port.
 */
TEST( bind, worksProperlySpecificAddressNonEphemeralPort )
{
    auto const in_sequence = InSequence{};

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_0;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ driver, socket_id, network_stack };

    auto const address = Address{ { 58, 64, 232, 182 } };
    auto const port    = Port{ 1263 };

    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( address.as_byte_array() ) );
    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( tcp_port_allocator, allocate( Ref( driver ), Port{} ) ).WillOnce( Return( port ) );
    EXPECT_CALL( driver, write_sn_port( socket_id, port.as_unsigned_integer() ) );
    EXPECT_CALL( driver, read_sn_mr( socket_id ) ).WillOnce( Return( 0b1'1'1'0'0'1'0'1 ) );
    EXPECT_CALL( driver, write_sn_mr( socket_id, 0b1'1'1'0'0001 ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x01 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0xFE ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x94 ) );
    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x13 ) );

    client.bind( { address, {} } );

    ASSERT_EQ( client.state(), Client::State::BOUND );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 56304 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::connect() properly handles a
 *        connection timeout.
 */
TEST( connectErrorHandling, connectionTimeout )
{
    auto driver             = Mock_Driver{};
    auto network_stack      = Mock_Network_Stack{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTING, driver, Socket_ID::_0, network_stack };

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );

    auto const result = client.connect( { Address{ { 191, 156, 3, 194 } }, 62778 } );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::OPERATION_TIMEOUT );

    EXPECT_EQ( client.state(), Client::State::CONNECTING );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 39995 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::connect() works properly
 *        when a connection attempt is initiated.
 */
TEST( connectConnectionAttemptInitiated, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_6;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::BOUND, driver, socket_id, network_stack };

    auto const endpoint = Endpoint{ Address{ { 192, 206, 29, 34 } }, 22919 };

    EXPECT_CALL( driver, write_sn_dipr( socket_id, endpoint.address().ipv4().as_byte_array() ) );
    EXPECT_CALL( driver, write_sn_dport( socket_id, endpoint.port().as_unsigned_integer() ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x04 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x54 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

    auto const result = client.connect( endpoint );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

    EXPECT_EQ( client.state(), Client::State::CONNECTING );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 57259 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::connect() connection attempt in
 *        progress test fixture.
 */
class connectConnectionAttemptInProgress : public TestWithParam<std::uint8_t> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::connect() works properly
 *        when a connection attempt is in progress.
 */
TEST_P( connectConnectionAttemptInProgress, worksProperly )
{
    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_7;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTING, driver, socket_id, network_stack };

    auto const sn_sr = GetParam();

    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( sn_sr ) );

    auto const result = client.connect( { Address{ { 110, 76, 229, 185 } }, 46793 } );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

    EXPECT_EQ( client.state(), Client::State::CONNECTING );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 57405 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

INSTANTIATE_TEST_SUITE_P( testCases, connectConnectionAttemptInProgress, Values<std::uint8_t>( 0x13, 0x15 ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::connect() connection established
 *        test fixture.
 */
class connectConnectionEstablished : public TestWithParam<std::uint8_t> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::connect() works properly
 *        when a connection is established.
 */
TEST_P( connectConnectionEstablished, worksProperly )
{
    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_7;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTING, driver, socket_id, network_stack };

    auto const sn_sr = GetParam();

    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( sn_sr ) );

    EXPECT_FALSE( client.connect( { Address{ { 11, 76, 132, 53 } }, 47623 } ).is_error() );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 44838 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

INSTANTIATE_TEST_SUITE_P( testCases, connectConnectionEstablished, Values<std::uint8_t>( 0x17, 0x1C ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::is_connected() test case.
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::is_connected() test fixture.
 */
class isConnected : public TestWithParam<isConnected_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::is_connected() works
 *        properly.
 */
TEST_P( isConnected, worksProperly )
{
    auto const test_case = GetParam();

    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_7;
    auto       network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, socket_id, network_stack };

    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );

    ASSERT_EQ( client.is_connected(), test_case.is_connected );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::is_connected() test cases.
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
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::remote_endpoint() works
 *        properly.
 */
TEST( remoteEndpoint, worksProperly )
{
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_2;
    auto       network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, socket_id, network_stack };

    auto const sn_dipr  = Array<std::uint8_t, 4>{ 71, 135, 47, 193 };
    auto const sn_dport = std::uint16_t{ 12224 };

    EXPECT_CALL( driver, read_sn_dipr( socket_id ) ).WillOnce( Return( sn_dipr ) );
    EXPECT_CALL( driver, read_sn_dport( socket_id ) ).WillOnce( Return( sn_dport ) );

    auto const endpoint = client.remote_endpoint();

    ASSERT_TRUE( endpoint.address().is_ipv4() );
    EXPECT_EQ( endpoint.address().ipv4().as_byte_array(), sn_dipr );
    EXPECT_EQ( endpoint.port().as_unsigned_integer(), sn_dport );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::local_endpoint() works
 *        properly.
 */
TEST( localEndpoint, worksProperly )
{
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_5;
    auto       network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, socket_id, network_stack };

    auto const sipr    = Array<std::uint8_t, 4>{ 99, 105, 185, 107 };
    auto const sn_port = std::uint16_t{ 32306 };

    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( sipr ) );
    EXPECT_CALL( driver, read_sn_port( socket_id ) ).WillOnce( Return( sn_port ) );

    auto const endpoint = client.local_endpoint();

    ASSERT_TRUE( endpoint.address().is_ipv4() );
    EXPECT_EQ( endpoint.address().ipv4().as_byte_array(), sipr );
    EXPECT_EQ( endpoint.port().as_unsigned_integer(), sn_port );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::outstanding() test case.
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::outstanding() test fixture.
 */
class outstanding : public TestWithParam<outstanding_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::outstanding() works
 *        properly.
 */
TEST_P( outstanding, worksProperly )
{
    auto const test_case = GetParam();

    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_7;
    auto       network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, socket_id, network_stack };

    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( test_case.sn_tx_fsr ) );

    ASSERT_EQ( client.outstanding(), test_case.outstanding );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::outstanding() test cases.
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() connection loss error
 *        handling test case.
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() connection loss error
 *        handling test fixture.
 */
class transmitErrorHandlingConnectionLoss :
    public TestWithParam<transmitErrorHandlingConnectionLoss_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() properly handles
 *        connection loss.
 */
TEST_P( transmitErrorHandlingConnectionLoss, connectionLoss )
{
    auto const test_case = GetParam();

    auto driver             = Mock_Driver{};
    auto network_stack      = Mock_Network_Stack{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{
        Client::State::CONNECTED, driver, Socket_ID::_5, network_stack, test_case.is_transmitting
    };

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( test_case.sn_sr ) );

    auto const data   = std::vector<std::uint8_t>{ 0x72, 0x5D };
    auto const result = client.transmit( &*data.begin(), &*data.end() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::NOT_CONNECTED );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );
    EXPECT_EQ( client.is_transmitting(), test_case.is_transmitting );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 22655 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() connection loss error
 *        handling test cases.
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
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() works properly
 *        when an in progress transmission is not complete.
 */
TEST( transmit, worksProperlyInProgressTransmissionNotComplete )
{
    auto const in_sequence = InSequence{};

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_1;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack, true };

    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
    EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( 0b001'0'1'1'0'1 ) );

    auto const data   = std::vector<std::uint8_t>{ 0x6D, 0x4A, 0xA0 };
    auto const result = client.transmit( &*data.begin(), &*data.end() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );
    EXPECT_TRUE( client.is_transmitting() );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 23538 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() works properly
 *        when a transmission is not in progress and the data block is empty.
 */
TEST( transmit, worksProperlyTransmissionNotInProgressEmptyDataBlock )
{
    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_4;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack, false };

    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );

    auto const data   = std::vector<std::uint8_t>{};
    auto const result = client.transmit( &*data.begin(), &*data.end() );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), &*data.end() );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );
    EXPECT_FALSE( client.is_transmitting() );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 26490 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() works properly
 *        when an in progress transmission is complete and the data block is empty.
 */
TEST( transmit, worksProperlyInProgressTransmissionCompleteEmptyDataBlock )
{
    auto const in_sequence = InSequence{};

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_4;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack, true };

    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
    EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( 0b110'1'1'0'1'0 ) );
    EXPECT_CALL( driver, write_sn_ir( socket_id, 0b000'1'0'0'0'0 ) );

    auto const data   = std::vector<std::uint8_t>{};
    auto const result = client.transmit( &*data.begin(), &*data.end() );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), &*data.end() );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );
    EXPECT_FALSE( client.is_transmitting() );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 27042 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() works properly
 *        when a transmission is not in progress and the transmit buffer is full.
 */
TEST( transmit, worksProperlyTransmissionNotInProgressTransmitBufferFull )
{
    auto const in_sequence = InSequence{};

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_3;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack, false };

    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( Socket_Buffer_Size::_8_KiB ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( 0 ) );

    auto const data   = std::vector<std::uint8_t>{ 0xCA, 0x67 };
    auto const result = client.transmit( &*data.begin(), &*data.end() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );
    EXPECT_FALSE( client.is_transmitting() );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 53641 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() works properly
 *        when an in progress transmission is complete and the transmit buffer is full.
 */
TEST( transmit, worksProperlyInProgressTransmissionCompleteTransmitBufferFull )
{
    auto const in_sequence = InSequence{};

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_3;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack, true };

    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
    EXPECT_CALL( driver, read_sn_ir( socket_id ) ).WillOnce( Return( 0b011'1'1'0'1'1 ) );
    EXPECT_CALL( driver, write_sn_ir( socket_id, 0b000'1'0'0'0'0 ) );
    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( Socket_Buffer_Size::_8_KiB ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( 0 ) );

    auto const data   = std::vector<std::uint8_t>{ 0x68, 0x2B, 0x9E, 0x28 };
    auto const result = client.transmit( &*data.begin(), &*data.end() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );
    EXPECT_FALSE( client.is_transmitting() );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 44744 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() transmit buffer capacity
 *        test case.
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() sufficient transmit
 *        buffer capacity test fixture.
 */
class transmitSufficientTransmitBufferCapacity :
    public TestWithParam<transmitTransmitBufferCapacity_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() works properly
 *        when a transmission is not in progress and there is sufficient transmit buffer
 *        capacity.
 */
TEST_P( transmitSufficientTransmitBufferCapacity, worksProperlyTransmissionNotInProgress )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_4;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack, false };

    auto const data = std::vector<std::uint8_t>{ 0xA6, 0x94, 0x18, 0x2D };

    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
    EXPECT_CALL( driver, read_sn_tx_fsr( socket_id ) ).WillOnce( Return( test_case.sn_tx_fsr ) );
    EXPECT_CALL( driver, read_sn_tx_wr( socket_id ) ).WillOnce( Return( test_case.sn_tx_wr_initial ) );
    EXPECT_CALL( driver, write_tx_buffer( socket_id, test_case.sn_tx_wr_initial, data ) );
    EXPECT_CALL( driver, write_sn_tx_wr( socket_id, test_case.sn_tx_wr_final ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x20 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x9A ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

    auto const result = client.transmit( &*data.begin(), &*data.end() );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), &*data.end() );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );
    EXPECT_TRUE( client.is_transmitting() );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 17538 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() works properly
 *        when an in progress transmission is complete and there is sufficient transmit
 *        buffer capacity.
 */
TEST_P( transmitSufficientTransmitBufferCapacity, worksProperlyInProgressTransmissionComplete )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_4;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack, true };

    auto const data = std::vector<std::uint8_t>{ 0xA0, 0xA9, 0xC8, 0x3F };

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

    auto const result = client.transmit( &*data.begin(), &*data.end() );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), &*data.end() );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );
    EXPECT_TRUE( client.is_transmitting() );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 8884 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() sufficient transmit
 *        buffer capacity test cases.
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() insufficient transmit
 *        buffer capacity test fixture.
 */
class transmitInsufficientTransmitBufferCapacity :
    public TestWithParam<transmitTransmitBufferCapacity_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() works properly
 *        when a transmission is not in progress and there is insufficient transmit buffer
 *        capacity.
 */
TEST_P( transmitInsufficientTransmitBufferCapacity, worksProperlyTransmissionNotInProgress )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_4;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack, false };

    auto const data = std::vector<std::uint8_t>{ 0x1C, 0x98, 0xAE, 0xBE };

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

    auto const result = client.transmit( &*data.begin(), &*data.end() );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), &*data.begin() + test_case.sn_tx_fsr );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );
    EXPECT_TRUE( client.is_transmitting() );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 28615 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() works properly
 *        when an in progress transmission is complete and there is insufficient transmit
 *        buffer capacity.
 */
TEST_P( transmitInsufficientTransmitBufferCapacity, worksProperlyInProgressTransmissionComplete )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_4;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack, true };

    auto const data = std::vector<std::uint8_t>{ 0x9A, 0x37, 0x71, 0xD9 };

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

    auto const result = client.transmit( &*data.begin(), &*data.end() );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), &*data.begin() + test_case.sn_tx_fsr );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );
    EXPECT_TRUE( client.is_transmitting() );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 57671 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::transmit() insufficient transmit
 *        buffer capacity test cases.
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::transmit_keepalive() connection
 *        loss error handling test fixture.
 */
class transmitKeepaliveErrorHandlingConnectionLoss : public TestWithParam<std::uint8_t> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit_keepalive()
 *        properly handles connection loss.
 */
TEST_P( transmitKeepaliveErrorHandlingConnectionLoss, connectionLoss )
{
    auto driver             = Mock_Driver{};
    auto network_stack      = Mock_Network_Stack{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTED, driver, Socket_ID::_1, network_stack };

    auto const sn_sr = GetParam();

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( sn_sr ) );

    auto const result = client.transmit_keepalive();

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::NOT_CONNECTED );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 25551 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

INSTANTIATE_TEST_SUITE_P(
    testCases,
    transmitKeepaliveErrorHandlingConnectionLoss,
    Values<std::uint8_t>( 0x00, 0x1C, 0x18, 0x1A, 0x1B, 0x1D ) );

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::transmit_keepalive() works
 *        properly.
 */
TEST( transmitKeepalive, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_2;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack };

    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x17 ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x22 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x39 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

    ASSERT_FALSE( client.transmit_keepalive().is_error() );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 33915 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::available() test case.
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::available() test fixture.
 */
class available : public TestWithParam<available_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::available() works properly.
 */
TEST_P( available, worksProperly )
{
    auto const test_case = GetParam();

    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_4;
    auto       network_stack = Mock_Network_Stack{};

    auto const client = Client{ driver, socket_id, network_stack };

    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
    EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) ).WillOnce( Return( test_case.sn_rx_rsr ) );

    ASSERT_EQ( client.available(), test_case.sn_rx_rsr );

    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::available() test cases.
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
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() properly handles
 *        connection loss.
 */
TEST( receiveErrorHandling, connectionLoss )
{
    auto driver             = Mock_Driver{};
    auto network_stack      = Mock_Network_Stack{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTED, driver, Socket_ID::_7, network_stack };

    EXPECT_CALL( driver, read_sn_sr( _ ) ).WillOnce( Return( 0x00 ) );

    auto       data   = std::vector<std::uint8_t>( 3 );
    auto const result = client.receive( &*data.begin(), &*data.end() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::NOT_CONNECTED );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 25536 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::receive() graceful shutdown test
 *        fixture.
 */
class receiveGracefulShutdown : public TestWithParam<std::uint8_t> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() works properly
 *        during graceful shutdown.
 */
TEST_P( receiveGracefulShutdown, worksProperly )
{
    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_6;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack };

    auto const sn_sr = GetParam();

    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( sn_sr ) );

    auto       data   = std::vector<std::uint8_t>( 2 );
    auto const result = client.receive( &*data.begin(), &*data.end() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::WOULD_BLOCK );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 31890 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

INSTANTIATE_TEST_SUITE_P( testCases, receiveGracefulShutdown, Values<std::uint8_t>( 0x18, 0x1A, 0x1B, 0x1D ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::receive() receive buffer empty test
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::receive() receive buffer empty test
 *        fixture.
 */
class receiveReceiveBufferEmpty : public TestWithParam<receiveReceiveBufferEmpty_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() works properly
 *        when the receive buffer is empty.
 */
TEST_P( receiveReceiveBufferEmpty, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_1;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack };

    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );
    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( Socket_Buffer_Size::_8_KiB ) );
    EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) ).WillOnce( Return( 0 ) );

    auto       data   = std::vector<std::uint8_t>( 5 );
    auto const result = client.receive( &*data.begin(), &*data.end() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), test_case.error );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 63658 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::receive() receive buffer empty test
 *        cases.
 */
receiveReceiveBufferEmpty_Test_Case const receiveReceiveBufferEmpty_TEST_CASES[]{
    // clang-format off

    { 0x17, Generic_Error::WOULD_BLOCK   },
    { 0x1C, Generic_Error::NOT_CONNECTED },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, receiveReceiveBufferEmpty, ValuesIn( receiveReceiveBufferEmpty_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::receive() empty data block test
 *        case.
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::receive() empty data block test
 *        fixture.
 */
class receiveEmptyDataBlock : public TestWithParam<receiveEmptyDataBlock_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() works properly
 *        when the data block is empty.
 */
TEST_P( receiveEmptyDataBlock, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_5;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack };

    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( test_case.sn_sr ) );
    EXPECT_CALL( network_stack, socket_buffer_size() ).WillOnce( Return( test_case.socket_buffer_size ) );
    EXPECT_CALL( driver, read_sn_rx_rsr( socket_id ) ).WillOnce( Return( test_case.sn_rx_rsr ) );

    auto       data   = std::vector<std::uint8_t>{};
    auto const result = client.receive( &*data.begin(), &*data.end() );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), &*data.end() );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 31634 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::receive() empty data block test
 *        cases.
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::receive() receive data test case.
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::receive() receive all data test
 *        fixture.
 */
class receiveAllData : public TestWithParam<receiveData_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() works properly
 *        when all available data can be received.
 */
TEST_P( receiveAllData, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_0;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack };

    auto const data_expected = std::vector<std::uint8_t>{ 0x03, 0xA2, 0xAD };

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
    auto const result = client.receive( &*data.begin(), &*data.end() );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), &*data.begin() + test_case.sn_rx_rsr );

    EXPECT_EQ(
        ( std::vector<std::uint8_t>{ &*data.begin(), result.value() } ),
        ( std::vector<std::uint8_t>{ data_expected.begin(),
                                     data_expected.begin() + test_case.sn_rx_rsr } ) );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 17225 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::receive() receive all data test
 *        cases.
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
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::receive() receive some data test
 *        fixture.
 */
class receiveSomeData : public TestWithParam<receiveData_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::receive() works properly
 *        when only some of the available data can be received.
 */
TEST_P( receiveSomeData, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_1;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack };

    auto const data_expected = std::vector<std::uint8_t>{ 0x4B, 0x9F, 0x62, 0xB8 };

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
    auto const result = client.receive( &*data.begin(), &*data.end() );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), &*data.end() );

    EXPECT_EQ( data, data_expected );

    EXPECT_EQ( client.state(), Client::State::CONNECTED );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 3232 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::receive() receive some data test
 *        cases.
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
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::shutdown() works properly
 *        when the connection has been lost.
 */
TEST( shutdownConnectionLost, worksProperly )
{
    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_7;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack };

    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( 0x00 ) );

    client.shutdown();

    EXPECT_EQ( client.state(), Client::State::CONNECTED );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 60992 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::shutdown() connection not lost test
 *        fixture.
 */
class shutdownConnectionNotLost : public TestWithParam<std::uint8_t> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::shutdown() works properly
 *        when the connection has not been lost.
 */
TEST_P( shutdownConnectionNotLost, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_2;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ Client::State::CONNECTED, driver, socket_id, network_stack };

    auto const sn_sr = GetParam();

    EXPECT_CALL( driver, read_sn_sr( socket_id ) ).WillOnce( Return( sn_sr ) );
    EXPECT_CALL( driver, write_sn_cr( socket_id, 0x08 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x28 ) );
    EXPECT_CALL( driver, read_sn_cr( socket_id ) ).WillOnce( Return( 0x00 ) );

    client.shutdown();

    EXPECT_EQ( client.state(), Client::State::CONNECTED );

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( _ ) ).WillOnce( Return( 21563 ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( _ ) );
    EXPECT_CALL( network_stack, deallocate_socket( _ ) );
}

INSTANTIATE_TEST_SUITE_P( testCases, shutdownConnectionNotLost, Values( 0x17, 0x1C ) );

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::close() works properly when
 *        the socket is in the
 *        picolibrary::WIZnet::W5500::IP::TCP::Client::State::UNINITIALIZED state.
 */
TEST( close, worksProperlyUninitialized )
{
    auto client = Client{};

    client.close();

    ASSERT_EQ( client.state(), Client::State::UNINITIALIZED );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::close() works properly when
 *        the socket is in the
 *        picolibrary::WIZnet::W5500::IP::TCP::Client::State::INITIALIZED state.
 */
TEST( close, worksProperlyInitialized )
{
    auto       driver        = Mock_Driver{};
    auto const socket_id     = Socket_ID::_4;
    auto       network_stack = Mock_Network_Stack{};

    auto client = Client{ driver, socket_id, network_stack };

    EXPECT_CALL( network_stack, deallocate_socket( socket_id ) );

    client.close();

    ASSERT_EQ( client.state(), Client::State::UNINITIALIZED );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Client::close() other states test fixture.
 */
class closeOtherStates : public TestWithParam<Client::State> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::Client::close() works properly when
 *        the socket is in states other than
 *        picolibrary::WIZnet::W5500::IP::TCP::Client::State::UNINITIALIZED and
 *        picolibrary::WIZnet::W5500::IP::TCP::Client::State::INITIALIZED.
 */
TEST_P( closeOtherStates, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto const state = GetParam();

    auto       driver             = Mock_Driver{};
    auto const socket_id          = Socket_ID::_5;
    auto       network_stack      = Mock_Network_Stack{};
    auto       tcp_port_allocator = Mock_Port_Allocator{};

    auto client = Client{ state, driver, socket_id, network_stack };

    auto const sn_port = std::uint16_t{ 62819 };

    EXPECT_CALL( network_stack, tcp_port_allocator() ).WillOnce( ReturnRef( tcp_port_allocator ) );
    EXPECT_CALL( driver, read_sn_port( socket_id ) ).WillOnce( Return( sn_port ) );
    EXPECT_CALL( tcp_port_allocator, deallocate( Port{ sn_port } ) );
    EXPECT_CALL( network_stack, deallocate_socket( socket_id ) );

    client.close();

    ASSERT_EQ( client.state(), Client::State::UNINITIALIZED );
}

INSTANTIATE_TEST_SUITE_P(
    testCases,
    closeOtherStates,
    Values<Client::State>( Client::State::BOUND, Client::State::CONNECTING, Client::State::CONNECTED ) );

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

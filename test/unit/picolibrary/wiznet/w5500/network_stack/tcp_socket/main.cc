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

#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/network_stack.h"

namespace {

using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::WIZnet::W5500::Mock_Driver;
using ::picolibrary::WIZnet::W5500::Network_Stack;
using ::picolibrary::WIZnet::W5500::No_Delayed_ACK;
using ::picolibrary::WIZnet::W5500::Socket_ID;
using ::testing::_;
using ::testing::Return;

class Socket : public Network_Stack<Mock_Driver>::TCP_Socket {
  public:
    Socket( Network_Stack<Mock_Driver> & network_stack, Socket_ID socket_id ) noexcept :
        Network_Stack<Mock_Driver>::TCP_Socket{ network_stack, socket_id }
    {
    }
};

} // namespace

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::socket_id() works
 *        properly.
 */
TEST( socketID, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

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

        auto network_stack = Network_Stack{ driver };

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

    auto network_stack = Network_Stack{ driver };

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

    auto network_stack = Network_Stack{ driver };

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

    auto network_stack = Network_Stack{ driver };

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

    auto network_stack = Network_Stack{ driver };

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

    auto network_stack = Network_Stack{ driver };

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

    auto network_stack = Network_Stack{ driver };

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

    auto network_stack = Network_Stack{ driver };

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

    auto network_stack = Network_Stack{ driver };

    auto const socket_id = random<Socket_ID>();

    auto const socket = Socket{ network_stack, socket_id };

    auto const sn_mssr = random<std::uint16_t>();

    EXPECT_CALL( driver, read_sn_mssr( socket_id ) ).WillOnce( Return( sn_mssr ) );

    auto const result = socket.maximum_segment_size();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_mssr );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::time_to_live() properly
 *        handles an SN_TTL register read error.
 */
TEST( timeToLive, snttlReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_ttl( _ ) ).WillOnce( Return( error ) );

    auto const result = socket.time_to_live();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::time_to_live() works
 *        properly.
 */
TEST( timeToLive, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const socket_id = random<Socket_ID>();

    auto const socket = Socket{ network_stack, socket_id };

    auto const sn_ttl = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_ttl( socket_id ) ).WillOnce( Return( sn_ttl ) );

    auto const result = socket.time_to_live();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_ttl );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::keepalive_period()
 *        properly handles an SN_KPALVTR register read error.
 */
TEST( keepalivePeriod, snkpalvtrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const socket = Socket{ network_stack, random<Socket_ID>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_sn_kpalvtr( _ ) ).WillOnce( Return( error ) );

    auto const result = socket.keepalive_period();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::Network_Stack::TCP_Socket::keepalive_period() works
 *        properly.
 */
TEST( keepalivePeriod, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const socket_id = random<Socket_ID>();

    auto const socket = Socket{ network_stack, socket_id };

    auto const sn_kpalvtr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_sn_kpalvtr( socket_id ) ).WillOnce( Return( sn_kpalvtr ) );

    auto const result = socket.keepalive_period();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sn_kpalvtr );
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

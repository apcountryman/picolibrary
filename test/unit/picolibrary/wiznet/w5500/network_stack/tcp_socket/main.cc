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

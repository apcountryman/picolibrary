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

#include <cstdint>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/wiznet/w5500.h"
#include "picolibrary/testing/unit/wiznet/w5500/ip/network_stack.h"
#include "picolibrary/void.h"
#include "picolibrary/wiznet/w5500/ip/tcp.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::WIZnet::W5500::Mock_Driver;
using ::picolibrary::Testing::Unit::WIZnet::W5500::IP::Mock_Network_Stack;
using ::picolibrary::WIZnet::W5500::Socket_ID;
using ::picolibrary::WIZnet::W5500::IP::TCP::Client;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

using State = Client<Mock_Driver, Mock_Network_Stack>::State;

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
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::enabled_interrupts() properly
 *        handles an SN_IMR read error.
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
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::TCP::enabled_interrupts() works properly.
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

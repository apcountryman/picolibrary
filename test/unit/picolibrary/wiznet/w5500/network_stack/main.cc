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
 * \brief picolibrary::WIZnet::W5500::Network_Stack unit test program.
 */

#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/wiznet/w5500.h"
#include "picolibrary/void.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/network_stack.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::WIZnet::W5500::Mock_Driver;
using ::picolibrary::WIZnet::W5500::Link_Mode;
using ::picolibrary::WIZnet::W5500::Link_Speed;
using ::picolibrary::WIZnet::W5500::Link_Status;
using ::picolibrary::WIZnet::W5500::Network_Stack;
using ::picolibrary::WIZnet::W5500::PHY_Mode;
using ::picolibrary::WIZnet::W5500::Ping_Blocking;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

} // namespace

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_phy() properly
 *        handles a PHYCFGR register write error when writing the PHY configuration.
 */
TEST( configurePhy, phycfgrWriteErrorPhyConfiguration )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_phycfgr( _ ) ).WillOnce( Return( error ) );

    auto const result = network_stack.configure_phy( random<PHY_Mode>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_phy() properly
 *        handles a PHYCFGR register write error when entering PHY reset.
 */
TEST( configurePhy, phycfgrWrtieErrorEnterReset )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_phycfgr( _ ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) )
        .WillOnce( Return( error ) );

    auto const result = network_stack.configure_phy( random<PHY_Mode>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_phy() properly
 *        handles a PHYCFGR register write error when exiting PHY reset.
 */
TEST( configurePhy, phycfgrWriteErrorExitReset )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, write_phycfgr( _ ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) )
        .WillOnce( Return( error ) );

    auto const result = network_stack.configure_phy( random<PHY_Mode>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_phy() works
 *        properly.
 */
TEST( configurePhy, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const phy_mode = random<PHY_Mode>();

    EXPECT_CALL( driver, write_phycfgr( static_cast<std::uint8_t>( phy_mode ) | 0b1'0'000'000 ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_phycfgr( static_cast<std::uint8_t>( phy_mode ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( driver, write_phycfgr( static_cast<std::uint8_t>( phy_mode ) | 0b1'0'000'000 ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( network_stack.configure_phy( phy_mode ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::phy_mode() properly handles a
 *        PHYCFGR register read error.
 */
TEST( phyMode, phycfgrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_phycfgr() ).WillOnce( Return( error ) );

    auto const result = network_stack.phy_mode();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::phy_mode() works properly.
 */
TEST( phyMode, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const phycfgr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_phycfgr() ).WillOnce( Return( phycfgr ) );

    auto const result = network_stack.phy_mode();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), static_cast<PHY_Mode>( phycfgr & 0b0'1'111'0'0'0 ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::link_status() properly handles
 *        a PHYCFGR register read error.
 */
TEST( linkStatus, phycfgrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_phycfgr() ).WillOnce( Return( error ) );

    auto const result = network_stack.link_status();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::link_status() works properly.
 */
TEST( linkStatus, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const phycfgr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_phycfgr() ).WillOnce( Return( phycfgr ) );

    auto const result = network_stack.link_status();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), static_cast<Link_Status>( phycfgr & 0b0'0'000'0'0'1 ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::link_mode() properly handles a
 *        PHYCFGR register read error.
 */
TEST( linkMode, phycfgrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_phycfgr() ).WillOnce( Return( error ) );

    auto const result = network_stack.link_mode();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::link_mode() works properly.
 */
TEST( linkMode, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const phycfgr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_phycfgr() ).WillOnce( Return( phycfgr ) );

    auto const result = network_stack.link_mode();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), static_cast<Link_Mode>( phycfgr & 0b0'0'000'1'0'0 ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::link_speed() properly handles
 *        a PHYCFGR register read error.
 */
TEST( linkSpeed, phycfgrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_phycfgr() ).WillOnce( Return( error ) );

    auto const result = network_stack.link_speed();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::link_speed() works properly.
 */
TEST( linkSpeed, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const phycfgr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_phycfgr() ).WillOnce( Return( phycfgr ) );

    auto const result = network_stack.link_speed();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), static_cast<Link_Speed>( phycfgr & 0b0'0'000'0'1'0 ) );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_ping_blocking()
 *        properly handles an MR register read error.
 */
TEST( configurePingBlocking, mrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_mr() ).WillOnce( Return( error ) );

    auto const result = network_stack.configure_ping_blocking( random<Ping_Blocking>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_ping_blocking()
 *        properly handles an MR register write error.
 */
TEST( configurePingBlocking, mrWriteError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_mr() ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( driver, write_mr( _ ) ).WillOnce( Return( error ) );

    auto const result = network_stack.configure_ping_blocking( random<Ping_Blocking>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_ping_blocking()
 *        works properly.
 */
TEST( configurePingBlocking, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const mr                          = random<std::uint8_t>();
    auto const ping_blocking_configuration = random<Ping_Blocking>();

    EXPECT_CALL( driver, read_mr() ).WillOnce( Return( mr ) );
    EXPECT_CALL( driver, write_mr( ( mr & 0b1'1'1'0'1'1'1'1 ) | static_cast<std::uint8_t>( ping_blocking_configuration ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( network_stack.configure_ping_blocking( ping_blocking_configuration ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::ping_blocking_configuration()
 *        properly handles an MR register read error.
 */
TEST( pingBlockingConfiguration, mrReadError )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( driver, read_mr() ).WillOnce( Return( error ) );

    auto const result = network_stack.ping_blocking_configuration();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::ping_blocking_configuration()
 *        works properly.
 */
TEST( pingBlockingConfiguration, worksProperly )
{
    auto driver = Mock_Driver{};

    auto network_stack = Network_Stack{ driver };

    auto const mr = random<std::uint8_t>();

    EXPECT_CALL( driver, read_mr() ).WillOnce( Return( mr ) );

    auto const result = network_stack.ping_blocking_configuration();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), static_cast<Ping_Blocking>( mr & 0b0'0'0'1'0'0'0'0 ) );
}

/**
 * \brief Execute the picolibrary::WIZnet::W5500::Network_Stack unit tests.
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

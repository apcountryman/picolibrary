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
using ::picolibrary::WIZnet::W5500::Network_Stack;
using ::picolibrary::WIZnet::W5500::PHY_Mode;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

} // namespace

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::configure_phy() properly
 *        handles a PHY configuration error.
 */
TEST( configurePhy, phyConfigurationError )
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
 *        handles a PHY reset entry error.
 */
TEST( configurePhy, phyResetEntryError )
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
 *        handles a PHY reset exit error.
 */
TEST( configurePhy, phyResetExitError )
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

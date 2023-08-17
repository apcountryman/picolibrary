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
 * \brief picolibrary::SPI::Controller automated test program.
 */

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/spi.h"
#include "picolibrary/testing/automated/spi.h"

namespace {

using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

using Controller = ::picolibrary::SPI::Controller<::picolibrary::Testing::Automated::SPI::Mock_Basic_Controller>;

} // namespace

/**
 * \brief Verify picolibrary::SPI::Controller::exchange( std::uint8_t const *,
 *        std::uint8_t const *, std::uint8_t *, std::uint8_t * ) works properly.
 */
TEST( exchangeBlock, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto controller = Controller{};

    constexpr auto SIZE = std::size_t{ 5 };
    using Array         = std::array<std::uint8_t, SIZE>;

    auto const tx          = Array{ 0x27, 0x9E, 0xFB, 0x2E, 0x3E };
    auto       rx          = Array{};
    auto const rx_expected = Array{ 0xC1, 0xB8, 0xCA, 0x75, 0xEB };

    for ( auto i = std::size_t{ 0 }; i < SIZE; ++i ) {
        EXPECT_CALL( controller, exchange( tx[ i ] ) ).WillOnce( Return( rx_expected[ i ] ) );
    } // for

    controller.exchange( tx.begin(), tx.end(), rx.begin(), rx.end() );

    EXPECT_EQ( rx, rx_expected );
}

/**
 * \brief Verify picolibrary::SPI::Controller::receive() works properly.
 */
TEST( receive, worksProperly )
{
    auto controller = Controller{};

    auto const data = std::uint8_t{ 0x13 };

    EXPECT_CALL( controller, exchange( _ ) ).WillOnce( Return( data ) );

    EXPECT_EQ( controller.receive(), data );
}

/**
 * \brief Verify picolibrary::SPI::Controller::receive( std::uint8_t *, std::uint8_t * )
 *        works properly.
 */
TEST( receiveBlock, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto controller = Controller{};

    using Array = std::array<std::uint8_t, 6>;

    auto       data          = Array{};
    auto const data_expected = Array{ 0x1B, 0x63, 0x77, 0xA3, 0xB0, 0xC5 };

    for ( auto const byte : data_expected ) {
        EXPECT_CALL( controller, exchange( _ ) ).WillOnce( Return( byte ) );
    } // for

    controller.receive( data.begin(), data.end() );

    EXPECT_EQ( data, data_expected );
}

/**
 * \brief Verify picolibrary::SPI::Controller::transmit( std::uint8_t ) works properly.
 */
TEST( transmit, worksProperly )
{
    auto controller = Controller{};

    auto const data = std::uint8_t{ 0x09 };

    EXPECT_CALL( controller, exchange( data ) ).WillOnce( Return( 0x50 ) );

    controller.transmit( data );
}

/**
 * \brief Verify picolibrary::SPI::Controller::transmit( std::uint8_t const *,
 *        std::uint8_t const * ) works properly.
 */
TEST( transmitBlock, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto controller = Controller{};

    auto const data = std::vector<std::uint8_t>{ 0x45, 0x90, 0x87, 0xBF };

    for ( auto const byte : data ) {
        EXPECT_CALL( controller, exchange( byte ) ).WillOnce( Return( 0xD9 ) );
    } // for

    controller.transmit( &*data.begin(), &*data.end() );
}

/**
 * \brief Execute the picolibrary::SPI::Controller automated tests.
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

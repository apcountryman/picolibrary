/**
 * picolibrary
 *
 * Copyright 2020-2022, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::SPI::Controller unit test program.
 */

#include <cstddef>
#include <cstdint>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/spi.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/spi.h"

namespace {

using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

using Controller = ::picolibrary::SPI::Controller<::picolibrary::Testing::Unit::SPI::Mock_Basic_Controller>;

} // namespace

/**
 * \brief Verify picolibrary::SPI::Controller::exchange( std::uint8_t const *,
 *        std::uint8_t const *, std::uint8_t *, std::uint8_t * ) works properly.
 */
TEST( exchangeBlock, worksProperly )
{
    {
        auto controller = Controller{};

        EXPECT_CALL( controller, exchange( _ ) ).Times( 0 );

        auto const tx = std::vector<std::uint8_t>{};
        auto       rx = std::vector<std::uint8_t>{};
        controller.exchange( &*tx.begin(), &*tx.end(), &*rx.begin(), &*rx.end() );
    }

    {
        auto const in_sequence = InSequence{};

        auto controller = Controller{};

        auto const size        = random<std::size_t>( 1, 15 );
        auto const tx          = random_container<std::vector<std::uint8_t>>( size );
        auto const rx_expected = random_container<std::vector<std::uint8_t>>( size );

        for ( auto i = std::size_t{}; i < size; ++i ) {
            EXPECT_CALL( controller, exchange( tx[ i ] ) ).WillOnce( Return( rx_expected[ i ] ) );
        } // for

        auto rx = std::vector<std::uint8_t>( size );
        controller.exchange( &*tx.begin(), &*tx.end(), &*rx.begin(), &*rx.end() );

        EXPECT_EQ( rx, rx_expected );
    }
}

/**
 * \brief Verify picolibrary::SPI::Controller::receive() works properly.
 */
TEST( receive, worksProperly )
{
    auto controller = Controller{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( controller, exchange( 0x00 ) ).WillOnce( Return( data ) );

    EXPECT_EQ( controller.receive(), data );
}

/**
 * \brief Verify picolibrary::SPI::Controller::receive( std::uint8_t *, std::uint8_t * )
 *        works properly.
 */
TEST( receiveBlock, worksProperly )
{
    {
        auto controller = Controller{};

        EXPECT_CALL( controller, exchange( _ ) ).Times( 0 );

        auto data = std::vector<std::uint8_t>{};
        controller.receive( &*data.begin(), &*data.end() );
    }

    {
        auto const in_sequence = InSequence{};

        auto controller = Controller{};

        auto const size          = random<std::size_t>( 1, 15 );
        auto const data_expected = random_container<std::vector<std::uint8_t>>( size );

        for ( auto const byte : data_expected ) {
            EXPECT_CALL( controller, exchange( 0x00 ) ).WillOnce( Return( byte ) );
        } // for

        auto data = std::vector<std::uint8_t>( size );
        controller.receive( &*data.begin(), &*data.end() );

        EXPECT_EQ( data, data_expected );
    }
}

/**
 * \brief Verify picolibrary::SPI::Controller::transmit( std::uint8_t ) works properly.
 */
TEST( transmit, worksProperly )
{
    auto controller = Controller{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( controller, exchange( data ) ).WillOnce( Return( random<std::uint8_t>() ) );

    controller.transmit( data );
}

/**
 * \brief Verify picolibrary::SPI::Controller::transmit( std::uint8_t const *,
 *        std::uint8_t const * ) works properly.
 */
TEST( transmitBlock, worksProperly )
{
    {
        auto controller = Controller{};

        EXPECT_CALL( controller, exchange( _ ) ).Times( 0 );

        auto const data = std::vector<std::uint8_t>{};
        controller.transmit( &*data.begin(), &*data.end() );
    }

    {
        auto const in_sequence = InSequence{};

        auto controller = Controller{};

        auto const data = random_container<std::vector<std::uint8_t>>( random<std::size_t>( 1, 15 ) );

        for ( auto const byte : data ) {
            EXPECT_CALL( controller, exchange( byte ) ).WillOnce( Return( random<std::uint8_t>() ) );
        } // for

        controller.transmit( &*data.begin(), &*data.end() );
    }
}

/**
 * \brief Execute the picolibrary::SPI::Controller unit tests.
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

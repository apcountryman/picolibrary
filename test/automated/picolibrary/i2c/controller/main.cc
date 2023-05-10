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
 * \brief picolibrary::I2C::Controller automated test program.
 */

#include <cstdint>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/i2c.h"
#include "picolibrary/testing/automated/i2c.h"

namespace {

using ::picolibrary::I2C::Response;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

using Controller = ::picolibrary::I2C::Controller<::picolibrary::Testing::Automated::I2C::Mock_Basic_Controller>;

} // namespace

/**
 * \brief Verify picolibrary::I2C::Controller::read( std::uint8_t *, std::uint8_t *,
 *        picolibrary::I2C::Response ) works properly when begin is equal to end and
 *        response is picolibrary::I2C::Response::ACK.
 */
TEST( readBlockResponse, worksProperlyBeginEqualEndACK )
{
    auto controller = Controller{};

    EXPECT_CALL( controller, read( _ ) ).Times( 0 );

    auto data = std::vector<std::uint8_t>{};
    controller.read( &*data.begin(), &*data.end(), Response::ACK );
}

/**
 * \brief Verify picolibrary::I2C::Controller::read( std::uint8_t *, std::uint8_t *,
 *        picolibrary::I2C::Response ) works properly when begin is equal to end and
 *        response is picolibrary::I2C::Response::NACK.
 */
TEST( readBlockResponse, worksProperlyBeginEqualEndNACK )
{
    auto controller = Controller{};

    EXPECT_CALL( controller, read( _ ) ).Times( 0 );

    auto data = std::vector<std::uint8_t>{};
    controller.read( &*data.begin(), &*data.end(), Response::NACK );
}

/**
 * \brief Verify picolibrary::I2C::Controller::read( std::uint8_t *, std::uint8_t *,
 *        picolibrary::I2C::Response ) works properly when begin is not equal to end and
 *        response is picolibrary::I2C::Response::ACK.
 */
TEST( readBlockResponse, worksProperlyBeginNotEqualEndACK )
{
    auto const in_sequence = InSequence{};

    auto controller = Controller{};

    auto const data_expected = std::vector<std::uint8_t>{
        0x7f, 0xD9, 0xF7, 0x30, 0x29, 0x72, 0x56, 0x17, 0xC6,
    };

    for ( auto const byte : data_expected ) {
        EXPECT_CALL( controller, read( Response::ACK ) ).WillOnce( Return( byte ) );
    } // for

    auto data = std::vector<std::uint8_t>( data_expected.size() );

    controller.read( &*data.begin(), &*data.end(), Response::ACK );

    EXPECT_EQ( data, data_expected );
}

/**
 * \brief Verify picolibrary::I2C::Controller::read( std::uint8_t *, std::uint8_t *,
 *        picolibrary::I2C::Response ) works properly when begin is not equal to end and
 *        response is picolibrary::I2C::Response::NACK.
 */
TEST( readBlockResponse, worksProperlyBeginNotEqualEndNACK )
{
    auto const in_sequence = InSequence{};

    auto controller = Controller{};

    auto const data_expected = std::vector<std::uint8_t>{
        0x7f, 0xD9, 0xF7, 0x30, 0x29, 0x72, 0x56, 0x17, 0xC6,
    };

    for ( auto begin = data_expected.begin(); begin != data_expected.end(); ++begin ) {
        EXPECT_CALL( controller, read( begin + 1 == data_expected.end() ? Response::NACK : Response::ACK ) )
            .WillOnce( Return( *begin ) );
    } // for

    auto data = std::vector<std::uint8_t>( data_expected.size() );

    controller.read( &*data.begin(), &*data.end(), Response::NACK );

    EXPECT_EQ( data, data_expected );
}

/**
 * \brief Verify picolibrary::I2C::Controller::write( std::uint8_t const *, std::uint8_t
 *        const * ) works properly when begin is equal to end.
 */
TEST( writeBlock, worksProperlyBeginEqualEnd )
{
    auto controller = Controller{};

    EXPECT_CALL( controller, write( _ ) ).Times( 0 );

    auto const data = std::vector<std::uint8_t>{};

    EXPECT_EQ( controller.write( &*data.begin(), &*data.end() ), Response::ACK );
}

/**
 * \brief Verify picolibrary::I2C::Controller::write( std::uint8_t const *, std::uint8_t
 *        const * ) works properly when begin is not equal to end and a NACK response is
 *        received.
 */
TEST( writeBlock, worksProperlyBeginNotEqualEndNACK )
{
    auto controller = Controller{};

    EXPECT_CALL( controller, write( _ ) ).WillOnce( Return( Response::NACK ) );

    auto const data = std::vector<std::uint8_t>{
        0x7f, 0xD9, 0xF7, 0x30, 0x29, 0x72, 0x56, 0x17, 0xC6,
    };

    EXPECT_EQ( controller.write( &*data.begin(), &*data.end() ), Response::NACK );
}

/**
 * \brief Verify picolibrary::I2C::Controller::write( std::uint8_t const *, std::uint8_t
 *        const * ) works properly when begin is not equal to end and no NACK responses
 *        are received.
 */
TEST( writeBlock, worksProperlyBeginNotEqualEndNoNACK )
{
    auto const in_sequence = InSequence{};

    auto controller = Controller{};

    auto const data = std::vector<std::uint8_t>{
        0x7f, 0xD9, 0xF7, 0x30, 0x29, 0x72, 0x56, 0x17, 0xC6,
    };

    for ( auto const byte : data ) {
        EXPECT_CALL( controller, write( byte ) ).WillOnce( Return( Response::ACK ) );
    } // for

    EXPECT_EQ( controller.write( &*data.begin(), &*data.end() ), Response::ACK );
}

/**
 * \brief Execute the picolibrary::I2C::Controller automated tests.
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

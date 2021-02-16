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
 * \brief picolibrary::I2C::Controller unit test program.
 */

#include <cstdint>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/i2c.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/i2c.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/utility.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::I2C::Response;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

using Controller = ::picolibrary::I2C::Controller<::picolibrary::Testing::Unit::I2C::Mock_Basic_Controller>;

} // namespace

/**
 * \brief Verify picolibrary::I2C::Controller::read( std::uint8_t *, std::uint8_t *,
 *        picolibrary::I2C::Response ) properly handles a read error.
 */
TEST( readBlock, readError )
{
    auto controller = Controller{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( controller, read( _ ) ).WillOnce( Return( error ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = controller.read( &*data.begin(), &*data.end(), random<Response>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Controller::read( std::uint8_t *, std::uint8_t *,
 *        picolibrary::I2C::Response ) works properly.
 */
TEST( readBlock, worksProperly )
{
    {
        auto const in_sequence = InSequence{};

        auto controller = Controller{};

        auto const size          = random<std::uint_fast8_t>();
        auto const data_expected = random_container<std::vector<std::uint8_t>>( size );

        for ( auto const byte : data_expected ) {
            EXPECT_CALL( controller, read( Response::ACK ) ).WillOnce( Return( byte ) );
        } // for

        auto data = std::vector<std::uint8_t>( size );
        EXPECT_FALSE( controller.read( &*data.begin(), &*data.end(), Response::ACK ).is_error() );

        EXPECT_EQ( data, data_expected );
    }

    {
        auto const in_sequence = InSequence{};

        auto controller = Controller{};

        auto const size          = random<std::uint_fast8_t>();
        auto const data_expected = random_container<std::vector<std::uint8_t>>( size );

        for ( auto begin = data_expected.begin(); begin != data_expected.end(); ) {
            auto const byte = *begin;
            EXPECT_CALL( controller, read( ++begin == data_expected.end() ? Response::NACK : Response::ACK ) )
                .WillOnce( Return( byte ) );
        } // for

        auto data = std::vector<std::uint8_t>( size );
        EXPECT_FALSE( controller.read( &*data.begin(), &*data.end(), Response::NACK ).is_error() );

        EXPECT_EQ( data, data_expected );
    }
}

/**
 * \brief Verify picolibrary::I2C:Controller::write( std::uint8_t const *, std::uint8_t
 *        const * ) properly handles a write error.
 */
TEST( writeBlock, writeError )
{
    auto controller = Controller{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( controller, write( _ ) ).WillOnce( Return( error ) );

    auto const data = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
    auto const result = controller.write( &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C:Controller::write( std::uint8_t const *, std::uint8_t
 *        const * ) works properly.
 */
TEST( writeBlock, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto controller = Controller{};

    auto const data = random_container<std::vector<std::uint8_t>>();

    for ( auto const byte : data ) {
        EXPECT_CALL( controller, write( byte ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    } // for

    EXPECT_FALSE( controller.write( &*data.begin(), &*data.end() ).is_error() );
}

/**
 * \brief Execute the picolibrary::I2C::Controller unit tests.
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

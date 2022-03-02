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
 * \brief picolibrary::I2C unit test program.
 */

#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/i2c.h"
#include "picolibrary/testing/unit/i2c.h"
#include "picolibrary/testing/unit/random.h"

namespace {

using ::picolibrary::I2C::Address_Transmitted;
using ::picolibrary::I2C::Operation;
using ::picolibrary::I2C::ping;
using ::picolibrary::I2C::Response;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::I2C::Mock_Controller;
using ::testing::InSequence;
using ::testing::Return;

} // namespace

/**
 * \brief Verify picolibrary::I2C::ping() works properly.
 */
TEST( ping, worksProperly )
{
    {
        struct {
            Response response;
        } const test_cases[]{
            // clang-format off

            { Response::ACK  },
            { Response::NACK },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto controller = Mock_Controller{};

            auto const address = random<Address_Transmitted>();

            EXPECT_CALL( controller, start() );
            EXPECT_CALL( controller, address( address, Operation::READ ) )
                .WillOnce( Return( test_case.response ) );
            EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( random<std::uint8_t>() ) );
            EXPECT_CALL( controller, stop() );

            EXPECT_EQ( ping( controller, address, Operation::READ ), test_case.response );
        } // for
    }

    {
        struct {
            Response response;
        } const test_cases[]{
            // clang-format off

            { Response::ACK  },
            { Response::NACK },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto controller = Mock_Controller{};

            auto const address = random<Address_Transmitted>();

            EXPECT_CALL( controller, start() );
            EXPECT_CALL( controller, address( address, Operation::WRITE ) )
                .WillOnce( Return( test_case.response ) );
            EXPECT_CALL( controller, stop() );

            EXPECT_EQ( ping( controller, address, Operation::WRITE ), test_case.response );
        } // for
    }

    {
        struct {
            Response response_read;
            Response response_write;
            Response response;
        } const test_cases[]{
            // clang-format off

            { Response::ACK,  Response::ACK,  Response::ACK  },
            { Response::ACK,  Response::NACK, Response::NACK },
            { Response::NACK, Response::ACK,  Response::NACK },
            { Response::NACK, Response::NACK, Response::NACK },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto controller = Mock_Controller{};

            auto const address = random<Address_Transmitted>();

            EXPECT_CALL( controller, start() );
            EXPECT_CALL( controller, address( address, Operation::READ ) )
                .WillOnce( Return( test_case.response_read ) );
            EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( random<std::uint8_t>() ) );
            EXPECT_CALL( controller, stop() );

            EXPECT_CALL( controller, start() );
            EXPECT_CALL( controller, address( address, Operation::WRITE ) )
                .WillOnce( Return( test_case.response_write ) );
            EXPECT_CALL( controller, stop() );

            EXPECT_EQ( ping( controller, address ), test_case.response );
        } // for
    }
}

/**
 * \brief Execute the picolibrary::I2C unit tests.
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

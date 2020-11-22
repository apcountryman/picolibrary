/**
 * picolibrary
 *
 * Copyright 2020 Andrew Countryman <apcountryman@gmail.com>
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

#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/spi.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/spi.h"

namespace {

using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::testing::_;
using ::testing::Return;

using Controller = ::picolibrary::SPI::Controller<::picolibrary::Testing::Unit::SPI::Mock_Basic_Controller>;

} // namespace

/**
 * \brief Verify picolibrary::SPI::Controller::receive() properly handles an exchange
 *        error.
 */
TEST( receive, exchangeError )
{
    auto controller = Controller{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( controller, exchange( _ ) ).WillOnce( Return( error ) );

    auto const result = controller.receive();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::SPI::Controller::receive() works properly.
 */
TEST( receive, worksProperly )
{
    auto controller = Controller{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( controller, exchange( _ ) ).WillOnce( Return( data ) );

    auto const result = controller.receive();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Execute the picolibrary::SPI::Controller unit tests.
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

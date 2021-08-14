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
 * \brief picolibrary::I2C::Bus_Control_Guard unit test program.
 */

#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/i2c.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/i2c.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/void.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::I2C::make_bus_control_guard;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::I2C::Mock_Controller;
using ::testing::Return;

using Bus_Control_Guard = ::picolibrary::I2C::Bus_Control_Guard<Mock_Controller>;

} // namespace

/**
 * \brief Verify picolibrary::I2C::Bus_Control_Guard::Bus_Control_Guard() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    Bus_Control_Guard{};
}

/**
 * \brief Verify picolibrary::I2C::make_bus_control_guard() properly handles a start
 *        condition transmission error.
 */
TEST( makeBusControlGuard, startError )
{
    auto controller = Mock_Controller{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( controller, start() ).WillOnce( Return( error ) );

    auto const result = make_bus_control_guard( controller );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::make_bus_control_guard() works properly.
 */
TEST( makeBusControlGuard, worksProperly )
{
    auto controller = Mock_Controller{};

    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, stop() ).Times( 0 );

    auto const result = make_bus_control_guard( controller );

    EXPECT_FALSE( result.is_error() );

    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Verify picolibrary::I2C::Bus_Control_Guard::Bus_Control_Guard(
 *        picolibrary::I2C::Bus_Control_Guard && ) works properly.
 */
TEST( constructorMove, worksProperly )
{
    {
        Bus_Control_Guard{ Bus_Control_Guard{} };
    }

    {
        auto controller = Mock_Controller{};

        EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

        auto result = make_bus_control_guard( controller );

        EXPECT_FALSE( result.is_error() );

        auto const guard = Bus_Control_Guard{ std::move( result ).value() };

        EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    }
}

/**
 * \brief Verify picolibrary::I2C::Bus_Control_Guard::~Bus_Control_Guard() properly
 *        handles a stop condition transmission error.
 */
TEST( destructorDeathTest, stopError )
{
    EXPECT_DEATH(
        {
            auto controller = Mock_Controller{};

            EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

            auto const result = make_bus_control_guard( controller );

            EXPECT_FALSE( result.is_error() );

            EXPECT_CALL( controller, stop() ).WillOnce( Return( random<Mock_Error>() ) );
        },
        "" );
}

/**
 * \brief Verify picolibrary::I2C::Bus_Control_Guard::operator=(
 *        picolibrary::I2C::Bus_Control_Guard && ) works properly.
 */
TEST( assignmentOperatorMove, worksProperly )
{
    {
        auto expression = Bus_Control_Guard{};
        auto object     = Bus_Control_Guard{};

        object = std::move( expression );
    }

    {
        auto controller = Mock_Controller{};

        EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

        auto expression = make_bus_control_guard( controller );
        auto object     = Bus_Control_Guard{};

        EXPECT_FALSE( expression.is_error() );

        EXPECT_CALL( controller, stop() ).Times( 0 );

        object = std::move( expression ).value();

        EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    }

    {
        auto controller = Mock_Controller{};

        EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

        auto expression = Bus_Control_Guard{};
        auto object     = make_bus_control_guard( controller );

        EXPECT_FALSE( object.is_error() );

        EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

        object.value() = std::move( expression );

        EXPECT_CALL( controller, stop() ).Times( 0 );
    }

    {
        auto controller_expression = Mock_Controller{};
        auto controller_object     = Mock_Controller{};

        EXPECT_CALL( controller_expression, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( controller_object, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

        auto expression = make_bus_control_guard( controller_expression );
        auto object     = make_bus_control_guard( controller_object );

        EXPECT_FALSE( expression.is_error() );
        EXPECT_FALSE( object.is_error() );

        EXPECT_CALL( controller_expression, stop() ).Times( 0 );
        EXPECT_CALL( controller_object, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

        object.value() = std::move( expression ).value();

        EXPECT_CALL( controller_expression, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( controller_object, stop() ).Times( 0 );
    }

    {
        auto guard = Bus_Control_Guard{};

        guard = std::move( guard );
    }

    {
        auto controller = Mock_Controller{};

        EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

        auto result = make_bus_control_guard( controller );

        EXPECT_FALSE( result.is_error() );

        EXPECT_CALL( controller, stop() ).Times( 0 );

        result.value() = std::move( result ).value();

        EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    }
}

/**
 * \brief Verify picolibrary::I2C::Bus_Control_Guard::repeated_start() properly handles a
 *        repeated start condition transmission error.
 */
TEST( repeatedStart, repeatedStartError )
{
    auto controller = Mock_Controller{};

    auto guard = Bus_Control_Guard{};
    {
        EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

        auto result = make_bus_control_guard( controller );

        EXPECT_FALSE( result.is_error() );

        guard = std::move( result ).value();
    }

    auto const error = random<Mock_Error>();

    EXPECT_CALL( controller, repeated_start() ).WillOnce( Return( error ) );

    auto const result = guard.repeated_start();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Verify picolibrary::I2C::Bus_Control_Guard::repeated_start() works properly.
 */
TEST( repeatedStart, worksProperly )
{
    auto controller = Mock_Controller{};

    auto guard = Bus_Control_Guard{};
    {
        EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

        auto result = make_bus_control_guard( controller );

        EXPECT_FALSE( result.is_error() );

        guard = std::move( result ).value();
    }

    EXPECT_CALL( controller, repeated_start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( guard.repeated_start().is_error() );

    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
}

/**
 * \brief Execute the picolibrary::I2C::Bus_Control_Guard unit tests.
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

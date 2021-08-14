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
 * \brief picolibrary::I2C unit test program.
 */

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
using ::picolibrary::Generic_Error;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::I2C::Address;
using ::picolibrary::I2C::Operation;
using ::picolibrary::I2C::ping;
using ::picolibrary::I2C::Response;
using ::picolibrary::I2C::scan;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::I2C::Mock_Controller;
using ::testing::_;
using ::testing::InSequence;
using ::testing::MockFunction;
using ::testing::Return;

} // namespace

/**
 * \brief Verify picolibrary::I2C::ping() properly handles a start condition transmission
 *        error.
 */
TEST( ping, startError )
{
    auto controller = Mock_Controller{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( controller, start() ).WillOnce( Return( error ) );

    auto const result = ping( controller, random<Address>(), random<Operation>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::ping() properly handles an addressing error.
 */
TEST( ping, addressingError )
{
    auto controller = Mock_Controller{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = ping( controller, random<Address>(), random<Operation>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::ping() properly handles a read error.
 */
TEST( ping, readError )
{
    auto controller = Mock_Controller{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, read( _ ) ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = ping( controller, random<Address>(), Operation::READ );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::ping() works properly.
 */
TEST( ping, worksProperly )
{
    {
        auto const in_sequence = InSequence{};

        auto controller = Mock_Controller{};

        auto const address = random<Address>();

        EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( controller, address( address, Operation::READ ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( random<std::uint8_t>() ) );
        EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

        EXPECT_FALSE( ping( controller, address, Operation::READ ).is_error() );
    }

    {
        auto const in_sequence = InSequence{};

        auto controller = Mock_Controller{};

        auto const address = random<Address>();

        EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( controller, address( address, Operation::WRITE ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

        EXPECT_FALSE( ping( controller, address, Operation::WRITE ).is_error() );
    }
}

/**
 * \brief Verify picolibrary::I2C::scan() properly handles a start condition transmission
 *        error.
 */
TEST( scan, startError )
{
    auto controller = Mock_Controller{};
    auto functor    = MockFunction<Result<Void, Error_Code>( Address, Operation )>{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( controller, start() ).WillOnce( Return( error ) );

    auto const result = scan( controller, functor.AsStdFunction() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::scan() properly handles an addressing error.
 */
TEST( scan, addressingError )
{
    auto controller = Mock_Controller{};
    auto functor    = MockFunction<Result<Void, Error_Code>( Address, Operation )>{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = scan( controller, functor.AsStdFunction() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::scan() properly handles a read error.
 */
TEST( scan, readError )
{
    auto controller = Mock_Controller{};
    auto functor    = MockFunction<Result<Void, Error_Code>( Address, Operation )>{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, read( _ ) ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = scan( controller, functor.AsStdFunction() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::scan() properly handles a functor error.
 */
TEST( scan, functorError )
{
    auto controller = Mock_Controller{};
    auto functor    = MockFunction<Result<Void, Error_Code>( Address, Operation )>{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, read( _ ) ).WillRepeatedly( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( functor, Call( _, _ ) ).WillOnce( Return( error ) );

    auto const result = scan( controller, functor.AsStdFunction() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::scan() works properly.
 */
TEST( scan, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto controller = Mock_Controller{};
    auto functor    = MockFunction<Result<Void, Error_Code>( Address, Operation )>{};

    Operation const operations[]{
        Operation::READ,
        Operation::WRITE,
    };

    for ( auto numeric_address = Address::Numeric::MIN; numeric_address <= Address::Numeric::MAX;
          ++numeric_address ) {
        for ( auto const operation : operations ) {
            auto const address = Address{ Address::NUMERIC, numeric_address };

            if ( random<bool>() ) {
                EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
                EXPECT_CALL( controller, address( address, operation ) )
                    .WillOnce( Return( Result<Void, Error_Code>{} ) );
                if ( operation == Operation::READ ) {
                    EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( random<std::uint8_t>() ) );
                } // if
                EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
                EXPECT_CALL( functor, Call( address, operation ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
            } else {
                EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
                EXPECT_CALL( controller, address( address, operation ) ).WillOnce( Return( Generic_Error::NONRESPONSIVE_DEVICE ) );
                EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
            } // else
        }     // for
    }         // for

    EXPECT_FALSE( scan( controller, functor.AsStdFunction() ).is_error() );
}

/**
 * \brief Execute the picolibrary::I2C unit tests.
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

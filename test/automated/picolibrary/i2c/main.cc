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
 * \brief picolibrary::I2C automated test program.
 */

#include <cstdint>
#include <type_traits>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/algorithm.h"
#include "picolibrary/i2c.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/i2c.h"
#include "picolibrary/testing/automated/random.h"

namespace {

using ::picolibrary::Functor_Can_Fail_Discard_Functor;
using ::picolibrary::Functor_Can_Fail_Return_Functor;
using ::picolibrary::Result;
using ::picolibrary::I2C::Address_Numeric;
using ::picolibrary::I2C::Address_Transmitted;
using ::picolibrary::I2C::Operation;
using ::picolibrary::I2C::ping;
using ::picolibrary::I2C::Response;
using ::picolibrary::I2C::scan;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::random;
using ::picolibrary::Testing::Automated::I2C::Mock_Controller;
using ::testing::_;
using ::testing::InSequence;
using ::testing::MockFunction;
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
            if ( test_case.response == Response::ACK ) {
                EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( random<std::uint8_t>() ) );
            } // if
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
            if ( test_case.response_read == Response::ACK ) {
                EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( random<std::uint8_t>() ) );
            } // if
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
 * \brief Verify picolibrary::I2C::scan() properly handles a functor error.
 */
TEST( scan, functorError )
{
    {
        auto controller = Mock_Controller{};
        auto functor = MockFunction<Result<void>( Address_Transmitted, Operation, Response )>{};

        auto const error = random<Mock_Error>();

        EXPECT_CALL( controller, start() );
        EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( random<Response>() ) );
        EXPECT_CALL( controller, read( _ ) ).WillRepeatedly( Return( random<std::uint8_t>() ) );
        EXPECT_CALL( controller, stop() );
        EXPECT_CALL( functor, Call( _, _, _ ) ).WillOnce( Return( error ) );

        auto const result = scan<Functor_Can_Fail_Return_Functor>(
            controller, functor.AsStdFunction() );

        ASSERT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );
    }

    {
        auto controller = Mock_Controller{};
        auto functor = MockFunction<Result<void>( Address_Transmitted, Operation, Response )>{};

        auto const error = random<Mock_Error>();

        EXPECT_CALL( controller, start() );
        EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( random<Response>() ) );
        EXPECT_CALL( controller, read( _ ) ).WillRepeatedly( Return( random<std::uint8_t>() ) );
        EXPECT_CALL( controller, stop() );
        EXPECT_CALL( functor, Call( _, _, _ ) ).WillOnce( Return( error ) );

        auto const result = scan<Functor_Can_Fail_Discard_Functor>(
            controller, functor.AsStdFunction() );

        ASSERT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );
    }
}

/**
 * \brief Verify picolibrary::I2C::scan() works properly.
 */
TEST( scan, worksProperly )
{
    {
        auto const in_sequence = InSequence{};

        auto controller = Mock_Controller{};
        auto functor = MockFunction<void( Address_Transmitted, Operation, Response )>{};

        for ( auto address_numeric = std::uint_fast8_t{ 0b0000000 }; address_numeric <= 0b1111111;
              ++address_numeric ) {
            auto const address_transmitted = Address_Transmitted{ Address_Numeric{ address_numeric } };

            {
                auto const response = random<Response>();

                EXPECT_CALL( controller, start() );
                EXPECT_CALL( controller, address( address_transmitted, Operation::READ ) ).WillOnce( Return( response ) );
                if ( response == Response::ACK ) {
                    EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( random<std::uint8_t>() ) );
                } // if
                EXPECT_CALL( controller, stop() );
                EXPECT_CALL( functor, Call( address_transmitted, Operation::READ, response ) );
            }

            {
                auto const response = random<Response>();

                EXPECT_CALL( controller, start() );
                EXPECT_CALL( controller, address( address_transmitted, Operation::WRITE ) )
                    .WillOnce( Return( response ) );
                EXPECT_CALL( controller, stop() );
                EXPECT_CALL( functor, Call( address_transmitted, Operation::WRITE, response ) );
            }
        } // for

        scan( controller, functor.AsStdFunction() );
    }

    {
        auto const in_sequence = InSequence{};

        auto controller = Mock_Controller{};
        auto functor = MockFunction<Result<void>( Address_Transmitted, Operation, Response )>{};

        for ( auto address_numeric = std::uint_fast8_t{ 0b0000000 }; address_numeric <= 0b1111111;
              ++address_numeric ) {
            auto const address_transmitted = Address_Transmitted{ Address_Numeric{ address_numeric } };

            {
                auto const response = random<Response>();

                EXPECT_CALL( controller, start() );
                EXPECT_CALL( controller, address( address_transmitted, Operation::READ ) ).WillOnce( Return( response ) );
                if ( response == Response::ACK ) {
                    EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( random<std::uint8_t>() ) );
                } // if
                EXPECT_CALL( controller, stop() );
                EXPECT_CALL( functor, Call( address_transmitted, Operation::READ, response ) )
                    .WillOnce( Return( Result<void>{} ) );
            }

            {
                auto const response = random<Response>();

                EXPECT_CALL( controller, start() );
                EXPECT_CALL( controller, address( address_transmitted, Operation::WRITE ) )
                    .WillOnce( Return( response ) );
                EXPECT_CALL( controller, stop() );
                EXPECT_CALL( functor, Call( address_transmitted, Operation::WRITE, response ) )
                    .WillOnce( Return( Result<void>{} ) );
            }
        } // for

        auto const result = scan<Functor_Can_Fail_Return_Functor>(
            controller, functor.AsStdFunction() );

        static_assert( std::is_same_v<decltype( result )::Value, decltype( functor.AsStdFunction() )> );

        ASSERT_FALSE( result.is_error() );

        EXPECT_CALL( functor, Call( _, _, _ ) ).WillOnce( Return( Result<void>{} ) );

        EXPECT_FALSE( result
                          .value()( random<Address_Transmitted>(), random<Operation>(), random<Response>() )
                          .is_error() );
    }

    {
        auto const in_sequence = InSequence{};

        auto controller = Mock_Controller{};
        auto functor = MockFunction<Result<void>( Address_Transmitted, Operation, Response )>{};

        for ( auto address_numeric = std::uint_fast8_t{ 0b0000000 }; address_numeric <= 0b1111111;
              ++address_numeric ) {
            auto const address_transmitted = Address_Transmitted{ Address_Numeric{ address_numeric } };

            {
                auto const response = random<Response>();

                EXPECT_CALL( controller, start() );
                EXPECT_CALL( controller, address( address_transmitted, Operation::READ ) ).WillOnce( Return( response ) );
                if ( response == Response::ACK ) {
                    EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( random<std::uint8_t>() ) );
                } // if
                EXPECT_CALL( controller, stop() );
                EXPECT_CALL( functor, Call( address_transmitted, Operation::READ, response ) )
                    .WillOnce( Return( Result<void>{} ) );
            }

            {
                auto const response = random<Response>();

                EXPECT_CALL( controller, start() );
                EXPECT_CALL( controller, address( address_transmitted, Operation::WRITE ) )
                    .WillOnce( Return( response ) );
                EXPECT_CALL( controller, stop() );
                EXPECT_CALL( functor, Call( address_transmitted, Operation::WRITE, response ) )
                    .WillOnce( Return( Result<void>{} ) );
            }
        } // for

        auto const result = scan<Functor_Can_Fail_Discard_Functor>(
            controller, functor.AsStdFunction() );

        static_assert( std::is_same_v<decltype( result )::Value, void> );

        EXPECT_FALSE( result.is_error() );
    }
}

/**
 * \brief Execute the picolibrary::I2C automated tests.
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

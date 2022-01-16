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
 * \brief picolibrary algorithm unit test program.
 */

#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/algorithm.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/void.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Functor_Can_Fail_Discard_Functor;
using ::picolibrary::Functor_Can_Fail_Return_Functor;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::testing::_;
using ::testing::InSequence;
using ::testing::MockFunction;
using ::testing::Ref;
using ::testing::Return;

} // namespace

/**
 * \brief Verify picolibrary::for_each() properly handles a functor error.
 */
TEST( forEach, functorError )
{
    {
        auto functor = MockFunction<Result<Void, Error_Code>( std::uint_fast8_t const & )>{};

        auto const error = random<Mock_Error>();

        EXPECT_CALL( functor, Call( _ ) ).WillOnce( Return( error ) );

        auto const values = random_container<std::vector<std::uint_fast8_t>>(
            random<std::uint_fast8_t>( 1, 15 ) );
        auto const result = ::picolibrary::for_each<Functor_Can_Fail_Return_Functor>(
            values.begin(), values.end(), functor.AsStdFunction() );

        ASSERT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );
    }

    {
        auto functor = MockFunction<Result<Void, Error_Code>( std::uint_fast8_t const & )>{};

        auto const error = random<Mock_Error>();

        EXPECT_CALL( functor, Call( _ ) ).WillOnce( Return( error ) );

        auto const values = random_container<std::vector<std::uint_fast8_t>>(
            random<std::uint_fast8_t>( 1, 15 ) );
        auto const result = ::picolibrary::for_each<Functor_Can_Fail_Discard_Functor>(
            values.begin(), values.end(), functor.AsStdFunction() );

        ASSERT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );
    }
}

/**
 * \brief Verify picolibrary::for_each() works properly.
 */
TEST( forEach, worksProperly )
{
    {
        auto const in_sequence = InSequence{};

        auto functor = MockFunction<void( std::uint_fast8_t const & )>{};

        auto const values = random_container<std::vector<std::uint_fast8_t>>();

        for ( auto const & value : values ) {
            EXPECT_CALL( functor, Call( Ref( value ) ) );
        } // for

        ::picolibrary::for_each( values.begin(), values.end(), functor.AsStdFunction() );
    }

    {
        auto const in_sequence = InSequence{};

        auto functor = MockFunction<Result<Void, Error_Code>( std::uint_fast8_t const & )>{};

        auto const values = random_container<std::vector<std::uint_fast8_t>>();

        for ( auto const & value : values ) {
            EXPECT_CALL( functor, Call( Ref( value ) ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        } // for

        auto const result = ::picolibrary::for_each<Functor_Can_Fail_Return_Functor>(
            values.begin(), values.end(), functor.AsStdFunction() );

        static_assert( std::is_same_v<decltype( result )::Value, decltype( functor.AsStdFunction() )> );

        ASSERT_TRUE( result.is_value() );

        EXPECT_CALL( functor, Call( _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

        EXPECT_FALSE( result.value()( random<std::uint_fast8_t>() ).is_error() );
    }

    {
        auto const in_sequence = InSequence{};

        auto functor = MockFunction<Result<Void, Error_Code>( std::uint_fast8_t const & )>{};

        auto const values = random_container<std::vector<std::uint_fast8_t>>();

        for ( auto const & value : values ) {
            EXPECT_CALL( functor, Call( Ref( value ) ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        } // for

        auto const result = ::picolibrary::for_each<Functor_Can_Fail_Discard_Functor>(
            values.begin(), values.end(), functor.AsStdFunction() );

        static_assert( std::is_same_v<decltype( result )::Value, Void> );

        ASSERT_TRUE( result.is_value() );
    }
}

/**
 * \brief picolibrary::min() works properly.
 */
TEST( min, worksProperly )
{
    {
        auto const a = random<std::uint_fast8_t>(
            0, std::numeric_limits<std::uint_fast8_t>::max() - 1 );
        auto const b = random<std::uint_fast8_t>( a + 1 );

        EXPECT_THAT( ::picolibrary::min( a, b ), Ref( a ) );
    }

    {
        auto const b = random<std::uint_fast8_t>(
            0, std::numeric_limits<std::uint_fast8_t>::max() - 1 );
        auto const a = random<std::uint_fast8_t>( b + 1 );

        EXPECT_THAT( ::picolibrary::min( a, b ), Ref( b ) );
    }

    {
        auto const a = random<std::uint_fast8_t>();
        auto const b = a;

        EXPECT_THAT( ::picolibrary::min( a, b ), Ref( a ) );
    }
}

/**
 * \brief Execute the picolibrary algorithm unit tests.
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

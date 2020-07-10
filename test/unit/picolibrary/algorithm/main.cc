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
 * \brief picolibrary::Algorithm unit test program.
 */

#include <cstdint>
#include <type_traits>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/algorithm.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/functor.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/utility.h"

namespace {

using ::picolibrary::Discard_Functor;
using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Return_Functor;
using ::picolibrary::Void;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::Mock_Functor;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Ref;
using ::testing::Return;

} // namespace

/**
 * \brief Verify picolibrary::for_each() properly handles a functor error.
 */
TEST( forEach, functorError )
{
    {
        auto const functor = Mock_Functor<Result<Void, Error_Code>, std::uint_fast8_t const &>{};

        auto const error = random<Mock_Error>();

        EXPECT_CALL( functor, call( _ ) ).WillOnce( Return( error ) );

        auto const values = random_container<std::vector<std::uint_fast8_t>>(
            random<std::uint_fast8_t>( 1 ) );
        auto const result = ::picolibrary::for_each<Return_Functor>(
            values.begin(), values.end(), functor.handle() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );
    }

    {
        auto const functor = Mock_Functor<Result<Void, Error_Code>, std::uint_fast8_t const &>{};

        auto const error = random<Mock_Error>();

        EXPECT_CALL( functor, call( _ ) ).WillOnce( Return( error ) );

        auto const values = random_container<std::vector<std::uint_fast8_t>>(
            random<std::uint_fast8_t>( 1 ) );
        auto const result = ::picolibrary::for_each<Discard_Functor>(
            values.begin(), values.end(), functor.handle() );

        EXPECT_TRUE( result.is_error() );
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

        auto const functor = Mock_Functor<Result<Void, Error_Code>, std::uint_fast8_t const &>{};

        auto const values = random_container<std::vector<std::uint_fast8_t>>();

        for ( auto const & value : values ) {
            EXPECT_CALL( functor, call( Ref( value ) ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        } // for

        auto const result = ::picolibrary::for_each<Return_Functor>(
            values.begin(), values.end(), functor.handle() );

        static_assert( std::is_same_v<decltype( result )::Value, decltype( functor.handle() )> );

        EXPECT_TRUE( result.is_value() );
        EXPECT_EQ( &result.value().mock(), &functor );
    }

    {
        auto const in_sequence = InSequence{};

        auto const functor = Mock_Functor<Result<Void, Error_Code>, std::uint_fast8_t const &>{};

        auto const values = random_container<std::vector<std::uint_fast8_t>>();

        for ( auto const & value : values ) {
            EXPECT_CALL( functor, call( Ref( value ) ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        } // for

        auto const result = ::picolibrary::for_each<Discard_Functor>(
            values.begin(), values.end(), functor.handle() );

        static_assert( std::is_same_v<decltype( result )::Value, Void> );

        EXPECT_FALSE( result.is_error() );
    }
}

/**
 * \brief Execute the picolibrary::Algorithm unit tests.
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

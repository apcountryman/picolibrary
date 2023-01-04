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
 * \brief picolibrary algorithm automated test program.
 */

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/algorithm.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/random.h"

namespace {

using ::picolibrary::Functor_Can_Fail;
using ::picolibrary::Functor_Can_Fail_Discard_Functor;
using ::picolibrary::Functor_Can_Fail_Return_Functor;
using ::picolibrary::Result;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::random;
using ::picolibrary::Testing::Automated::random_container;
using ::picolibrary::Testing::Automated::random_unique_container_pair;
using ::picolibrary::Testing::Automated::random_unique_pair;
using ::testing::_;
using ::testing::Each;
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
        auto functor = MockFunction<Result<void>( std::uint_fast8_t const & )>{};

        auto const error = random<Mock_Error>();

        EXPECT_CALL( functor, Call( _ ) ).WillOnce( Return( error ) );

        auto const values = random_container<std::vector<std::uint_fast8_t>>(
            random<std::size_t>( 1, 15 ) );
        auto const result = ::picolibrary::for_each<Functor_Can_Fail_Return_Functor>(
            values.begin(), values.end(), functor.AsStdFunction() );

        ASSERT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );
    }

    {
        auto functor = MockFunction<Result<void>( std::uint_fast8_t const & )>{};

        auto const error = random<Mock_Error>();

        EXPECT_CALL( functor, Call( _ ) ).WillOnce( Return( error ) );

        auto const values = random_container<std::vector<std::uint_fast8_t>>(
            random<std::size_t>( 1, 15 ) );
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

        auto functor = MockFunction<Result<void>( std::uint_fast8_t const & )>{};

        auto const values = random_container<std::vector<std::uint_fast8_t>>();

        for ( auto const & value : values ) {
            EXPECT_CALL( functor, Call( Ref( value ) ) ).WillOnce( Return( Result<void>{} ) );
        } // for

        auto const result = ::picolibrary::for_each<Functor_Can_Fail_Return_Functor>(
            values.begin(), values.end(), functor.AsStdFunction() );

        static_assert( std::is_same_v<decltype( result )::Value, decltype( functor.AsStdFunction() )> );

        ASSERT_FALSE( result.is_error() );

        EXPECT_CALL( functor, Call( _ ) ).WillOnce( Return( Result<void>{} ) );

        EXPECT_FALSE( result.value()( random<std::uint_fast8_t>() ).is_error() );
    }

    {
        auto const in_sequence = InSequence{};

        auto functor = MockFunction<Result<void>( std::uint_fast8_t const & )>{};

        auto const values = random_container<std::vector<std::uint_fast8_t>>();

        for ( auto const & value : values ) {
            EXPECT_CALL( functor, Call( Ref( value ) ) ).WillOnce( Return( Result<void>{} ) );
        } // for

        auto const result = ::picolibrary::for_each<Functor_Can_Fail_Discard_Functor>(
            values.begin(), values.end(), functor.AsStdFunction() );

        static_assert( std::is_same_v<decltype( result )::Value, void> );

        ASSERT_FALSE( result.is_error() );
    }
}

/**
 * \brief Verify picolibrary::generate() properly handles a functor error.
 */
TEST( generate, functorError )
{
    auto functor = MockFunction<Result<std::uint_fast8_t>()>{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( functor, Call() ).WillOnce( Return( error ) );

    auto       output = std::vector<std::uint_fast8_t>( random<std::size_t>( 1, 15 ) );
    auto const result = ::picolibrary::generate<Functor_Can_Fail>(
        output.begin(), output.end(), functor.AsStdFunction() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::fill() works properly.
 */
TEST( fill, worksProperly )
{
    auto       container = random_container<std::vector<std::uint_fast8_t>>();
    auto const value     = random<std::uint_fast8_t>();

    ::picolibrary::fill( container.begin(), container.end(), value );

    EXPECT_THAT( container, Each( value ) );
}

/**
 * \brief Verify picolibrary::generate() works properly.
 */
TEST( generate, worksProperly )
{
    {
        auto const in_sequence = InSequence{};

        auto functor = MockFunction<std::uint_fast8_t()>{};

        auto const values = random_container<std::vector<std::uint_fast8_t>>();

        for ( auto const value : values ) {
            EXPECT_CALL( functor, Call() ).WillOnce( Return( value ) );
        } // for

        auto output = std::vector<std::uint_fast8_t>( values.size() );

        ::picolibrary::generate( output.begin(), output.end(), functor.AsStdFunction() );

        EXPECT_EQ( output, values );
    }

    {
        auto const in_sequence = InSequence{};

        auto functor = MockFunction<Result<std::uint_fast8_t>()>{};

        auto const values = random_container<std::vector<std::uint_fast8_t>>();

        for ( auto const value : values ) {
            EXPECT_CALL( functor, Call() ).WillOnce( Return( value ) );
        } // for

        auto output = std::vector<std::uint_fast8_t>( values.size() );

        EXPECT_FALSE( ::picolibrary::generate<Functor_Can_Fail>(
                          output.begin(), output.end(), functor.AsStdFunction() )
                          .is_error() );

        EXPECT_EQ( output, values );
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
 * \brief picolibrary::max() works properly.
 */
TEST( max, worksProperly )
{
    {
        auto const a = random<std::uint_fast8_t>(
            0, std::numeric_limits<std::uint_fast8_t>::max() - 1 );
        auto const b = random<std::uint_fast8_t>( a + 1 );

        EXPECT_THAT( ::picolibrary::max( a, b ), Ref( b ) );
    }

    {
        auto const b = random<std::uint_fast8_t>(
            0, std::numeric_limits<std::uint_fast8_t>::max() - 1 );
        auto const a = random<std::uint_fast8_t>( b + 1 );

        EXPECT_THAT( ::picolibrary::max( a, b ), Ref( a ) );
    }

    {
        auto const a = random<std::uint_fast8_t>();
        auto const b = a;

        EXPECT_THAT( ::picolibrary::max( a, b ), Ref( a ) );
    }
}

/**
 * \brief Verify picolibrary::equal() works properly.
 */
TEST( equal, worksProperly )
{
    {
        auto const values = random_container<std::vector<std::uint_fast8_t>>();

        EXPECT_TRUE( ::picolibrary::equal( values.begin(), values.end(), values.begin() ) );
        EXPECT_TRUE( ::picolibrary::equal(
            values.begin(), values.end(), values.begin(), []( auto a, auto b ) {
                return a == b;
            } ) );
    }

    {
        auto const [ values_1, values_2 ] = random_unique_container_pair<std::vector<std::uint_fast8_t>>();

        EXPECT_FALSE( ::picolibrary::equal( values_1.begin(), values_1.end(), values_2.begin() ) );
        EXPECT_FALSE( ::picolibrary::equal(
            values_1.begin(), values_1.end(), values_2.begin(), []( auto a, auto b ) {
                return a == b;
            } ) );
    }

    {
        auto const values = random_container<std::vector<std::uint_fast8_t>>();

        EXPECT_TRUE( ::picolibrary::equal(
            values.begin(), values.end(), values.begin(), values.end() ) );
        EXPECT_TRUE( ::picolibrary::equal(
            values.begin(), values.end(), values.begin(), values.end(), []( auto a, auto b ) {
                return a == b;
            } ) );
    }

    {
        auto const [ size_1, size_2 ] = random_unique_pair<std::uint8_t>();
        auto const values_1 = random_container<std::vector<std::uint_fast8_t>>( size_1 );
        auto const values_2 = random_container<std::vector<std::uint_fast8_t>>( size_2 );

        EXPECT_FALSE( ::picolibrary::equal(
            values_1.begin(), values_1.end(), values_2.begin(), values_2.end() ) );
        EXPECT_FALSE( ::picolibrary::equal(
            values_1.begin(), values_1.end(), values_2.begin(), values_2.end(), []( auto a, auto b ) {
                return a == b;
            } ) );
    }

    {
        auto const [ values_1, values_2 ] = random_unique_container_pair<std::vector<std::uint_fast8_t>>();

        EXPECT_FALSE( ::picolibrary::equal(
            values_1.begin(), values_1.end(), values_2.begin(), values_2.end() ) );
        EXPECT_FALSE( ::picolibrary::equal(
            values_1.begin(), values_1.end(), values_2.begin(), values_2.end(), []( auto a, auto b ) {
                return a == b;
            } ) );
    }
}

/**
 * \brief Execute the picolibrary algorithm automated tests.
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

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
 * \brief picolibrary::Algorithm unit test program.
 */

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>
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

using ::picolibrary::Discard_Functor;
using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Return_Functor;
using ::picolibrary::Void;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::testing::_;
using ::testing::InSequence;
using ::testing::MockFunction;
using ::testing::Ref;
using ::testing::Return;

template<typename T>
auto random_unique_values()
{
    auto const a = random<T>();
    auto const b = random<T>();

    return std::pair<T, T>{ a, b != a ? b : b ^ random<T>( 1 ) };
}

template<typename Container>
auto random_unique_containers( std::size_t size = random<std::uint_fast8_t>( 1 ) )
{
    auto a = random_container<Container>( size );
    auto b = random_container<Container>( size );

    if ( a == b ) {
        b[ random<std::size_t>( 0, size - 1 ) ] ^= random<typename Container::value_type>( 1 );
    } // if

    return std::pair<Container, Container>{ std::move( a ), std::move( b ) };
}

} // namespace

/**
 * \brief Verify picolibrary::min() works properly.
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
 * \brief Verify picolibrary::equal( Iterator_1, Iterator_1, Iterator_2 ),
 *        picolibrary::equal( Iterator_1, Iterator_1, Iterator_2, Iterator_2 ),
 *        picolibrary::equal( Iterator_1, Iterator_1, Iterator_2, Predicate ), and
 *        picolibrary::equal( Iterator_1, Iterator_1, Iterator_2, Iterator_2, Predicate )
 *        work properly.
 */
TEST( equal, worksProperly )
{
    // picolibrary::equal( Iterator_1, Iterator_1, Iterator_2 )
    // picolibrary::equal( Iterator_1, Iterator_1, Iterator_2, Predicate )
    {
        auto const values = random_container<std::vector<std::uint_fast8_t>>();

        EXPECT_TRUE( ::picolibrary::equal( values.begin(), values.end(), values.begin() ) );
        EXPECT_TRUE( ::picolibrary::equal(
            values.begin(), values.end(), values.begin(), []( auto a, auto b ) {
                return a == b;
            } ) );
    }

    {
        auto const [ values_1, values_2 ] = random_unique_containers<std::vector<std::uint_fast8_t>>();

        EXPECT_FALSE( ::picolibrary::equal( values_1.begin(), values_1.end(), values_2.begin() ) );
        EXPECT_FALSE( ::picolibrary::equal(
            values_1.begin(), values_1.end(), values_2.begin(), []( auto a, auto b ) {
                return a == b;
            } ) );
    }

    // picolibrary::equal( Iterator_1, Iterator_1, Iterator_2, Iterator_2 )
    // picolibrary::equal( Iterator_1, Iterator_1, Iterator_2, Iterator_2, Predicate )
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
        auto const [ size_1, size_2 ] = random_unique_values<std::uint_fast8_t>();
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
        auto const [ values_1, values_2 ] = random_unique_containers<std::vector<std::uint_fast8_t>>();

        EXPECT_FALSE( ::picolibrary::equal(
            values_1.begin(), values_1.end(), values_2.begin(), values_2.end() ) );
        EXPECT_FALSE( ::picolibrary::equal(
            values_1.begin(), values_1.end(), values_2.begin(), values_2.end(), []( auto a, auto b ) {
                return a == b;
            } ) );
    }
}

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
            random<std::uint_fast8_t>( 1 ) );
        auto const result = ::picolibrary::for_each<Return_Functor>(
            values.begin(), values.end(), functor.AsStdFunction() );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );
    }

    {
        auto functor = MockFunction<Result<Void, Error_Code>( std::uint_fast8_t const & )>{};

        auto const error = random<Mock_Error>();

        EXPECT_CALL( functor, Call( _ ) ).WillOnce( Return( error ) );

        auto const values = random_container<std::vector<std::uint_fast8_t>>(
            random<std::uint_fast8_t>( 1 ) );
        auto const result = ::picolibrary::for_each<Discard_Functor>(
            values.begin(), values.end(), functor.AsStdFunction() );

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

        auto const result = ::picolibrary::for_each<Return_Functor>(
            values.begin(), values.end(), functor.AsStdFunction() );

        static_assert( std::is_same_v<decltype( result )::Value, decltype( functor.AsStdFunction() )> );

        EXPECT_TRUE( result.is_value() );

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

        auto const result = ::picolibrary::for_each<Discard_Functor>(
            values.begin(), values.end(), functor.AsStdFunction() );

        static_assert( std::is_same_v<decltype( result )::Value, Void> );

        EXPECT_FALSE( result.is_error() );
    }
}

/**
 * \brief Verify picolibrary::generate() properly handles a functor error.
 */
TEST( generate, functorError )
{
    auto functor = MockFunction<Result<std::uint_fast8_t, Error_Code>()>{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( functor, Call() ).WillOnce( Return( error ) );

    auto       output = std::vector<std::uint_fast8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = ::picolibrary::generate(
        output.begin(), output.end(), functor.AsStdFunction() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::generate() works properly.
 */
TEST( generate, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto functor = MockFunction<Result<std::uint_fast8_t, Error_Code>()>{};

    auto const values = random_container<std::vector<std::uint_fast8_t>>();

    for ( auto const value : values ) {
        EXPECT_CALL( functor, Call() ).WillOnce( Return( value ) );
    } // for

    auto output = std::vector<std::uint_fast8_t>( values.size() );

    EXPECT_FALSE(
        ::picolibrary::generate( output.begin(), output.end(), functor.AsStdFunction() ).is_error() );

    EXPECT_EQ( output, values );
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

/**
 * picolibrary
 *
 * Copyright 2020-2023, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
#include <type_traits>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/algorithm.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/automated/error.h"

namespace {

using ::picolibrary::Functor_Can_Fail;
using ::picolibrary::Functor_Can_Fail_Discard_Functor;
using ::picolibrary::Functor_Can_Fail_Return_Functor;
using ::picolibrary::Result;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::testing::_;
using ::testing::Each;
using ::testing::InSequence;
using ::testing::MockFunction;
using ::testing::Ref;
using ::testing::Return;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

} // namespace

/**
 * \brief picolibrary::for_each() test cases.
 */
std::vector<std::uint_fast8_t> const forEach_TEST_CASES[]{
    // clang-format off

    {},
    {
        0x1C,
        0x0C,
        0xE6,
        0xEC,
        0x99,
    }

    // clang-format on
};

/**
 * \brief picolibrary::for_each( Iterator, Iterator, Functor ) test fixture.
 */
class forEach : public TestWithParam<std::vector<std::uint_fast8_t>> {
};

INSTANTIATE_TEST_SUITE_P( testCases, forEach, ValuesIn( forEach_TEST_CASES ) );

/**
 * \brief Verify picolibrary::for_each() works properly.
 */
TEST_P( forEach, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto functor = MockFunction<void( std::uint_fast8_t const & )>{};

    auto const values = GetParam();

    for ( auto const & value : values ) {
        EXPECT_CALL( functor, Call( Ref( value ) ) );
    } // for

    ::picolibrary::for_each( values.begin(), values.end(), functor.AsStdFunction() );
}

/**
 * \brief Verify picolibrary::for_each( Iterator, Iterator, Functor,
 *        picolibrary::Functor_Can_Fail_Return_Functor ) properly handles a functor error.
 */
TEST( forEachFunctorCanFailReturnFunctorErrorHandling, functorError )
{
    auto functor = MockFunction<Result<void>( std::uint_fast8_t const & )>{};

    auto const error = Mock_Error{ 0xA2 };

    EXPECT_CALL( functor, Call( _ ) ).WillOnce( Return( error ) );

    auto const values = std::vector<std::uint_fast8_t>{
        // clang-format off

        0x1C,
        0x0C,
        0xE6,
        0xEC,
        0x99,

        // clang-format on
    };

    auto const result = ::picolibrary::for_each<Functor_Can_Fail_Return_Functor>(
        values.begin(), values.end(), functor.AsStdFunction() );

    ASSERT_TRUE( result.is_error() );
    ASSERT_EQ( result.error(), error );
}

/**
 * \brief picolibrary::for_each( Iterator, Iterator, Functor,
 *        picolibrary::Functor_Can_Fail_Return_Functor ) test fixture.
 */
class forEachFunctorCanFailReturnFunctor : public TestWithParam<std::vector<std::uint_fast8_t>> {
};

INSTANTIATE_TEST_SUITE_P( testCases, forEachFunctorCanFailReturnFunctor, ValuesIn( forEach_TEST_CASES ) );

/**
 * \brief Verify picolibrary::for_each( Iterator, Iterator, Functor,
 *        picolibrary::Functor_Can_Fail_Return_Functor ) works properly.
 */
TEST_P( forEachFunctorCanFailReturnFunctor, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto functor = MockFunction<Result<void>( std::uint_fast8_t const & )>{};

    auto const values = GetParam();

    for ( auto const & value : values ) {
        EXPECT_CALL( functor, Call( Ref( value ) ) ).WillOnce( Return( Result<void>{} ) );
    } // for

    auto const result = ::picolibrary::for_each<Functor_Can_Fail_Return_Functor>(
        values.begin(), values.end(), functor.AsStdFunction() );

    static_assert( std::is_same_v<decltype( result )::Value, decltype( functor.AsStdFunction() )> );

    ASSERT_FALSE( result.is_error() );

    EXPECT_CALL( functor, Call( _ ) ).WillOnce( Return( Result<void>{} ) );

    ASSERT_FALSE( result.value()( 0x06 ).is_error() );
}

/**
 * \brief Verify picolibrary::for_each( Iterator, Iterator, Functor,
 *        picolibrary::Functor_Can_Fail_Discard_Functor ) properly handles a functor error.
 */
TEST( forEachFunctorCanFailDiscardFunctorErrorHandling, functorError )
{
    auto functor = MockFunction<Result<void>( std::uint_fast8_t const & )>{};

    auto const error = Mock_Error{ 0xA2 };

    EXPECT_CALL( functor, Call( _ ) ).WillOnce( Return( error ) );

    auto const values = std::vector<std::uint_fast8_t>{
        // clang-format off

        0x1C,
        0x0C,
        0xE6,
        0xEC,
        0x99,

        // clang-format on
    };

    auto const result = ::picolibrary::for_each<Functor_Can_Fail_Discard_Functor>(
        values.begin(), values.end(), functor.AsStdFunction() );

    ASSERT_TRUE( result.is_error() );
    ASSERT_EQ( result.error(), error );
}

/**
 * \brief picolibrary::for_each( Iterator, Iterator, Functor,
 *        picolibrary::Functor_Can_Fail_Discard_Functor ) test fixture.
 */
class forEachFunctorCanFailDiscardFunctor : public TestWithParam<std::vector<std::uint_fast8_t>> {
};

INSTANTIATE_TEST_SUITE_P( testCases, forEachFunctorCanFailDiscardFunctor, ValuesIn( forEach_TEST_CASES ) );

/**
 * \brief Verify picolibrary::for_each( Iterator, Iterator, Functor,
 *        picolibrary::Functor_Can_Fail_Discard_Functor ) works properly.
 */
TEST_P( forEachFunctorCanFailDiscardFunctor, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto functor = MockFunction<Result<void>( std::uint_fast8_t const & )>{};

    auto const values = GetParam();

    for ( auto const & value : values ) {
        EXPECT_CALL( functor, Call( Ref( value ) ) ).WillOnce( Return( Result<void>{} ) );
    } // for

    auto const result = ::picolibrary::for_each<Functor_Can_Fail_Discard_Functor>(
        values.begin(), values.end(), functor.AsStdFunction() );

    static_assert( std::is_same_v<decltype( result )::Value, void> );

    ASSERT_FALSE( result.is_error() );
}

/**
 * \brief picolibrary::fill() test cases.
 */
std::size_t const fill_TEST_CASES[]{
    // clang-format off

    0,
    5,

    // clang-format on
};

/**
 * \brief picolibrary::fill() test fixture.
 */
class fill : public TestWithParam<std::size_t> {
};

INSTANTIATE_TEST_SUITE_P( testCases, fill, ValuesIn( fill_TEST_CASES ) );

/**
 * \brief Verify picolibrary::fill() works properly.
 */
TEST_P( fill, worksProperly )
{
    auto const size = GetParam();

    auto       container = std::vector<std::uint_fast8_t>( size );
    auto const value     = std::uint_fast8_t{ 0xCB };

    ::picolibrary::fill( container.begin(), container.end(), value );

    ASSERT_THAT( container, Each( value ) );
}

/**
 * \brief picolibrary::generate() test cases.
 */
std::vector<std::uint_fast8_t> const generate_TEST_CASES[]{
    // clang-format off

    {},
    {
        0x1C,
        0x0C,
        0xE6,
        0xEC,
        0x99,
    }

    // clang-format on
};

/**
 * \brief picolibrary::generate( Iterator, Iterator, Functor ) test fixture.
 */
class generate : public TestWithParam<std::vector<std::uint_fast8_t>> {
};

INSTANTIATE_TEST_SUITE_P( testCases, generate, ValuesIn( generate_TEST_CASES ) );

/**
 * \brief Verify picolibrary::generate( Iterator, Iterator, Functor ) works properly.
 */
TEST_P( generate, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto functor = MockFunction<std::uint_fast8_t()>{};

    auto const values = GetParam();

    for ( auto const value : values ) {
        EXPECT_CALL( functor, Call() ).WillOnce( Return( value ) );
    } // for

    auto output = std::vector<std::uint_fast8_t>( values.size() );

    ::picolibrary::generate( output.begin(), output.end(), functor.AsStdFunction() );

    ASSERT_EQ( output, values );
}

/**
 * \brief Verify picolibrary::generate( Iterator, Iterator, Functor,
 *        picolibrary::Functor_Can_Fail ) properly handles a functor error.
 */
TEST( generateFunctorCanFailErrorHandling, functorError )
{
    auto functor = MockFunction<Result<std::uint_fast8_t>()>{};

    auto const error = Mock_Error{ 0xA2 };

    EXPECT_CALL( functor, Call() ).WillOnce( Return( error ) );

    auto       output = std::vector<std::uint_fast8_t>( 5 );
    auto const result = ::picolibrary::generate<Functor_Can_Fail>(
        output.begin(), output.end(), functor.AsStdFunction() );

    ASSERT_TRUE( result.is_error() );
    ASSERT_EQ( result.error(), error );
}

/**
 * \brief picolibrary::generate( Iterator, Iterator, Functor,
 *        picolibrary::Functor_Can_Fail ) test fixture.
 */
class generateFunctorCanFail : public TestWithParam<std::vector<std::uint_fast8_t>> {
};

INSTANTIATE_TEST_SUITE_P( testCases, generateFunctorCanFail, ValuesIn( generate_TEST_CASES ) );

/**
 * \brief Verify picolibrary::generate( Iterator, Iterator, Functor,
 *        picolibrary::Functor_Can_Fail ) works properly.
 */
TEST_P( generateFunctorCanFail, functorError )
{
    auto const in_sequence = InSequence{};

    auto functor = MockFunction<Result<std::uint_fast8_t>()>{};

    auto const values = GetParam();

    for ( auto const value : values ) {
        EXPECT_CALL( functor, Call() ).WillOnce( Return( value ) );
    } // for

    auto output = std::vector<std::uint_fast8_t>( values.size() );

    EXPECT_FALSE( ::picolibrary::generate<Functor_Can_Fail>(
                      output.begin(), output.end(), functor.AsStdFunction() )
                      .is_error() );

    ASSERT_EQ( output, values );
}

/**
 * \brief Verify picolibrary::min() works properly when a is less than b, and a and b are
 *        not adjacent.
 */
TEST( min, worksProperlyALessBNotAdjacent )
{
    auto const a = std::uint_fast8_t{ 28 };
    auto const b = std::uint_fast8_t{ 199 };

    ASSERT_THAT( ::picolibrary::min( a, b ), Ref( a ) );
}

/**
 * \brief Verify picolibrary::min() works properly when a is less than b, and a and b are
 *        adjacent.
 */
TEST( min, worksProperlyALessBAdjacent )
{
    auto const a = std::uint_fast8_t{ 198 };
    auto const b = std::uint_fast8_t{ 199 };

    ASSERT_THAT( ::picolibrary::min( a, b ), Ref( a ) );
}

/**
 * \brief Verify picolibrary::min() works properly when a equals b.
 */
TEST( min, worksProperlyAEqualB )
{
    auto const a = std::uint_fast8_t{ 199 };
    auto const b = std::uint_fast8_t{ 199 };

    ASSERT_THAT( ::picolibrary::min( a, b ), Ref( a ) );
}

/**
 * \brief Verify picolibrary::min() works properly when a is greater than b, and a and b
 *        are adjacent.
 */
TEST( min, worksProperlyAGreaterBAdjacent )
{
    auto const a = std::uint_fast8_t{ 200 };
    auto const b = std::uint_fast8_t{ 199 };

    ASSERT_THAT( ::picolibrary::min( a, b ), Ref( b ) );
}

/**
 * \brief Verify picolibrary::min() works properly when a is greater than b, and a and b
 *        are not adjacent.
 */
TEST( min, worksProperlyAGreaterBNotAdjacent )
{
    auto const a = std::uint_fast8_t{ 232 };
    auto const b = std::uint_fast8_t{ 199 };

    ASSERT_THAT( ::picolibrary::min( a, b ), Ref( b ) );
}

/**
 * \brief Verify picolibrary::max() works properly when a is less than b, and a and b are
 *        not adjacent.
 */
TEST( max, worksProperlyALessBNotAdjacent )
{
    auto const a = std::uint_fast8_t{ 28 };
    auto const b = std::uint_fast8_t{ 199 };

    ASSERT_THAT( ::picolibrary::max( a, b ), Ref( b ) );
}

/**
 * \brief Verify picolibrary::max() works properly when a is less than b, and a and b are
 *        adjacent.
 */
TEST( max, worksProperlyALessBAdjacent )
{
    auto const a = std::uint_fast8_t{ 198 };
    auto const b = std::uint_fast8_t{ 199 };

    ASSERT_THAT( ::picolibrary::max( a, b ), Ref( b ) );
}

/**
 * \brief Verify picolibrary::max() works properly when a equals b.
 */
TEST( max, worksProperlyAEqualB )
{
    auto const a = std::uint_fast8_t{ 199 };
    auto const b = std::uint_fast8_t{ 199 };

    ASSERT_THAT( ::picolibrary::max( a, b ), Ref( a ) );
}

/**
 * \brief Verify picolibrary::max() works properly when a is greater than b, and a and b
 *        are adjacent.
 */
TEST( max, worksProperlyAGreaterBAdjacent )
{
    auto const a = std::uint_fast8_t{ 200 };
    auto const b = std::uint_fast8_t{ 199 };

    ASSERT_THAT( ::picolibrary::max( a, b ), Ref( a ) );
}

/**
 * \brief Verify picolibrary::max() works properly when a is greater than b, and a and b
 *        are not adjacent.
 */
TEST( max, worksProperlyAGreaterBNotAdjacent )
{
    auto const a = std::uint_fast8_t{ 232 };
    auto const b = std::uint_fast8_t{ 199 };

    ASSERT_THAT( ::picolibrary::max( a, b ), Ref( a ) );
}

/**
 * \brief Verify picolibrary::equal( Iterator_1, Iterator_1, Iterator_2 ) works properly
 *        when the range is empty.
 */
TEST( equal3Iterators, worksProperlyEmpty )
{
    // clang-format off
    auto const range_1 = std::vector<std::uint_fast8_t>{};
    auto const range_2 = std::vector<std::uint_fast8_t>{};
    // clang-format on

    ASSERT_TRUE( ::picolibrary::equal( range_1.begin(), range_1.end(), range_2.begin() ) );
}

/**
 * \brief Verify picolibrary::equal( Iterator_1, Iterator_1, Iterator_2 ) works properly
 *        when the contents of the ranges are not equal.
 */
TEST( equal3Iterators, worksProperlyContentsNotEqual )
{
    // clang-format off
    auto const range_1 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6B, 0x4A, 0x32, };
    auto const range_2 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6C, 0x4A, 0x32, };
    // clang-format on

    ASSERT_FALSE( ::picolibrary::equal( range_1.begin(), range_1.end(), range_2.begin() ) );
}

/**
 * \brief Verify picolibrary::equal( Iterator_1, Iterator_1, Iterator_2 ) works properly
 *        when the contents of the ranges are equal.
 */
TEST( equal3Iterators, worksProperlyContentsEqual )
{
    // clang-format off
    auto const range_1 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6B, 0x4A, 0x32, };
    auto const range_2 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6B, 0x4A, 0x32, };
    // clang-format on

    ASSERT_TRUE( ::picolibrary::equal( range_1.begin(), range_1.end(), range_2.begin() ) );
}

/**
 * \brief Verify picolibrary::equal( Iterator_1, Iterator_1, Iterator_2, Iterator_2 )
 *        works properly when both ranges are empty.
 */
TEST( equal4Iterators, worksProperlyEmpty )
{
    // clang-format off
    auto const range_1 = std::vector<std::uint_fast8_t>{};
    auto const range_2 = std::vector<std::uint_fast8_t>{};
    // clang-format on

    ASSERT_TRUE( ::picolibrary::equal(
        range_1.begin(), range_1.end(), range_2.begin(), range_2.end() ) );
}

/**
 * \brief Verify picolibrary::equal( Iterator_1, Iterator_1, Iterator_2, Iterator_2 )
 *        works properly when the sizes of the ranges are not equal, and the contents of
 *        the ranges are not equal.
 */
TEST( equal4Iterators, worksProperlySizesNotEqualContentsNotEqual )
{
    // clang-format off
    auto const range_1 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6B, 0x4A, };
    auto const range_2 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6C, 0x4A, 0x32, };
    // clang-format on

    ASSERT_FALSE( ::picolibrary::equal(
        range_1.begin(), range_1.end(), range_2.begin(), range_2.end() ) );
}

/**
 * \brief Verify picolibrary::equal( Iterator_1, Iterator_1, Iterator_2, Iterator_2 )
 *        works properly when the sizes of the ranges are not equal, and the contents of
 *        the ranges are equal.
 */
TEST( equal4Iterators, worksProperlySizesNotEqualContentsEqual )
{
    // clang-format off
    auto const range_1 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6B, 0x4A, };
    auto const range_2 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6B, 0x4A, 0x32, };
    // clang-format on

    ASSERT_FALSE( ::picolibrary::equal(
        range_1.begin(), range_1.end(), range_2.begin(), range_2.end() ) );
}

/**
 * \brief Verify picolibrary::equal( Iterator_1, Iterator_1, Iterator_2, Iterator_2 )
 *        works properly when the sizes of the ranges are equal, and the contents of the
 *        ranges are not equal.
 */
TEST( equal4Iterators, worksProperlySizesEqualContentsNotEqual )
{
    // clang-format off
    auto const range_1 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6B, 0x4A, 0x32, };
    auto const range_2 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6C, 0x4A, 0x32, };
    // clang-format on

    ASSERT_FALSE( ::picolibrary::equal(
        range_1.begin(), range_1.end(), range_2.begin(), range_2.end() ) );
}

/**
 * \brief Verify picolibrary::equal( Iterator_1, Iterator_1, Iterator_2, Iterator_2 )
 *        works properly when the sizes of the ranges are equal, and the contents of the
 *        ranges are equal.
 */
TEST( equal4Iterators, worksProperlySizesEqualContentsEqual )
{
    // clang-format off
    auto const range_1 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6B, 0x4A, 0x32, };
    auto const range_2 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6B, 0x4A, 0x32, };
    // clang-format on

    ASSERT_TRUE( ::picolibrary::equal(
        range_1.begin(), range_1.end(), range_2.begin(), range_2.end() ) );
}

/**
 * \brief Verify picolibrary::equal( Iterator_1, Iterator_1, Iterator_2, Predicate ) works
 *        properly when the range is empty.
 */
TEST( equal3IteratorsPredicate, worksProperlyEmpty )
{
    // clang-format off
    auto const range_1 = std::vector<std::uint_fast8_t>{};
    auto const range_2 = std::vector<std::uint_fast8_t>{};
    // clang-format on

    ASSERT_TRUE( ::picolibrary::equal(
        range_1.begin(), range_1.end(), range_2.begin(), []( auto a, auto b ) {
            return a == b;
        } ) );
}

/**
 * \brief Verify picolibrary::equal( Iterator_1, Iterator_1, Iterator_2, Predicate ) works
 *        properly when the contents of the ranges are not equal.
 */
TEST( equal3IteratorsPredicate, worksProperlyContentsNotEqual )
{
    // clang-format off
    auto const range_1 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6B, 0x4A, 0x32, };
    auto const range_2 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6C, 0x4A, 0x32, };
    // clang-format on

    ASSERT_FALSE( ::picolibrary::equal(
        range_1.begin(), range_1.end(), range_2.begin(), []( auto a, auto b ) {
            return a == b;
        } ) );
}

/**
 * \brief Verify picolibrary::equal( Iterator_1, Iterator_1, Iterator_2, Predicate ) works
 *        properly when the contents of the ranges are equal.
 */
TEST( equal3IteratorsPredicate, worksProperlyContentsEqual )
{
    // clang-format off
    auto const range_1 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6B, 0x4A, 0x32, };
    auto const range_2 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6B, 0x4A, 0x32, };
    // clang-format on

    ASSERT_TRUE( ::picolibrary::equal(
        range_1.begin(), range_1.end(), range_2.begin(), []( auto a, auto b ) {
            return a == b;
        } ) );
}

/**
 * \brief Verify picolibrary::equal( Iterator_1, Iterator_1, Iterator_2, Iterator_2,
 *        Predicate ) works properly when both ranges are empty.
 */
TEST( equal4IteratorsPredicate, worksProperlyEmpty )
{
    // clang-format off
    auto const range_1 = std::vector<std::uint_fast8_t>{};
    auto const range_2 = std::vector<std::uint_fast8_t>{};
    // clang-format on

    ASSERT_TRUE( ::picolibrary::equal(
        range_1.begin(), range_1.end(), range_2.begin(), range_2.end(), []( auto a, auto b ) {
            return a == b;
        } ) );
}

/**
 * \brief Verify picolibrary::equal( Iterator_1, Iterator_1, Iterator_2, Iterator_2,
 *        Predicate ) works properly when the sizes of the ranges are not equal, and the
 *        contents of the ranges are not equal.
 */
TEST( equal4IteratorsPredicate, worksProperlySizesNotEqualContentsNotEqual )
{
    // clang-format off
    auto const range_1 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6B, 0x4A, };
    auto const range_2 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6C, 0x4A, 0x32, };
    // clang-format on

    ASSERT_FALSE( ::picolibrary::equal(
        range_1.begin(), range_1.end(), range_2.begin(), range_2.end(), []( auto a, auto b ) {
            return a == b;
        } ) );
}

/**
 * \brief Verify picolibrary::equal( Iterator_1, Iterator_1, Iterator_2, Iterator_2,
 *        Predicate ) works properly when the sizes of the ranges are not equal, and the
 *        contents of the ranges are equal.
 */
TEST( equal4IteratorsPredicate, worksProperlySizesNotEqualContentsEqual )
{
    // clang-format off
    auto const range_1 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6B, 0x4A, };
    auto const range_2 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6B, 0x4A, 0x32, };
    // clang-format on

    ASSERT_FALSE( ::picolibrary::equal(
        range_1.begin(), range_1.end(), range_2.begin(), range_2.end(), []( auto a, auto b ) {
            return a == b;
        } ) );
}

/**
 * \brief Verify picolibrary::equal( Iterator_1, Iterator_1, Iterator_2, Iterator_2,
 *        Predicate ) works properly when the sizes of the ranges are equal, and the
 *        contents of the ranges are not equal.
 */
TEST( equal4IteratorsPredicate, worksProperlySizesEqualContentsNotEqual )
{
    // clang-format off
    auto const range_1 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6B, 0x4A, 0x32, };
    auto const range_2 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6C, 0x4A, 0x32, };
    // clang-format on

    ASSERT_FALSE( ::picolibrary::equal(
        range_1.begin(), range_1.end(), range_2.begin(), range_2.end(), []( auto a, auto b ) {
            return a == b;
        } ) );
}

/**
 * \brief Verify picolibrary::equal( Iterator_1, Iterator_1, Iterator_2, Iterator_2,
 *        Predicate ) works properly when the sizes of the ranges are equal, and the
 *        contents of the ranges are equal.
 */
TEST( equal4IteratorsPredicate, worksProperlySizesEqualContentsEqual )
{
    // clang-format off
    auto const range_1 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6B, 0x4A, 0x32, };
    auto const range_2 = std::vector<std::uint_fast8_t>{ 0x9B, 0x50, 0x6B, 0x4A, 0x32, };
    // clang-format on

    ASSERT_TRUE( ::picolibrary::equal(
        range_1.begin(), range_1.end(), range_2.begin(), range_2.end(), []( auto a, auto b ) {
            return a == b;
        } ) );
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

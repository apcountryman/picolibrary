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
 * \brief picolibrary::Stream_Buffer unit test program.
 */

#include <cstdint>
#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/stream.h"
#include "picolibrary/utility.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::Mock_Stream_Buffer;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::testing::A;
using ::testing::Eq;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::SafeMatcherCast;

} // namespace

/**
 * \brief Verify picolibrary::Stream_Buffer::put( char const *, char const * ) properly
 *        handles a put error.
 */
TEST( putCharBlock, putError )
{
    auto buffer = Mock_Stream_Buffer{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( buffer, put( A<char>() ) ).WillOnce( Return( error ) );

    auto const string = random_container<std::string>( random<std::uint_fast8_t>( 1 ) );
    auto const result = buffer.Stream_Buffer::put( &*string.begin(), &*string.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Stream_Buffer::put( char const *, char const * ) works
 *        properly.
 */
TEST( putCharBlock, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto buffer = Mock_Stream_Buffer{};

    auto const string = random_container<std::string>();

    for ( auto const character : string ) {
        EXPECT_CALL( buffer, put( SafeMatcherCast<char>( Eq( character ) ) ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
    } // for

    EXPECT_FALSE( buffer.Stream_Buffer::put( &*string.begin(), &*string.end() ).is_error() );
}

/**
 * \brief Verify picolibrary::Stream_Buffer::put( char const * ) properly handles a put
 *        error.
 */
TEST( putNullTerminatedString, putError )
{
    auto buffer = Mock_Stream_Buffer{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( buffer, put( A<char>() ) ).WillOnce( Return( error ) );

    auto const result = buffer.Stream_Buffer::put(
        random_container<std::string>( random<std::uint_fast8_t>( 1 ) ).c_str() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Stream_Buffer::put( char const * ) works properly.
 */
TEST( putNullTerminatedString, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto buffer = Mock_Stream_Buffer{};

    auto const string = random_container<std::string>();

    for ( auto const character : string ) {
        EXPECT_CALL( buffer, put( SafeMatcherCast<char>( Eq( character ) ) ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
    } // for

    EXPECT_FALSE( buffer.Stream_Buffer::put( string.c_str() ).is_error() );
}

/**
 * \brief Verify picolibrary::Stream_Buffer::put( std::uint8_t const *, std::uint8_t const
 *        * ) properly handles a put error.
 */
TEST( putUnsignedByteBlock, putError )
{
    auto buffer = Mock_Stream_Buffer{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( buffer, put( A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const values = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );
    auto const result = buffer.Stream_Buffer::put( &*values.begin(), &*values.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Stream_Buffer::put( std::uint8_t const *, std::uint8_t const
 *        * ) works properly.
 */
TEST( putUnsignedByteBlock, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto buffer = Mock_Stream_Buffer{};

    auto const values = random_container<std::vector<std::uint8_t>>();

    for ( auto const value : values ) {
        EXPECT_CALL( buffer, put( SafeMatcherCast<std::uint8_t>( Eq( value ) ) ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
    } // for

    EXPECT_FALSE( buffer.Stream_Buffer::put( &*values.begin(), &*values.end() ).is_error() );
}

/**
 * \brief Verify picolibrary::Stream_Buffer::put( std::int8_t const *, std::int8_t const *
 *        ) properly handles a put error.
 */
TEST( putSignedByteBlock, putError )
{
    auto buffer = Mock_Stream_Buffer{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( buffer, put( A<std::int8_t>() ) ).WillOnce( Return( error ) );

    auto const values = random_container<std::vector<std::int8_t>>( random<std::uint_fast8_t>( 1 ) );
    auto const result = buffer.Stream_Buffer::put( &*values.begin(), &*values.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Stream_Buffer::put( std::int8_t const *, std::int8_t const *
 *        ) works properly.
 */
TEST( putSignedByteBlock, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto buffer = Mock_Stream_Buffer{};

    auto const values = random_container<std::vector<std::int8_t>>();

    for ( auto const value : values ) {
        EXPECT_CALL( buffer, put( SafeMatcherCast<std::int8_t>( Eq( value ) ) ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
    } // for

    EXPECT_FALSE( buffer.Stream_Buffer::put( &*values.begin(), &*values.end() ).is_error() );
}

/**
 * \brief Execute the picolibrary::Stream_Buffer unit tests.
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

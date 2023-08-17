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
 * \brief picolibrary::Stream_Buffer automated test program.
 */

#include <cstdint>
#include <string_view>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/stream.h"

namespace {

using ::picolibrary::Result;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::Mock_Stream_Buffer;
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
TEST( putCharBlockErrorHandling, putError )
{
    auto buffer = Mock_Stream_Buffer{};

    auto const error = Mock_Error{ 254 };

    EXPECT_CALL( buffer, put( A<char>() ) ).WillOnce( Return( error ) );

    auto const string = std::string_view{ "ocWsrYVYyIuSvW8" };

    auto const result = buffer.Stream_Buffer::put( string.begin(), string.end() );

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

    auto const string = std::string_view{ "XnQmlICQXCrJ" };

    for ( auto const character : string ) {
        EXPECT_CALL( buffer, put( SafeMatcherCast<char>( Eq( character ) ) ) )
            .WillOnce( Return( Result<void>{} ) );
    } // for

    EXPECT_FALSE( buffer.Stream_Buffer::put( string.begin(), string.end() ).is_error() );
}

/**
 * \brief Verify picolibrary::Stream_Buffer::put( char const * ) properly handles a put
 *        error.
 */
TEST( putNullTerminatedStringErrorHandling, putError )
{
    auto buffer = Mock_Stream_Buffer{};

    auto const error = Mock_Error{ 96 };

    EXPECT_CALL( buffer, put( A<char>() ) ).WillOnce( Return( error ) );

    auto const result = buffer.Stream_Buffer::put( "ifYRD2I" );

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

    auto const string = std::string_view{ "0cCzet1DQC" };

    for ( auto const character : string ) {
        EXPECT_CALL( buffer, put( SafeMatcherCast<char>( Eq( character ) ) ) )
            .WillOnce( Return( Result<void>{} ) );
    } // for

    EXPECT_FALSE( buffer.Stream_Buffer::put( string.data() ).is_error() );
}

/**
 * \brief Verify picolibrary::Stream_Buffer::put( std::uint8_t const *, std::uint8_t const
 *        * ) properly handles a put error.
 */
TEST( putUnsignedByteBlockErrorHandling, putError )
{
    auto buffer = Mock_Stream_Buffer{};

    auto const error = Mock_Error{ 224 };

    EXPECT_CALL( buffer, put( A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const values = std::vector<std::uint8_t>{ 0x8D, 0xBA, 0xF1, 0x37, 0x1F };

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

    auto const values = std::vector<std::uint8_t>{ 0x0E, 0x2D, 0xE6, 0xB0, 0x4D, 0x35 };

    for ( auto const value : values ) {
        EXPECT_CALL( buffer, put( SafeMatcherCast<std::uint8_t>( Eq( value ) ) ) )
            .WillOnce( Return( Result<void>{} ) );
    } // for

    EXPECT_FALSE( buffer.Stream_Buffer::put( &*values.begin(), &*values.end() ).is_error() );
}

/**
 * \brief Verify picolibrary::Stream_Buffer::put( std::int8_t const *, std::int8_t const *
 *        ) properly handles a put error.
 */
TEST( putSignedByteBlockErrorHandling, putError )
{
    auto buffer = Mock_Stream_Buffer{};

    auto const error = Mock_Error{ 114 };

    EXPECT_CALL( buffer, put( A<std::int8_t>() ) ).WillOnce( Return( error ) );

    auto const values = std::vector<std::int8_t>{ 0x35, 0x3B, 0x49, 0x44 };

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

    auto const values = std::vector<std::int8_t>{ 0x7D, 0x27, 0x2B, 0x1C, 0x7D };

    for ( auto const value : values ) {
        EXPECT_CALL( buffer, put( SafeMatcherCast<std::int8_t>( Eq( value ) ) ) )
            .WillOnce( Return( Result<void>{} ) );
    } // for

    EXPECT_FALSE( buffer.Stream_Buffer::put( &*values.begin(), &*values.end() ).is_error() );
}

/**
 * \brief Execute the picolibrary::Stream_Buffer automated tests.
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

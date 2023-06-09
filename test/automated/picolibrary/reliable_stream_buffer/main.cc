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
 * \brief picolibrary::Reliable_Stream_Buffer automated test program.
 */

#include <cstdint>
#include <string_view>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/stream.h"
#include "picolibrary/testing/automated/stream.h"

namespace {

using ::picolibrary::Testing::Automated::Mock_Reliable_Stream_Buffer;
using ::testing::Eq;
using ::testing::InSequence;
using ::testing::SafeMatcherCast;

} // namespace

/**
 * \brief Verify picolibrary::Reliable_Stream_Buffer::put( char const *, char const * )
 *        works properly.
 */
TEST( putCharBlock, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto buffer = Mock_Reliable_Stream_Buffer{};

    auto const string = std::string_view{ "XnQmlICQXCrJ" };

    for ( auto const character : string ) {
        EXPECT_CALL( buffer, put( SafeMatcherCast<char>( Eq( character ) ) ) );
    } // for

    buffer.Reliable_Stream_Buffer::put( string.begin(), string.end() );
}

/**
 * \brief Verify picolibrary::Reliable_Stream_Buffer::put( char const * ) works properly.
 */
TEST( putNullTerminatedString, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto buffer = Mock_Reliable_Stream_Buffer{};

    auto const string = std::string_view{ "0cCzet1DQC" };

    for ( auto const character : string ) {
        EXPECT_CALL( buffer, put( SafeMatcherCast<char>( Eq( character ) ) ) );
    } // for

    buffer.Reliable_Stream_Buffer::put( string.data() );
}

/**
 * \brief Verify picolibrary::Reliable_Stream_Buffer::put( std::uint8_t const *,
 *        std::uint8_t const * ) works properly.
 */
TEST( putUnsignedByteBlock, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto buffer = Mock_Reliable_Stream_Buffer{};

    auto const values = std::vector<std::uint8_t>{ 0x0E, 0x2D, 0xE6, 0xB0, 0x4D, 0x35 };

    for ( auto const value : values ) {
        EXPECT_CALL( buffer, put( SafeMatcherCast<std::uint8_t>( Eq( value ) ) ) );
    } // for

    buffer.Reliable_Stream_Buffer::put( &*values.begin(), &*values.end() );
}

/**
 * \brief Verify picolibrary::Reliable_Stream_Buffer::put( std::int8_t const *,
 *        std::int8_t const * ) works properly.
 */
TEST( putSignedByteBlock, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto buffer = Mock_Reliable_Stream_Buffer{};

    auto const values = std::vector<std::int8_t>{ 0x7D, 0x27, 0x2B, 0x1C, 0x7D };

    for ( auto const value : values ) {
        EXPECT_CALL( buffer, put( SafeMatcherCast<std::int8_t>( Eq( value ) ) ) );
    } // for

    buffer.Reliable_Stream_Buffer::put( &*values.begin(), &*values.end() );
}

/**
 * \brief Execute the picolibrary::Reliable_Stream_Buffer automated tests.
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

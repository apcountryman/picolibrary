/**
 * picolibrary
 *
 * Copyright 2020-2024, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream_Buffer
 *        automated test program.
 */

#include <cstdint>
#include <string_view>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/asynchronous_serial.h"
#include "picolibrary/testing/automated/asynchronous_serial.h"

namespace {

using ::picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream_Buffer;
using ::picolibrary::Testing::Automated::Asynchronous_Serial::Mock_Transmitter;
using ::testing::InSequence;

} // namespace

/**
 * \brief Verify
 *        picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream_Buffer::put(
 *        char ) works properly.
 */
TEST( putChar, worksProperly )
{
    auto transmitter = Mock_Transmitter<std::uint8_t>{};

    auto buffer = Reliable_Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    auto const character = 'L';

    EXPECT_CALL( transmitter, transmit( character ) );

    buffer.put( character );
}

/**
 * \brief Verify
 *        picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream_Buffer::put(
 *        char const *, char const * ) works properly.
 */
TEST( putCharBlock, worksProperly )
{
    auto transmitter = Mock_Transmitter<std::uint8_t>{};

    auto buffer = Reliable_Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    auto const string = std::string_view{ "12OA2mCrC1GL" };

    EXPECT_CALL( transmitter, transmit( std::vector<std::uint8_t>{ string.begin(), string.end() } ) );

    buffer.put( &*string.begin(), &*string.end() );
}

/**
 * \brief Verify
 *        picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream_Buffer::put(
 *        char const * ) works properly.
 */
TEST( putNullTerminatedString, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto transmitter = Mock_Transmitter<std::uint8_t>{};

    auto buffer = Reliable_Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    auto const string = std::string_view{ "hpT6C7BvWvUBk" };

    for ( auto const character : string ) {
        EXPECT_CALL( transmitter, transmit( character ) );
    } // for

    buffer.put( string.data() );
}

/**
 * \brief Verify
 *        picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream_Buffer::put(
 *        std::uint8_t ) works properly.
 */
TEST( putUnsignedByte, worksProperly )
{
    auto transmitter = Mock_Transmitter<std::uint8_t>{};

    auto buffer = Reliable_Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    auto const value = std::uint8_t{ 0xDE };

    EXPECT_CALL( transmitter, transmit( value ) );

    buffer.put( value );
}

/**
 * \brief Verify
 *        picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream_Buffer::put(
 *        std::uint8_t const *, std::uint8_t const * ) works properly.
 */
TEST( putUnsignedByteBlock, worksProperly )
{
    auto transmitter = Mock_Transmitter<std::uint8_t>{};

    auto buffer = Reliable_Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    auto const values = std::vector<std::uint8_t>{ 0x50, 0x2D, 0xD2, 0xA9, 0x90, 0xDC };

    EXPECT_CALL( transmitter, transmit( values ) );

    buffer.put( &*values.begin(), &*values.end() );
}

/**
 * \brief Verify
 *        picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream_Buffer::put(
 *        std::int8_t ) works properly.
 */
TEST( putSignedByte, worksProperly )
{
    auto transmitter = Mock_Transmitter<std::uint8_t>{};

    auto buffer = Reliable_Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    auto const value = std::int8_t{ 0x25 };

    EXPECT_CALL( transmitter, transmit( value ) );

    buffer.put( value );
}

/**
 * \brief Verify
 *        picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream_Buffer::put(
 *        std::int8_t const *, std::int8_t const * ) works properly.
 */
TEST( putSignedByteBlock, worksProperly )
{
    auto transmitter = Mock_Transmitter<std::uint8_t>{};

    auto buffer = Reliable_Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    auto const values = std::vector<std::int8_t>{ 0x59, 0x5F, 0x70, 0x60, 0x4D };

    EXPECT_CALL( transmitter, transmit( std::vector<std::uint8_t>{ values.begin(), values.end() } ) );

    buffer.put( &*values.begin(), &*values.end() );
}

/**
 * \brief Execute the
 *        picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream_Buffer
 *        automated tests.
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

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
 * \brief picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer unit test
 *        program.
 */

#include <cstdint>
#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/asynchronous_serial/stream.h"
#include "picolibrary/testing/unit/asynchronous_serial.h"
#include "picolibrary/testing/unit/random.h"

namespace {

using ::picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::picolibrary::Testing::Unit::Asynchronous_Serial::Mock_Transmitter;
using ::testing::InSequence;

} // namespace

/**
 * \brief Verify picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer::put(
 *        char ) works properly.
 */
TEST( putChar, worksProperly )
{
    auto transmitter = Mock_Transmitter<std::uint8_t>{};

    auto buffer = Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    auto const character = random<char>();

    EXPECT_CALL( transmitter, transmit( character ) );

    EXPECT_FALSE( buffer.put( character ).is_error() );
}

/**
 * \brief Verify picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer::put(
 *        char const * begin, char const * end ) works properly.
 */
TEST( putCharBlock, worksProperly )
{
    auto transmitter = Mock_Transmitter<std::uint8_t>{};

    auto buffer = Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    auto const string = random_container<std::string>();

    EXPECT_CALL( transmitter, transmit( std::vector<std::uint8_t>{ string.begin(), string.end() } ) );

    EXPECT_FALSE( buffer.put( &*string.begin(), &*string.end() ).is_error() );
}

/**
 * \brief Verify picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer::put(
 *        char const * ) works properly.
 */
TEST( putNullTerminatedString, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto transmitter = Mock_Transmitter<std::uint8_t>{};

    auto buffer = Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    auto const string = random_container<std::string>();

    for ( auto const character : string ) {
        EXPECT_CALL( transmitter, transmit( character ) );
    } // for

    EXPECT_FALSE( buffer.put( string.c_str() ).is_error() );
}

/**
 * \brief Verify picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer::put(
 *        std::uint8_t ) works properly.
 */
TEST( putUnsignedByte, worksProperly )
{
    auto transmitter = Mock_Transmitter<std::uint8_t>{};

    auto buffer = Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    auto const value = random<std::uint8_t>();

    EXPECT_CALL( transmitter, transmit( value ) );

    EXPECT_FALSE( buffer.put( value ).is_error() );
}

/**
 * \brief Verify picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer::put(
 *        std::uint8_t const * begin, std::uint8_t const * end ) works properly.
 */
TEST( putUnsignedByteBlock, worksProperly )
{
    auto transmitter = Mock_Transmitter<std::uint8_t>{};

    auto buffer = Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    auto const values = random_container<std::vector<std::uint8_t>>();

    EXPECT_CALL( transmitter, transmit( values ) );

    EXPECT_FALSE( buffer.put( &*values.begin(), &*values.end() ).is_error() );
}

/**
 * \brief Verify picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer::put(
 *        std::int8_t ) works properly.
 */
TEST( putSignedByte, worksProperly )
{
    auto transmitter = Mock_Transmitter<std::uint8_t>{};

    auto buffer = Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    auto const value = random<std::int8_t>();

    EXPECT_CALL( transmitter, transmit( value ) );

    EXPECT_FALSE( buffer.put( value ).is_error() );
}

/**
 * \brief Verify picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer::put(
 *        std::int8_t const * begin, std::int8_t const * end ) works properly.
 */
TEST( putSignedByteBlock, worksProperly )
{
    auto transmitter = Mock_Transmitter<std::uint8_t>{};

    auto buffer = Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    auto const values = random_container<std::vector<std::int8_t>>();

    EXPECT_CALL( transmitter, transmit( std::vector<std::uint8_t>{ values.begin(), values.end() } ) );

    EXPECT_FALSE( buffer.put( &*values.begin(), &*values.end() ).is_error() );
}

/**
 * \brief Execute the picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer
 *        unit tests.
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

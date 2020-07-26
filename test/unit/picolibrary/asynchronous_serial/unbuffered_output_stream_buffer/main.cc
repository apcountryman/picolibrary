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
 * \brief picolibrary::Asynchronous_Serial::Unuffered_Output_Stream_Buffer unit test
 *        program.
 */

#include <cstdint>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/asynchronous_serial/stream.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/asynchronous_serial.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/utility.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::testing::A;
using ::testing::Return;

using Mock_Transmitter = ::picolibrary::Testing::Unit::Asynchronous_Serial::Mock_Transmitter<std::uint8_t>;

} // namespace

/**
 * \brief Verify picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer::put(
 *        char ) properly handles a put error.
 */
TEST( putChar, putError )
{
    auto transmitter = Mock_Transmitter{};

    auto buffer = Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( transmitter, transmit( A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = buffer.put( random<char>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer::put(
 *        char ) works properly.
 */
TEST( putChar, worksProperly )
{
    auto transmitter = Mock_Transmitter{};

    auto buffer = Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    auto const character = random<char>();

    EXPECT_CALL( transmitter, transmit( character ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( buffer.put( character ).is_error() );
}

/**
 * \brief Verify picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer::put(
 *        std::uint8_t ) properly handles a put error.
 */
TEST( putUnsignedByte, putError )
{
    auto transmitter = Mock_Transmitter{};

    auto buffer = Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( transmitter, transmit( A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = buffer.put( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer::put(
 *        std::uint8_t ) works properly.
 */
TEST( putUnsignedByte, worksProperly )
{
    auto transmitter = Mock_Transmitter{};

    auto buffer = Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    auto const value = random<std::uint8_t>();

    EXPECT_CALL( transmitter, transmit( value ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( buffer.put( value ).is_error() );
}

/**
 * \brief Verify picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer::put(
 *        std::int8_t ) properly handles a put error.
 */
TEST( putSignedByte, putError )
{
    auto transmitter = Mock_Transmitter{};

    auto buffer = Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( transmitter, transmit( A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = buffer.put( random<std::int8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer::put(
 *        std::int8_t ) works properly.
 */
TEST( putSignedByte, worksProperly )
{
    auto transmitter = Mock_Transmitter{};

    auto buffer = Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    auto const value = random<std::int8_t>();

    EXPECT_CALL( transmitter, transmit( value ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( buffer.put( value ).is_error() );
}

/**
 * \brief Verify
 *        picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer::flush() works
 *        properly.
 */
TEST( flush, worksProperly )
{
    auto transmitter = Mock_Transmitter{};

    auto buffer = Unbuffered_Output_Stream_Buffer{ transmitter.handle() };

    EXPECT_FALSE( buffer.flush().is_error() );
}

/**
 * \brief Execute the picolibrary::Asynchronous_Serial::Unuffered_Output_Stream_Buffer
 *        unit tests.
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

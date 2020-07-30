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
 * \brief picolibrary::Asynchronous_Serial::Unuffered_Output_Stream unit test program.
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
using ::picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::testing::Return;

using Mock_Transmitter = ::picolibrary::Testing::Unit::Asynchronous_Serial::Mock_Transmitter<std::uint8_t>;

} // namespace

/**
 * \brief Verify
 *        picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream::Unbuffered_Output_Stream()
 *        works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const stream = Unbuffered_Output_Stream<Mock_Transmitter::Handle>{};

    EXPECT_FALSE( stream.buffer_is_set() );
}

/**
 * \brief Verify
 *        picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream::Unbuffered_Output_Stream(
 *        Transmitter ) works properly.
 */
TEST( constructorTransmitter, worksProperly )
{
    auto transmitter = Mock_Transmitter{};

    auto const stream = Unbuffered_Output_Stream{ transmitter.handle() };

    EXPECT_TRUE( stream.buffer_is_set() );
}

/**
 * \brief Verify
 *        picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream::Unbuffered_Output_Stream(
 *        picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream && ) works properly.
 */
TEST( constructorMove, worksProperly )
{
    {
        auto       source      = Unbuffered_Output_Stream<Mock_Transmitter::Handle>{};
        auto const destination = Unbuffered_Output_Stream{ std::move( source ) };

        EXPECT_FALSE( source.buffer_is_set() );
        EXPECT_FALSE( destination.buffer_is_set() );
    }

    {
        auto transmitter = Mock_Transmitter{};

        auto       source      = Unbuffered_Output_Stream{ transmitter.handle() };
        auto const destination = Unbuffered_Output_Stream{ std::move( source ) };

        EXPECT_FALSE( source.buffer_is_set() );
        EXPECT_TRUE( destination.buffer_is_set() );
    }
}

/**
 * \brief Verify
 *        picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream::operator=(
 *        picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream && ) works properly.
 */
TEST( assignmentOperatorMove, worksProperly )
{
    {
        auto expression = Unbuffered_Output_Stream<Mock_Transmitter::Handle>{};
        auto object     = Unbuffered_Output_Stream<Mock_Transmitter::Handle>{};

        object = std::move( expression );

        EXPECT_FALSE( expression.buffer_is_set() );
        EXPECT_FALSE( object.buffer_is_set() );
    }

    {
        auto transmitter = Mock_Transmitter{};

        auto expression = Unbuffered_Output_Stream{ transmitter.handle() };
        auto object     = Unbuffered_Output_Stream<Mock_Transmitter::Handle>{};

        object = std::move( expression );

        EXPECT_FALSE( expression.buffer_is_set() );
        EXPECT_TRUE( object.buffer_is_set() );
    }

    {
        auto transmitter = Mock_Transmitter{};

        auto expression = Unbuffered_Output_Stream<Mock_Transmitter::Handle>{};
        auto object     = Unbuffered_Output_Stream{ transmitter.handle() };

        object = std::move( expression );

        EXPECT_FALSE( expression.buffer_is_set() );
        EXPECT_FALSE( object.buffer_is_set() );
    }

    {
        auto transmitter = Mock_Transmitter{};

        auto expression = Unbuffered_Output_Stream{ transmitter.handle() };
        auto object     = Unbuffered_Output_Stream{ transmitter.handle() };

        object = std::move( expression );

        EXPECT_FALSE( expression.buffer_is_set() );
        EXPECT_TRUE( object.buffer_is_set() );
    }
}

/**
 * \brief Verify picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream::initialize()
 *        properly handles a transmitter initialization error.
 */
TEST( initialize, transmitterInitializationError )
{
    auto transmitter = Mock_Transmitter{};

    auto stream = Unbuffered_Output_Stream{ transmitter.handle() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( transmitter, initialize() ).WillOnce( Return( error ) );

    auto const result = stream.initialize();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream::initialize()
 *        works properly.
 */
TEST( initialize, worksProperly )
{
    auto transmitter = Mock_Transmitter{};

    auto stream = Unbuffered_Output_Stream{ transmitter.handle() };

    EXPECT_CALL( transmitter, initialize() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( stream.initialize().is_error() );
}

/**
 * \brief Execute the picolibrary::Asynchronous_Serial::Unuffered_Output_Stream unit
 *        tests.
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

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
 * \brief picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream automated
 *        test program.
 */

#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/asynchronous_serial/stream.h"
#include "picolibrary/testing/automated/asynchronous_serial.h"

namespace {

using ::picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream;
using ::picolibrary::Testing::Automated::Asynchronous_Serial::Mock_Transmitter;

} // namespace

/**
 * \brief Verify
 *        picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream::Reliable_Unbuffered_Output_Stream()
 *        works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const stream = Reliable_Unbuffered_Output_Stream<Mock_Transmitter<std::uint8_t>::Handle>{};

    EXPECT_FALSE( stream.buffer_is_set() );
}

/**
 * \brief Verify
 *        picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream::Reliable_Unbuffered_Output_Stream(
 *        Transmitter ) works properly.
 */
TEST( constructorTransmitter, worksProperly )
{
    auto transmitter = Mock_Transmitter<std::uint8_t>{};

    auto const stream = Reliable_Unbuffered_Output_Stream{ transmitter.handle() };

    EXPECT_TRUE( stream.buffer_is_set() );
}

/**
 * \brief Verify
 *        picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream::Reliable_Unbuffered_Output_Stream(
 *        picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream && ) works
 *        properly.
 */
TEST( constructorMove, worksProperly )
{
    {
        auto source = Reliable_Unbuffered_Output_Stream<Mock_Transmitter<std::uint8_t>::Handle>{};
        auto const destination = Reliable_Unbuffered_Output_Stream{ std::move( source ) };

        EXPECT_FALSE( source.buffer_is_set() );
        EXPECT_FALSE( destination.buffer_is_set() );
    }

    {
        auto transmitter = Mock_Transmitter<std::uint8_t>{};

        auto       source = Reliable_Unbuffered_Output_Stream{ transmitter.handle() };
        auto const destination = Reliable_Unbuffered_Output_Stream{ std::move( source ) };

        EXPECT_FALSE( source.buffer_is_set() );
        EXPECT_TRUE( destination.buffer_is_set() );
    }
}

/**
 * \brief Verify
 *        picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream::operator=(
 *        picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream && ) works
 *        properly.
 */
TEST( assignmentOperatorMove, worksProperly )
{
    {
        auto expression = Reliable_Unbuffered_Output_Stream<Mock_Transmitter<std::uint8_t>::Handle>{};
        auto object = Reliable_Unbuffered_Output_Stream<Mock_Transmitter<std::uint8_t>::Handle>{};

        object = std::move( expression );

        EXPECT_FALSE( expression.buffer_is_set() );
        EXPECT_FALSE( object.buffer_is_set() );
    }

    {
        auto transmitter = Mock_Transmitter<std::uint8_t>{};

        auto expression = Reliable_Unbuffered_Output_Stream{ transmitter.handle() };
        auto object = Reliable_Unbuffered_Output_Stream<Mock_Transmitter<std::uint8_t>::Handle>{};

        object = std::move( expression );

        EXPECT_FALSE( expression.buffer_is_set() );
        EXPECT_TRUE( object.buffer_is_set() );
    }

    {
        auto transmitter = Mock_Transmitter<std::uint8_t>{};

        auto expression = Reliable_Unbuffered_Output_Stream<Mock_Transmitter<std::uint8_t>::Handle>{};
        auto object = Reliable_Unbuffered_Output_Stream{ transmitter.handle() };

        object = std::move( expression );

        EXPECT_FALSE( expression.buffer_is_set() );
        EXPECT_FALSE( object.buffer_is_set() );
    }

    {
        auto transmitter = Mock_Transmitter<std::uint8_t>{};

        auto expression = Reliable_Unbuffered_Output_Stream{ transmitter.handle() };
        auto object     = Reliable_Unbuffered_Output_Stream{ transmitter.handle() };

        object = std::move( expression );

        EXPECT_FALSE( expression.buffer_is_set() );
        EXPECT_TRUE( object.buffer_is_set() );
    }

    {
        auto stream = Reliable_Unbuffered_Output_Stream<Mock_Transmitter<std::uint8_t>::Handle>{};

        stream = std::move( stream );

        EXPECT_FALSE( stream.buffer_is_set() );
    }

    {
        auto transmitter = Mock_Transmitter<std::uint8_t>{};

        auto stream = Reliable_Unbuffered_Output_Stream{ transmitter.handle() };

        stream = std::move( stream );

        EXPECT_TRUE( stream.buffer_is_set() );
    }
}

/**
 * \brief Execute the picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream
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

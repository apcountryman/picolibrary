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

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/asynchronous_serial/stream.h"
#include "picolibrary/testing/unit/asynchronous_serial.h"

namespace {

using Mock_Transmitter = ::picolibrary::Testing::Unit::Asynchronous_Serial::Mock_Transmitter<std::uint8_t>;

using Unbuffered_Output_Stream_Buffer =
    ::picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer<Mock_Transmitter>;

} // namespace

/**
 * \brief Verify
 *        picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer::Unbuffered_Output_Stream_Buffer()
 *        works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const buffer = Unbuffered_Output_Stream_Buffer{};

    EXPECT_EQ( buffer.transmitter(), nullptr );
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

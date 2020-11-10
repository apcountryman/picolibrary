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
 * \brief picolibrary::Asynchronous_Serial::Transmitter unit test program.
 */

#include <cstdint>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/asynchronous_serial.h"
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
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

using Transmitter = ::picolibrary::Asynchronous_Serial::Transmitter<
    ::picolibrary::Testing::Unit::Asynchronous_Serial::Mock_Basic_Transmitter<std::uint8_t>>;

} // namespace

/**
 * \brief Verify picolibrary::Asynchronous_Serial::Transmitter::transmit() properly
 *        handles a transmission error.
 */
TEST( transmit, transmissionError )
{
    auto transmitter = Transmitter{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( transmitter, transmit( _ ) ).WillOnce( Return( error ) );

    auto const values = random_container<std::vector<std::uint8_t>>( random<std::uint_fast8_t>( 1 ) );

    auto const result = transmitter.transmit( &*values.begin(), &*values.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Asynchronous_Serial::Transmitter::transmit() works properly.
 */
TEST( transmit, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto transmitter = Transmitter{};

    auto const values = random_container<std::vector<std::uint8_t>>();

    for ( auto const value : values ) {
        EXPECT_CALL( transmitter, transmit( value ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    } // for

    EXPECT_FALSE( transmitter.transmit( &*values.begin(), &*values.end() ).is_error() );
}

/**
 * \brief Execute the picolibrary::Asynchronous_Serial::Transmitter unit tests.
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

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
 * \brief picolibrary::Asynchronous_Serial::Transmitter automated test program.
 */

#include <cstdint>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/asynchronous_serial.h"
#include "picolibrary/testing/automated/asynchronous_serial.h"
#include "picolibrary/testing/automated/random.h"

namespace {

using ::picolibrary::Asynchronous_Serial::Transmitter;
using ::picolibrary::Testing::Automated::random_container;
using ::picolibrary::Testing::Automated::Asynchronous_Serial::Mock_Basic_Transmitter;
using ::testing::InSequence;

} // namespace

/**
 * \brief Verify picolibrary::Asynchronous_Serial::Transmitter::transmit() works properly.
 */
TEST( transmit, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto transmitter = Transmitter<Mock_Basic_Transmitter<std::uint8_t>>{};

    auto const values = random_container<std::vector<std::uint8_t>>();

    for ( auto const value : values ) {
        EXPECT_CALL( transmitter, transmit( value ) );
    } // for

    transmitter.transmit( &*values.begin(), &*values.end() );
}

/**
 * \brief Execute the picolibrary::Asynchronous_Serial::Transmitter automated tests.
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

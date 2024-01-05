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
 * \brief picolibrary::Debounce::Multiple_Signals_Debouncer automated test program.
 */

#include <cstddef>
#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/debounce.h"

namespace {

using ::picolibrary::Debounce::Multiple_Signals_Debouncer;

} // namespace

/**
 * \brief Verify picolibrary::Debounce::Multiple_Signals_Debouncer works properly.
 */
TEST( debouncer, worksProperly )
{
    auto signals = Multiple_Signals_Debouncer<std::uint8_t, 6>{ 0b10100011 };

    EXPECT_EQ( signals.state(), 0b10100011 );

    struct {
        std::uint8_t sample;
        std::uint8_t state;
    } const data[]{
        // clang-format off

        { 0b10100011, 0b10100011 },
        { 0b10100011, 0b10100011 },
        { 0b10100011, 0b10100011 },
        { 0b10100011, 0b10100011 },
        { 0b10100011, 0b10100011 },
        { 0b10100011, 0b10100011 },

        { 0b01011100, 0b10100011 },
        { 0b10100011, 0b10100011 },
        { 0b10100011, 0b10100011 },
        { 0b10100011, 0b10100011 },
        { 0b10100011, 0b10100011 },
        { 0b10100011, 0b10100011 },

        { 0b01011100, 0b10100011 },
        { 0b01011100, 0b10100011 },
        { 0b01011100, 0b10100011 },
        { 0b01011100, 0b10100011 },
        { 0b01011100, 0b10100011 },
        { 0b10100011, 0b10100011 },

        { 0b00100000, 0b10100011 },
        { 0b10111101, 0b10100011 },
        { 0b01000111, 0b10100011 },
        { 0b01111100, 0b10100011 },
        { 0b10111011, 0b10100011 },
        { 0b10100011, 0b10100011 },

        { 0b01011100, 0b10100011 },
        { 0b01011100, 0b10100011 },
        { 0b01011100, 0b10100011 },
        { 0b01011100, 0b10100011 },
        { 0b01011100, 0b10100011 },
        { 0b01011100, 0b01011100 },

        // clang-format on
    };

    for ( auto i = std::size_t{}; i < sizeof( data ) / sizeof( data[ 0 ] ); ++i ) {
        auto const [ sample, state ] = data[ i ];

        signals.debounce( sample );

        EXPECT_EQ( signals.state(), state ) << "i: " << i;
    } // for
}

/**
 * \brief Execute the picolibrary::Debounce::Multiple_Signals_Debouncer automated tests.
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

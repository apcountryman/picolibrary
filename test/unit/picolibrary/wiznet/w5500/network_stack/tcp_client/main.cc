/**
 * picolibrary
 *
 * Copyright 2020-2021, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::WIZnet::W5500::Network_Stack::TCP_Client unit test program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/testing/unit/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/network_stack.h"

namespace {

using ::picolibrary::Testing::Unit::WIZnet::W5500::Mock_Driver;
using ::picolibrary::WIZnet::W5500::Socket_ID;

using TCP_Client = ::picolibrary::WIZnet::W5500::Network_Stack<Mock_Driver>::TCP_Client;

} // namespace

/**
 * \brief Verify picolibrary::WIZnet::W5500::Network_Stack::TCP_Client::TCP_Client(
 *        picolibrary::WIZnet::W5500::Socket_ID ) works properly.
 */
TEST( constructor, worksProperly )
{
    struct {
        Socket_ID socket_id;
    } const test_cases[]{
        // clang-format off

        { Socket_ID::_0 },
        { Socket_ID::_1 },
        { Socket_ID::_2 },
        { Socket_ID::_3 },
        { Socket_ID::_4 },
        { Socket_ID::_5 },
        { Socket_ID::_6 },
        { Socket_ID::_7 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const client = TCP_Client{ test_case.socket_id };

        EXPECT_EQ( client.socket_id(), test_case.socket_id );
    } // for
}

/**
 * \brief Execute the picolibrary::WIZnet::W5500::Network_Stack::TCP_Client unit tests.
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

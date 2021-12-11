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
 * \brief picolibrary::Testing::Unit::IP::Network_Stack interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_IP_NETWORK_STACK_H
#define PICOLIBRARY_TESTING_UNIT_IP_NETWORK_STACK_H

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/ip/tcp.h"

namespace picolibrary::Testing::Unit::IP {

/**
 * \brief Mock IP network stack.
 */
class Mock_Network_Stack {
  public:
    using TCP_Client = TCP::Mock_Client::Handle;

    using TCP_Server = TCP::Mock_Server::Handle;

    using TCP_Acceptor = TCP::Mock_Acceptor::Handle;

    Mock_Network_Stack() = default;

    Mock_Network_Stack( Mock_Network_Stack && ) = delete;

    Mock_Network_Stack( Mock_Network_Stack const & ) = delete;

    ~Mock_Network_Stack() noexcept = default;

    auto operator=( Mock_Network_Stack && ) = delete;

    auto operator=( Mock_Network_Stack const & ) = delete;

    MOCK_METHOD( (Result<TCP_Client, Error_Code>), make_tcp_client, () );

    MOCK_METHOD( (Result<TCP_Acceptor, Error_Code>), make_tcp_acceptor, () );
};

} // namespace picolibrary::Testing::Unit::IP

#endif // PICOLIBRARY_TESTING_UNIT_IP_NETWORK_STACK_H

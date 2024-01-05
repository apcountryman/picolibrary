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
 * \brief picolibrary::Testing::Automated::IP::Network_Stack interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_IP_NETWORK_STACK_H
#define PICOLIBRARY_TESTING_AUTOMATED_IP_NETWORK_STACK_H

#include "picolibrary/testing/automated/ip/tcp.h"
#include "picolibrary/testing/automated/ip/udp.h"

namespace picolibrary::Testing::Automated::IP {

/**
 * \brief Mock network stack.
 */
class Mock_Network_Stack {
  public:
    using TCP_Client = TCP::Mock_Client::Handle;

    using TCP_Server = TCP::Mock_Server::Handle;

    using UDP_Socket = UDP::Mock_Socket::Handle;

    Mock_Network_Stack() = default;

    Mock_Network_Stack( Mock_Network_Stack && ) = delete;

    Mock_Network_Stack( Mock_Network_Stack const & ) = delete;

    ~Mock_Network_Stack() noexcept = default;

    auto operator=( Mock_Network_Stack && ) = delete;

    auto operator=( Mock_Network_Stack const & ) = delete;

    MOCK_METHOD( TCP_Client, make_tcp_client, () );

    MOCK_METHOD( TCP_Server, make_tcp_server, () );

    MOCK_METHOD( UDP_Socket, make_udp_socket, () );
};

} // namespace picolibrary::Testing::Automated::IP

#endif // PICOLIBRARY_TESTING_AUTOMATED_IP_NETWORK_STACK_H

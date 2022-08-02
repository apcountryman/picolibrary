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
 * \brief picolibrary::IP::Network_Stack interface.
 */

#ifndef PICOLIBRARY_IP_NETWORK_STACK_H
#define PICOLIBRARY_IP_NETWORK_STACK_H

#include "picolibrary/ip/tcp.h"

namespace picolibrary::IP {

/**
 * \brief Network stack concept.
 */
class Network_Stack_Concept {
  public:
    /**
     * \brief The type of TCP client socket that is used to interact with the network
     *        stack.
     */
    using TCP_Client = TCP::Client_Concept;

    /**
     * \brief The type of TCP server socket that is used to interact with the network
     *        stack.
     */
    using TCP_Server = TCP::Server_Concept;

    /**
     * \brief The type of TCP acceptor socket that is used to interact with the network
     *        stack.
     */
    using TCP_Acceptor = TCP::Acceptor_Concept;

    /**
     * \brief Constructor.
     */
    Network_Stack_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Network_Stack_Concept( Network_Stack_Concept && source ) noexcept;

    Network_Stack_Concept( Network_Stack_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Network_Stack_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Network_Stack_Concept && expression ) noexcept -> Network_Stack_Concept &;

    auto operator=( Network_Stack_Concept const & ) = delete;

    /**
     * \brief Construct a TCP client socket.
     *
     * \pre sufficient resources are available to create the client socket
     *
     * \return The constructed TCP client socket.
     */
    auto make_tcp_client() noexcept -> TCP_Client;

    /**
     * \brief Construct a TCP acceptor socket.
     *
     * \pre sufficient resources are available to create the acceptor socket
     *
     * \return The constructed TCP acceptor socket.
     */
    auto make_tcp_acceptor() noexcept -> TCP_Acceptor;
};

} // namespace picolibrary::IP

#endif // PICOLIBRARY_IP_NETWORK_STACK_H
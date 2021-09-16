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
 * \brief picolibrary::IP::Network_Stack interface.
 */

#ifndef PICOLIBRARY_IP_NETWORK_STACK_H
#define PICOLIBRARY_IP_NETWORK_STACK_H

#include "picolibrary/error.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/result.h"

namespace picolibrary::IP {

/**
 * \brief IP network stack concept.
 */
class Network_Stack_Concept {
  public:
    /**
     * \brief The type of TCP client socket that is used to interact with the network
     *        stack.
     *
     * \attention This type alias should be omitted if the network stack does not support
     *            TCP.
     */
    using TCP_Client = TCP::Client_Concept;

    /**
     * \brief The type of TCP server socket that is used to interact with the network
     *        stack.
     *
     * \attention This type alias should be omitted if the network stack does not support
     *            TCP.
     */
    using TCP_Server = TCP::Server_Concept;

    /**
     * \brief The type of TCP acceptor socket that is used to interact with the network
     *        stack.
     *
     * \attention This type alias should be omitted if the network stack does not support
     *            TCP.
     */
    using TCP_Acceptor = TCP::Acceptor_Concept;

    /**
     * \brief Constructor.
     */
    Network_Stack_Concept() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Network_Stack_Concept( Network_Stack_Concept && source ) noexcept = default;

    Network_Stack_Concept( Network_Stack_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Network_Stack_Concept() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Network_Stack_Concept && expression ) noexcept -> Network_Stack_Concept & = default;

    auto operator=( Network_Stack_Concept const & ) = delete;

    /**
     * \brief Construct a TCP client socket.
     *
     * \return The constructed TCP client socket if TCP client socket construction
     *         succeeded.
     * \return picolibrary::Generic_Error::NO_SOCKETS_AVAILABLE if no sockets are
     *         available.
     * \return An error code if TCP client socket construction failed for any other
     *         reason.
     */
    auto make_tcp_client() noexcept -> Result<TCP_Client, Error_Code>;

    /**
     * \brief Construct a TCP acceptor socket.
     *
     * \return The constructed TCP acceptor socket if TCP acceptor socket construction
     *         succeeded.
     * \return picolibrary::Generic_Error::NO_SOCKETS_AVAILABLE if no sockets are
     *         available.
     * \return An error code if TCP acceptor socket construction failed for any other
     *         reason.
     */
    auto make_tcp_acceptor() noexcept -> Result<TCP_Acceptor, Error_Code>;
};

} // namespace picolibrary::IP

#endif // PICOLIBRARY_IP_NETWORK_STACK_H

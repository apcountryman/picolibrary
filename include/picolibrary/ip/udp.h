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
 * \brief picolibrary::IP::UDP interface.
 */

#ifndef PICOLIBRARY_IP_UDP_H
#define PICOLIBRARY_IP_UDP_H

#include <cstdint>

#include "picolibrary/ip.h"
#include "picolibrary/result.h"

/**
 * \brief UDP over IP facilities.
 */
namespace picolibrary::IP::UDP {

/**
 * \brief Port.
 */
using Port = ::picolibrary::IP::Port;

/**
 * \brief Endpoint.
 */
using Endpoint = ::picolibrary::IP::Endpoint;

/**
 * \brief Reception result.
 */
struct Reception_Result {
    /**
     * \brief The endpoint the datagram was received from.
     */
    Endpoint endpoint;

    /**
     * \brief The end of the data read from the datagram.
     */
    std::uint8_t * end;
};

/**
 * \brief Socket concept.
 */
class Socket_Concept {
  public:
    /**
     * \brief Constructor.
     */
    Socket_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Socket_Concept( Socket_Concept && source ) noexcept;

    Socket_Concept( Socket_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Socket_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Socket_Concept && expression ) noexcept -> Socket_Concept &;

    auto operator=( Socket_Concept const & ) = delete;

    /**
     * \brief Bind the socket to a local endpoint.
     *
     * \pre the socket is not already bound to a local endpoint
     *
     * \param[in] endpoint The local endpoint to bind the socket to.
     *
     * \pre endpoint is a valid local endpoint
     * \pre endpoint is not already in use
     * \pre if an ephemeral port is requested, an ephemeral port is available
     */
    void bind( Endpoint const & endpoint = Endpoint{} ) noexcept;

    /**
     * \brief Get the socket's local endpoint.
     *
     * \return The socket's local endpoint.
     */
    auto local_endpoint() const noexcept -> Endpoint;

    /**
     * \brief Transmit a datagram to a remote endpoint.
     *
     * \pre the socket has been bound to a local endpoint
     *
     * \param[in] endpoint The endpoint to transmit the datagram to.
     * \param[in] begin The beginning of the block of data to transmit in the datagram.
     * \param[in] end The end of the block of data to transmit in the datagram.
     *
     * \pre endpoint is a valid remote endpoint
     *
     * \return Nothing if transmitting the datagram succeeded.
     * \return picolibrary::Generic_Error::EXCESSIVE_MESSAGE_SIZE if the block of data is
     *         too large to transmit in a single datagram.
     * \return picolibrary::Generic_Error::WOULD_BLOCK or
     *         picolibrary::Generic_Error::OPERATION_TIMEOUT if the socket is in a
     *         non-blocking mode, and the datagram cannot be transmitted without blocking.
     * \return picolibrary::Generic_Error::OPERATION_TIMEOUT if a timeout occurred before
     *         the datagram could be transmitted.
     */
    auto transmit( Endpoint const & endpoint, std::uint8_t const * begin, std::uint8_t const * end ) noexcept
        -> Result<void>;

    /**
     * \brief Receive a datagram from a remote endpoint.
     *
     * \pre the socket has been bound to a local endpoint
     *
     * \param[out] begin The beginning of the block of data read from the datagram.
     * \param[out] end The end of the block of data read from the datagram.
     *
     * \return The endpoint the datagram was received from and the end of the data read
     *         from the datagram if receiving a datagram succeeded.
     * \return picolibrary::Generic_Error::WOULD_BLOCK or
     *         picolibrary::Generic_Error::OPERATION_TIMEOUT if the socket is in a
     *         non-blocking mode, and a datagram could not be received without blocking.
     * \return picolibrary::Generic_Error::OPERATION_TIMEOUT if a timeout occurred before
     *         a datagram could be received.
     */
    auto receive( std::uint8_t * begin, std::uint8_t * end ) noexcept -> Result<Reception_Result>;

    /**
     * \brief Close the socket.
     */
    void close() noexcept;
};

} // namespace picolibrary::IP::UDP

#endif // PICOLIBRARY_IP_UDP_H

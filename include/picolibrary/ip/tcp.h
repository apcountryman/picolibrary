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
 * \brief picolibrary::IP::TCP interface.
 */

#ifndef PICOLIBRARY_IP_TCP_H
#define PICOLIBRARY_IP_TCP_H

#include <cstddef>
#include <cstdint>

#include "picolibrary/error.h"
#include "picolibrary/ip.h"
#include "picolibrary/result.h"
#include "picolibrary/void.h"

/**
 * \brief Transmission Control Protocol (TCP) over IP facilities.
 */
namespace picolibrary::IP::TCP {

/**
 * \brief Port number.
 */
using Port = IP::Port;

/**
 * \brief Endpoint.
 */
using Endpoint = IP::Endpoint;

/**
 * \brief Client socket concept.
 */
class Client_Concept {
  public:
    /**
     * \brief The unsigned integral type used to report transmit/receive buffer
     *        information.
     */
    using Size = std::size_t;

    /**
     * \brief Constructor.
     */
    Client_Concept() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Client_Concept( Client_Concept && source ) noexcept = default;

    Client_Concept( Client_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Client_Concept() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Client_Concept && expression ) noexcept -> Client_Concept & = default;

    auto operator=( Client_Concept const & ) = delete;

    /**
     * \brief Bind the socket to a specific local endpoint.
     *
     * \param[in] endpoint The local endpoint to bind the socket to.
     *
     * \return Nothing if binding the socket to the local endpoint succeeded.
     * \return picolibrary::Generic_Error::INVALID_ARGUMENT if endpoint is not a valid
     *         local endpoint.
     * \return picolibrary::Generic_Error::LOGIC_ERROR if the socket has already been
     *         bound to a local endpoint.
     * \return picolibrary::Generic_Error::ENDPOINT_IN_USE if endpoint is already in use.
     * \return picolibrary::Generic_Error::EPHEMERAL_PORTS_EXHAUSTED if an ephemeral port
     *         was requested and no ephemeral ports are available.
     * \return An error code if binding the socket to the local endpoint failed for any
     *         other reason.
     */
    auto bind( Endpoint const & endpoint = Endpoint{} ) noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Connect to a remote endpoint.
     *
     * \param[in] endpoint The remote endpoint to connect to.
     *
     * \return Nothing if connecting to the remote endpoint succeeded.
     * \return picolibrary::Generic_Error::INVALID_ARGUMENT if endpoint is not a valid
     *         remote endpoint.
     * \return picolibrary::Generic_Error::LOGIC_ERROR if the socket is not in a state
     *         that allows it to connect to a remote endpoint.
     * \return picolibrary::Generic_Error::LOGIC_ERROR if the socket is already connected
     *         to a remote endpoint.
     * \return picolibrary::Generic_Error::LOGIC_ERROR if the socket does not support
     *         connecting to a remote endpoint without having first bound the socket to a
     *         local endpoint, and the socket has not been bound to a local endpoint.
     * \return picolibrary::Generic_Error::EPHEMERAL_PORTS_EXHAUSTED if the socket does
     *         support connecting to a remote endpoint without having first bound the
     *         socket to a local endpoint and the socket has not been bound to a local
     *         endpoint, but no ephemeral ports are available.
     * \return picolibrary::Generic_Error::WOULD_BLOCK if the socket is in a non-blocking
     *         mode, and connecting to the remote endpoint cannot succeed immediately.
     * \return picolibrary::Generic_Error::OPERATION_TIMEOUT if connecting to the remote
     *         endpoint timed out.
     * \return An error code if connecting to the remote endpoint failed for any other
     *         reason.
     */
    auto connect( Endpoint const & endpoint ) noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Check if the socket is connected to a remote endpoint.
     *
     * \return true if getting the socket's connection state succeeded and the socket is
     *         connected to a remote endpoint.
     * \return false if getting the socket's connection state succeeded and the socket is
     *         not connected to a remote endpoint.
     * \return An error code if getting the socket's connection state failed. If getting
     *         the socket's connection state cannot fail, return picolibrary::Result<bool,
     *         picolibrary::Void>.
     */
    auto is_connected() const noexcept -> Result<bool, Error_Code>;

    /**
     * \brief Get the connection's remote endpoint.
     *
     * \return The connection's remote endpoint if getting the connection's remote
     *         endpoint succeeded.
     * \return An error code if getting the connection's remote endpoint failed. If
     *         getting the connection's remote endpoint cannot fail, return
     *         picolibrary::Result<picolibrary::IP::TCP::Endpoint, picolibrary::Void>.
     */
    auto remote_endpoint() const noexcept -> Result<Endpoint, Error_Code>;

    /**
     * \brief Get the connection's local endpoint.
     *
     * \return The connection's local endpoint if getting the connection's local endpoint
     *         succeeded.
     * \return An error code if getting the connection's local endpoint failed. If getting
     *         the connection's local endpoint cannot fail, return
     *         picolibrary::Result<picolibrary::IP::TCP::Endpoint, picolibrary::Void>.
     */
    auto local_endpoint() const noexcept -> Result<Endpoint, Error_Code>;

    /**
     * \brief Get the amount of data that has yet to be transmitted to the remote
     *        endpoint.
     *
     * \return The amount of data that has yet to be transmitted to the remote endpoint if
     *         getting the amount of data that has yet to be transmitted to the remote
     *         endpoint succeeded.
     * \return An error code if getting the amount of data that has yet to be transmitted
     *         to the remote endpoint failed. If getting the amount of data that has yet
     *         to be transmitted to the remote endpoint cannot fail, return
     *         picolibrary::Result<Size, picolibrary::Void>.
     */
    auto outstanding() const noexcept -> Result<Size, Error_Code>;

    /**
     * \brief Transmit data to the remote endpoint.
     *
     * \param[in] begin The beginning of the block of data to write to the socket's
     *            transmit buffer.
     * \param[in] end The end of the block of data to write to the socket's transmit
     *            buffer.
     *
     * \return The end of the data that was written to the socket's transmit buffer if
     *         writing data to the socket's transmit buffer succeeded.
     * \return picolibrary::Generic_Error::NOT_CONNECTED if the socket is not connected to
     *         a remote endpoint.
     * \return picolibrary::Generic_Error::WOULD_BLOCK or
     *         picolibrary::Generic_Error::OPERATION_TIMEOUT if the socket is in a
     *         non-blocking mode, and no data could be written to the socket's transmit
     *         buffer without blocking.
     * \return picolibrary::Generic_Error::OPERATION_TIMEOUT if a timeout occurred before
     *         any data could be written to the socket's transmit buffer.
     * \return An error code if writing data to the socket's transmit buffer failed for
     *         any other reason.
     */
    auto transmit( std::uint8_t const * begin, std::uint8_t const * end ) noexcept
        -> Result<std::uint8_t const *, Error_Code>;

    /**
     * \brief Get the amount of data that is immediately available to be received from the
     *        remote endpoint.
     *
     * \return The amount of data that is immediately available to be received from the
     *         remote endpoint if getting the amount of data that is immediately available
     *         to be received from the remote endpoint succeeded.
     * \return An error code if getting the amount of data that is immediately available
     *         to be received from the remote endpoint failed. If getting the amount of
     *         data that is immediately available to be received from the remote endpoint
     *         cannot fail, return picolibrary::Result<Size, picolibrary::Void>.
     */
    auto available() const noexcept -> Result<Size, Error_Code>;

    /**
     * \brief Receive data from the remote endpoint.
     *
     * \param[out] begin The beginning of the block of data read from the socket's receive
     *             buffer.
     * \param[out] end The end of the block of data read from the socket's receive buffer.
     *
     * \return The end of the data that was read from the socket's receive buffer if
     *         reading data from the socket's receive buffer succeeded.
     * \return picolibrary::Generic_Error::NOT_CONNECTED if the socket is not connected to
     *         a remote endpoint.
     * \return picolibrary::Generic_Error::WOULD_BLOCK or
     *         picolibrary::Generic_Error::OPERATION_TIMEOUT if the socket is in a
     *         non-blocking mode, and no data could be read from the socket's receive
     *         buffer without blocking.
     * \return picolibrary::Generic_Error::OPERATION_TIMEOUT if a timeout occurred before
     *         any data could be read from the socket's receive buffer.
     * \return An error code if reading data from the socket's receive buffer failed for
     *         any other reason.
     */
    auto receive( std::uint8_t * begin, std::uint8_t * end ) noexcept
        -> Result<std::uint8_t *, Error_Code>;

    /**
     * \brief Disable further data transmission and reception.
     *
     * \return Nothing if disabling further data transmission and reception succeeded.
     * \return picolibrary::Generic_Error::CONNECTION_LOST if the socket is not connected
     *         to a remote endpoint.
     * \return An error code if disabling further data transmission and reception failed
     *         for any other reason.
     */
    auto shutdown() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Close the socket.
     *
     * \return Nothing if closing the socket succeeded.
     * \return An error code if closing the socket failed. If closing the socket cannot
     *         fail, return picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto close() noexcept -> Result<Void, Error_Code>;
};

/**
 * \brief Server socket concept.
 */
class Server_Concept {
  public:
    /**
     * \brief The unsigned integral type used to report transmit/receive buffer
     *        information.
     */
    using Size = std::size_t;

    /**
     * \brief Constructor.
     */
    Server_Concept() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Server_Concept( Server_Concept && source ) noexcept = default;

    Server_Concept( Server_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Server_Concept() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Server_Concept && expression ) noexcept -> Server_Concept & = default;

    auto operator=( Server_Concept const & ) = delete;

    /**
     * \brief Check if the socket is connected to a remote endpoint.
     *
     * \return true if getting the socket's connection state succeeded and the socket is
     *         connected to a remote endpoint.
     * \return false if getting the socket's connection state succeeded and the socket is
     *         not connected to a remote endpoint.
     * \return An error code if getting the socket's connection state failed. If getting
     *         the socket's connection state cannot fail, return picolibrary::Result<bool,
     *         picolibrary::Void>.
     */
    auto is_connected() const noexcept -> Result<bool, Error_Code>;

    /**
     * \brief Get the connection's remote endpoint.
     *
     * \return The connection's remote endpoint if getting the connection's remote
     *         endpoint succeeded.
     * \return An error code if getting the connection's remote endpoint failed. If
     *         getting the connection's remote endpoint cannot fail, return
     *         picolibrary::Result<picolibrary::IP::TCP::Endpoint, picolibrary::Void>.
     */
    auto remote_endpoint() const noexcept -> Result<Endpoint, Error_Code>;

    /**
     * \brief Get the connection's local endpoint.
     *
     * \return The connection's local endpoint if getting the connection's local endpoint
     *         succeeded.
     * \return An error code if getting the connection's local endpoint failed. If getting
     *         the connection's local endpoint cannot fail, return
     *         picolibrary::Result<picolibrary::IP::TCP::Endpoint, picolibrary::Void>.
     */
    auto local_endpoint() const noexcept -> Result<Endpoint, Error_Code>;

    /**
     * \brief Get the amount of data that has yet to be transmitted to the remote
     *        endpoint.
     *
     * \return The amount of data that has yet to be transmitted to the remote endpoint if
     *         getting the amount of data that has yet to be transmitted to the remote
     *         endpoint succeeded.
     * \return An error code if getting the amount of data that has yet to be transmitted
     *         to the remote endpoint failed. If getting the amount of data that has yet
     *         to be transmitted to the remote endpoint cannot fail, return
     *         picolibrary::Result<Size, picolibrary::Void>.
     */
    auto outstanding() const noexcept -> Result<Size, Error_Code>;

    /**
     * \brief Transmit data to the remote endpoint.
     *
     * \param[in] begin The beginning of the block of data to write to the socket's
     *            transmit buffer.
     * \param[in] end The end of the block of data to write to the socket's transmit
     *            buffer.
     *
     * \return The end of the data that was written to the socket's transmit buffer if
     *         writing data to the socket's transmit buffer succeeded.
     * \return picolibrary::Generic_Error::NOT_CONNECTED if the socket is not connected to
     *         a remote endpoint.
     * \return picolibrary::Generic_Error::WOULD_BLOCK or
     *         picolibrary::Generic_Error::OPERATION_TIMEOUT if the socket is in a
     *         non-blocking mode, and no data could be written to the socket's transmit
     *         buffer without blocking.
     * \return picolibrary::Generic_Error::OPERATION_TIMEOUT if a timeout occurred before
     *         any data could be written to the socket's transmit buffer.
     * \return An error code if writing data to the socket's transmit buffer failed for
     *         any other reason.
     */
    auto transmit( std::uint8_t const * begin, std::uint8_t const * end ) noexcept
        -> Result<std::uint8_t const *, Error_Code>;

    /**
     * \brief Get the amount of data that is immediately available to be received from the
     *        remote endpoint.
     *
     * \return The amount of data that is immediately available to be received from the
     *         remote endpoint if getting the amount of data that is immediately available
     *         to be received from the remote endpoint succeeded.
     * \return An error code if getting the amount of data that is immediately available
     *         to be received from the remote endpoint failed. If getting the amount of
     *         data that is immediately available to be received from the remote endpoint
     *         cannot fail, return picolibrary::Result<Size, picolibrary::Void>.
     */
    auto available() const noexcept -> Result<Size, Error_Code>;

    /**
     * \brief Receive data from the remote endpoint.
     *
     * \param[out] begin The beginning of the block of data read from the socket's receive
     *             buffer.
     * \param[out] end The end of the block of data read from the socket's receive buffer.
     *
     * \return The end of the data that was read from the socket's receive buffer if
     *         reading data from the socket's receive buffer succeeded.
     * \return picolibrary::Generic_Error::NOT_CONNECTED if the socket is not connected to
     *         a remote endpoint.
     * \return picolibrary::Generic_Error::WOULD_BLOCK or
     *         picolibrary::Generic_Error::OPERATION_TIMEOUT if the socket is in a
     *         non-blocking mode, and no data could be read from the socket's receive
     *         buffer without blocking.
     * \return picolibrary::Generic_Error::OPERATION_TIMEOUT if a timeout occurred before
     *         any data could be read from the socket's receive buffer.
     * \return An error code if reading data from the socket's receive buffer failed for
     *         any other reason.
     */
    auto receive( std::uint8_t * begin, std::uint8_t * end ) noexcept
        -> Result<std::uint8_t *, Error_Code>;

    /**
     * \brief Disable further data transmission and reception.
     *
     * \return Nothing if disabling further data transmission and reception succeeded.
     * \return picolibrary::Generic_Error::CONNECTION_LOST if the socket is not connected
     *         to a remote endpoint.
     * \return An error code if disabling further data transmission and reception failed
     *         for any other reason.
     */
    auto shutdown() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Close the socket.
     *
     * \return Nothing if closing the socket succeeded.
     * \return An error code if closing the socket failed. If closing the socket cannot
     *         fail, return picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto close() noexcept -> Result<Void, Error_Code>;
};

/**
 * \brief Acceptor socket concept.
 */
class Acceptor_Concept {
  public:
    /**
     * \brief The type of server socket produced by the acceptor socket.
     */
    using Server = Server_Concept;

    /**
     * \brief Constructor.
     */
    Acceptor_Concept() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Acceptor_Concept( Acceptor_Concept && source ) noexcept = default;

    Acceptor_Concept( Acceptor_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Acceptor_Concept() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Acceptor_Concept && expression ) noexcept -> Acceptor_Concept & = default;

    auto operator=( Acceptor_Concept const & ) = delete;

    /**
     * \brief Bind the socket to a specific local endpoint.
     *
     * \param[in] endpoint The local endpoint to bind the socket to.
     *
     * \return Nothing if binding the socket to the local endpoint succeeded.
     * \return picolibrary::Generic_Error::INVALID_ARGUMENT if endpoint is not a valid
     *         local endpoint.
     * \return picolibrary::Generic_Error::LOGIC_ERROR if the socket has already been
     *         bound to a local endpoint.
     * \return picolibrary::Generic_Error::ENDPOINT_IN_USE if endpoint is already in use.
     * \return picolibrary::Generic_Error::EPHEMERAL_PORTS_EXHAUSTED if an ephemeral port
     *         was requested and no ephemeral ports are available.
     * \return An error code if binding the socket to the local endpoint failed for any
     *         other reason.
     */
    auto bind( Endpoint const & endpoint = Endpoint{} ) noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Listen for incoming connection requests.
     *
     * \param[in] backlog The maximum number of simultaneously connected clients.
     *
     * \attention Implementations may interpret the backlog argument differently.
     *
     * \return Nothing if listening for incoming connection requests succeeded.
     * \return picolibrary::Generic_Error::LOGIC_ERROR if the socket is not in a state
     *         that allows it to listen for incoming connection requests.
     * \return picolibrary::Generic_Error::LOGIC_ERROR if the socket is already listening
     *         for incoming connection requests.
     * \return picolibrary::Generic_Error::LOGIC_ERROR if the socket does not support
     *         listening for incoming connection requests without having first bound the
     *         socket to a local endpoint, and the socket has not been bound to a local
     *         endpoint.
     * \return picolibrary::Generic_Error::EPHEMERAL_PORTS_EXAUSTED if the socket does
     *         support listening for incoming connection requests without having first
     *         bound the socket to a local endpoint and the socket has not been bound to a
     *         local endpoint, but no ephemeral ports are available.
     * \return picolibrary::Generic_Error::INSUFFICIENT_SOCKETS_AVAILABLE if insufficient
     *         sockets are available to support the requested backlog.
     * \return An error code if listening for incoming connection requests failed for any
     *         other reason.
     */
    auto listen( std::uint_fast8_t backlog ) noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Check if the socket is listening for incoming connection requests.
     *
     * \return true if getting the socket's listening state succeeded and the socket is
     *         listening for incoming connection requests.
     * \return false if getting the socket's listening state succeeded and the socket is
     *         not listening for incoming connection requests.
     * \return An error code if getting the socket's listening state failed. If getting
     *         the socket's listening state cannot fail, return picolibrary::Result<bool,
     *         picolibrary::Void>.
     */
    auto is_listening() const noexcept -> Result<bool, Error_Code>;

    /**
     * \brief Get the endpoint on which the socket is listening for incoming connection
     *        requests.
     *
     * \return The endpoint on which the socket is listening for incoming connection
     *         requests if getting the endpoint on which the socket is listening for
     *         incoming connection requests succeeded.
     * \return An error code if getting the endpoint on which the socket is listening for
     *         incoming connection requests failed. If getting the endpoint on which the
     *         socket is listening for incoming connection requests cannot fail, return
     *         picolibrary::Result<picolibrary::IP::TCP::Endpoint, picolibrary::Void>.
     */
    auto local_endpoint() const noexcept -> Result<Endpoint, Error_Code>;

    /**
     * \brief Accept an incoming connection request.
     *
     * \return A server socket for handling the connection if accepting an incoming
     *         connection request succeeded.
     * \return picolibrary::Generic_Error::LOGIC_ERROR if the socket is not listening for
     *         incoming connections.
     * \return picolibrary::Generic_Error::WOULD_BLOCK or
     *         picolibrary::Generic_Error::OPERATION_TIMEOUT if the socket is in a
     *         non-blocking mode, and an incoming connection request could not be accepted
     *         without blocking.
     * \return picolibrary::Generic_Error::OPERATION_TIMEOUT if a timeout occurred before
     *         an incoming connection request could be accepted.
     * \return picolibrary::Generic_Error::NO_SOCKETS_AVAILABLE if a server socket for
     *         handling the connection could not be allocated.
     * \return An error code if accepting an incoming connection request failed for any
     *         other reason.
     */
    auto accept() noexcept -> Result<Server, Error_Code>;

    /**
     * \brief Close the socket.
     *
     * \return Nothing if closing the socket succeeded.
     * \return An error code if closing the socket failed. If closing the socket cannot
     *         fail, return picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto close() noexcept -> Result<Void, Error_Code>;
};

} // namespace picolibrary::IP::TCP

#endif // PICOLIBRARY_IP_TCP_H

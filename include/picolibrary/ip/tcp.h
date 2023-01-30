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
 * \brief picolibrary::IP::TCP interface.
 */

#ifndef PICOLIBRARY_IP_TCP_H
#define PICOLIBRARY_IP_TCP_H

#include <cstddef>
#include <cstdint>

#include "picolibrary/ip.h"
#include "picolibrary/result.h"

/**
 * \brief Transmission Control Protocol (TCP) over IP facilities.
 */
namespace picolibrary::IP::TCP {

/**
 * \brief Port.
 */
using Port = ::picolibrary::IP::Port;

/**
 * \brief Endpoint.
 */
using Endpoint = ::picolibrary::IP::Endpoint;

/**
 * \brief Client socket concept.
 */
class Client_Concept {
  public:
    /**
     * \brief The unsigned integer type used to report transmit/receive buffer
     *        information.
     */
    using Size = std::size_t;

    /**
     * \brief Constructor.
     */
    Client_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Client_Concept( Client_Concept && source ) noexcept;

    Client_Concept( Client_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Client_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Client_Concept && expression ) noexcept -> Client_Concept &;

    auto operator=( Client_Concept const & ) = delete;

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
     * \brief Connect to a remote endpoint.
     *
     * \pre the socket is in a state that allows it to connect to a remote endpoint
     * \pre the socket is not already connected to a remote endpoint
     *
     * \param[in] endpoint The remote endpoint to connect to.
     *
     * \pre endpoint is a valid remote endpoint
     *
     * \return Nothing if connecting to the remote endpoint succeeded.
     * \return picolibrary::Generic_Error::WOULD_BLOCK if the socket is in a non-blocking
     *         mode, and connecting to the remote endpoint cannot succeed immediately.
     * \return picolibrary::Generic_Error::OPERATION_TIMEOUT if connecting to the remote
     *         endpoint timed out.
     */
    auto connect( Endpoint const & endpoint ) noexcept -> Result<void>;

    /**
     * \brief Check if the socket is connected to a remote endpoint.
     *
     * \return true if the socket is connected to a remote endpoint.
     * \return false if the socket is not connected to a remote endpoint.
     */
    auto is_connected() const noexcept -> bool;

    /**
     * \brief Get the connection's remote endpoint.
     *
     * \return The connection's remote endpoint.
     */
    auto remote_endpoint() const noexcept -> Endpoint;

    /**
     * \brief Get the connection's local endpoint.
     *
     * \return The connection's local endpoint.
     */
    auto local_endpoint() const noexcept -> Endpoint;

    /**
     * \brief Get the amount of data that has yet to be transmitted to the remote
     *        endpoint.
     *
     * \return The amount of data that has yet to be transmitted to the remote endpoint.
     */
    auto outstanding() const noexcept -> Size;

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
     */
    auto transmit( std::uint8_t const * begin, std::uint8_t const * end ) noexcept
        -> Result<std::uint8_t const *>;

    /**
     * \brief Get the amount of data that is immediately available to be received from the
     *        remote endpoint.
     *
     * \return The amount of data that is immediately available to be received from the
     *         remote endpoint.
     */
    auto available() const noexcept -> Size;

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
     */
    auto receive( std::uint8_t * begin, std::uint8_t * end ) noexcept -> Result<std::uint8_t *>;

    /**
     * \brief Disable further data transmission and reception.
     */
    void shutdown() noexcept;

    /**
     * \brief Close the socket.
     *
     * The following sequence of operations can be used to gracefully shutdown a socket
     * that has finished sending and receiving data, or a socket that has reported that it
     * is no longer connected to a remote endpoint before the socket is closed.
     * \code
     * socket.shutdown();
     *
     * for ( ;; ) {
     *     auto result = socket.receive( buffer.begin(), buffer.end() );
     *     if ( result.is_error() ) {
     *         if ( result.error() == picolibrary::Generic_Error::NOT_CONNECTED ) {
     *             // shutdown complete
     *             break;
     *         } else if ( result.error() == picolibrary::Generic_Error::WOULD_BLOCK ) {
     *             // shutdown not complete
     *         } else if ( result.error() == picolibrary::Generic_Error::OPERATION_TIMEOUT ) {
     *             // shutdown not complete
     *         } else {
     *             // handle implementation specific or unexpected error
     *         } // else
     *     } // if
     * } // for
     *
     * socket.close();
     * \endcode
     */
    void close() noexcept;
};

/**
 * \brief Server socket concept.
 */
class Server_Concept {
  public:
    /**
     * \brief The unsigned integer type used to report transmit/receive buffer
     *        information.
     */
    using Size = std::size_t;

    /**
     * \brief Constructor.
     */
    Server_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Server_Concept( Server_Concept && source ) noexcept;

    Server_Concept( Server_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Server_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Server_Concept && expression ) noexcept -> Server_Concept &;

    auto operator=( Server_Concept const & ) = delete;

    /**
     * \brief Check if the socket is connected to a remote endpoint.
     *
     * \return true if the socket is connected to a remote endpoint.
     * \return false if the socket is not connected to a remote endpoint.
     */
    auto is_connected() const noexcept -> bool;

    /**
     * \brief Get the connection's remote endpoint.
     *
     * \return The connection's remote endpoint.
     */
    auto remote_endpoint() const noexcept -> Endpoint;

    /**
     * \brief Get the connection's local endpoint.
     *
     * \return The connection's local endpoint.
     */
    auto local_endpoint() const noexcept -> Endpoint;

    /**
     * \brief Get the amount of data that has yet to be transmitted to the remote
     *        endpoint.
     *
     * \return The amount of data that has yet to be transmitted to the remote endpoint.
     */
    auto outstanding() const noexcept -> Size;

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
     */
    auto transmit( std::uint8_t const * begin, std::uint8_t const * end ) noexcept
        -> Result<std::uint8_t const *>;

    /**
     * \brief Get the amount of data that is immediately available to be received from the
     *        remote endpoint.
     *
     * \return The amount of data that is immediately available to be received from the
     *         remote endpoint.
     */
    auto available() const noexcept -> Size;

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
     */
    auto receive( std::uint8_t * begin, std::uint8_t * end ) noexcept -> Result<std::uint8_t *>;

    /**
     * \brief Disable further data transmission and reception.
     */
    void shutdown() noexcept;

    /**
     * \brief Close the socket.
     *
     * The following sequence of operations can be used to gracefully shutdown a socket
     * that has finished sending and receiving data, or a socket that has reported that it
     * is no longer connected to a remote endpoint before the socket is closed.
     * \code
     * socket.shutdown();
     *
     * for ( ;; ) {
     *     auto result = socket.receive( buffer.begin(), buffer.end() );
     *     if ( result.is_error() ) {
     *         if ( result.error() == picolibrary::Generic_Error::NOT_CONNECTED ) {
     *             // shutdown complete
     *             break;
     *         } else if ( result.error() == picolibrary::Generic_Error::WOULD_BLOCK ) {
     *             // shutdown not complete
     *         } else if ( result.error() == picolibrary::Generic_Error::OPERATION_TIMEOUT ) {
     *             // shutdown not complete
     *         } else {
     *             // handle implementation specific or unexpected error
     *         } // else
     *     } // if
     * } // for
     *
     * socket.close();
     * \endcode
     */
    void close() noexcept;
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
    Acceptor_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Acceptor_Concept( Acceptor_Concept && source ) noexcept;

    Acceptor_Concept( Acceptor_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Acceptor_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Acceptor_Concept && expression ) noexcept -> Acceptor_Concept &;

    auto operator=( Acceptor_Concept const & ) = delete;

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
     * \brief Listen for incoming connection requests.
     *
     * \pre the socket is in a state that allows it to listen for incoming connection
     *      requests
     * \pre the socket is not already listening for incoming connection requests
     *
     * \param[in] backlog The maximum number of simultaneously connected clients.
     *
     * \attention Implementations may interpret the backlog argument differently.
     *
     * \pre sufficient resources are available to support the requested backlog
     */
    void listen( std::uint_fast8_t backlog ) noexcept;

    /**
     * \brief Check if the socket is listening for incoming connection requests.
     *
     * \return true if the socket is listening for incoming connection requests.
     * \return false if the socket is not listening for incoming connection requests.
     */
    auto is_listening() const noexcept -> bool;

    /**
     * \brief Get the local endpoint on which the socket is listening for incoming
     *        connection requests.
     *
     * \return The local endpoint on which the socket is listening for incoming connection
     *         requests.
     */
    auto local_endpoint() const noexcept -> Endpoint;

    /**
     * \brief Accept an incoming connection request.
     *
     * \pre the socket is listening for incoming connection requests
     *
     * \return A server socket for handling the connection if accepting an incoming
     *         connection request succeeded.
     * \return picolibrary::Generic_Error::WOULD_BLOCK or
     *         picolibrary::Generic_Error::OPERATION_TIMEOUT if the socket is in a
     *         non-blocking mode, and an incoming connection request could not be accepted
     *         without blocking.
     * \return picolibrary::Generic_Error::OPERATION_TIMEOUT if a timeout occurred before
     *         an incoming connection request could be accepted.
     */
    auto accept() noexcept -> Result<Server>;

    /**
     * \brief Close the socket.
     */
    void close() noexcept;
};

} // namespace picolibrary::IP::TCP

#endif // PICOLIBRARY_IP_TCP_H

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
 * \brief picolibrary::Testing::Interactive::IP::TCP interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_IP_TCP_H
#define PICOLIBRARY_TESTING_INTERACTIVE_IP_TCP_H

#include <cstdint>
#include <utility>

#include "picolibrary/array.h"
#include "picolibrary/format.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/precondition.h"
#include "picolibrary/result.h"
#include "picolibrary/rom.h"
#include "picolibrary/stream.h"

/**
 * \brief TCP over IP interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::IP::TCP {

/**
 * \brief Socket data reception interactive testing helper.
 *
 * \tparam Socket The type of socket used to receive data.
 *
 * \param[in] socket The socket to use to receive data.
 * \param[out] begin The beginning of the block of data read from the socket's receive
 *             buffer.
 * \param[out] end The end of the block of data read from the socket's receive buffer.
 *
 * \return The end of the data that was read from the socket's receive buffer if reading
 *         data from the socket's receive buffer succeeded.
 * \return picolibrary::Generic_Error::NOT_CONNECTED if the socket is not connected to a
 *         remote endpoint.
 */
template<typename Socket>
auto receive_some( Socket & socket, std::uint8_t * begin, std::uint8_t * end ) noexcept
    -> Result<std::uint8_t *>
{
    for ( ;; ) {
        auto result = socket.receive( begin, end );
        if ( result.is_error() ) {
            if ( result.error() == Generic_Error::NOT_CONNECTED ) {
                return Generic_Error::NOT_CONNECTED;
            } // if

            PICOLIBRARY_EXPECT(
                result.error() == Generic_Error::WOULD_BLOCK
                    or result.error() == Generic_Error::OPERATION_TIMEOUT,
                Generic_Error::LOGIC_ERROR );
        } else {
            return result.value();
        } // else
    }     // for
}

/**
 * \brief Socket data transmission interactive testing helper
 *
 * \tparam Socket The type of socket used to transmit data.
 *
 * \param[in] socket The socket to use to transmit data.
 * \param[in] begin The beginning of the block of data to write to the socket's transmit
 *            buffer.
 * \param[in] end The end of the block of data to write to the socket's transmit buffer.
 *
 * \return Nothing if writing all the data to the socket's transmit buffer succeeded.
 * \return picolibrary::Generic_Error::NOT_CONNECTED if the socket is not connected to a
 *         remote endpoint.
 */
template<typename Socket>
auto transmit_all( Socket & socket, std::uint8_t const * begin, std::uint8_t const * end ) noexcept
    -> Result<void>
{
    while ( begin != end ) {
        auto result = socket.transmit( begin, end );
        if ( result.is_error() ) {
            if ( result.error() == Generic_Error::NOT_CONNECTED ) {
                return Generic_Error::NOT_CONNECTED;
            } // if

            PICOLIBRARY_EXPECT(
                result.error() == Generic_Error::WOULD_BLOCK
                    or result.error() == Generic_Error::OPERATION_TIMEOUT,
                Generic_Error::LOGIC_ERROR );
        } else {
            begin = result.value();
        } // else
    }     // while

    return {};
}

/**
 * \brief Socket graceful shutdown interactive testing helper.
 *
 * \tparam Socket The type of socket to gracefully shutdown.
 *
 * \param[in] socket The socket to gracefully shutdown.
 */
template<typename Socket>
void shutdown_gracefully( Socket & socket ) noexcept
{
    socket.shutdown();

    Array<std::uint8_t, 64> buffer;
    for ( ;; ) {
        auto result = socket.receive( buffer.begin(), buffer.end() );
        if ( result.is_error() ) {
            if ( result.error() == Generic_Error::NOT_CONNECTED ) {
                return;
            } // if

            PICOLIBRARY_EXPECT(
                result.error() == Generic_Error::WOULD_BLOCK
                    or result.error() == Generic_Error::OPERATION_TIMEOUT,
                Generic_Error::LOGIC_ERROR );
        } // if
    }     // for
}

/**
 * \brief Socket echo interactive test helper.
 *
 * \tparam Socket The type of socket to use to echo received data.
 *
 * \param[in] stream The stream to write test output to.
 * \param[in] socket The socket to use to echo received data.
 */
template<typename Socket>
// NOLINTNEXTLINE(readability-function-size)
void echo( Reliable_Output_Stream & stream, Socket socket ) noexcept
{
    // #lizard forgives the length

    Array<std::uint8_t, 64> buffer;
    auto                    end = static_cast<std::uint8_t const *>( nullptr );
    for ( ;; ) {
        {
            auto result = receive_some( socket, buffer.begin(), buffer.end() );
            if ( result.is_error() ) {
                break;
            } // if

            end = result.value();
        }

        stream.print(
            PICOLIBRARY_ROM_STRING( "echoing:\n" ), Format::Hex_Dump{ buffer.cbegin(), end } );
        stream.flush();

        {
            auto result = transmit_all( socket, buffer.cbegin(), end );
            if ( result.is_error() ) {
                break;
            } // if
        }
    } // for

    stream.put(
        PICOLIBRARY_ROM_STRING( "connection lost, attempting graceful shutdown\n" ) );
    stream.flush();

    shutdown_gracefully( socket );
}

/**
 * \brief Client socket remote endpoint connection interactive testing helper.
 *
 * \tparam Client The type of client socket to connect to a remote endpoint.
 *
 * \param[in] client The client socket to connect to the remote endpoint.
 * \param[in] endpoint The remote endpoint to connect to.
 *
 * \return Nothing if the connecting to the remote endpoint succeeded.
 * \return picolibrary::Generic_Error::OPERATION_TIMEOUT if connecting to the remote
 *         endpoint timed out.
 */
template<typename Client>
auto connect( Client & client, ::picolibrary::IP::TCP::Endpoint const & endpoint ) noexcept
    -> Result<void>
{
    for ( ;; ) {
        auto result = client.connect( endpoint );
        if ( result.is_error() ) {
            if ( result.error() == Generic_Error::OPERATION_TIMEOUT ) {
                return Generic_Error::OPERATION_TIMEOUT;
            } // if

            PICOLIBRARY_EXPECT( result.error() == Generic_Error::WOULD_BLOCK, Generic_Error::LOGIC_ERROR );
        } else {
            return {};
        } // else
    }     // for
}

/**
 * \brief Acceptor socket incoming connection request acceptance interactive testing
 *        helper.
 *
 * \tparam Acceptor The type of acceptor socket to use to accept the incoming connection
 *         request.
 *
 * \param[in] acceptor The acceptor socket to use to accept the incoming connection
 *            request.
 *
 * \return A server socket for handling the connection.
 */
template<typename Acceptor>
auto accept( Acceptor & acceptor ) noexcept -> typename Acceptor::Server
{
    for ( ;; ) {
        auto result = acceptor.accept();
        if ( result.is_error() ) {
            PICOLIBRARY_EXPECT(
                result.error() == Generic_Error::WOULD_BLOCK
                    or result.error() == Generic_Error::OPERATION_TIMEOUT,
                Generic_Error::LOGIC_ERROR );
        } else {
            return std::move( result ).value();
        } // else
    }     // for
}

/**
 * \brief Client socket echo interactive test helper.
 *
 * \tparam Network_Stack The type of network stack to use to construct client sockets.
 * \tparam Socket_Options_Configurator A unary functor that takes the client socket whose
 *         socket options are to be configured by reference, and configures the client
 *         socket's socket options.
 *
 * \param[in] stream The stream to write test output to.
 * \param[in] network_stack The network stack to use to construct client sockets.
 * \param[in] configure_socket_options The functor to use to configure client socket
 *            socket options.
 * \param[in] local_endpoint The local endpoint to bind client sockets to.
 * \param[in] remote_endpoint The remote endpoint to connect to.
 */
template<typename Network_Stack, typename Socket_Options_Configurator>
// NOLINTNEXTLINE(readability-function-size)
[[noreturn]] void echo_client(
    Reliable_Output_Stream &                 stream,
    Network_Stack &                          network_stack,
    Socket_Options_Configurator              configure_socket_options,
    ::picolibrary::IP::TCP::Endpoint const & local_endpoint,
    ::picolibrary::IP::TCP::Endpoint const & remote_endpoint ) noexcept
{
    // #lizard forgives the length

    for ( ;; ) {
        auto client = network_stack.make_tcp_client();

        configure_socket_options( client );

        client.bind( local_endpoint );

        stream.print(
            PICOLIBRARY_ROM_STRING( "attempting to connect to " ),
            remote_endpoint,
            PICOLIBRARY_ROM_STRING( " from " ),
            client.local_endpoint(),
            '\n' );
        stream.flush();
        auto result = connect( client, remote_endpoint );
        if ( result.is_error() ) {
            stream.put( PICOLIBRARY_ROM_STRING( "connection failed\n" ) );
            stream.flush();
        } else {
            stream.put( PICOLIBRARY_ROM_STRING( "connection established\n" ) );
            stream.flush();

            echo( stream, std::move( client ) );
        } // else
    }     // for
}

} // namespace picolibrary::Testing::Interactive::IP::TCP

#endif // PICOLIBRARY_TESTING_INTERACTIVE_IP_TCP_H

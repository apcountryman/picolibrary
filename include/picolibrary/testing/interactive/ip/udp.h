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
 * \brief picolibrary::Testing::Interactive::IP::UDP interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_IP_UDP_H
#define PICOLIBRARY_TESTING_INTERACTIVE_IP_UDP_H

#include <cstdint>

#include "picolibrary/array.h"
#include "picolibrary/error.h"
#include "picolibrary/format.h"
#include "picolibrary/ip/udp.h"
#include "picolibrary/precondition.h"
#include "picolibrary/rom.h"
#include "picolibrary/stream.h"

/**
 * \brief UDP over IP interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::IP::UDP {

/**
 * \brief Socket datagram reception interactive testing helper.
 *
 * \tparam Socket The type of socket used to receive datagrams.
 *
 * \param[in] socket The socket to use to receive a datagram.
 * \param[out] begin The beginning of the block of data read from the datagram.
 * \param[out] end The end of the block of data read from the datagram.
 *
 * \return The endpoint the datagram was received from and the end of the data read from
 *         the datagram.
 */
template<typename Socket>
auto receive( Socket & socket, std::uint8_t * begin, std::uint8_t * end ) noexcept
    -> ::picolibrary::IP::UDP::Reception_Result
{
    for ( ;; ) {
        auto result = socket.receive( begin, end );
        if ( result.is_error() ) {
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
 * \brief Socket datagram transmission interactive testing helper.
 *
 * \tparam Socket The type of socket used to transmit datagrams.
 *
 * \param[in] socket The socket to use to transmit the datagram.
 * \param[in] endpoint The beginning of the block of data to transmit in the datagram.
 * \param[in] end The end of the block of data to transmit in the datagram.
 */
template<typename Socket>
void transmit(
    Socket &                                 socket,
    ::picolibrary::IP::UDP::Endpoint const & endpoint,
    std::uint8_t const *                     begin,
    std::uint8_t const *                     end ) noexcept
{
    for ( ;; ) {
        auto result = socket.transmit( endpoint, begin, end );
        if ( result.is_error() ) {
            PICOLIBRARY_EXPECT(
                result.error() != Generic_Error::EXCESSIVE_MESSAGE_SIZE,
                Generic_Error::EXCESSIVE_MESSAGE_SIZE );
            PICOLIBRARY_EXPECT(
                result.error() == Generic_Error::WOULD_BLOCK
                    or result.error() == Generic_Error::OPERATION_TIMEOUT,
                Generic_Error::LOGIC_ERROR );
        } else {
            return;
        } // else
    }     // for
}

/**
 * \brief Socket echo interactive test helper.
 *
 * \tparam Network Stack The type of network stack to use to construct a socket.
 * \tparam Socket_Options_Configurator A unary functor that takes the socket whose socket
 *         options are to be configured by reference, and configures the socket's socket
 *         options.
 *
 * \param[in] stream The stream to write test output to.
 * \param[in] network_stack The network stack to use to construct a socket.
 * \param[in] configure_socket_options The functor to use to configure socket socket
 *            options.
 * \param[in] local_endpoint The local endpoint to bind the socket to.
 */
template<typename Network_Stack, typename Socket_Options_Configurator>
[[noreturn]] void echo(
    Reliable_Output_Stream &                 stream,
    Network_Stack &                          network_stack,
    Socket_Options_Configurator              configure_socket_options,
    ::picolibrary::IP::UDP::Endpoint const & local_endpoint ) noexcept
{
    // #lizard forgives the length

    auto socket = network_stack.make_udp_socket();

    configure_socket_options( socket );

    socket.bind( local_endpoint );

    stream.print( PICOLIBRARY_ROM_STRING( "echoing datagrams on " ), socket.local_endpoint(), '\n' );
    stream.flush();

    using Buffer = Array<std::uint8_t, 64>;
    Buffer buffer;
    for ( ;; ) {
        auto const [ endpoint, end ] = receive( socket, buffer.begin(), buffer.end() );

        stream.print(
            PICOLIBRARY_ROM_STRING( "echoing datagram received from " ),
            endpoint,
            PICOLIBRARY_ROM_STRING( ":\n" ),
            Format::Hex_Dump{ buffer.cbegin(), static_cast<Buffer::Const_Iterator>( end ) } );
        stream.flush();

        transmit( socket, endpoint, buffer.cbegin(), end );
    } // for
}

} // namespace picolibrary::Testing::Interactive::IP::UDP

#endif // PICOLIBRARY_TESTING_INTERACTIVE_IP_UDP_H

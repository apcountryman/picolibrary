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

#include "picolibrary/error.h"
#include "picolibrary/ip/udp.h"
#include "picolibrary/precondition.h"

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

} // namespace picolibrary::Testing::Interactive::IP::UDP

#endif // PICOLIBRARY_TESTING_INTERACTIVE_IP_UDP_H

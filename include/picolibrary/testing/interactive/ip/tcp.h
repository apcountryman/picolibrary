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
 * \brief picolibrary::Testing::Interactive::IP::TCP interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_IP_TCP_H
#define PICOLIBRARY_TESTING_INTERACTIVE_IP_TCP_H

#include <cstdint>

#include "picolibrary/array.h"
#include "picolibrary/error.h"
#include "picolibrary/precondition.h"
#include "picolibrary/result.h"
#include "picolibrary/void.h"

/**
 * \brief Transmission Control Protocol (TCP) over IP interactive testing facilities.
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
 * \pre the socket behaves as expected
 *
 * \return The end of the data that was read from the socket's receive buffer if reading
 *         data from the socket's receive buffer succeeded.
 * \return picolibrary::Generic_Error::NOT_CONNECTED if the socket is not connected to a
 *         remote endpoint.
 */
template<typename Socket>
auto receive( Socket & socket, std::uint8_t * begin, std::uint8_t * end ) noexcept
    -> Result<std::uint8_t *, Error_Code>
{
    for ( ;; ) {
        auto result = socket.receive( begin, end );

        if ( result.is_value() ) {
            return result.value();
        } // if

        if ( result.error() == Generic_Error::NOT_CONNECTED ) {
            return Generic_Error::NOT_CONNECTED;
        } // if

        expect(
            result.error() == Generic_Error::WOULD_BLOCK or result.error() == Generic_Error::OPERATION_TIMEOUT,
            Generic_Error::LOGIC_ERROR );
    } // for
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
 * \pre the socket behaves as expected
 *
 * \return Nothing if writing all the data to the socket's transmit buffer succeeded.
 * \return picolibrary::Generic_Error::NOT_CONNECTED if the socket is not connected to a
 *         remote endpoint.
 */
template<typename Socket>
auto transmit( Socket & socket, std::uint8_t const * begin, std::uint8_t const * end ) noexcept
    -> Result<Void, Error_Code>
{
    while ( begin != end ) {
        auto result = socket.transmit( begin, end );

        if ( result.is_value() ) {
            begin = result.value();
        } else {
            if ( result.error() == Generic_Error::NOT_CONNECTED ) {
                return Generic_Error::NOT_CONNECTED;
            } // if

            expect(
                result.error() == Generic_Error::WOULD_BLOCK
                    or result.error() == Generic_Error::OPERATION_TIMEOUT,
                Generic_Error::LOGIC_ERROR );
        } // else
    }     // while
}

/**
 * \brief Socket graceful shutdown interactive testing helper.
 *
 * \tparam Socket The type of socket to gracefully shutdown.
 *
 * \param[in] socket The socket to gracefully shutdown.
 *
 * \pre the socket behaves as expected
 */
template<typename Socket>
void shutdown( Socket & socket ) noexcept
{
    socket.shutdown();

    Array<std::uint8_t, 64> buffer;

    for ( ;; ) {
        auto result = socket.receive( buffer.begin(), buffer.end() );
        if ( result.is_error() ) {
            if ( result.error() == Generic_Error::NOT_CONNECTED ) {
                return;
            } // if

            expect(
                result.error() == Generic_Error::WOULD_BLOCK
                    or result.error() == Generic_Error::OPERATION_TIMEOUT,
                Generic_Error::LOGIC_ERROR );
        } // if
    }     // for
}

} // namespace picolibrary::Testing::Interactive::IP::TCP

#endif // PICOLIBRARY_TESTING_INTERACTIVE_IP_TCP_H

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
 * \brief picolibrary::Testing::Automated::IP interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_IP_H
#define PICOLIBRARY_TESTING_AUTOMATED_IP_H

#include <ios>
#include <ostream>
#include <stdexcept>

#include "picolibrary/ip.h"
#include "picolibrary/testing/automated/ipv4.h"

namespace picolibrary::IP {

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::IP::Address to.
 * \param[in] address The picolibrary::IP::Address to write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Address const & address ) -> std::ostream &
{
    if ( address.is_any() ) {
        switch ( address.version() ) {
            case Version::UNSPECIFIED: return stream << "ANY (unspecified)";
            case Version::_4: return stream << "ANY (IPv4)";
        } // switch

        throw std::invalid_argument{
            "address.version() is not a valid ::picolibrary::IP::Version"
        };
    } // if

    switch ( address.version() ) {
        case Version::UNSPECIFIED: return stream;
        case Version::_4: return stream << address.ipv4();
    } // if

    throw std::invalid_argument{
        "address.version() is not a valid ::picolibrary::IP::Version"
    };
}

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::IP::Port to.
 * \param[in] port The picolibrary::IP::Port to write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Port port ) -> std::ostream &
{
    return stream << std::dec << port.as_unsigned_integer();
}

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::IP::Endpoint to.
 * \param[in] endpoint The picolibrary::IP::Endpoint to write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Endpoint const & endpoint ) -> std::ostream &
{
    return stream << endpoint.address() << ':' << endpoint.port();
}

} // namespace picolibrary::IP

/**
 * \brief IP automated testing facilities.
 */
namespace picolibrary::Testing::Automated::IP {
} // namespace picolibrary::Testing::Automated::IP

#endif // PICOLIBRARY_TESTING_AUTOMATED_IP_H

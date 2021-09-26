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
 * \brief picolibrary::Testing::Unit::IP interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_IP_H
#define PICOLIBRARY_TESTING_UNIT_IP_H

#include "picolibrary/ip.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/testing/unit/ipv4.h"
#include "picolibrary/testing/unit/random.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Generate a pseudo-random IP version.
 *
 * \brief A pseudo-random IP version.
 */
template<>
inline auto random<IP::Version>()
{
    return random<bool>() ? IP::Version::UNSPECIFIED : IP::Version::_4;
}

/**
 * \brief Generate a pseudo-random IP address.
 *
 * \return A pseudo-random IP address.
 */
template<>
inline auto random<IP::Address>()
{
    switch ( random<IP::Version>() ) {
        case IP::Version::_4:
            return IP::Address{ random<::picolibrary::IPv4::Address>() };
        default: return IP::Address{};
    } // switch
}

/**
 * \brief Generate a pseudo-random IP port number within the specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random IP port number in the range [min,max].
 */
template<>
inline auto random<IP::Port>( IP::Port min, IP::Port max )
{
    return IP::Port{ random<IP::Port::Unsigned_Integer>(
        min.as_unsigned_integer(), max.as_unsigned_integer() ) };
}

/**
 * \brief Generate a pseudo-random IP port number greater than or equal to a minimum port
 *        number.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random IP port number in the range [min,picolibrary::IP::Port::max()].
 */
template<>
inline auto random<IP::Port>( IP::Port min )
{
    return random<IP::Port>( min, IP::Port::max() );
}

/**
 * \brief Generate a pseudo-random IP port number.
 *
 * \return A pseudo-random IP port number in the range
 *         [picolibrary::IP::Port::min(),picolibrary::IP::Port::max()].
 */
template<>
inline auto random<::picolibrary::IP::Port>()
{
    return random<IP::Port>( IP::Port::min(), IP::Port::max() );
}

/**
 * \brief Generate a pseudo-random IP endpoint.
 *
 * \brief A pseudo-random IP endpoint.
 */
template<>
inline auto random<::picolibrary::IP::Endpoint>()
{
    return ::picolibrary::IP::Endpoint{ random<::picolibrary::IP::Address>(),
                                        random<::picolibrary::IP::Port>() };
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief Internet Protocol (IP) unit testing facilities.
 */
namespace picolibrary::Testing::Unit::IP {
} // namespace picolibrary::Testing::Unit::IP

#endif // PICOLIBRARY_TESTING_UNIT_IP_H

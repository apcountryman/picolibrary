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
 * \brief Generate a pseudo-random IP port number.
 *
 * \return A pseudo-random IP port number.
 */
template<>
inline auto random<::picolibrary::IP::Port>()
{
    return ::picolibrary::IP::Port{ random<::picolibrary::IP::Port::Unsigned_Integer>() };
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief Internet Protocol (IP) unit testing facilities.
 */
namespace picolibrary::Testing::Unit::IP {
} // namespace picolibrary::Testing::Unit::IP

#endif // PICOLIBRARY_TESTING_UNIT_IP_H

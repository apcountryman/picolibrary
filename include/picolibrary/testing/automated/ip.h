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
 * \brief picolibrary::Testing::Automated::IP interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_IP_H
#define PICOLIBRARY_TESTING_AUTOMATED_IP_H

#include "picolibrary/ip.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/testing/automated/ipv4.h"
#include "picolibrary/testing/automated/random.h"

namespace picolibrary::Testing::Automated {

/**
 * \brief Generate a pseudo-random picolibrary::IP::Version.
 *
 * \return A pseudo-randomly generated picolibrary::IP::Version.
 */
template<>
inline auto random<IP::Version>() -> IP::Version
{
    return random<bool>() ? IP::Version::UNSPECIFIED : IP::Version::_4;
}

/**
 * \brief Generate a pseudo-random picolibrary::IP::Address.
 *
 * \return A pseudo-randomly generated picolibrary::IP::Address.
 */
template<>
inline auto random<IP::Address>() -> IP::Address
{
    switch ( random<IP::Version>() ) {
        case IP::Version::_4: return random<::picolibrary::IPv4::Address>();
        default: return {};
    } // switch
}

/**
 * \brief Generate a pseudo-random picolibrary::IP::Port within the specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::IP::Port in the range [min,max].
 */
template<>
inline auto random<IP::Port>( IP::Port min, IP::Port max ) -> IP::Port
{
    return IP::Port{ random<IP::Port::Unsigned_Integer>(
        min.as_unsigned_integer(), max.as_unsigned_integer() ) };
}

/**
 * \brief Generate a pseudo-random picolibrary::IP::Port greater than or equal to a
 *        minimum picolibrary::IP::Port.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::IP::Port in the range
 *         [min,picolibrary::IP::Port::max()].
 */
template<>
inline auto random<IP::Port>( IP::Port min ) -> IP::Port
{
    return random<IP::Port>( min, IP::Port::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::IP::Port.
 *
 * \return A pseudo-random picolibrary::IP::Port in the range
 *         [picolibrary::IP::Port::min(),picolibrary::IP::Port::max()].
 */
template<>
inline auto random<IP::Port>() -> IP::Port
{
    return random<IP::Port>( IP::Port::min(), IP::Port::max() );
}

} // namespace picolibrary::Testing::Automated

/**
 * \brief Internet Protocol (IP) automated testing facilities.
 */
namespace picolibrary::Testing::Automated::IP {
} // namespace picolibrary::Testing::Automated::IP

#endif // PICOLIBRARY_TESTING_AUTOMATED_IP_H

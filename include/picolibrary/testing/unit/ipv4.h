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
 * \brief picolibrary::Testing::Unit::IPv4 interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_IPV4_H
#define PICOLIBRARY_TESTING_UNIT_IPV4_H

#include <limits>

#include "picolibrary/ipv4.h"
#include "picolibrary/testing/unit/random.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Generate a pseudo-random IPv4 address within the specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random IPv4 address in the range [min,max].
 */
template<>
inline auto random<IPv4::Address>( IPv4::Address min, IPv4::Address max )
{
    return IPv4::Address{ random<IPv4::Address::Unsigned_Integer>(
        min.as_unsigned_integer(), max.as_unsigned_integer() ) };
}

/**
 * \brief Generate a pseudo-random IPv4 address greater than or equal to a minimum
 *        address.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random IPv4 address in the range
 *         [min,picolibrary::IPv4::Address::max()].
 */
template<>
inline auto random<IPv4::Address>( IPv4::Address min )
{
    return random<IPv4::Address>( min, IPv4::Address::max() );
}

/**
 * \brief Generate a pseudo-random IPv4 address.
 *
 * \return A pseudo-random IPv4 address in the range
 *         [picolibrary::IPv4::Address::min(),picolibrary::IPv4::Address::max()].
 */
template<>
inline auto random<IPv4::Address>()
{
    return random<IPv4::Address>( IPv4::Address::min(), IPv4::Address::max() );
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief Internet Protocol version 4 (IPv4) unit testing facilities.
 */
namespace picolibrary::Testing::Unit::IPv4 {
} // namespace picolibrary::Testing::Unit::IPv4

#endif // PICOLIBRARY_TESTING_UNIT_IPV4_H

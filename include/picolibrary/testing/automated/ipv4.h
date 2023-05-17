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
 * \brief picolibrary::Testing::Automated::IPv4 interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_IPV4_H
#define PICOLIBRARY_TESTING_AUTOMATED_IPV4_H

#include <cstdint>
#include <ostream>

#include "picolibrary/ipv4.h"
#include "picolibrary/testing/automated/random.h"

namespace picolibrary::IPv4 {

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::IPv4::Address to.
 * \param[in] address The picolibrary::IPv4::Address to write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Address const & address ) -> std::ostream &
{
    return stream << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 0 ] ) << '.'
                  << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 1 ] ) << '.'
                  << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 2 ] ) << '.'
                  << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 3 ] );
}

} // namespace picolibrary::IPv4

namespace picolibrary::Testing::Automated {

/**
 * \brief Generate a pseudo-random picolibrary::IPv4::Address within the specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::IPv4::Address in the range [min,max].
 */
template<>
inline auto random<IPv4::Address>( IPv4::Address min, IPv4::Address max ) -> IPv4::Address
{
    return IPv4::Address{ random<IPv4::Address::Unsigned_Integer>(
        min.as_unsigned_integer(), max.as_unsigned_integer() ) };
}

/**
 * \brief Generate a pseudo-random picolibrary::IPv4::Address greater than or equal to a
 *        minimum picolibrary::IPv4::Address.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::IPv4::Address in the range
 *         [min,picolibrary::IPv4::Address::max()].
 */
template<>
inline auto random<IPv4::Address>( IPv4::Address min ) -> IPv4::Address
{
    return random<IPv4::Address>( min, IPv4::Address::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::IPv4::Address.
 *
 * \return A pseudo-random picolibrary::IPv4::Address in the range
 *         [picolibrary::IPv4::Address::min(),picolibrary::IPv4::Address::max()].
 */
template<>
inline auto random<IPv4::Address>() -> IPv4::Address
{
    return random<IPv4::Address>( IPv4::Address::min(), IPv4::Address::max() );
}

} // namespace picolibrary::Testing::Automated

/**
 * \brief IPv4 automated testing facilities.
 */
namespace picolibrary::Testing::Automated::IPv4 {
} // namespace picolibrary::Testing::Automated::IPv4

#endif // PICOLIBRARY_TESTING_AUTOMATED_IPV4_H

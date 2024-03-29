/**
 * picolibrary
 *
 * Copyright 2020-2024, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
#include <ios>
#include <ostream>

#include "picolibrary/ipv4.h"

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
    // clang-format off

    return stream << std::dec << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 0 ] ) << '.'
                  << std::dec << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 1 ] ) << '.'
                  << std::dec << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 2 ] ) << '.'
                  << std::dec << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 3 ] );

    // clang-format on
}

} // namespace picolibrary::IPv4

/**
 * \brief IPv4 automated testing facilities.
 */
namespace picolibrary::Testing::Automated::IPv4 {
} // namespace picolibrary::Testing::Automated::IPv4

#endif // PICOLIBRARY_TESTING_AUTOMATED_IPV4_H

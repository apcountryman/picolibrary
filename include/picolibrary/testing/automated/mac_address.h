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
 * \brief picolibrary::Testing::Automated::MAC_Address interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_MAC_ADDRESS_H
#define PICOLIBRARY_TESTING_AUTOMATED_MAC_ADDRESS_H

#include <cstdint>
#include <iomanip>
#include <ios>
#include <limits>
#include <ostream>

#include "picolibrary/mac_address.h"

namespace picolibrary {

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::MAC_Address to.
 * \param[in] address The picolibrary::MAC_Address to write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, MAC_Address const & address ) -> std::ostream &
{
    // clang-format off

    return stream << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 0 ] ) << '-'
                  << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 1 ] ) << '-'
                  << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 2 ] ) << '-'
                  << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 3 ] ) << '-'
                  << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 4 ] ) << '-'
                  << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 5 ] );

    // clang-format on
}

} // namespace picolibrary

#endif // PICOLIBRARY_TESTING_AUTOMATED_MAC_ADDRESS_H

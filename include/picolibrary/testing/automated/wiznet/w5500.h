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
 * \brief picolibrary::Testing::Automated::WIZnet::W5500 interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_H
#define PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_H

#include <cstdint>

#include "picolibrary/testing/automated/random.h"
#include "picolibrary/wiznet/w5500.h"

namespace picolibrary::Testing::Automated {

/**
 * \brief Generate a pseudo-random picolibrary::WIZnet::W5500::Socket_ID.
 *
 * \return A pseudo-randomly generated picolibrary::WIZnet::W5500::Socket_ID.
 */
template<>
inline auto random<WIZnet::W5500::Socket_ID>() -> WIZnet::W5500::Socket_ID
{
    return static_cast<WIZnet::W5500::Socket_ID>( random<std::uint_fast8_t>( 0, 7 ) << 5 );
}

/**
 * \brief Generate a pseudo-random picolibrary::WIZnet::W5500::Socket_Memory_Block.
 *
 * \return A pseudo-randomly generated picolibrary::WIZnet::W5500::Socket_Memory_Block.
 */
template<>
inline auto random<WIZnet::W5500::Socket_Memory_Block>() -> WIZnet::W5500::Socket_Memory_Block
{
    return static_cast<WIZnet::W5500::Socket_Memory_Block>(
        random<std::uint_fast8_t>( 0b01, 0b11 ) << 3 );
}

} // namespace picolibrary::Testing::Automated

/**
 * \brief WIZnet W5500 automated testing facilities.
 */
namespace picolibrary::Testing::Automated::WIZnet::W5500 {
} // namespace picolibrary::Testing::Automated::WIZnet::W5500

#endif // PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_H

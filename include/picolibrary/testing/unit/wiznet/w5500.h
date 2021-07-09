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
 * \brief picolibrary::Testing::Unit::WIZnet::W5500 interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_WIZNET_W5500_H
#define PICOLIBRARY_TESTING_UNIT_WIZNET_W5500_H

#include <cstdint>

#include "picolibrary/testing/unit/random.h"
#include "picolibrary/wiznet/w5500.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Generate a pseudo-random WIZnet W5500 socket ID.
 *
 * \return A pseudo-randomly generated WIZnet W5500 socket ID.
 */
template<>
inline auto random<WIZnet::W5500::Socket_ID>()
{
    return static_cast<WIZnet::W5500::Socket_ID>( random<std::uint8_t>(
        static_cast<std::uint8_t>( WIZnet::W5500::Socket_ID::_0 ),
        static_cast<std::uint8_t>( WIZnet::W5500::Socket_ID::_7 ) ) );
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief WIZnet W5500 unit testing facilities.
 */
namespace picolibrary::Testing::Unit::WIZnet::W5500 {
} // namespace picolibrary::Testing::Unit::WIZnet::W5500

#endif // PICOLIBRARY_TESTING_UNIT_WIZNET_W5500_H

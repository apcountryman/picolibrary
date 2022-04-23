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
 * \brief picolibrary::Testing::Unit::Microchip::MCP3008 interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_MICROCHIP_MCP3008_H
#define PICOLIBRARY_TESTING_UNIT_MICROCHIP_MCP3008_H

#include <cstdint>

#include "picolibrary/microchip/mcp3008.h"
#include "picolibrary/testing/unit/random.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Generate a pseudo-random picolibrary::Microchip::MCP3008::Input.
 *
 * \return A pseudo-randomly generated picolibrary::Microchip::MCP3008::Input.
 */
template<>
inline auto random<Microchip::MCP3008::Input>() -> Microchip::MCP3008::Input
{
    return static_cast<Microchip::MCP3008::Input>( random<std::uint_fast8_t>( 0b0'000, 0b1'111 ) << 4 );
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief Microchip MCP3008 unit testing facilities.
 */
namespace picolibrary::Testing::Unit::Microchip::MCP3008 {
} // namespace picolibrary::Testing::Unit::Microchip::MCP3008

#endif // PICOLIBRARY_TESTING_UNIT_MICROCHIP_MCP3008_H

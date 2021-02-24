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
 * \brief picolibrary::Microchip::MCP23008 interface.
 */

#ifndef PICOLIBRARY_MICROCHIP_MCP23008_H
#define PICOLIBRARY_MICROCHIP_MCP23008_H

#include <cstdint>

/**
 * \brief Microchip MCP23008 facilities.
 */
namespace picolibrary::Microchip::MCP23008 {

/**
 * \brief I/O Direction (IODIR) register.
 */
struct IODIR {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x00 };

    /**
     * \brief POR value.
     */
    static constexpr auto POR = std::uint8_t{ 0xFF };
};

/**
 * \brief Input Polarity (IPOL) register.
 */
struct IPOL {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x01 };

    /**
     * \brief POR value.
     */
    static constexpr auto POR = std::uint8_t{ 0x00 };
};

} // namespace picolibrary::Microchip::MCP23008

#endif // PICOLIBRARY_MICROCHIP_MCP23008_H

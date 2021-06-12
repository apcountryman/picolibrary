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
 * \brief picolibrary::WIZnet::W5500 interface.
 */

#ifndef PICOLIBRARY_WIZNET_W5500_H
#define PICOLIBRARY_WIZNET_W5500_H

#include <cstdint>

#include "picolibrary/bit_manipulation.h"

/**
 * \brief WIZnet W5500 facilities.
 */
namespace picolibrary::WIZnet::W5500 {

/**
 * \brief Control byte definitions.
 *
 * The control byte has the following fields:
 * - SPI Operation Mode Bits (OM)
 * - Read/Write Access Mode Bit (RWB)
 * - Block Select Bits (BSB)
 * - Region (REGION)
 * - Socket (SOCKET)
 */
struct Control_Byte {
    /**
     * \brief Field sizes.
     */
    struct Size {
        static constexpr auto OM  = std::uint_fast8_t{ 2 }; ///< OM.
        static constexpr auto RWB = std::uint_fast8_t{ 1 }; ///< RWB.
        static constexpr auto BSB = std::uint_fast8_t{ 5 }; ///< BSB.

        static constexpr auto REGION = std::uint_fast8_t{ 2 }; ///< REGION.
        static constexpr auto SOCKET = std::uint_fast8_t{ 3 }; ///< SOCKET.
    };

    /**
     * \brief Field bit positions.
     */
    struct Bit {
        static constexpr auto OM  = std::uint_fast8_t{};                  ///< OM.
        static constexpr auto RWB = std::uint_fast8_t{ OM + Size::OM };   ///< RWB.
        static constexpr auto BSB = std::uint_fast8_t{ RWB + Size::RWB }; ///< BSB.

        static constexpr auto REGION = std::uint_fast8_t{ RWB + Size::RWB }; ///< REGION.
        static constexpr auto SOCKET = std::uint_fast8_t{ REGION + Size::REGION }; ///< SOCKET.
    };

    /**
     * \brief Field bit masks.
     */
    struct Mask {
        static constexpr auto OM  = mask<std::uint8_t>( Size::OM, Bit::OM );   ///< OM.
        static constexpr auto RWB = mask<std::uint8_t>( Size::RWB, Bit::RWB ); ///< RWB.
        static constexpr auto BSB = mask<std::uint8_t>( Size::BSB, Bit::BSB ); ///< BSB.

        static constexpr auto REGION = mask<std::uint8_t>( Size::REGION, Bit::REGION ); ///< REGION.
        static constexpr auto SOCKET = mask<std::uint8_t>( Size::SOCKET, Bit::SOCKET ); ///< SOCKET.
    };
};

/**
 * \brief Socket ID.
 */
enum class Socket_ID : std::uint8_t {
    _0 = 0 << Control_Byte::Bit::SOCKET, ///< 0.
    _1 = 1 << Control_Byte::Bit::SOCKET, ///< 1.
    _2 = 2 << Control_Byte::Bit::SOCKET, ///< 2.
    _3 = 3 << Control_Byte::Bit::SOCKET, ///< 3.
    _4 = 4 << Control_Byte::Bit::SOCKET, ///< 4.
    _5 = 5 << Control_Byte::Bit::SOCKET, ///< 5.
    _6 = 6 << Control_Byte::Bit::SOCKET, ///< 6.
    _7 = 7 << Control_Byte::Bit::SOCKET, ///< 7.
};

/**
 * \brief Region.
 */
enum class Region : std::uint8_t {
    REGISTERS = 0b01 << Control_Byte::Bit::REGION, ///< Registers.
    TX_BUFFER = 0b10 << Control_Byte::Bit::REGION, ///< TX buffer.
    RX_BUFFER = 0b11 << Control_Byte::Bit::REGION, ///< RX buffer.
};

} // namespace picolibrary::WIZnet::W5500

#endif // PICOLIBRARY_WIZNET_W5500_H

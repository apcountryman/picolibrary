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
 * \brief Memory offset.
 */
using Offset = std::uint16_t;

/**
 * \brief Control byte.
 *
 * The control byte has the following fields:
 * - SPI Operation Mode Bits (OM)
 * - Read/Write Access Mode Bit (RWB)
 * - Block Select Bits (BSB)
 *
 * The BSB field can be split into the following fields when accessing socket memory:
 * - Socket Memory Block (BLOCK)
 * - Socket ID (SOCKET)
 */
struct Control_Byte {
    /**
     * \brief Field sizes.
     */
    struct Size {
        static constexpr auto OM  = std::uint_fast8_t{ 2 }; ///< OM.
        static constexpr auto RWB = std::uint_fast8_t{ 1 }; ///< RWB.
        static constexpr auto BSB = std::uint_fast8_t{ 5 }; ///< BSB.

        static constexpr auto BLOCK  = std::uint_fast8_t{ 2 }; ///< BLOCK.
        static constexpr auto SOCKET = std::uint_fast8_t{ 3 }; ///< SOCKET.
    };

    /**
     * \brief Field bit positions.
     */
    struct Bit {
        static constexpr auto OM  = std::uint_fast8_t{};                  ///< OM.
        static constexpr auto RWB = std::uint_fast8_t{ OM + Size::OM };   ///< RWB.
        static constexpr auto BSB = std::uint_fast8_t{ RWB + Size::RWB }; ///< BSB.

        static constexpr auto BLOCK = std::uint_fast8_t{ RWB + Size::RWB }; ///< BLOCK.
        static constexpr auto SOCKET = std::uint_fast8_t{ BLOCK + Size::BLOCK }; ///< SOCKET.
    };

    /**
     * \brief Field bit masks.
     */
    struct Mask {
        static constexpr auto OM  = mask<std::uint8_t>( Size::OM, Bit::OM );   ///< OM.
        static constexpr auto RWB = mask<std::uint8_t>( Size::RWB, Bit::RWB ); ///< RWB.
        static constexpr auto BSB = mask<std::uint8_t>( Size::BSB, Bit::BSB ); ///< BSB.

        static constexpr auto BLOCK = mask<std::uint8_t>( Size::BLOCK, Bit::BLOCK ); ///< BLOCK.
        static constexpr auto SOCKET = mask<std::uint8_t>( Size::SOCKET, Bit::SOCKET ); ///< SOCKET.
    };

    /**
     * \brief OM.
     */
    enum OM : std::uint8_t {
        OM_VDM = 0b00 << Bit::OM, ///< Variable Length Data Mode (VDM), N bytes data length (N >= 1).
        OM_FDM_1_BYTE = 0b01 << Bit::OM, ///< Fixed Length Data Mode (FDM), 1 byte data length.
        OM_FDM_2_BYTES = 0b10 << Bit::OM, ///< Fixed Length Data Mode (FDM), 2 bytes data length.
        OM_FDM_4_BYTES = 0b11 << Bit::OM, ///< Fixed Length Data Mode (FDM), 4 bytes data length.
    };

    /**
     * \brief RWB.
     */
    enum RWB : std::uint8_t {
        RWB_READ  = 0 << Bit::RWB, ///< Read.
        RWB_WRITE = 1 << Bit::RWB, ///< Write.
    };

    /**
     * \brief BSB.
     */
    enum BSB : std::uint8_t {
        BSB_COMMON_REGISTERS = 0b000'00 << Bit::BSB, ///< Common registers.

        BSB_SOCKET_0_REGISTERS = 0b000'01 << Bit::BSB, ///< Socket 0 registers.
        BSB_SOCKET_0_TX_BUFFER = 0b000'10 << Bit::BSB, ///< Socket 0 TX buffer.
        BSB_SOCKET_0_RX_BUFFER = 0b000'11 << Bit::BSB, ///< Socket 0 RX buffer.

        BSB_SOCKET_1_REGISTERS = 0b001'01 << Bit::BSB, ///< Socket 1 registers.
        BSB_SOCKET_1_TX_BUFFER = 0b001'10 << Bit::BSB, ///< Socket 1 TX buffer.
        BSB_SOCKET_1_RX_BUFFER = 0b001'11 << Bit::BSB, ///< Socket 1 RX buffer.

        BSB_SOCKET_2_REGISTERS = 0b010'01 << Bit::BSB, ///< Socket 2 registers.
        BSB_SOCKET_2_TX_BUFFER = 0b010'10 << Bit::BSB, ///< Socket 2 TX buffer.
        BSB_SOCKET_2_RX_BUFFER = 0b010'11 << Bit::BSB, ///< Socket 2 RX buffer.

        BSB_SOCKET_3_REGISTERS = 0b011'01 << Bit::BSB, ///< Socket 3 registers.
        BSB_SOCKET_3_TX_BUFFER = 0b011'10 << Bit::BSB, ///< Socket 3 TX buffer.
        BSB_SOCKET_3_RX_BUFFER = 0b011'11 << Bit::BSB, ///< Socket 3 RX buffer.

        BSB_SOCKET_4_REGISTERS = 0b100'01 << Bit::BSB, ///< Socket 4 registers.
        BSB_SOCKET_4_TX_BUFFER = 0b100'10 << Bit::BSB, ///< Socket 4 TX buffer.
        BSB_SOCKET_4_RX_BUFFER = 0b100'11 << Bit::BSB, ///< Socket 4 RX buffer.

        BSB_SOCKET_5_REGISTERS = 0b101'01 << Bit::BSB, ///< Socket 5 registers.
        BSB_SOCKET_5_TX_BUFFER = 0b101'10 << Bit::BSB, ///< Socket 5 TX buffer.
        BSB_SOCKET_5_RX_BUFFER = 0b101'11 << Bit::BSB, ///< Socket 5 RX buffer.

        BSB_SOCKET_6_REGISTERS = 0b110'01 << Bit::BSB, ///< Socket 6 registers.
        BSB_SOCKET_6_TX_BUFFER = 0b110'10 << Bit::BSB, ///< Socket 6 TX buffer.
        BSB_SOCKET_6_RX_BUFFER = 0b110'11 << Bit::BSB, ///< Socket 6 RX buffer.

        BSB_SOCKET_7_REGISTERS = 0b111'01 << Bit::BSB, ///< Socket 7 registers.
        BSB_SOCKET_7_TX_BUFFER = 0b111'10 << Bit::BSB, ///< Socket 7 TX buffer.
        BSB_SOCKET_7_RX_BUFFER = 0b111'11 << Bit::BSB, ///< Socket 7 RX buffer.
    };

    /**
     * \brief BLOCK.
     */
    enum BLOCK : std::uint8_t {
        BLOCK_REGISTERS = 0b01 << Bit::BLOCK, ///< Registers.
        BLOCK_TX_BUFFER = 0b10 << Bit::BLOCK, ///< TX buffer.
        BLOCK_RX_BUFFER = 0b11 << Bit::BLOCK, ///< RX buffer.
    };

    /**
     * \brief SOCKET.
     */
    enum SOCKET : std::uint8_t {
        SOCKET_0 = 0b000 << Bit::SOCKET, ///< Socket 0.
        SOCKET_1 = 0b001 << Bit::SOCKET, ///< Socket 1.
        SOCKET_2 = 0b010 << Bit::SOCKET, ///< Socket 2.
        SOCKET_3 = 0b011 << Bit::SOCKET, ///< Socket 3.
        SOCKET_4 = 0b100 << Bit::SOCKET, ///< Socket 4.
        SOCKET_5 = 0b101 << Bit::SOCKET, ///< Socket 5.
        SOCKET_6 = 0b110 << Bit::SOCKET, ///< Socket 6.
        SOCKET_7 = 0b111 << Bit::SOCKET, ///< Socket 7.
    };
};

/**
 * \brief Sockets.
 */
inline constexpr auto SOCKETS = std::uint_fast8_t{ 8 };

/**
 * \brief Socket ID.
 */
enum class Socket_ID : std::uint8_t {
    _0 = Control_Byte::SOCKET_0, ///< 0.
    _1 = Control_Byte::SOCKET_1, ///< 1.
    _2 = Control_Byte::SOCKET_2, ///< 2.
    _3 = Control_Byte::SOCKET_3, ///< 3.
    _4 = Control_Byte::SOCKET_4, ///< 4.
    _5 = Control_Byte::SOCKET_5, ///< 5.
    _6 = Control_Byte::SOCKET_6, ///< 6.
    _7 = Control_Byte::SOCKET_7, ///< 7.
};

} // namespace picolibrary::WIZnet::W5500

#endif // PICOLIBRARY_WIZNET_W5500_H

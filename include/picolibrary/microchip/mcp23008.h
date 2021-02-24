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
 * \brief Microchip MCP23008 I/O Direction Register (IODIR).
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
 * \brief Microchip MCP23008 Input Polarity Port Register (IPOL).
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

/**
 * \brief Microchip MCP23008 Interrupt-On-Change Pins Register (GPINTEN).
 */
struct GPINTEN {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x02 };

    /**
     * \brief POR value.
     */
    static constexpr auto POR = std::uint8_t{ 0x00 };
};

/**
 * \brief Microchip MCP23008 Default Value Register (DEFVAL).
 */
struct DEFVAL {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x03 };

    /**
     * \brief POR value.
     */
    static constexpr auto POR = std::uint8_t{ 0x00 };
};

/**
 * \brief Microchip MCP23008 Interrupt-On-Change Control Register (INTCON).
 */
struct INTCON {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x04 };

    /**
     * \brief POR value.
     */
    static constexpr auto POR = std::uint8_t{ 0x00 };
};

/**
 * \brief Microchip MCP23008 I/O Expander Configuration Register (IOCON).
 *
 * This register has the following fields:
 * - Interrupt Polarity Control (INTPOL)
 * - Open-Drain Control (ODR)
 * - Slew Rate Control (DISSLW)
 * - Sequential Operation Mode (SEQOP)
 *
 * The combination of the INTPOL and ODR fields is referred to as the interrupt mode
 * field.
 */
struct IOCON {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x05 };

    /**
     * \brief POR value.
     */
    static constexpr auto POR = std::uint8_t{ 0x00 };

    /**
     * \brief Field size.
     */
    struct Size {
        static constexpr auto RESERVED0 = std::uint_fast8_t{ 1 }; ///< Reserved.
        static constexpr auto INTPOL    = std::uint_fast8_t{ 1 }; ///< INTPOL.
        static constexpr auto ODR       = std::uint_fast8_t{ 1 }; ///< ODR.
        static constexpr auto RESERVED3 = std::uint_fast8_t{ 1 }; ///< Reserved.
        static constexpr auto DISSLW    = std::uint_fast8_t{ 1 }; ///< DISSLW.
        static constexpr auto SEQOP     = std::uint_fast8_t{ 1 }; ///< SEQOP.
        static constexpr auto RESERVED6 = std::uint_fast8_t{ 2 }; ///< Reserved.

        static constexpr auto INTERRUPT_MODE = INTPOL + ODR; ///< Interrupt mode.
    };

    /**
     * \brief Field bit positions.
     */
    struct Bit {
        static constexpr auto RESERVED0 = std::uint_fast8_t{}; ///< Reserved.
        static constexpr auto INTPOL = std::uint_fast8_t{ RESERVED0 + Size::RESERVED0 }; ///< INTPOL.
        static constexpr auto ODR = std::uint_fast8_t{ INTPOL + Size::INTPOL }; ///< ODR.
        static constexpr auto RESERVED3 = std::uint_fast8_t{ ODR + Size::ODR }; ///< Reserved.
        static constexpr auto DISSLW = std::uint_fast8_t{ RESERVED3 + Size::RESERVED3 }; ///< DISSLW.
        static constexpr auto SEQOP = std::uint_fast8_t{ DISSLW + Size::DISSLW }; ///< SEQOP.
        static constexpr auto RESERVED6 = std::uint_fast8_t{ SEQOP + Size::SEQOP }; ///< Reserved.

        static constexpr auto INTERRUPT_MODE = INTPOL; ///< Interrupt mode.
    };

    /**
     * \brief Field bit masks.
     */
    struct Mask {
        static constexpr auto RESERVED0 = std::uint8_t{ 0b1 << Bit::RESERVED0 }; ///< Reserved.
        static constexpr auto INTPOL = std::uint8_t{ 0b1 << Bit::INTPOL }; ///< INTPOL.
        static constexpr auto ODR    = std::uint8_t{ 0b1 << Bit::ODR };    ///< ODR.
        static constexpr auto RESERVED3 = std::uint8_t{ 0b1 << Bit::RESERVED3 }; ///< Reserved.
        static constexpr auto DISSLW = std::uint8_t{ 0b1 << Bit::DISSLW }; ///< DISSLW.
        static constexpr auto SEQOP  = std::uint8_t{ 0b1 << Bit::SEQOP };  ///< SEQOP.
        static constexpr auto RESERVED6 = std::uint8_t{ 0b11 << Bit::RESERVED6 }; ///< Reserved.

        static constexpr auto INTERRUPT_MODE = std::uint8_t{ 0b11 << Bit::INTERRUPT_MODE }; ///< Interrupt mode.
    };
};

/**
 * \brief Microchip MCP23008 GPIO Pull-Up Resistor Register (GPPU).
 */
struct GPPU {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x06 };

    /**
     * \brief POR value.
     */
    static constexpr auto POR = std::uint8_t{ 0x00 };
};

/**
 * \brief Microchip MCP23008 Interrupt Flag Register (INTF).
 */
struct INTF {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x07 };
};

/**
 * \brief Microchip MCP23008 Interrupt Captured Value For Port Register (INTCAP).
 */
struct INTCAP {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x08 };
};

/**
 * \brief Microchip MCP23008 General Purpose I/O Port Register (GPIO).
 */
struct GPIO {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x09 };

    /**
     * \brief POR value.
     */
    static constexpr auto POR = std::uint8_t{ 0x00 };
};

/**
 * \brief Microchip MCP23008 Output Latch Register (OLAT).
 */
struct OLAT {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x0A };

    /**
     * \brief POR value.
     */
    static constexpr auto POR = std::uint8_t{ 0x00 };
};

} // namespace picolibrary::Microchip::MCP23008

#endif // PICOLIBRARY_MICROCHIP_MCP23008_H

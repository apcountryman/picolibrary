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

/**
 * \brief Interrupt-On-Change Control (GPINTEN) register.
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
 * \brief Default Compare (DEFVAL) register.
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
 * \brief Interrupt Control (INTCON) register.
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
 * \brief Configuration (IOCON) register.
 *
 * This register has the following fields:
 * - Interrupt Polarity (INTPOL)
 * - Open-Drain (ODR)
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
 * \brief Pull-Up Resistor Configuration (GPPU) register.
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
 * \brief Interrupt Flag (INTF) register.
 */
struct INTF {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x07 };
};

/**
 * \brief Interrupt Capture (INTCAP) register.
 */
struct INTCAP {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x08 };
};

/**
 * \brief Port (GPIO) register.
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

} // namespace picolibrary::Microchip::MCP23008

#endif // PICOLIBRARY_MICROCHIP_MCP23008_H

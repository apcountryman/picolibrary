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
 * \brief picolibrary::Microchip::MCP23X08 interface.
 */

#ifndef PICOLIBRARY_MICROCHIP_MCP23X08_H
#define PICOLIBRARY_MICROCHIP_MCP23X08_H

#include <cstdint>

#include "picolibrary/bit_manipulation.h"

/**
 * \brief Microchip MCP23008/MCP23S08 facilities.
 */
namespace picolibrary::Microchip::MCP23X08 {

/**
 * \brief I/O Direction (IODIR) register.
 */
struct IODIR {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x00 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = std::uint8_t{ 0xFF };
};

/**
 * \brief Input Polarity PORT (IPOL) register.
 */
struct IPOL {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x01 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = std::uint8_t{ 0x00 };
};

/**
 * \brief Interrupt-On-Change Pins (GPINTEN) register.
 */
struct GPINTEN {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x02 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = std::uint8_t{ 0x00 };
};

/**
 * \brief Default Value (DEFVAL) register.
 */
struct DEFVAL {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x03 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = std::uint8_t{ 0x00 };
};

/**
 * \brief Interrupt-On-Change Control (INTCON) register.
 */
struct INTCON {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x04 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = std::uint8_t{ 0x00 };
};

/**
 * \brief I/O Expander Configuration Register (IOCON) register.
 *
 * This register has the following fields:
 * - Interrupt Polarity (INTPOL)
 * - Open-Drain (ODR)
 * - Hardware Address Enable (HAEN) (MCP23S08 only)
 * - Slew Rate Control (DISSLW)
 * - Sequential Operation Mode (SEQOP)
 */
struct IOCON {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x05 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = std::uint8_t{ 0x00 };

    /**
     * \brief Field sizes.
     */
    struct Size {
        static constexpr auto RESERVED0 = std::uint_fast8_t{ 1 }; ///< RESERVED0.
        static constexpr auto INTPOL    = std::uint_fast8_t{ 1 }; ///< INTPOL.
        static constexpr auto ODR       = std::uint_fast8_t{ 1 }; ///< ODR.
        static constexpr auto HAEN      = std::uint_fast8_t{ 1 }; ///< HAEN.
        static constexpr auto DISSLW    = std::uint_fast8_t{ 1 }; ///< DISSLW.
        static constexpr auto SEQOP     = std::uint_fast8_t{ 1 }; ///< SEQOP.
        static constexpr auto RESERVED6 = std::uint_fast8_t{ 2 }; ///< RESERVED6.
    };

    /**
     * \brief Field bit positions.
     */
    struct Bit {
        static constexpr auto RESERVED0 = std::uint_fast8_t{}; ///< RESERVED0.
        static constexpr auto INTPOL = std::uint_fast8_t{ RESERVED0 + Size::RESERVED0 }; ///< INTPOL.
        static constexpr auto ODR  = std::uint_fast8_t{ INTPOL + Size::INTPOL }; ///< ODR.
        static constexpr auto HAEN = std::uint_fast8_t{ ODR + Size::ODR };     ///< HAEN.
        static constexpr auto DISSLW = std::uint_fast8_t{ HAEN + Size::HAEN }; ///< DISSLW.
        static constexpr auto SEQOP = std::uint_fast8_t{ DISSLW + Size::DISSLW }; ///< SEQOP.
        static constexpr auto RESERVED6 = std::uint_fast8_t{ SEQOP + Size::SEQOP }; ///< RESERVED6.
    };

    /**
     * \brief Field bit masks.
     */
    struct Mask {
        static constexpr auto RESERVED0 = mask<std::uint8_t>( Size::RESERVED0, Bit::RESERVED0 ); ///< RESERVED0.
        static constexpr auto INTPOL = mask<std::uint8_t>( Size::INTPOL, Bit::INTPOL ); ///< INTPOL.
        static constexpr auto ODR = mask<std::uint8_t>( Size::ODR, Bit::ODR ); ///< ODR.
        static constexpr auto HAEN = mask<std::uint8_t>( Size::HAEN, Bit::HAEN ); ///< HAEN.
        static constexpr auto DISSLW = mask<std::uint8_t>( Size::DISSLW, Bit::DISSLW ); ///< DISSLW.
        static constexpr auto SEQOP = mask<std::uint8_t>( Size::SEQOP, Bit::SEQOP ); ///< SEQOP.
        static constexpr auto RESERVED6 = mask<std::uint8_t>( Size::RESERVED6, Bit::RESERVED6 ); ///< RESERVED6.
    };
};

/**
 * \brief GPIO Pull-Up Resistor (GPPU) register.
 */
struct GPPU {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x06 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = std::uint8_t{ 0x00 };
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
 * \brief Interrupt Captured Value for PORT (INTCAP) register.
 */
struct INTCAP {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x08 };
};

/**
 * \brief General Purpose I/O PORT (GPIO) register.
 */
struct GPIO {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x09 };
};

/**
 * \brief Output Latch Register (OLAT) register.
 */
struct OLAT {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x0A };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = std::uint8_t{ 0x00 };
};

} // namespace picolibrary::Microchip::MCP23X08

#endif // PICOLIBRARY_MICROCHIP_MCP23X08_H

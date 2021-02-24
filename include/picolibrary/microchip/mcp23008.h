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
 * \brief I/O Direction Register (IODIR).
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
 * \brief Input Polarity Port Register (IPOL).
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
 * \brief Interrupt-On-Change Pins Register (GPINTEN).
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
 * \brief Default Value Register (DEFVAL).
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
 * \brief Interrupt-On-Change Control Register (INTCON).
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
 * \brief I/O Expander Configuration Register (IOCON).
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
 * \brief GPIO Pull-Up Resistor Register (GPPU).
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
 * \brief Interrupt Flag Register (INTF).
 */
struct INTF {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x07 };
};

/**
 * \brief Interrupt Captured Value For Port Register (INTCAP).
 */
struct INTCAP {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x08 };
};

/**
 * \brief General Purpose I/O Port Register (GPIO).
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
 * \brief Output Latch Register (OLAT).
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

/**
 * \brief Microchip MCP23008 register cache.
 */
class Register_Cache {
  public:
    /**
     * \brief Destructor.
     */
    ~Register_Cache() noexcept = default;

    /**
     * \brief Get the cached IODIR register value.
     *
     * \return The cached IODIR register value.
     */
    constexpr auto iodir() const noexcept
    {
        return m_iodir;
    }

    /**
     * \brief Get the cached IPOL register value.
     *
     * \return The cached IPOL register value.
     */
    constexpr auto ipol() const noexcept
    {
        return m_ipol;
    }

    /**
     * \brief Get the cached GPINTEN register value.
     *
     * \return The cached GPINTEN register value.
     */
    constexpr auto gpinten() const noexcept
    {
        return m_gpinten;
    }

    /**
     * \brief Get the cached DEFVAL register value.
     *
     * \return The cached DEFVAL register value.
     */
    constexpr auto defval() const noexcept
    {
        return m_defval;
    }

    /**
     * \brief Get the cached INTCON register value.
     *
     * \return The cached INTCON register value.
     */
    constexpr auto intcon() const noexcept
    {
        return m_intcon;
    }

    /**
     * \brief Get the cached IOCON register value.
     *
     * \return The cached IOCON register value.
     */
    constexpr auto iocon() const noexcept
    {
        return m_iocon;
    }

  protected:
    /**
     * \brief Constructor.
     */
    constexpr Register_Cache() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Register_Cache( Register_Cache && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Register_Cache( Register_Cache const & original ) noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Register_Cache && expression ) noexcept -> Register_Cache & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator =( Register_Cache const & expression ) noexcept
        -> Register_Cache & = default;

    /**
     * \brief Reset all cached register valies to the register POR values.
     */
    constexpr void initialize() noexcept
    {
        *this = {};
    }

    /**
     * \brief Update the cached IODIR register value.
     *
     * \param[in] value The updated IODIR register value.
     */
    constexpr void cache_iodir( std::uint8_t value ) noexcept
    {
        m_iodir = value;
    }

    /**
     * \brief Update the cached IPOL register value.
     *
     * \param[in] value The updated IPOL register value.
     */
    constexpr void cache_ipol( std::uint8_t value ) noexcept
    {
        m_ipol = value;
    }

    /**
     * \brief Update the cached GPINTEN register value.
     *
     * \param[in] value The updated GPINTEN register value.
     */
    constexpr void cache_gpinten( std::uint8_t value ) noexcept
    {
        m_gpinten = value;
    }

    /**
     * \brief Update the cached DEFVAL register value.
     *
     * \param[in] value The updated DEFVAL register value.
     */
    constexpr void cache_defval( std::uint8_t value ) noexcept
    {
        m_defval = value;
    }

    /**
     * \brief Update the cached INTCON register value.
     *
     * \param[in] value The updated INTCON register value.
     */
    constexpr void cache_intcon( std::uint8_t value ) noexcept
    {
        m_intcon = value;
    }

    /**
     * \brief Update the cached IOCON register value.
     *
     * \param[in] value The updated IOCON register value.
     */
    constexpr void cache_iocon( std::uint8_t value ) noexcept
    {
        m_iocon = value;
    }

  private:
    /**
     * \brief The cached IODIR register value.
     */
    std::uint8_t m_iodir{ IODIR::POR };

    /**
     * \brief The cached IPOL register value.
     */
    std::uint8_t m_ipol{ IPOL::POR };

    /**
     * \brief The cached GPINTEN register value.
     */
    std::uint8_t m_gpinten{ GPINTEN::POR };

    /**
     * \brief The cached DEFVAL register value.
     */
    std::uint8_t m_defval{ DEFVAL::POR };

    /**
     * \brief The cached INTCON register value.
     */
    std::uint8_t m_intcon{ INTCON::POR };

    /**
     * \brief The cached IOCON register value.
     */
    std::uint8_t m_iocon{ IOCON::POR };
};

} // namespace picolibrary::Microchip::MCP23008

#endif // PICOLIBRARY_MICROCHIP_MCP23008_H

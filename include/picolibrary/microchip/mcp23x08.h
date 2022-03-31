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
 * \brief Input Polarity Port (IPOL) register.
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
 * \brief Interrupt Captured Value for Port (INTCAP) register.
 */
struct INTCAP {
    /**
     * \brief Register address.
     */
    static constexpr auto ADDRESS = std::uint8_t{ 0x08 };
};

/**
 * \brief General Purpose I/O Port (GPIO) register.
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

/**
 * \brief Driver concept.
 */
class Driver_Concept {
  public:
    /**
     * \brief Constructor.
     */
    Driver_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Driver_Concept( Driver_Concept && source ) noexcept;

    Driver_Concept( Driver_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Driver_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Driver_Concept && expression ) noexcept -> Driver_Concept &;

    auto operator=( Driver_Concept const & ) = delete;

    /**
     * \brief Read the IODIR register.
     *
     * \return The data read from the IODIR register.
     */
    auto read_iodir() const noexcept -> std::uint8_t;

    /**
     * \brief Write to the IODIR register.
     *
     * \param[in] data The data to write to the IODIR register.
     */
    void write_iodir( std::uint8_t data ) noexcept;

    /**
     * \brief Read the IPOL register.
     *
     * \return The data read from the IPOL register.
     */
    auto read_ipol() const noexcept -> std::uint8_t;

    /**
     * \brief Write to the IPOL register.
     *
     * \param[in] data The data to write to the IPOL register.
     */
    void write_ipol( std::uint8_t data ) noexcept;

    /**
     * \brief Read the GPINTEN register.
     *
     * \return The data read from the GPINTEN register.
     */
    auto read_gpinten() const noexcept -> std::uint8_t;

    /**
     * \brief Write to the GPINTEN register.
     *
     * \param[in] data The data to write to the GPINTEN register.
     */
    void write_gpinten( std::uint8_t data ) noexcept;

    /**
     * \brief Read the DEFVAL register.
     *
     * \return The data read from the DEFVAL register.
     */
    auto read_defval() const noexcept -> std::uint8_t;

    /**
     * \brief Write to the DEFVAL register.
     *
     * \param[in] data The data to write to the DEFVAL register.
     */
    void write_defval( std::uint8_t data ) noexcept;

    /**
     * \brief Read the INTCON register.
     *
     * \return The data read from the INTCON register.
     */
    auto read_intcon() const noexcept -> std::uint8_t;

    /**
     * \brief Write to the INTCON register.
     *
     * \param[in] data The data to write to the INTCON register.
     */
    void write_intcon( std::uint8_t data ) noexcept;

    /**
     * \brief Read the IOCON register.
     *
     * \return The data read from the IOCON register.
     */
    auto read_iocon() const noexcept -> std::uint8_t;

    /**
     * \brief Write to the IOCON register.
     *
     * \param[in] data The data to write to the IOCON register.
     */
    void write_iocon( std::uint8_t data ) noexcept;

    /**
     * \brief Read the GPPU register.
     *
     * \return The data read from the GPPU register.
     */
    auto read_gppu() const noexcept -> std::uint8_t;

    /**
     * \brief Write to the GPPU register.
     *
     * \param[in] data The data to write to the GPPU register.
     */
    void write_gppu( std::uint8_t data ) noexcept;

    /**
     * \brief Read the INTF register.
     *
     * \return The data read from the INTF register.
     */
    auto read_intf() const noexcept -> std::uint8_t;

    /**
     * \brief Read the INTCAP register.
     *
     * \return The data read from the INTCAP register.
     */
    auto read_intcap() const noexcept -> std::uint8_t;

    /**
     * \brief Read the GPIO register.
     *
     * \return The data read from the GPIO register.
     */
    auto read_gpio() const noexcept -> std::uint8_t;

    /**
     * \brief Write to the GPIO register.
     *
     * \param[in] data The data to write to the GPIO register.
     */
    void write_gpio( std::uint8_t data ) noexcept;

    /**
     * \brief Read the OLAT register.
     *
     * \return The data read from the OLAT register.
     */
    auto read_olat() const noexcept -> std::uint8_t;

    /**
     * \brief Write to the OLAT register.
     *
     * \param[in] data The data to write to the OLAT register.
     */
    void write_olat( std::uint8_t data ) noexcept;
};

/**
 * \brief Caching driver concept.
 */
class Caching_Driver_Concept {
  public:
    /**
     * \brief Constructor.
     */
    Caching_Driver_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Caching_Driver_Concept( Caching_Driver_Concept && source ) noexcept;

    Caching_Driver_Concept( Caching_Driver_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Caching_Driver_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Caching_Driver_Concept && expression ) noexcept -> Caching_Driver_Concept &;

    auto operator=( Caching_Driver_Concept const & ) = delete;

    /**
     * \brief Reset all cached register values.
     */
    void reset_cache() noexcept;

    /**
     * \brief Read the IODIR register.
     *
     * \return The data read from the IODIR register.
     */
    auto read_iodir() const noexcept -> std::uint8_t;

    /**
     * \brief Write to the IODIR register.
     *
     * \param[in] data The data to write to the IODIR register.
     */
    void write_iodir( std::uint8_t data ) noexcept;

    /**
     * \brief Get the cached IODIR register value.
     *
     * \return The cached IODIR register value.
     */
    auto iodir() const noexcept -> std::uint8_t;

    /**
     * \brief Read the IPOL register.
     *
     * \return The data read from the IPOL register.
     */
    auto read_ipol() const noexcept -> std::uint8_t;

    /**
     * \brief Write to the IPOL register.
     *
     * \param[in] data The data to write to the IPOL register.
     */
    void write_ipol( std::uint8_t data ) noexcept;

    /**
     * \brief Get the cached IPOL register value.
     *
     * \return The cached IPOL register value.
     */
    auto ipol() const noexcept -> std::uint8_t;

    /**
     * \brief Read the GPINTEN register.
     *
     * \return The data read from the GPINTEN register.
     */
    auto read_gpinten() const noexcept -> std::uint8_t;

    /**
     * \brief Write to the GPINTEN register.
     *
     * \param[in] data The data to write to the GPINTEN register.
     */
    void write_gpinten( std::uint8_t data ) noexcept;

    /**
     * \brief Get the cached GPINTEN register value.
     *
     * \return The cached GPINTEN register value.
     */
    auto gpinten() const noexcept -> std::uint8_t;

    /**
     * \brief Read the DEFVAL register.
     *
     * \return The data read from the DEFVAL register.
     */
    auto read_defval() const noexcept -> std::uint8_t;

    /**
     * \brief Write to the DEFVAL register.
     *
     * \param[in] data The data to write to the DEFVAL register.
     */
    void write_defval( std::uint8_t data ) noexcept;

    /**
     * \brief Get the cached DEFVAL register value.
     *
     * \return The cached DEFVAL register value.
     */
    auto defval() const noexcept -> std::uint8_t;

    /**
     * \brief Read the INTCON register.
     *
     * \return The data read from the INTCON register.
     */
    auto read_intcon() const noexcept -> std::uint8_t;

    /**
     * \brief Write to the INTCON register.
     *
     * \param[in] data The data to write to the INTCON register.
     */
    void write_intcon( std::uint8_t data ) noexcept;

    /**
     * \brief Get the cached INTCON register value.
     *
     * \return The cached INTCON register value.
     */
    auto intcon() const noexcept -> std::uint8_t;

    /**
     * \brief Read the IOCON register.
     *
     * \return The data read from the IOCON register.
     */
    auto read_iocon() const noexcept -> std::uint8_t;

    /**
     * \brief Write to the IOCON register.
     *
     * \param[in] data The data to write to the IOCON register.
     */
    void write_iocon( std::uint8_t data ) noexcept;

    /**
     * \brief Get the cached IOCON register value.
     *
     * \return The cached IOCON register value.
     */
    auto iocon() const noexcept -> std::uint8_t;

    /**
     * \brief Read the GPPU register.
     *
     * \return The data read from the GPPU register.
     */
    auto read_gppu() const noexcept -> std::uint8_t;

    /**
     * \brief Write to the GPPU register.
     *
     * \param[in] data The data to write to the GPPU register.
     */
    void write_gppu( std::uint8_t data ) noexcept;

    /**
     * \brief Get the cached GPPU register value.
     *
     * \return The cached GPPU register value.
     */
    auto gppu() const noexcept -> std::uint8_t;

    /**
     * \brief Read the INTF register.
     *
     * \return The data read from the INTF register.
     */
    auto read_intf() const noexcept -> std::uint8_t;

    /**
     * \brief Read the INTCAP register.
     *
     * \return The data read from the INTCAP register.
     */
    auto read_intcap() const noexcept -> std::uint8_t;

    /**
     * \brief Read the GPIO register.
     *
     * \return The data read from the GPIO register.
     */
    auto read_gpio() const noexcept -> std::uint8_t;

    /**
     * \brief Write to the GPIO register.
     *
     * \param[in] data The data to write to the GPIO register.
     */
    void write_gpio( std::uint8_t data ) noexcept;

    /**
     * \brief Read the OLAT register.
     *
     * \return The data read from the OLAT register.
     */
    auto read_olat() const noexcept -> std::uint8_t;

    /**
     * \brief Write to the OLAT register.
     *
     * \param[in] data The data to write to the OLAT register.
     */
    void write_olat( std::uint8_t data ) noexcept;

    /**
     * \brief Get the cached OLAT register value.
     *
     * \return The cached OLAT register value.
     */
    auto olat() const noexcept -> std::uint8_t;
};

} // namespace picolibrary::Microchip::MCP23X08

#endif // PICOLIBRARY_MICROCHIP_MCP23X08_H

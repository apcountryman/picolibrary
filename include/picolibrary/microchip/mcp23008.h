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
#include <utility>

#include "picolibrary/error.h"
#include "picolibrary/i2c.h"
#include "picolibrary/result.h"

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

    /**
     * \brief Get the cached GPPU register value.
     *
     * \return The cached GPPU register value.
     */
    constexpr auto gppu() const noexcept
    {
        return m_gppu;
    }

    /**
     * \brief Get the cached GPIO register value.
     *
     * \attention Writing to the GPIO register actually modifies the state of the OLAT
     *            register. Therefore, this function actually returns the cached OLAT
     *            register value.
     *
     * \return The cached GPIO register value.
     */
    constexpr auto gpio() const noexcept
    {
        return m_olat;
    }

    /**
     * \brief Get the cached OLAT register value.
     *
     * \return The cached OLAT register value.
     */
    constexpr auto olat() const noexcept
    {
        return m_olat;
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

    /**
     * \brief Update the cached GPPU register value.
     *
     * \param[in] value The updated GPPU register value.
     */
    constexpr void cache_gppu( std::uint8_t value ) noexcept
    {
        m_gppu = value;
    }

    /**
     * \brief Update the cached GPIO register value.
     *
     * \attention Writing to the GPIO register actually modifies the state of the OLAT
     *            register. Therefore, this function actually updates the cached OLAT
     *            register value.
     *
     * \param[in] value The updated GPIO register value.
     */
    constexpr void cache_gpio( std::uint8_t value ) noexcept
    {
        m_olat = value;
    }

    /**
     * \brief Update the cached OLAT register value.
     *
     * \param[in] value The updated OLAT register value.
     */
    constexpr void cache_olat( std::uint8_t value ) noexcept
    {
        m_olat = value;
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

    /**
     * \brief The cached GPPU register value.
     */
    std::uint8_t m_gppu{ GPPU::POR };

    /**
     * \brief The cached OLAT register value.
     */
    std::uint8_t m_olat{ OLAT::POR };
};

/**
 * \brief Microchip MCP23008 sequential operation mode.
 */
enum class Sequential_Operation_Mode : std::uint8_t {
    ENABLED  = 0b0 << IOCON::Bit::SEQOP, ///< Enabled.
    DISABLED = 0b1 << IOCON::Bit::SEQOP, ///< Disabled.
};

/**
 * \brief Microchip MCP23008 SDA slew rate control configuration.
 */
enum class SDA_Slew_Rate_Control_Configuration : std::uint8_t {
    ENABLED  = 0b0 << IOCON::Bit::DISSLW, ///< Enabled.
    DISABLED = 0b1 << IOCON::Bit::DISSLW, ///< Disabled.
};

/**
 * \brief Microchip MCP23008 interrupt mode.
 */
enum class Interrupt_Mode : std::uint8_t {
    OPEN_DRAIN            = 0b10 << IOCON::Bit::INTERRUPT_MODE, ///< Open-drain.
    PUSH_PULL_ACTIVE_HIGH = 0b01 << IOCON::Bit::INTERRUPT_MODE, ///< Push-pull, active high.
    PUSH_PULL_ACTIVE_LOW = 0b00 << IOCON::Bit::INTERRUPT_MODE, ///< Push-pull, active low.
};

/**
 * \brief Microchip MCP23008 device address range.
 */
struct Address {
    /**
     * \brief The minimum device address.
     */
    static constexpr auto MIN = I2C::Address{ I2C::Address::NUMERIC, 0b010'0000 };

    /**
     * \brief The maximum device address.
     */
    static constexpr auto MAX = I2C::Address{ I2C::Address::NUMERIC, 0b010'0111 };

    /**
     * \brief Get the minimum device address.
     *
     * \return The minimum device address.
     */
    static constexpr auto min() noexcept
    {
        return MIN;
    }

    /**
     * \brief Get the maximum device address.
     *
     * \return The maximum device address.
     */
    static constexpr auto max() noexcept
    {
        return MAX;
    }
};

/**
 * \brief Microchip MCP23008 driver.
 *
 * \tparam Bus_Multiplexer_Aligner A nullary functor that returns either
 *         picolibrary::Result<Void, Error_Code> or picolibrary::Result<Void, Void>. The
 *         functor must be default constructable, move constructable, and move assignable.
 *         When called, this functor should align the I2C bus's multiplexer(s) (if any) to
 *         enable communication with the Microchip MCP23008.
 * \tparam Controller The type of I2C controller used to interact with the bus the
 *         Microchip MCP23008 is attached to.
 * \tparam Register_Cache The type of Microchip MCP23008 register cache implementation
 *         used by the driver. The default Microchip MCP23008 register cache
 *         implementation should be used unless memory use is being optimized, or a mock
 *         Microchip MCP23008 register cache is being injected to support unit testing of
 *         this driver.
 * \tparam Device The type of I2C device implementation used by the driver. The default
 *         I2C device implementation should be used unless a mock I2C device
 *         implementation is being injected to support unit testing of this driver.
 */
template<
    typename Bus_Multiplexer_Aligner,
    typename Controller,
    typename Register_Cache = MCP23008::Register_Cache,
    typename Device = I2C::Device<Bus_Multiplexer_Aligner, Controller, std::uint8_t>>
class Driver : public Device, public Register_Cache {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Driver() = default;

    /**
     * \brief Constructor.
     *
     * \param[in] bus_multiplexer_aligner The MCP23008's bus multiplexer aligner.
     * \param[in] controller The I2C controller used to interact with the bus the MCP23008
     *            is attached to.
     * \param[in] address The MCP23008's address.
     * \param[in] nonresponsive_device_error The error code to return when the MCP23008
     *            does not respond when addressed, or does not acknowledge a write.
     */
    constexpr Driver(
        Bus_Multiplexer_Aligner bus_multiplexer_aligner,
        Controller &            controller,
        I2C::Address            address,
        Error_Code const &      nonresponsive_device_error ) noexcept :
        Device{ std::move( bus_multiplexer_aligner ), controller, address, nonresponsive_device_error },
        Register_Cache{}
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Driver( Driver && source ) noexcept = default;

    Driver( Driver const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Driver() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Driver && expression ) noexcept -> Driver & = default;

    auto operator=( Driver const & ) = delete;

    /**
     * \brief Read the IODIR register.
     *
     * \return The data read from the IODIR register if the read succeeded.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the MCP23008 is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the MCP23008.
     * \return An error code if the read failed for any other reason.
     */
    auto read_iodir() const noexcept
    {
        return this->read( IODIR::ADDRESS );
    }

    /**
     * \brief Write to the IODIR register.
     *
     * \param[in] data The data to write to the IODIR register.
     *
     * \return Nothing if the write succeeded.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the MCP23008 is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the MCP23008.
     * \return An error code if the read failed for any other reason.
     */
    auto write_iodir( std::uint8_t data ) noexcept -> Result<Void, Error_Code>
    {
        auto result = this->write( IODIR::ADDRESS, data );
        if ( result.is_error() ) {
            return result.error();
        } // if

        this->cache_iodir( data );

        return {};
    }

    /**
     * \brief Read the IPOL register.
     *
     * \return The data read from the IPOL register if the read succeeded.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the MCP23008 is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the MCP23008.
     * \return An error code if the read failed for any other reason.
     */
    auto read_ipol() const noexcept
    {
        return this->read( IPOL::ADDRESS );
    }

    /**
     * \brief Write to the IPOL register.
     *
     * \param[in] data The data to write to the IPOL register.
     *
     * \return Nothing if the write succeeded.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the MCP23008 is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the MCP23008.
     * \return An error code if the read failed for any other reason.
     */
    auto write_ipol( std::uint8_t data ) noexcept -> Result<Void, Error_Code>
    {
        auto result = this->write( IPOL::ADDRESS, data );
        if ( result.is_error() ) {
            return result.error();
        } // if

        this->cache_ipol( data );

        return {};
    }

    /**
     * \brief Read the GPINTEN register.
     *
     * \return The data read from the GPINTEN register if the read succeeded.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the MCP23008 is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the MCP23008.
     * \return An error code if the read failed for any other reason.
     */
    auto read_gpinten() const noexcept
    {
        return this->read( GPINTEN::ADDRESS );
    }

    /**
     * \brief Write to the GPINTEN register.
     *
     * \param[in] data The data to write to the GPINTEN register.
     *
     * \return Nothing if the write succeeded.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the MCP23008 is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the MCP23008.
     * \return An error code if the read failed for any other reason.
     */
    auto write_gpinten( std::uint8_t data ) noexcept -> Result<Void, Error_Code>
    {
        auto result = this->write( GPINTEN::ADDRESS, data );
        if ( result.is_error() ) {
            return result.error();
        } // if

        this->cache_gpinten( data );

        return {};
    }

    /**
     * \brief Read the DEFVAL register.
     *
     * \return The data read from the DEFVAL register if the read succeeded.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the MCP23008 is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the MCP23008.
     * \return An error code if the read failed for any other reason.
     */
    auto read_defval() const noexcept
    {
        return this->read( DEFVAL::ADDRESS );
    }

    /**
     * \brief Write to the DEFVAL register.
     *
     * \param[in] data The data to write to the DEFVAL register.
     *
     * \return Nothing if the write succeeded.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the MCP23008 is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the MCP23008.
     * \return An error code if the read failed for any other reason.
     */
    auto write_defval( std::uint8_t data ) noexcept -> Result<Void, Error_Code>
    {
        auto result = this->write( DEFVAL::ADDRESS, data );
        if ( result.is_error() ) {
            return result.error();
        } // if

        this->cache_defval( data );

        return {};
    }

    /**
     * \brief Read the INTCON register.
     *
     * \return The data read from the INTCON register if the read succeeded.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the MCP23008 is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the MCP23008.
     * \return An error code if the read failed for any other reason.
     */
    auto read_intcon() const noexcept
    {
        return this->read( INTCON::ADDRESS );
    }

    /**
     * \brief Write to the INTCON register.
     *
     * \param[in] data The data to write to the INTCON register.
     *
     * \return Nothing if the write succeeded.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the MCP23008 is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the MCP23008.
     * \return An error code if the read failed for any other reason.
     */
    auto write_intcon( std::uint8_t data ) noexcept -> Result<Void, Error_Code>
    {
        auto result = this->write( INTCON::ADDRESS, data );
        if ( result.is_error() ) {
            return result.error();
        } // if

        this->cache_intcon( data );

        return {};
    }

    /**
     * \brief Read the IOCON register.
     *
     * \return The data read from the IOCON register if the read succeeded.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the MCP23008 is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the MCP23008.
     * \return An error code if the read failed for any other reason.
     */
    auto read_iocon() const noexcept
    {
        return this->read( IOCON::ADDRESS );
    }

    /**
     * \brief Write to the IOCON register.
     *
     * \param[in] data The data to write to the IOCON register.
     *
     * \return Nothing if the write succeeded.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the MCP23008 is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the MCP23008.
     * \return An error code if the read failed for any other reason.
     */
    auto write_iocon( std::uint8_t data ) noexcept -> Result<Void, Error_Code>
    {
        auto result = this->write( IOCON::ADDRESS, data );
        if ( result.is_error() ) {
            return result.error();
        } // if

        this->cache_iocon( data );

        return {};
    }

    /**
     * \brief Read the GPPU register.
     *
     * \return The data read from the GPPU register if the read succeeded.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the MCP23008 is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the MCP23008.
     * \return An error code if the read failed for any other reason.
     */
    auto read_gppu() const noexcept
    {
        return this->read( GPPU::ADDRESS );
    }

    /**
     * \brief Write to the GPPU register.
     *
     * \param[in] data The data to write to the GPPU register.
     *
     * \return Nothing if the write succeeded.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the MCP23008 is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the MCP23008.
     * \return An error code if the read failed for any other reason.
     */
    auto write_gppu( std::uint8_t data ) noexcept -> Result<Void, Error_Code>
    {
        auto result = this->write( GPPU::ADDRESS, data );
        if ( result.is_error() ) {
            return result.error();
        } // if

        this->cache_gppu( data );

        return {};
    }

    /**
     * \brief Read the INTF register.
     *
     * \return The data read from the INTF register if the read succeeded.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the MCP23008 is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the MCP23008.
     * \return An error code if the read failed for any other reason.
     */
    auto read_intf() const noexcept
    {
        return this->read( INTF::ADDRESS );
    }

    /**
     * \brief Read the INTCAP register.
     *
     * \return The data read from the INTCAP register if the read succeeded.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the MCP23008 is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the MCP23008.
     * \return An error code if the read failed for any other reason.
     */
    auto read_intcap() const noexcept
    {
        return this->read( INTCAP::ADDRESS );
    }
};

/**
 * \brief Construct a picolibrary::Microchip::MCP23008::Driver.
 *
 * \tparam Bus_Multiplexer_Aligner A nullary functor that returns either
 *         picolibrary::Result<Void, Error_Code> or picolibrary::Result<Void, Void>. The
 *         functor must be default constructable, move constructable, and move assignable.
 *         When called, this functor should align the I2C bus's multiplexer(s) (if any) to
 *         enable communication with the Microchip MCP23008.
 * \tparam Controller The type of I2C controller used to interact with the bus the
 *         Microchip MCP23008 is attached to.
 * \tparam Register_Cache The type of Microchip MCP23008 register cache implementation
 *         used by the driver. The default Microchip MCP23008 register cache
 *         implementation should be used unless memory use is being optimized, or a mock
 *         Microchip MCP23008 register cache is being injected to support unit testing of
 *         this driver.
 * \tparam Device The type of I2C device implementation used by the driver. The default
 *         I2C device implementation should be used unless a mock I2C device
 *         implementation is being injected to support unit testing of this driver.
 *
 * \param[in] bus_multiplexer_aligner The MCP23008's bus multiplexer aligner.
 * \param[in] controller The I2C controller used to interact with the bus the MCP23008 is
 *            attached to.
 * \param[in] address The MCP23008's address.
 * \param[in] nonresponsive_device_error The error code to return when the MCP23008
 *            does not respond when addressed, or does not acknowledge a write.
 *
 * \return The driver if address is greater than or equal to
 *         picolibrary::Microchip::MCP23008::Address::MIN, and less than or equal to
 *         picolibrary::Microchip::MCP23008::Address::MAX.
 * \return picolibrary::Generic_Error::INVALID_ARGUMENT if address is less than
 *         picolibrary::Microchip::MCP23008::Address::MIN, or greater than
 *         picolibrary::Microchip::MCP23008::Address::MAX.
 */
template<
    typename Bus_Multiplexer_Aligner,
    typename Controller,
    typename Register_Cache = MCP23008::Register_Cache,
    typename Device = I2C::Device<Bus_Multiplexer_Aligner, Controller, std::uint8_t>>
constexpr auto make_driver(
    Bus_Multiplexer_Aligner bus_multiplexer_aligner,
    Controller &            controller,
    I2C::Address            address,
    Error_Code const &      nonresponsive_device_error ) noexcept
    -> Result<Driver<Bus_Multiplexer_Aligner, Controller, Register_Cache, Device>, Error_Code>
{
    if ( address < Address::MIN or address > Address::MAX ) {
        return Generic_Error::INVALID_ARGUMENT;
    } // if

    return Driver{ std::move( bus_multiplexer_aligner ), controller, address, nonresponsive_device_error };
}

} // namespace picolibrary::Microchip::MCP23008

#endif // PICOLIBRARY_MICROCHIP_MCP23008_H

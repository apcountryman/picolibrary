/**
 * picolibrary
 *
 * Copyright 2020-2024, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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

#include <utility>

#include "picolibrary/error.h"
#include "picolibrary/i2c.h"
#include "picolibrary/microchip/mcp23x08.h"
#include "picolibrary/precondition.h"

/**
 * \brief Microchip MCP23008 facilities.
 */
namespace picolibrary::Microchip::MCP23008 {

using MCP23X08::IODIR;

using MCP23X08::IPOL;

using MCP23X08::GPINTEN;

using MCP23X08::DEFVAL;

using MCP23X08::INTCON;

using MCP23X08::IOCON;

using MCP23X08::GPPU;

using MCP23X08::INTF;

using MCP23X08::INTCAP;

using MCP23X08::GPIO;

using MCP23X08::OLAT;

/**
 * \brief Address, numeric format.
 */
using Address_Numeric = I2C::Device_Address_Numeric<0b0100'000, 0b0100'111>;

/**
 * \brief Address, transmitted format.
 */
using Address_Transmitted = I2C::Device_Address_Transmitted<0b0100'000'0, 0b0100'111'0>;

/**
 * \brief Driver.
 *
 * \tparam Bus_Multiplexer_Aligner A nullary functor. The functor must be default
 *         constructable, move constructable, and move assignable. When called, the
 *         functor should align the bus's multiplexer(s) (if any) to enable communication
 *         with the MCP23008.
 * \tparam Controller The type of controller used to communicate with the MCP23008.
 * \tparam Device The type of device implementation used by the driver. The default device
 *         implementation should be used unless a mock device implementation is being
 *         injected to support automated testing of this driver.
 */
template<typename Bus_Multiplexer_Aligner, typename Controller, typename Device = I2C::Device<Bus_Multiplexer_Aligner, Controller>>
class Driver : public Device {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Driver() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] bus_multiplexer_aligner The MCP23008's bus multiplexer aligner.
     * \param[in] controller The controller used to communicate with the MCP23008.
     * \param[in] address The MCP23008's address.
     * \param[in] nonresponsive_device_error The fatal error that occurs if the MCP23008
     *            does not respond when addressed or does not acknowledge a write.
     */
    constexpr Driver(
        Bus_Multiplexer_Aligner bus_multiplexer_aligner,
        Controller &            controller,
        Address_Transmitted     address,
        Error_Code const &      nonresponsive_device_error ) noexcept :
        Device{ std::move( bus_multiplexer_aligner ), controller, std::move( address ), nonresponsive_device_error }
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
     * \pre the MCP23008 is responsive
     *
     * \return The data read from the IODIR register.
     */
    auto read_iodir() const noexcept -> std::uint8_t
    {
        return this->read( IODIR::ADDRESS );
    }

    /**
     * \brief Write to the IODIR register.
     *
     * \pre the MCP23008 is responsive
     *
     * \param[in] data The data to write to the IODIR register.
     */
    void write_iodir( std::uint8_t data ) noexcept
    {
        this->write( IODIR::ADDRESS, data );
    }

    /**
     * \brief Read the IPOL register.
     *
     * \pre the MCP23008 is responsive
     *
     * \return The data read from the IPOL register.
     */
    auto read_ipol() const noexcept -> std::uint8_t
    {
        return this->read( IPOL::ADDRESS );
    }

    /**
     * \brief Write to the IPOL register.
     *
     * \pre the MCP23008 is responsive
     *
     * \param[in] data The data to write to the IPOL register.
     */
    void write_ipol( std::uint8_t data ) noexcept
    {
        this->write( IPOL::ADDRESS, data );
    }

    /**
     * \brief Read the GPINTEN register.
     *
     * \pre the MCP23008 is responsive
     *
     * \return The data read from the GPINTEN register.
     */
    auto read_gpinten() const noexcept -> std::uint8_t
    {
        return this->read( GPINTEN::ADDRESS );
    }

    /**
     * \brief Write to the GPINTEN register.
     *
     * \pre the MCP23008 is responsive
     *
     * \param[in] data The data to write to the GPINTEN register.
     */
    void write_gpinten( std::uint8_t data ) noexcept
    {
        this->write( GPINTEN::ADDRESS, data );
    }

    /**
     * \brief Read the DEFVAL register.
     *
     * \pre the MCP23008 is responsive
     *
     * \return The data read from the DEFVAL register.
     */
    auto read_defval() const noexcept -> std::uint8_t
    {
        return this->read( DEFVAL::ADDRESS );
    }

    /**
     * \brief Write to the DEFVAL register.
     *
     * \pre the MCP23008 is responsive
     *
     * \param[in] data The data to write to the DEFVAL register.
     */
    void write_defval( std::uint8_t data ) noexcept
    {
        this->write( DEFVAL::ADDRESS, data );
    }

    /**
     * \brief Read the INTCON register.
     *
     * \pre the MCP23008 is responsive
     *
     * \return The data read from the INTCON register.
     */
    auto read_intcon() const noexcept -> std::uint8_t
    {
        return this->read( INTCON::ADDRESS );
    }

    /**
     * \brief Write to the INTCON register.
     *
     * \pre the MCP23008 is responsive
     *
     * \param[in] data The data to write to the INTCON register.
     */
    void write_intcon( std::uint8_t data ) noexcept
    {
        this->write( INTCON::ADDRESS, data );
    }

    /**
     * \brief Read the IOCON register.
     *
     * \pre the MCP23008 is responsive
     *
     * \return The data read from the IOCON register.
     */
    auto read_iocon() const noexcept -> std::uint8_t
    {
        return this->read( IOCON::ADDRESS );
    }

    /**
     * \brief Write to the IOCON register.
     *
     * \pre the MCP23008 is responsive
     *
     * \param[in] data The data to write to the IOCON register.
     */
    void write_iocon( std::uint8_t data ) noexcept
    {
        this->write( IOCON::ADDRESS, data );
    }

    /**
     * \brief Read the GPPU register.
     *
     * \pre the MCP23008 is responsive
     *
     * \return The data read from the GPPU register.
     */
    auto read_gppu() const noexcept -> std::uint8_t
    {
        return this->read( GPPU::ADDRESS );
    }

    /**
     * \brief Write to the GPPU register.
     *
     * \pre the MCP23008 is responsive
     *
     * \param[in] data The data to write to the GPPU register.
     */
    void write_gppu( std::uint8_t data ) noexcept
    {
        this->write( GPPU::ADDRESS, data );
    }

    /**
     * \brief Read the INTF register.
     *
     * \pre the MCP23008 is responsive
     *
     * \return The data read from the INTF register.
     */
    auto read_intf() const noexcept -> std::uint8_t
    {
        return this->read( INTF::ADDRESS );
    }

    /**
     * \brief Read the INTCAP register.
     *
     * \pre the MCP23008 is responsive
     *
     * \return The data read from the INTCAP register.
     */
    auto read_intcap() const noexcept -> std::uint8_t
    {
        return this->read( INTCAP::ADDRESS );
    }

    /**
     * \brief Read the GPIO register.
     *
     * \pre the MCP23008 is responsive
     *
     * \return The data read from the GPIO register.
     */
    auto read_gpio() const noexcept -> std::uint8_t
    {
        return this->read( GPIO::ADDRESS );
    }

    /**
     * \brief Write to the GPIO register.
     *
     * \pre the MCP23008 is responsive
     *
     * \param[in] data The data to write to the GPIO register.
     */
    void write_gpio( std::uint8_t data ) noexcept
    {
        this->write( GPIO::ADDRESS, data );
    }

    /**
     * \brief Read the OLAT register.
     *
     * \pre the MCP23008 is responsive
     *
     * \return The data read from the OLAT register.
     */
    auto read_olat() const noexcept -> std::uint8_t
    {
        return this->read( OLAT::ADDRESS );
    }

    /**
     * \brief Write to the OLAT register.
     *
     * \pre the MCP23008 is responsive
     *
     * \param[in] data The data to write to the OLAT register.
     */
    void write_olat( std::uint8_t data ) noexcept
    {
        this->write( OLAT::ADDRESS, data );
    }
};

/**
 * \brief Caching driver.
 *
 * \tparam Bus_Multiplexer_Aligner A nullary functor. The functor must be default
 *         constructable, move constructable, and move assignable. When called, the
 *         functor should align the bus's multiplexer(s) (if any) to enable communication
 *         with the MCP23008.
 * \tparam Controller The type of controller used to communicate with the MCP23008.
 */
template<typename Bus_Multiplexer_Aligner, typename Controller>
using Caching_Driver = MCP23X08::Caching_Driver<Driver<Bus_Multiplexer_Aligner, Controller>>;

using MCP23X08::Pin;

using MCP23X08::Internally_Pulled_Up_Input_Pin;

using MCP23X08::Open_Drain_IO_Pin;

using MCP23X08::Push_Pull_IO_Pin;

} // namespace picolibrary::Microchip::MCP23008

#endif // PICOLIBRARY_MICROCHIP_MCP23008_H

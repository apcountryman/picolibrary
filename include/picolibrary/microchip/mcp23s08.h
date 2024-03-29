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
 * \brief picolibrary::Microchip::MCP23S08 interface.
 */

#ifndef PICOLIBRARY_MICROCHIP_MCP23S08_H
#define PICOLIBRARY_MICROCHIP_MCP23S08_H

#include <cstdint>
#include <utility>

#include "picolibrary/error.h"
#include "picolibrary/microchip/mcp23x08.h"
#include "picolibrary/precondition.h"
#include "picolibrary/spi.h"
#include "picolibrary/utility.h"

/**
 * \brief Microchip MCP23S08 facilities.
 */
namespace picolibrary::Microchip::MCP23S08 {

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
 * \brief MCP23S08 address, numeric format.
 */
class Address_Numeric;

/**
 * \brief MCP23S08 address, transmitted format.
 */
class Address_Transmitted;

class Address_Numeric {
  public:
    /**
     * \brief Address unsigned integer representation.
     */
    using Unsigned_Integer = std::uint_fast8_t;

    /**
     * \brief Get the minimum valid address.
     *
     * \return The minimum valid address.
     */
    static constexpr auto min() noexcept -> Address_Numeric
    {
        return Address_Numeric{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, 0b01000'00 };
    }

    /**
     * \brief Get the maximum valid address.
     *
     * \return The maximum valid address.
     */
    static constexpr auto max() noexcept -> Address_Numeric
    {
        return Address_Numeric{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, 0b01000'11 };
    }

    /**
     * \brief Constructor.
     */
    constexpr Address_Numeric() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] address The address.
     *
     * \pre address >= picolibrary::Microchip::MCP23S08::Numeric_Address::min().as_unsigned_integer()
     * \pre address <= picolibrary::Microchip::MCP23S08::Numeric_Address::max().as_unsigned_integer()
     */
    constexpr Address_Numeric( Unsigned_Integer address ) noexcept : m_address{ address }
    {
        PICOLIBRARY_EXPECT(
            address >= min().as_unsigned_integer() and address <= max().as_unsigned_integer(),
            Generic_Error::INVALID_ARGUMENT );
    }

    /**
     * \brief Constructor.
     *
     * \param[in] address The address.
     */
    constexpr Address_Numeric( Bypass_Precondition_Expectation_Checks, Unsigned_Integer address ) noexcept :
        m_address{ address }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] address The address.
     */
    constexpr Address_Numeric( Address_Transmitted address ) noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Address_Numeric( Address_Numeric && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Address_Numeric( Address_Numeric const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Address_Numeric() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Address_Numeric && expression ) noexcept -> Address_Numeric & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator  =( Address_Numeric const & expression ) noexcept
        -> Address_Numeric & = default;

    /**
     * \brief Get the address in its unsigned integer representation.
     *
     * \return The address in its unsigned integer representation.
     */
    constexpr auto as_unsigned_integer() const noexcept -> Unsigned_Integer
    {
        return m_address;
    }

  private:
    /**
     * \brief The address.
     */
    Unsigned_Integer m_address{ 0b01000'00 };
};

class Address_Transmitted {
  public:
    /**
     * \brief Address unsigned integer representation.
     */
    using Unsigned_Integer = std::uint8_t;

    /**
     * \brief Get the minimum valid address.
     *
     * \return The minimum valid address.
     */
    static constexpr auto min() noexcept -> Address_Transmitted
    {
        return Address_Transmitted{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, 0b01000'00'0 };
    }

    /**
     * \brief Get the maximum valid address.
     *
     * \return The maximum valid address.
     */
    static constexpr auto max() noexcept -> Address_Transmitted
    {
        return Address_Transmitted{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, 0b01000'11'0 };
    }

    /**
     * \brief Constructor.
     */
    constexpr Address_Transmitted() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] address The address.
     *
     * \pre address >= picolibrary::Microchip::MCP23S08::Transmitted_Address::min().as_unsigned_integer()
     * \pre address <= picolibrary::Microchip::MCP23S08::Transmitted_Address::max().as_unsigned_integer()
     * \pre not ( address & 0b1 )
     */
    constexpr Address_Transmitted( Unsigned_Integer address ) noexcept :
        m_address{ address }
    {
        PICOLIBRARY_EXPECT(
            address >= min().as_unsigned_integer()
                and address <= max().as_unsigned_integer() and not( address & 0b1 ),
            Generic_Error::INVALID_ARGUMENT );
    }

    /**
     * \brief Constructor.
     *
     * \param[in] address The address.
     */
    constexpr Address_Transmitted( Bypass_Precondition_Expectation_Checks, Unsigned_Integer address ) noexcept :
        m_address{ address }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] address The address.
     */
    constexpr Address_Transmitted( Address_Numeric address ) noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Address_Transmitted( Address_Transmitted && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Address_Transmitted( Address_Transmitted const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Address_Transmitted() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator      =( Address_Transmitted && expression ) noexcept
        -> Address_Transmitted & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator      =( Address_Transmitted const & expression ) noexcept
        -> Address_Transmitted & = default;

    /**
     * \brief Get the address in its unsigned integer representation.
     *
     * \return The address in its unsigned integer representation.
     */
    constexpr auto as_unsigned_integer() const noexcept -> Unsigned_Integer
    {
        return m_address;
    }

  private:
    /**
     * \brief The address.
     */
    Unsigned_Integer m_address{ 0b01000'00'0 };
};

constexpr Address_Numeric::Address_Numeric( Address_Transmitted address ) noexcept :
    m_address{ static_cast<Unsigned_Integer>( address.as_unsigned_integer() >> 1 ) }
{
}

/**
 * \brief Equality operator.
 *
 * \relatedalso picolibrary::Microchip::MCP23S08::Address_Numeric
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is equal to rhs.
 * \return false if lhs is not equal to rhs.
 */
constexpr auto operator==( Address_Numeric lhs, Address_Numeric rhs ) noexcept -> bool
{
    return lhs.as_unsigned_integer() == rhs.as_unsigned_integer();
}

/**
 * \brief Inequality operator.
 *
 * \relatedalso picolibrary::Microchip::MCP23S08::Address_Numeric
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is not equal to rhs.
 * \return false if lhs is equal to rhs.
 */
constexpr auto operator!=( Address_Numeric lhs, Address_Numeric rhs ) noexcept -> bool
{
    return not( lhs == rhs );
}

/**
 * \brief Less than operator.
 *
 * \relatedalso picolibrary::Microchip::MCP23S08::Address_Numeric
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than rhs.
 * \return false if lhs is not less than rhs.
 */
constexpr auto operator<( Address_Numeric lhs, Address_Numeric rhs ) noexcept -> bool
{
    return lhs.as_unsigned_integer() < rhs.as_unsigned_integer();
}

/**
 * \brief Greater than operator.
 *
 * \relatedalso picolibrary::Microchip::MCP23S08::Address_Numeric
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than rhs.
 * \return false if lhs is not greater than rhs.
 */
constexpr auto operator>( Address_Numeric lhs, Address_Numeric rhs ) noexcept -> bool
{
    return rhs < lhs;
}

/**
 * \brief Less than or equal to operator.
 *
 * \relatedalso picolibrary::Microchip::MCP23S08::Address_Numeric
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than or equal to rhs.
 * \return false if lhs is not less than or equal to rhs.
 */
constexpr auto operator<=( Address_Numeric lhs, Address_Numeric rhs ) noexcept -> bool
{
    return not( lhs > rhs );
}

/**
 * \brief Greater than or equal to operator.
 *
 * \relatedalso picolibrary::Microchip::MCP23S08::Address_Numeric
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than or equal to rhs.
 * \return false if lhs is not greater than or equal to rhs.
 */
constexpr auto operator>=( Address_Numeric lhs, Address_Numeric rhs ) noexcept -> bool
{
    return not( lhs < rhs );
}

constexpr Address_Transmitted::Address_Transmitted( Address_Numeric address ) noexcept :
    m_address{ static_cast<Unsigned_Integer>( address.as_unsigned_integer() << 1 ) }
{
}

/**
 * \brief Equality operator.
 *
 * \relatedalso picolibrary::Microchip::MCP23S08::Address_Transmitted
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is equal to rhs.
 * \return false if lhs is not equal to rhs.
 */
constexpr auto operator==( Address_Transmitted lhs, Address_Transmitted rhs ) noexcept -> bool
{
    return lhs.as_unsigned_integer() == rhs.as_unsigned_integer();
}

/**
 * \brief Inequality operator.
 *
 * \relatedalso picolibrary::Microchip::MCP23S08::Address_Transmitted
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is not equal to rhs.
 * \return false if lhs is equal to rhs.
 */
constexpr auto operator!=( Address_Transmitted lhs, Address_Transmitted rhs ) noexcept -> bool
{
    return not( lhs == rhs );
}

/**
 * \brief Less than operator.
 *
 * \relatedalso picolibrary::Microchip::MCP23S08::Address_Transmitted
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than rhs.
 * \return false if lhs is not less than rhs.
 */
constexpr auto operator<( Address_Transmitted lhs, Address_Transmitted rhs ) noexcept -> bool
{
    return lhs.as_unsigned_integer() < rhs.as_unsigned_integer();
}

/**
 * \brief Greater than operator.
 *
 * \relatedalso picolibrary::Microchip::MCP23S08::Address_Transmitted
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than rhs.
 * \return false if lhs is not greater than rhs.
 */
constexpr auto operator>( Address_Transmitted lhs, Address_Transmitted rhs ) noexcept -> bool
{
    return rhs < lhs;
}

/**
 * \brief Less than or equal to operator.
 *
 * \relatedalso picolibrary::Microchip::MCP23S08::Address_Transmitted
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than or equal to rhs.
 * \return false if lhs is not less than or equal to rhs.
 */
constexpr auto operator<=( Address_Transmitted lhs, Address_Transmitted rhs ) noexcept -> bool
{
    return not( lhs > rhs );
}

/**
 * \brief Greater than or equal to operator.
 *
 * \relatedalso picolibrary::Microchip::MCP23S08::Address_Transmitted
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than or equal to rhs.
 * \return false if lhs is not greater than or equal to rhs.
 */
constexpr auto operator>=( Address_Transmitted lhs, Address_Transmitted rhs ) noexcept -> bool
{
    return not( lhs < rhs );
}

/**
 * \brief Communication controller.
 *
 * \tparam Controller The type of controller used to communicate with the MCP23S08.
 * \tparam Device_Selector The type of device selector used to select and deselect the
 *         MCP23S08.
 * \tparam Device The type of device implementation used by the communication controller.
 *         The default device implementation should be used unless a mock device
 *         implementation is being injected to support automated testing of this
 *         communication controller.
 */
template<typename Controller, typename Device_Selector, typename Device = SPI::Device<Controller, Device_Selector>>
class Communication_Controller : public Device {
  public:
    Communication_Controller( Communication_Controller const & ) = delete;

    auto operator=( Communication_Controller const & ) = delete;

    /**
     * \brief Get the MCP23S08's address.
     *
     * \return The MCP23S08's address.
     */
    constexpr auto address() const noexcept -> Address_Transmitted
    {
        return m_address;
    }

    using Device::initialize;

  protected:
    /**
     * \brief Constructor.
     */
    constexpr Communication_Controller() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] controller The controller used to communicate with the MCP23S08.
     * \param[in] configuration The controller clock and data exchange bit order
     *            configuration that meets the MCP23S08's communication requirements.
     * \param[in] device_selector The device selector used to select and deselect the
     *            MCP23S08.
     * \param[in] address The MCP23S08's address.
     */
    constexpr Communication_Controller(
        Controller &                               controller,
        typename Controller::Configuration const & configuration,
        Device_Selector                            device_selector,
        Address_Transmitted                        address ) noexcept :
        Device{ controller, configuration, std::move( device_selector ) },
        m_address{ std::move( address ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Communication_Controller( Communication_Controller && source ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Communication_Controller() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator           =( Communication_Controller && expression ) noexcept
        -> Communication_Controller & = default;

    /**
     * \brief Read a register.
     *
     * \param[in] register_address The address of the register to read.
     *
     * \return The data read from the register.
     */
    auto read( std::uint8_t register_address ) const noexcept -> std::uint8_t
    {
        this->configure();

        auto const guard = SPI::Device_Selection_Guard{ this->device_selector() };

        this->transmit( m_address.as_unsigned_integer() | to_underlying( Operation::READ ) );
        this->transmit( register_address );
        return this->receive();
    }

    /**
     * \brief Write to a register.
     *
     * \param[in] register_address The address of the register to write to.
     * \param[in] data The data to write to the register.
     */
    void write( std::uint8_t register_address, std::uint8_t data ) noexcept
    {
        this->configure();

        auto const guard = SPI::Device_Selection_Guard{ this->device_selector() };

        this->transmit( m_address.as_unsigned_integer() | to_underlying( Operation::WRITE ) );
        this->transmit( register_address );
        this->transmit( data );
    }

  private:
    /**
     * \brief Operation.
     */
    enum class Operation : std::uint8_t {
        READ  = 0b1, ///< Read.
        WRITE = 0b0, ///< Write.
    };

    /**
     * \brief The MCP23S08's address.
     */
    Address_Transmitted m_address{};
};

/**
 * \brief Driver.
 *
 * \tparam Controller The type of controller used to communicate with the MCP23S08.
 * \tparam Device_Selector The type of device selector used to select and deselect the
 *         MCP23S08.
 * \tparam Communication_Controller The type of communication controller implementation
 *         used by the driver. The default communication controller implementation should
 *         be used unless a mock communication controller implementation is being injected
 *         to support automated testing of this driver.
 */
template<typename Controller, typename Device_Selector, typename Communication_Controller = ::picolibrary::Microchip::MCP23S08::Communication_Controller<Controller, Device_Selector>>
class Driver : public Communication_Controller {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Driver() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] controller The controller used to communicate with the MCP23S08.
     * \param[in] configuration The controller clock and data exchange bit order
     *            configuration that meets the MCP23S08's communication requirements.
     * \param[in] device_selector The device selector used to select and deselect the
     *            MCP23S08.
     * \param[in] address The MCP23S08's address.
     */
    constexpr Driver(
        Controller &                               controller,
        typename Controller::Configuration const & configuration,
        Device_Selector                            device_selector,
        Address_Transmitted                        address ) noexcept :
        Communication_Controller{ controller, configuration, std::move( device_selector ), std::move( address ) }
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
     * \return The data read from the IODIR register.
     */
    auto read_iodir() const noexcept -> std::uint8_t
    {
        return this->read( IODIR::ADDRESS );
    }

    /**
     * \brief Write to the IODIR register.
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
     * \return The data read from the IPOL register.
     */
    auto read_ipol() const noexcept -> std::uint8_t
    {
        return this->read( IPOL::ADDRESS );
    }

    /**
     * \brief Write to the IPOL register.
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
     * \return The data read from the GPINTEN register.
     */
    auto read_gpinten() const noexcept -> std::uint8_t
    {
        return this->read( GPINTEN::ADDRESS );
    }

    /**
     * \brief Write to the GPINTEN register.
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
     * \return The data read from the DEFVAL register.
     */
    auto read_defval() const noexcept -> std::uint8_t
    {
        return this->read( DEFVAL::ADDRESS );
    }

    /**
     * \brief Write to the DEFVAL register.
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
     * \return The data read from the INTCON register.
     */
    auto read_intcon() const noexcept -> std::uint8_t
    {
        return this->read( INTCON::ADDRESS );
    }

    /**
     * \brief Write to the INTCON register.
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
     * \return The data read from the IOCON register.
     */
    auto read_iocon() const noexcept -> std::uint8_t
    {
        return this->read( IOCON::ADDRESS );
    }

    /**
     * \brief Write to the IOCON register.
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
     * \return The data read from the GPPU register.
     */
    auto read_gppu() const noexcept -> std::uint8_t
    {
        return this->read( GPPU::ADDRESS );
    }

    /**
     * \brief Write to the GPPU register.
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
     * \return The data read from the INTF register.
     */
    auto read_intf() const noexcept -> std::uint8_t
    {
        return this->read( INTF::ADDRESS );
    }

    /**
     * \brief Read the INTCAP register.
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
     * \return The data read from the GPIO register.
     */
    auto read_gpio() const noexcept -> std::uint8_t
    {
        return this->read( GPIO::ADDRESS );
    }

    /**
     * \brief Write to the GPIO register.
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
     * \return The data read from the OLAT register.
     */
    auto read_olat() const noexcept -> std::uint8_t
    {
        return this->read( OLAT::ADDRESS );
    }

    /**
     * \brief Write to the OLAT register.
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
 * \tparam Controller The type of controller used to communicate with the MCP23S08.
 * \tparam Device_Selector The type of device selector used to select and deselect the
 *         MCP23S08.
 */
template<typename Controller, typename Device_Selector>
using Caching_Driver = MCP23X08::Caching_Driver<Driver<Controller, Device_Selector>>;

using MCP23X08::Pin;

using MCP23X08::Internally_Pulled_Up_Input_Pin;

using MCP23X08::Open_Drain_IO_Pin;

using MCP23X08::Push_Pull_IO_Pin;

} // namespace picolibrary::Microchip::MCP23S08

#endif // PICOLIBRARY_MICROCHIP_MCP23S08_H

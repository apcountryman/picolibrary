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
 * \brief picolibrary::I2C interface.
 */

#ifndef PICOLIBRARY_I2C_H
#define PICOLIBRARY_I2C_H

#include <cstdint>
#include <utility>

#include "picolibrary/algorithm.h"
#include "picolibrary/error.h"
#include "picolibrary/fatal_error.h"
#include "picolibrary/result.h"
#include "picolibrary/void.h"

/**
 * \brief Inter-Integrated Circuit (I2C) facilities.
 */
namespace picolibrary::I2C {

/**
 * \brief Device address.
 */
class Address {
  public:
    /**
     * \brief Device address numeric form tag.
     */
    struct Numeric {
        /**
         * \brief The minimum supported device address in numeric form.
         */
        static constexpr auto MIN = std::uint_fast8_t{ 0b000'0000 };

        /**
         * \brief The maximum supported device address in numeric form.
         */
        static constexpr auto MAX = std::uint_fast8_t{ 0b111'1111 };
    };

    /**
     * \brief Device address transmitted form tag.
     */
    struct Transmitted {
        /**
         * \brief The minimum supported device address in transmitted form.
         */
        static constexpr auto MIN = std::uint_fast8_t{ Numeric::MIN << 1 };

        /**
         * \brief The maximum supported device address in transmitted form.
         */
        static constexpr auto MAX = std::uint_fast8_t{ Numeric::MAX << 1 };
    };

    /**
     * \brief Device address numeric form tag.
     */
    static constexpr auto NUMERIC = Numeric{};

    /**
     * \brief Device address transmitted form tag.
     */
    static constexpr auto TRANSMITTED = Transmitted{};

    /**
     * \brief Get the minimum supported device address.
     *
     * \return The minimum supported device address.
     */
    static constexpr auto min() noexcept
    {
        return Address{ NUMERIC, Numeric::MIN };
    }

    /**
     * \brief Get the maximum supported device address.
     *
     * \return The maximum supported device address.
     */
    static constexpr auto max() noexcept
    {
        return Address{ NUMERIC, Numeric::MAX };
    }

    /**
     * \brief Constructor.
     */
    constexpr Address() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \warning This constructor does not validate its arguments. Use
     *          picolibrary::I2C::make_address( picolibrary::I2C::Address::Numeric,
     *          std::uint_fast8_t ) if argument validation is desired.
     *
     * \param[in] address The device address in numeric form.
     */
    constexpr Address( Numeric, std::uint_fast8_t address ) noexcept :
        m_address{ static_cast<std::uint8_t>( address << 1 ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \warning This constructor does not validate its arguments. Use
     *          picolibrary::I2C::make_address( picolibrary::I2C::Address::Transmitted,
     *          std::uint8_t ) if argument validation is desired.
     *
     * \param[in] address The device address in transmitted form.
     */
    constexpr Address( Transmitted, std::uint8_t address ) noexcept : m_address{ address }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Address( Address && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Address( Address const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Address() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Address && expression ) noexcept -> Address & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Address const & expression ) noexcept -> Address & = default;

    /**
     * \brief Get the device address in numeric form.
     *
     * \return The device address in numeric form.
     */
    constexpr auto numeric() const noexcept -> std::uint_fast8_t
    {
        return m_address >> 1;
    }

    /**
     * \brief Get the device address in transmitted form.
     *
     * \return The device address in transmitted form.
     */
    constexpr auto transmitted() const noexcept
    {
        return m_address;
    }

  private:
    /**
     * \brief The device address in transmitted form.
     */
    std::uint8_t m_address{};
};

/**
 * \brief Construct a picolibrary::I2C::Address.
 *
 * \relatedalso picolibrary::I2C::Address
 *
 * \param[in] address The device address in numeric form.
 *
 * \return The device address if address is less than or equal to
 *         picolibrary::I2C::Address::Numeric::MAX.
 * \return picolibrary::Generic_Error::INVALID_ARGUMENT if address is greater than
 *         picolibrary::I2C::Address::Numeric::MAX.
 */
constexpr auto make_address( Address::Numeric, std::uint_fast8_t address ) noexcept
    -> Result<Address, Error_Code>
{
    if ( address > Address::Numeric::MAX ) {
        return Generic_Error::INVALID_ARGUMENT;
    } // if

    return Address{ Address::NUMERIC, address };
}

/**
 * \brief Construct a picolibrary::I2C::Address.
 *
 * \relatedalso picolibrary::I2C::Address
 *
 * \param[in] address The device address in transmitted form.
 *
 * \return The device address if the least significant bit of address is not set.
 * \return picolibrary::Generic_Error::INVALID_ARGUMENT if the least significant bit of
 *         address is set.
 */
constexpr auto make_address( Address::Transmitted, std::uint8_t address ) noexcept
    -> Result<Address, Error_Code>
{
    if ( address & 0x01 ) {
        return Generic_Error::INVALID_ARGUMENT;
    } // if

    return Address{ Address::TRANSMITTED, address };
}

/**
 * \brief Equality operator.
 *
 * \relatedalso picolibrary::I2C::Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is equal to rhs.
 * \return false if lhs is not equal to rhs.
 */
constexpr auto operator==( Address lhs, Address rhs ) noexcept
{
    return lhs.transmitted() == rhs.transmitted();
}

/**
 * \brief Inequality operator.
 *
 * \relatedalso picolibrary::I2C::Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is not equal to rhs.
 * \return false if lhs is equal to rhs.
 */
constexpr auto operator!=( Address lhs, Address rhs ) noexcept
{
    return not( lhs == rhs );
}

/**
 * \brief Less than operator.
 *
 * \relatedalso picolibrary::I2C::Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than rhs.
 * \return false if lhs is not less than rhs.
 */
constexpr auto operator<( Address lhs, Address rhs ) noexcept
{
    return lhs.transmitted() < rhs.transmitted();
}

/**
 * \brief Greater than operator.
 *
 * \relatedalso picolibrary::I2C::Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than rhs.
 * \return false if lhs is not greater than rhs.
 */
constexpr auto operator>( Address lhs, Address rhs ) noexcept
{
    return rhs < lhs;
}

/**
 * \brief Less than or equal to operator.
 *
 * \relatedalso picolibrary::I2C::Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than or equal to rhs.
 * \return false if lhs is not less than or equal to rhs.
 */
constexpr auto operator<=( Address lhs, Address rhs ) noexcept
{
    return not( lhs > rhs );
}

/**
 * \brief Greater than or equal to operator.
 *
 * \relatedalso picolibrary::I2C::Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than or equal to rhs.
 * \return false if lhs is not greater than or equal to rhs.
 */
constexpr auto operator>=( Address lhs, Address rhs ) noexcept
{
    return not( lhs < rhs );
}

/**
 * \brief Operation.
 */
enum class Operation : std::uint8_t {
    READ  = 0b0000'0001, ///< Read.
    WRITE = 0b0000'0000, ///< Write.
};

/**
 * \brief Response.
 */
enum class Response : std::uint_fast8_t {
    ACK  = 0b0, ///< ACK.
    NACK = 0b1, ///< NACK.
};

/**
 * \brief I2C basic controller concept.
 */
class Basic_Controller_Concept {
  public:
    /**
     * \brief Constructor.
     */
    Basic_Controller_Concept() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Basic_Controller_Concept( Basic_Controller_Concept && source ) noexcept = default;

    Basic_Controller_Concept( Basic_Controller_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Basic_Controller_Concept() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator                     =( Basic_Controller_Concept && expression ) noexcept
        -> Basic_Controller_Concept & = default;

    auto operator=( Basic_Controller_Concept const & ) = delete;

    /**
     * \brief Initialize the controller's hardware.
     *
     * \return Nothing if controller hardware initialization succeeded.
     * \return An error code if controller hardware initialization failed. If controller
     *         hardware initialization cannot fail, return
     *         picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto initialize() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Transmit a start condition.
     *
     * \return Nothing if start condition transmission succeeded.
     * \return An error code if start condition transmission failed. If start condition
     *         transmission cannot fail, return picolibrary::Result<picolibrary::Void,
     *         picolibrary::Void>.
     */
    auto start() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Transmit a repeated start condition.
     *
     * \return Nothing if repeated start condition transmission succeeded.
     * \return An error code if repeated start condition transmission failed. If repeated
     *         start condition transmission cannot fail, return
     *         picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto repeated_start() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Transmit a stop condition.
     *
     * \return Nothing if stop condition transmission succeeded.
     * \return An error code if stop condition transmission failed. If stop condition
     *         transmission cannot fail, return picolibrary::Result<picolibrary::Void,
     *         picolibrary::Void>.
     */
    auto stop() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Address a device.
     *
     * \param[in] address The address of the device to address.
     * \param[in] operation The operation that will be performed once the device has been
     *            addressed.
     *
     * \return Nothing if addressing the device succeeded.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to address the device.
     * \return picolibrary::Generic_Error::NONRESPONSIVE_DEVICE if the device did not
     *         respond when addressed.
     * \return An error code if addressing the device failed for any other reason.
     */
    auto address( Address address, Operation operation ) noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Read data from a device.
     *
     * \param[in] response The response to send after the data is read.
     *
     * \return The read data if the read succeeded.
     * \return An error code if the read failed. If the read cannot fail, return
     *         picolibrary::Result<std::uint8_t, picolibrary::Void>.
     */
    auto read( Response response ) noexcept -> Result<std::uint8_t, Error_Code>;

    /**
     * \brief Write data to a device.
     *
     * \param[in] data The data to write.
     *
     * \return Nothing if the write succeeded.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration during the write.
     * \return picolibrary::Generic_Error::NONRESPONSIVE_DEVICE if the device did not
     *         acknowledge the write.
     * \return An error code if the write failed for any other reason.
     */
    auto write( std::uint8_t data ) noexcept -> Result<Void, Error_Code>;
};

/**
 * \brief I2C controller concept.
 */
class Controller_Concept {
  public:
    /**
     * \brief Constructor.
     */
    Controller_Concept() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Controller_Concept( Controller_Concept && source ) noexcept = default;

    Controller_Concept( Controller_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Controller_Concept() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Controller_Concept && expression ) noexcept -> Controller_Concept & = default;

    auto operator=( Controller_Concept const & ) = delete;

    /**
     * \brief Initialize the controller's hardware.
     *
     * \return Nothing if controller hardware initialization succeeded.
     * \return An error code if controller hardware initialization failed. If controller
     *         hardware initialization cannot fail, return
     *         picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto initialize() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Transmit a start condition.
     *
     * \return Nothing if start condition transmission succeeded.
     * \return An error code if start condition transmission failed. If start condition
     *         transmission cannot fail, return picolibrary::Result<picolibrary::Void,
     *         picolibrary::Void>.
     */
    auto start() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Transmit a repeated start condition.
     *
     * \return Nothing if repeated start condition transmission succeeded.
     * \return An error code if repeated start condition transmission failed. If repeated
     *         start condition transmission cannot fail, return
     *         picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto repeated_start() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Transmit a stop condition.
     *
     * \return Nothing if stop condition transmission succeeded.
     * \return An error code if stop condition transmission failed. If stop condition
     *         transmission cannot fail, return picolibrary::Result<picolibrary::Void,
     *         picolibrary::Void>.
     */
    auto stop() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Address a device.
     *
     * \param[in] address The address of the device to address.
     * \param[in] operation The operation that will be performed once the device has been
     *            addressed.
     *
     * \return Nothing if addressing the device succeeded.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to address the device.
     * \return picolibrary::Generic_Error::NONRESPONSIVE_DEVICE if the device did not
     *         respond when addressed.
     * \return An error code if addressing the device failed for any other reason.
     */
    auto address( Address address, Operation operation ) noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Read data from a device.
     *
     * \param[in] response The response to send after the data is read.
     *
     * \return The read data if the read succeeded.
     * \return An error code if the read failed. If the read cannot fail, return
     *         picolibrary::Result<std::uint8_t, picolibrary::Void>.
     */
    auto read( Response response ) noexcept -> Result<std::uint8_t, Error_Code>;

    /**
     * \brief Read a block of data from a device.
     *
     * \param[out] begin The beginning of the block of read data.
     * \param[out] end The end of the block of read data.
     * \param[in] response The response to send after the last byte of the block is read.
     *
     * \return Nothing if the read succeeded.
     * \return An error code if the read failed. If the read cannot fail, return
     *         picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto read( std::uint8_t * begin, std::uint8_t * end, Response response ) noexcept
        -> Result<Void, Error_Code>;

    /**
     * \brief Write data to a device.
     *
     * \param[in] data The data to write.
     *
     * \return Nothing if the write succeeded.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration during the write.
     * \return picolibrary::Generic_Error::NONRESPONSIVE_DEVICE if the device did not
     *         acknowledge the write.
     * \return An error code if the write failed for any other reason.
     */
    auto write( std::uint8_t data ) noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Write a block of data to a device.
     *
     * \param[in] begin The beginning of the block of data to write.
     * \param[in] end The end of the block of data to write.
     *
     * \return Nothing if the write succeeded.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration during the write.
     * \return picolibrary::Generic_Error::NONRESPONSIVE_DEVICE if the device did not
     *         acknowledge the write.
     * \return An error code if the write failed for any other reason.
     */
    auto write( std::uint8_t const * begin, std::uint8_t const * end ) noexcept
        -> Result<Void, Error_Code>;
};

/**
 * \brief I2C controller.
 *
 * \tparam Basic_Controller The I2C basic controller to add I2C controller functionality
 * to.
 */
template<typename Basic_Controller>
class Controller : public Basic_Controller {
  public:
    using Basic_Controller::Basic_Controller;

    using Basic_Controller::read;

    /**
     * \brief Read a block of data from a device.
     *
     * \param[out] begin The beginning of the block of read data.
     * \param[out] end The end of the block of read data.
     * \param[in] response The response to send after the last byte of the block is read.
     *
     * \return Nothing if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read( std::uint8_t * begin, std::uint8_t * end, Response response ) noexcept
    {
        return generate( begin, end, [ this, &begin, end, response ]() noexcept {
            return read( ++begin == end ? response : Response::ACK );
        } );
    }

    using Basic_Controller::write;

    /**
     * \brief Write a block of data to a device.
     *
     * \param[in] begin The beginning of the block of data to write.
     * \param[in] end The end of the block of data to write.
     *
     * \return Nothing if the write succeeded.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration during the write.
     * \return picolibrary::Generic_Error::NONRESPONSIVE_DEVICE if the device did not
     *         acknowledge the write.
     * \return An error code if the write failed for any other reason.
     */
    auto write( std::uint8_t const * begin, std::uint8_t const * end ) noexcept
    {
        return for_each<Discard_Functor>(
            begin, end, [ this ]( auto data ) noexcept { return write( data ); } );
    }
};

/**
 * \brief RAII I2C bus control guard.
 *
 * \tparam[in] Controller The type of I2C controller that is used to interact with the
 *             bus.
 *
 * \warning Stop condition transmission failure is treated as a fatal error.
 */
template<typename Controller>
class Bus_Control_Guard;

/**
 * \brief Transmit a start condition and construct a picolibrary::I2C::Bus_Control_Guard.
 *
 * \relatedalso picolibrary::I2C::Bus_Selection_Guard
 *
 * \tparam[in] Controller The type of I2C controller used to interact with the bus.
 *
 * \param[in] controller The I2C controller used to interact with the bus.
 *
 * \return The constructed picolibrary::I2C::Bus_Control_Guard if start condition
 *         transmission succeeded.
 * \return The error reported by the I2C controller if start condition transmission
 *         failed.
 */
template<typename Controller>
auto make_bus_control_guard( Controller & controller ) noexcept
    -> Result<Bus_Control_Guard<Controller>, typename decltype( std::declval<Controller>().start() )::Error, false>;

template<typename Controller>
class Bus_Control_Guard {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Bus_Control_Guard() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Bus_Control_Guard( Bus_Control_Guard && source ) noexcept :
        m_controller{ source.m_controller }
    {
        source.m_controller = nullptr;
    }

    Bus_Control_Guard( Bus_Control_Guard const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Bus_Control_Guard() noexcept
    {
        stop();
    }

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto & operator=( Bus_Control_Guard && expression ) noexcept
    {
        if ( &expression != this ) {
            stop();

            m_controller = expression.m_controller;

            expression.m_controller = nullptr;
        } // if

        return *this;
    }

    auto operator=( Bus_Control_Guard const & ) = delete;

    /**
     * \brief Transmit a repeated start condition.
     *
     * \return Nothing if repeated start condition transmission succeeded.
     * \return The error reported by the I2C controller if repeated start condition
     *         transmission failed.
     */
    auto repeated_start() noexcept
    {
        return m_controller->repeated_start();
    }

  private:
    /**
     * \brief The I2C controller used to interact with the bus.
     */
    Controller * m_controller{};

    friend auto make_bus_control_guard<Controller>( Controller & controller ) noexcept -> Result<
        Bus_Control_Guard<Controller>,
        typename decltype( std::declval<Controller>().start() )::Error,
        false>;

    /**
     * \brief Constructor.
     *
     * \param[in] controller The I2C controller used to interact with the bus.
     */
    constexpr Bus_Control_Guard( Controller & controller ) noexcept :
        m_controller{ &controller }
    {
    }

    /**
     * \brief Transmit a stop condition.
     */
    constexpr void stop() noexcept
    {
        if ( m_controller ) {
            auto result = m_controller->stop();
            if ( result.is_error() ) {
                trap_fatal_error();
            } // if
        }     // if
    }
};

template<typename Controller>
auto make_bus_control_guard( Controller & controller ) noexcept
    -> Result<Bus_Control_Guard<Controller>, typename decltype( std::declval<Controller>().start() )::Error, false>
{
    auto result = controller.start();
    if ( result.is_error() ) {
        return result.error();
    } // if

    return Bus_Control_Guard{ controller };
}

/**
 * \brief Check if a device is responsive.
 *
 * \tparam Controller The type of I2C controller used to communicate with the device.
 *
 * \param[in] controller The I2C controller to use to communicate with the device.
 * \param[in] address The address of the device.
 * \param[in] operation The operation to request when addressing the device.
 *
 * \return Nothing if the device is responsive.
 * \return picolibrary::Generic_Error::NONRESPONSIVE_DEVICE if the device is not
 *         responsive.
 * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost arbitration
 *         while attempting to communicate with the device.
 * \return An error code if the check failed for any other reason.
 */
template<typename Controller>
auto ping( Controller & controller, Address address, Operation operation ) noexcept
    -> Result<Void, Error_Code>
{
    auto guard = Bus_Control_Guard<Controller>{};
    {
        auto result = make_bus_control_guard( controller );
        if ( result.is_error() ) {
            return result.error();
        } // if

        guard = std::move( result ).value();
    }

    {
        auto result = controller.address( address, operation );
        if ( result.is_error() ) {
            return result.error();
        } // if
    }

    if ( operation == Operation::READ ) {
        auto result = controller.read( Response::NACK );
        if ( result.is_error() ) {
            return result.error();
        } // if
    }     // if

    return {};
}

/**
 * \brief Scan a bus for responsive devices.
 *
 * \tparam Controller The type of I2C controller used to interact with the bus.
 * \tparam Functor A binary functor that takes a device address / operation pair for which
 *         a responsive device was found, and returns either
 *         picolibrary::Result<picolibrary::Void, picolibrary::Error_Code> or
 *         picolibrary::Result<picolibrary::Void, picolibrary::Void>. If an error is
 *         returned by the functor, the scan halts and the error is returned.
 *
 * \param[in] controller The I2C controller for the bus to be scanned.
 * \param[in] functor The functor to pass the device address / operation pairs for which
 *            responsive devices are found.
 *
 * \return Nothing if the scan succeeded.
 * \return An error code if the scan failed.
 */
template<typename Controller, typename Functor>
auto scan( Controller & controller, Functor functor ) noexcept -> Result<Void, Error_Code>
{
    Operation const operations[]{
        Operation::READ,
        Operation::WRITE,
    };

    for ( auto numeric_address = Address::Numeric::MIN; numeric_address <= Address::Numeric::MAX;
          ++numeric_address ) {
        auto const address = Address{ Address::NUMERIC, numeric_address };

        for ( auto const operation : operations ) {
            {
                auto result = ping( controller, address, operation );
                if ( result.is_error() ) {
                    if ( result.error() != Generic_Error::NONRESPONSIVE_DEVICE ) {
                        return result.error();
                    } // if

                    continue;
                } // if
            }

            auto result = functor( address, operation );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }     // for
    }         // for

    return {};
}

/**
 * \brief I2C device.
 *
 * \tparam Bus_Multiplexer_Aligner A nullary functor that returns either
 *         picolibrary::Result<Void, Error_Code> or picolibrary::Result<Void, Void>. The
 *         functor must be default constructable, move constructable, and move assignable.
 *         When called, this functor should align the I2C bus's multiplexer(s) (if any) to
 *         enable communication with the device.
 * \tparam Controller The type of I2C controller used to interact with the bus the device
 *         is attached to.
 * \tparam Register_Address The integral type used to hold device register addresses.
 */
template<typename Bus_Multiplexer_Aligner, typename Controller, typename Register_Address>
class Device;

/**
 * \brief 8-bit register address space I2C device.
 *
 * \tparam Bus_Multiplexer_Aligner A nullary functor that returns either
 *         picolibrary::Result<Void, Error_Code> or picolibrary::Result<Void, Void>. The
 *         functor must be default constructable, move constructable, and move assignable.
 *         When called, this functor should align the I2C bus's multiplexer(s) (if any) to
 *         enable communication with the device.
 * \tparam Controller The type of I2C controller used to interact with the bus the device
 *         is attached to.
 */
template<typename Bus_Multiplexer_Aligner, typename Controller>
class Device<Bus_Multiplexer_Aligner, Controller, std::uint8_t> {
  public:
    Device( Device const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Device() noexcept = default;

    auto operator=( Device const & ) = delete;

    /**
     * \brief Get the device's address.
     *
     * \return The device's address.
     */
    constexpr auto address() const noexcept
    {
        return m_address;
    }

    /**
     * \brief Get the error code that is returned when the device does not respond when
     *        addressed, or does does not acknowledge a write.
     *
     * \return The error code that is returned when the device does not respond when
     *         addressed, or does does not acknowledge a write.
     */
    constexpr auto const & nonresponsive_device_error() const noexcept
    {
        return m_nonresponsive_device_error;
    }

    /**
     * \brief Check if the device is responsive.
     *
     * \param[in] operation The operation to request when addressing the device.
     *
     * \return Nothing if the device is responsive.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the device is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the device.
     * \return An error code if the check failed for any other reason.
     */
    auto ping( Operation operation ) const noexcept -> Result<Void, Error_Code>
    {
        {
            auto result = m_align_bus_multiplexer();
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        {
            auto result = I2C::ping( *m_controller, m_address, operation );
            if ( result.is_error() ) {
                if ( result.error() == Generic_Error::NONRESPONSIVE_DEVICE ) {
                    return m_nonresponsive_device_error;
                } // if

                return result.error();
            } // if
        }

        return {};
    }

    /**
     * \brief Check if the device is responsive.
     *
     * \return Nothing if the device is responsive.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the device is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the device.
     * \return An error code if the check failed for any other reason.
     */
    auto ping() const noexcept -> Result<Void, Error_Code>
    {
        Operation const operations[]{
            Operation::READ,
            Operation::WRITE,
        };

        for ( auto const operation : operations ) {
            auto result = ping( operation );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }     // for

        return {};
    }

  protected:
    /**
     * \brief Constructor.
     */
    constexpr Device() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] bus_multiplexer_aligner The device's bus multiplexer aligner.
     * \param[in] controller The I2C controller used to interact with the bus the device
     *            is attached to.
     * \param[in] address The device's address.
     * \param[in] nonresponsive_device_error The error code to return when the device does
     *            not respond when addressed, or does not acknowledge a write.
     */
    constexpr Device(
        Bus_Multiplexer_Aligner bus_multiplexer_aligner,
        Controller &            controller,
        Address                 address,
        Error_Code const &      nonresponsive_device_error ) noexcept :
        m_align_bus_multiplexer{ std::move( bus_multiplexer_aligner ) },
        m_controller{ &controller },
        m_address{ address },
        m_nonresponsive_device_error{ nonresponsive_device_error }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Device( Device && source ) noexcept :
        m_align_bus_multiplexer{ std::move( source.m_align_bus_multiplexer ) },
        m_controller{ source.m_controller },
        m_address{ source.m_address },
        m_nonresponsive_device_error{ source.m_nonresponsive_device_error }
    {
        source.m_controller = nullptr;
    }

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto & operator=( Device && expression ) noexcept
    {
        if ( &expression != this ) {
            m_align_bus_multiplexer = std::move( expression.m_align_bus_multiplexer );
            m_controller            = expression.m_controller;
            m_address               = expression.m_address;
            m_nonresponsive_device_error = expression.m_nonresponsive_device_error;

            expression.m_controller = nullptr;
        } // if

        return *this;
    }

    /**
     * \brief Change the device's address.
     *
     * \param[in] address The device's new address.
     */
    void change_address( Address address ) noexcept
    {
        m_address = address;
    }

    /**
     * \brief Align the I2C bus's multiplexer(s) (if any) to enable communication with the
     *        device.
     *
     * \return Nothing if alignment succeeded.
     * \return An error code if alignment failed.
     */
    auto align_bus_multiplexer() const noexcept
    {
        return m_align_bus_multiplexer();
    }

    /**
     * \brief Get the I2C controller used to interact with the bus the device is attached
     *        to.
     *
     * \return The I2C controller used to interact with the bus the device is attached to.
     */
    auto & controller() const noexcept
    {
        return *m_controller;
    }

    /**
     * \brief Read a register.
     *
     * \param[in] register_address The address of the register to read.
     *
     * \return The data read from the register if the read succeeded.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the device is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the device.
     * \return An error code if the read failed for any other reason.
     */
    auto read( std::uint8_t register_address ) const noexcept -> Result<std::uint8_t, Error_Code>
    {
        {
            auto result = m_align_bus_multiplexer();
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        auto guard = Bus_Control_Guard<Controller>{};
        {
            auto result = make_bus_control_guard( *m_controller );
            if ( result.is_error() ) {
                return result.error();
            } // if

            guard = std::move( result ).value();
        }

        {
            auto result = m_controller->address( m_address, Operation::WRITE );
            if ( result.is_error() ) {
                if ( result.error() == Generic_Error::NONRESPONSIVE_DEVICE ) {
                    return m_nonresponsive_device_error;
                } // if

                return result.error();
            } // if
        }

        {
            auto result = m_controller->write( register_address );
            if ( result.is_error() ) {
                if ( result.error() == Generic_Error::NONRESPONSIVE_DEVICE ) {
                    return m_nonresponsive_device_error;
                } // if

                return result.error();
            } // if
        }

        {
            auto result = guard.repeated_start();
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        {
            auto result = m_controller->address( m_address, Operation::READ );
            if ( result.is_error() ) {
                if ( result.error() == Generic_Error::NONRESPONSIVE_DEVICE ) {
                    return m_nonresponsive_device_error;
                } // if

                return result.error();
            } // if
        }

        {
            return m_controller->read( Response::NACK );
        }
    }

    /**
     * \brief Read a block of registers.
     *
     * \param[in] register_address The address of the block of registers to read.
     * \param[in] begin The beginning of the data read from the block of registers.
     * \param[in] end The end of the data read from the block of registers.
     *
     * \warning This function does not verify that the register block size is non-zero. If
     *          the register block size is zero, a NACK terminated read will never be
     *          performed which results in the device retaining control of the SDA signal,
     *          locking up the bus.
     *
     * \return Nothing if the read succeeded.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the device is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the device.
     * \return An error code if the read failed for any other reason.
     */
    auto read( std::uint8_t register_address, std::uint8_t * begin, std::uint8_t * end ) const noexcept
        -> Result<Void, Error_Code>
    {
        {
            auto result = m_align_bus_multiplexer();
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        auto guard = Bus_Control_Guard<Controller>{};
        {
            auto result = make_bus_control_guard( *m_controller );
            if ( result.is_error() ) {
                return result.error();
            } // if

            guard = std::move( result ).value();
        }

        {
            auto result = m_controller->address( m_address, Operation::WRITE );
            if ( result.is_error() ) {
                if ( result.error() == Generic_Error::NONRESPONSIVE_DEVICE ) {
                    return m_nonresponsive_device_error;
                } // if

                return result.error();
            } // if
        }

        {
            auto result = m_controller->write( register_address );
            if ( result.is_error() ) {
                if ( result.error() == Generic_Error::NONRESPONSIVE_DEVICE ) {
                    return m_nonresponsive_device_error;
                } // if

                return result.error();
            } // if
        }

        {
            auto result = guard.repeated_start();
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        {
            auto result = m_controller->address( m_address, Operation::READ );
            if ( result.is_error() ) {
                if ( result.error() == Generic_Error::NONRESPONSIVE_DEVICE ) {
                    return m_nonresponsive_device_error;
                } // if

                return result.error();
            } // if
        }

        {
            return m_controller->read( begin, end, Response::NACK );
        }
    }

    /**
     * \brief Write to a register.
     *
     * \param[in] register_address The address of the register to write to.
     * \param[in] data The data to write to the register.
     *
     * \return Nothing if the write succeeded.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the device is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the device.
     * \return An error code if the read failed for any other reason.
     */
    auto write( std::uint8_t register_address, std::uint8_t data ) noexcept -> Result<Void, Error_Code>
    {
        {
            auto result = m_align_bus_multiplexer();
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        auto guard = Bus_Control_Guard<Controller>{};
        {
            auto result = make_bus_control_guard( *m_controller );
            if ( result.is_error() ) {
                return result.error();
            } // if

            guard = std::move( result ).value();
        }

        {
            auto result = m_controller->address( m_address, Operation::WRITE );
            if ( result.is_error() ) {
                if ( result.error() == Generic_Error::NONRESPONSIVE_DEVICE ) {
                    return m_nonresponsive_device_error;
                } // if

                return result.error();
            } // if
        }

        {
            auto result = m_controller->write( register_address );
            if ( result.is_error() ) {
                if ( result.error() == Generic_Error::NONRESPONSIVE_DEVICE ) {
                    return m_nonresponsive_device_error;
                } // if

                return result.error();
            } // if
        }

        {
            auto result = m_controller->write( data );
            if ( result.is_error() ) {
                if ( result.error() == Generic_Error::NONRESPONSIVE_DEVICE ) {
                    return m_nonresponsive_device_error;
                } // if

                return result.error();
            } // if
        }

        return {};
    }

    /**
     * \brief Write to a block of registers.
     *
     * \param[in] register_address The address of the block of registers to write to.
     * \param[in] begin The beginning of the data to write to the block of registers.
     * \param[in] end The end of the data to write to the block of registers.
     *
     * \return Nothing if the write succeeded.
     * \return picolibrary::I2C::Device<Bus_Multiplexer_Aligner, Controller,
     *         std::uint8_t>::nonresponsive_device_error() if the device is not
     *         responsive.
     * \return picolibrary::Generic_Error::ARBITRATION_LOST if the controller lost
     *         arbitration while attempting to communicate with the device.
     * \return An error code if the read failed for any other reason.
     */
    auto write( std::uint8_t register_address, std::uint8_t const * begin, std::uint8_t const * end ) noexcept
        -> Result<Void, Error_Code>
    {
        {
            auto result = m_align_bus_multiplexer();
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        auto guard = Bus_Control_Guard<Controller>{};
        {
            auto result = make_bus_control_guard( *m_controller );
            if ( result.is_error() ) {
                return result.error();
            } // if

            guard = std::move( result ).value();
        }

        {
            auto result = m_controller->address( m_address, Operation::WRITE );
            if ( result.is_error() ) {
                if ( result.error() == Generic_Error::NONRESPONSIVE_DEVICE ) {
                    return m_nonresponsive_device_error;
                } // if

                return result.error();
            } // if
        }

        {
            auto result = m_controller->write( register_address );
            if ( result.is_error() ) {
                if ( result.error() == Generic_Error::NONRESPONSIVE_DEVICE ) {
                    return m_nonresponsive_device_error;
                } // if

                return result.error();
            } // if
        }

        {
            auto result = m_controller->write( begin, end );
            if ( result.is_error() ) {
                if ( result.error() == Generic_Error::NONRESPONSIVE_DEVICE ) {
                    return m_nonresponsive_device_error;
                } // if

                return result.error();
            } // if
        }

        return {};
    }

  private:
    /**
     * \brief Align the I2C bus's multiplexer(s) (if any) to enable communication with the
     *        device.
     */
    Bus_Multiplexer_Aligner m_align_bus_multiplexer{};

    /**
     * \brief The I2C controller used to interact with the bus the device is attached to.
     */
    Controller * m_controller{};

    /**
     * \brief The device's address.
     */
    Address m_address{};

    /**
     * \brief The error code that is returned when the device does not respond when
     *        addressed, or does does not acknowledge a write.
     */
    Error_Code m_nonresponsive_device_error{};
};

} // namespace picolibrary::I2C

#endif // PICOLIBRARY_I2C_H

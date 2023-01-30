/**
 * picolibrary
 *
 * Copyright 2020-2023, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
#include "picolibrary/precondition.h"
#include "picolibrary/result.h"

/**
 * \brief Inter-Integrated Circuit (I2C) facilities.
 */
namespace picolibrary::I2C {

/**
 * \brief Device address, numeric format.
 */
class Address_Numeric;

/**
 * \brief Device address, transmitted format.
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
        return Address_Numeric{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, 0b0000000 };
    }

    /**
     * \brief Get the maximum valid address.
     *
     * \return The maximum valid address.
     */
    static constexpr auto max() noexcept -> Address_Numeric
    {
        return Address_Numeric{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, 0b1111111 };
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
     * \pre address <= picolibrary::I2C::Numeric_Address::max().as_unsigned_integer()
     */
    constexpr Address_Numeric( Unsigned_Integer address ) noexcept : m_address{ address }
    {
        PICOLIBRARY_EXPECT( address <= max().as_unsigned_integer(), Generic_Error::INVALID_ARGUMENT );
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
    Unsigned_Integer m_address{};
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
        return Address_Transmitted{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, 0b0000000'0 };
    }

    /**
     * \brief Get the maximum valid address.
     *
     * \return The maximum valid address.
     */
    static constexpr auto max() noexcept -> Address_Transmitted
    {
        return Address_Transmitted{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, 0b1111111'0 };
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
     * \pre not ( address & 0b1 )
     */
    constexpr Address_Transmitted( Unsigned_Integer address ) noexcept :
        m_address{ address }
    {
        PICOLIBRARY_EXPECT( not( address & 0b1 ), Generic_Error::INVALID_ARGUMENT );
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
    Unsigned_Integer m_address{};
};

constexpr Address_Numeric::Address_Numeric( Address_Transmitted address ) noexcept :
    m_address{ static_cast<Unsigned_Integer>( address.as_unsigned_integer() >> 1 ) }
{
}

/**
 * \brief Equality operator.
 *
 * \relatedalso picolibrary::I2C::Address_Numeric
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
 * \relatedalso picolibrary::I2C::Address_Numeric
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
 * \relatedalso picolibrary::I2C::Address_Numeric
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
 * \relatedalso picolibrary::I2C::Address_Numeric
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
 * \relatedalso picolibrary::I2C::Address_Numeric
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
 * \relatedalso picolibrary::I2C::Address_Numeric
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
 * \relatedalso picolibrary::I2C::Address_Transmitted
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
 * \relatedalso picolibrary::I2C::Address_Transmitted
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
 * \relatedalso picolibrary::I2C::Address_Transmitted
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
 * \relatedalso picolibrary::I2C::Address_Transmitted
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
 * \relatedalso picolibrary::I2C::Address_Transmitted
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
 * \relatedalso picolibrary::I2C::Address_Transmitted
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
 * \brief Operation.
 */
enum class Operation : std::uint8_t {
    READ  = 0b1, ///< Read.
    WRITE = 0b0, ///< Write.
};

/**
 * \brief Response.
 */
enum class Response : std::uint_fast8_t {
    ACK  = 0b0, ///< ACK.
    NACK = 0b1, ///< NACK.
};

/**
 * \brief Basic controller concept.
 */
class Basic_Controller_Concept {
  public:
    /**
     * \brief Constructor.
     */
    Basic_Controller_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Basic_Controller_Concept( Basic_Controller_Concept && source ) noexcept;

    Basic_Controller_Concept( Basic_Controller_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Basic_Controller_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Basic_Controller_Concept && expression ) noexcept -> Basic_Controller_Concept &;

    auto operator=( Basic_Controller_Concept const & ) = delete;

    /**
     * \brief Initialize the controller's hardware.
     */
    void initialize() noexcept;

    /**
     * \brief Check if a bus error is present.
     *
     * \return true if a bus error is present.
     * \return false if a bus error is not present.
     */
    auto bus_error_present() const noexcept -> bool;

    /**
     * \brief Transmit a start condition.
     */
    void start() noexcept;

    /**
     * \brief Transmit a repeated start condition.
     */
    void repeated_start() noexcept;

    /**
     * \brief Transmit a stop condition.
     */
    void stop() noexcept;

    /**
     * \brief Address a device.
     *
     * \param[in] address The address of the device to address.
     * \param[in] operation The operation that will be performed once the device has been
     *            addressed.
     *
     * \return picolibrary::I2C::Response::ACK if an ACK response is received.
     * \return picolibrary::I2C::Response::NACK if a NACK response is received.
     */
    auto address( Address_Transmitted address, Operation operation ) noexcept -> Response;

    /**
     * \brief Read data from a device.
     *
     * \param[in] response The response to transmit once the data has been read.
     *
     * \return The data read from the device.
     */
    auto read( Response response ) noexcept -> std::uint8_t;

    /**
     * \brief Write data to a device.
     *
     * \param[in] data The data to write to the device.
     *
     * \return picolibrary::I2C::Response::ACK if an ACK response is received.
     * \return picolibrary::I2C::Response::NACK if a NACK response is received.
     */
    auto write( std::uint8_t data ) noexcept -> Response;
};

/**
 * \brief Controller concept.
 */
class Controller_Concept {
  public:
    /**
     * \brief Constructor.
     */
    Controller_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Controller_Concept( Controller_Concept && source ) noexcept;

    Controller_Concept( Controller_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Controller_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Controller_Concept && expression ) noexcept -> Controller_Concept &;

    auto operator=( Controller_Concept const & ) = delete;

    /**
     * \brief Initialize the controller's hardware.
     */
    void initialize() noexcept;

    /**
     * \brief Check if a bus error is present.
     *
     * \return true if a bus error is present.
     * \return false if a bus error is not present.
     */
    auto bus_error_present() const noexcept -> bool;

    /**
     * \brief Transmit a start condition.
     */
    void start() noexcept;

    /**
     * \brief Transmit a repeated start condition.
     */
    void repeated_start() noexcept;

    /**
     * \brief Transmit a stop condition.
     */
    void stop() noexcept;

    /**
     * \brief Address a device.
     *
     * \param[in] address The address of the device to address.
     * \param[in] operation The operation that will be performed once the device has been
     *            addressed.
     *
     * \return picolibrary::I2C::Response::ACK if an ACK response is received.
     * \return picolibrary::I2C::Response::NACK if a NACK response is received.
     */
    auto address( Address_Transmitted address, Operation operation ) noexcept -> Response;

    /**
     * \brief Read data from a device.
     *
     * \param[in] response The response to transmit once the data has been read.
     *
     * \return The data read from the device.
     */
    auto read( Response response ) noexcept -> std::uint8_t;

    /**
     * \brief Read a block of data from a device.
     *
     * \param[out] begin The beginning of the block of read data.
     * \param[out] end The end of the block of read data.
     * \param[in] response The response to transmit once the last byte in the block has
     *            been read.
     */
    void read( std::uint8_t * begin, std::uint8_t * end, Response response ) noexcept;

    /**
     * \brief Write data to a device.
     *
     * \param[in] data The data to write to the device.
     *
     * \return picolibrary::I2C::Response::ACK if an ACK response is received.
     * \return picolibrary::I2C::Response::NACK if a NACK response is received.
     */
    auto write( std::uint8_t data ) noexcept -> Response;

    /**
     * \brief Write a block of data to a device.
     *
     * \param[in] begin The beginning of the block of data to write.
     * \param[in] end The end of the block of data to write.
     *
     * \return picolibrary::I2C::Response::ACK if an ACK response is received for every
     *         byte written.
     * \return picolibrary::I2C::Response::NACK if a NACK response is received.
     */
    auto write( std::uint8_t const * begin, std::uint8_t const * end ) noexcept -> Response;
};

/**
 * \brief Controller.
 *
 * \tparam Basic_Controller The basic controller to add controller functionality to.
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
     * \param[in] response The response to transmit once the last byte in the block has
     *            been read.
     */
    void read( std::uint8_t * begin, std::uint8_t * end, Response response ) noexcept
    {
        for ( ; begin != end; ++begin ) {
            *begin = read( begin + 1 == end ? response : Response::ACK );
        } // for
    }

    using Basic_Controller::write;

    /**
     * \brief Write a block of data to a device.
     *
     * \param[in] begin The beginning of the block of data to write.
     * \param[in] end The end of the block of data to write.
     *
     * \return picolibrary::I2C::Response::ACK if an ACK response is received for every
     *         byte written.
     * \return picolibrary::I2C::Response::NACK if a NACK response is received.
     */
    auto write( std::uint8_t const * begin, std::uint8_t const * end ) noexcept -> Response
    {
        for ( ; begin != end; ++begin ) {
            if ( write( *begin ) != Response::ACK ) {
                return Response::NACK;
            } // if
        }     // for

        return Response::ACK;
    }
};

/**
 * \brief RAII bus control guard.
 *
 * \tparam Controller The type of controller that is being used to interact with the bus.
 */
template<typename Controller>
class Bus_Control_Guard {
  public:
    Bus_Control_Guard() = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] controller The controller that is being used to interact with the bus.
     */
    Bus_Control_Guard( Controller & controller ) noexcept : m_controller{ controller }
    {
        m_controller.start();
    }

    Bus_Control_Guard( Bus_Control_Guard && ) = delete;

    Bus_Control_Guard( Bus_Control_Guard const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Bus_Control_Guard() noexcept
    {
        m_controller.stop();
    }

    auto operator=( Bus_Control_Guard && ) = delete;

    auto operator=( Bus_Control_Guard const & ) = delete;

  private:
    /**
     * \brief The controller that is being used to interact with the bus.
     */
    Controller & m_controller;
};

/**
 * \brief Check if a device is responsive.
 *
 * \tparam Controller The type of controller used to communicate with the device.
 *
 * \param[in] controller The controller to use to communicate with the device.
 * \param[in] address The device's address.
 * \param[in] operation The operation to request when addressing the device.
 *
 * \return picolibrary::I2C::Response::ACK if the device is responsive.
 * \return picolibrary::I2C::Response::NACK if the device is not responsive.
 */
template<typename Controller>
auto ping( Controller & controller, Address_Transmitted address, Operation operation ) noexcept
    -> Response
{
    auto const guard = Bus_Control_Guard{ controller };

    auto const response = controller.address( address, operation );

    if ( operation == Operation::READ and response == Response::ACK ) {
        controller.read( Response::NACK );
    } // if

    return response;
}

/**
 * \brief Check if a device is responsive.
 *
 * \tparam Controller The type of controller used to communicate with the device.
 *
 * \param[in] controller The controller to use to communicate with the device.
 * \param[in] address The device's address.
 *
 * \return picolibrary::I2C::Response::ACK if the device is responsive.
 * \return picolibrary::I2C::Response::NACK if the device is not responsive.
 */
template<typename Controller>
auto ping( Controller & controller, Address_Transmitted address ) noexcept -> Response
{
    auto const response_read  = ping( controller, address, Operation::READ );
    auto const response_write = ping( controller, address, Operation::WRITE );

    return response_read == Response::ACK and response_write == Response::ACK ? Response::ACK
                                                                              : Response::NACK;
}

/**
 * \brief Scan a bus.
 *
 * \tparam Controller The type of controller used to communicate with devices on the bus.
 * \tparam Functor A trinary functor that takes a device address and operation pair, and
 *         the ping response for the device address and operation pair.
 *
 * \param[in] controller The controller used to communicate with devices on the bus.
 * \param[in] functor The functor to pass scan results to.
 *
 * \return The functor.
 */
template<typename Controller, typename Functor>
auto scan( Controller & controller, Functor functor ) noexcept -> Functor
{
    Operation const operations[]{
        Operation::READ,
        Operation::WRITE,
    };

    for ( auto address_numeric = Address_Numeric::min().as_unsigned_integer();
          address_numeric <= Address_Numeric::max().as_unsigned_integer();
          ++address_numeric ) {
        auto const address_transmitted = Address_Transmitted{ Address_Numeric{ address_numeric } };

        for ( auto const operation : operations ) {
            functor( address_transmitted, operation, ping( controller, address_transmitted, operation ) );
        } // for
    }     // for

    return functor;
}

/**
 * \brief Scan a bus.
 *
 * \tparam Controller The type of controller used to communicate with devices on the bus.
 * \tparam Functor A trinary functor that takes a device address and operation pair, and
 *         the ping response for the device address and operation pair; and returns
 *         picolibrary::Result<void>. If an error is returned by the functor, the scan
 *         will halt and the error is returned.
 *
 * \param[in] controller The controller used to communicate with devices on the bus.
 * \param[in] functor The functor to pass scan results to.
 *
 * \return The functor if the functor does not encounter any errors during the scan.
 * \return An error code if the functor does encounter an error during the scan.
 */
template<typename Controller, typename Functor>
auto scan( Controller & controller, Functor functor, Functor_Can_Fail_Return_Functor ) noexcept
    -> Result<Functor>
{
    Operation const operations[]{
        Operation::READ,
        Operation::WRITE,
    };

    for ( auto address_numeric = Address_Numeric::min().as_unsigned_integer();
          address_numeric <= Address_Numeric::max().as_unsigned_integer();
          ++address_numeric ) {
        auto const address_transmitted = Address_Transmitted{ Address_Numeric{ address_numeric } };

        for ( auto const operation : operations ) {
            auto result = functor(
                address_transmitted, operation, ping( controller, address_transmitted, operation ) );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }     // for
    }         // for

    return functor;
}

/**
 * \brief Scan a bus.
 *
 * \tparam Controller The type of controller used to communicate with devices on the bus.
 * \tparam Functor A trinary functor that takes a device address and operation pair, and
 *         the ping response for the device address and operation pair; and returns
 *         picolibrary::Result<void>. If an error is returned by the functor, the scan
 *         will halt and the error is returned.
 *
 * \param[in] controller The controller used to communicate with devices on the bus.
 * \param[in] functor The functor to pass scan results to.
 *
 * \return Nothing if the functor does not encounter any errors during the scan.
 * \return An error code if the functor does encounter an error during the scan.
 */
template<typename Controller, typename Functor>
auto scan( Controller & controller, Functor functor, Functor_Can_Fail_Discard_Functor ) noexcept
    -> Result<void>
{
    Operation const operations[]{
        Operation::READ,
        Operation::WRITE,
    };

    for ( auto address_numeric = Address_Numeric::min().as_unsigned_integer();
          address_numeric <= Address_Numeric::max().as_unsigned_integer();
          ++address_numeric ) {
        auto const address_transmitted = Address_Transmitted{ Address_Numeric{ address_numeric } };

        for ( auto const operation : operations ) {
            auto result = functor(
                address_transmitted, operation, ping( controller, address_transmitted, operation ) );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }     // for
    }         // for

    return {};
}

/**
 * \brief Scan a bus.
 *
 * \tparam Policy The algorithm policy to use
 *         (picolibrary::Functor_Can_Fail_Return_Functor or
 *         picolibrary::Functor_Can_Fail_Discard_Functor).
 * \tparam Controller The type of controller used to communicate with devices on the bus.
 * \tparam Functor A trinary functor that takes a device address and operation pair, and
 *         the ping response for the device address and operation pair; and returns
 *         picolibrary::Result<void>. If an error is returned by the functor, the scan
 *         will halt and the error is returned.
 *
 * \param[in] controller The controller used to communicate with devices on the bus.
 * \param[in] functor The functor to pass scan results to.
 *
 * \return The functor if Policy is picolibrary::Functor_Can_Fail_Return_Functor and the
 *         functor does not encounter any errors during the scan.
 * \return Nothing if Policy is picolibrary::Functor_Can_Fail_Discard_Functor and the
 *         functor does not encounter any errors during the scan.
 * \return An error code if the functor does encounter an error during the scan.
 */
template<typename Policy, typename Controller, typename Functor>
auto scan( Controller & controller, Functor functor ) noexcept
{
    return scan( controller, std::move( functor ), Policy{} );
}

/**
 * \brief Device address, numeric format.
 *
 * \tparam MIN The minimum valid address.
 * \tparam MAX The maximum valid address.
 */
template<Address_Numeric::Unsigned_Integer MIN, Address_Numeric::Unsigned_Integer MAX>
class Device_Address_Numeric;

/**
 * \brief Device address, transmitted format.
 *
 * \tparam MIN The minimum valid address.
 * \tparam MAX The maximum valid address.
 */
template<Address_Transmitted::Unsigned_Integer MIN, Address_Transmitted::Unsigned_Integer MAX>
class Device_Address_Transmitted;

template<Address_Numeric::Unsigned_Integer MIN, Address_Numeric::Unsigned_Integer MAX>
class Device_Address_Numeric : public Address_Numeric {
  public:
    static_assert( MIN >= Address_Numeric::min().as_unsigned_integer() );

    static_assert( MAX <= Address_Numeric::max().as_unsigned_integer() );

    static_assert( MIN <= MAX );

    /**
     * \brief Get the minimum valid address.
     *
     * \return The minimum valid address.
     */
    static constexpr auto min() noexcept -> Device_Address_Numeric
    {
        return Device_Address_Numeric{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, MIN };
    }

    /**
     * \brief Get the maximum valid address.
     *
     * \return The maximum valid address.
     */
    static constexpr auto max() noexcept -> Device_Address_Numeric
    {
        return Device_Address_Numeric{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, MAX };
    }

    /**
     * \brief Constructor.
     */
    constexpr Device_Address_Numeric() noexcept :
        Address_Numeric{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, MIN }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] address The address.
     *
     * \pre address >= picolibrary::I2C::Device_Address_Numeric::min().as_unsigned_integer()
     * \pre address <= picolibrary::I2C::Device_Address_Numeric::max().as_unsigned_integer()
     */
    constexpr Device_Address_Numeric( Unsigned_Integer address ) noexcept :
        Address_Numeric{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, address }
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
    constexpr Device_Address_Numeric( Bypass_Precondition_Expectation_Checks, Unsigned_Integer address ) noexcept
        :
        Address_Numeric{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, address }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] address The address.
     */
    constexpr Device_Address_Numeric( Device_Address_Transmitted<( MIN << 1 ), ( MAX << 1 )> address ) noexcept :
        Address_Numeric{ address }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Device_Address_Numeric( Device_Address_Numeric && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Device_Address_Numeric( Device_Address_Numeric const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Device_Address_Numeric() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator         =( Device_Address_Numeric && expression ) noexcept
        -> Device_Address_Numeric & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Device_Address_Numeric const & expression ) noexcept
        -> Device_Address_Numeric & = default;
};

template<Address_Transmitted::Unsigned_Integer MIN, Address_Transmitted::Unsigned_Integer MAX>
class Device_Address_Transmitted : public Address_Transmitted {
  public:
    static_assert( MIN >= Address_Numeric::min().as_unsigned_integer() );
    static_assert( not( MIN & 0b1 ) );

    static_assert( MAX <= Address_Numeric::max().as_unsigned_integer() );
    static_assert( not( MAX & 0b1 ) );

    static_assert( MIN <= MAX );

    /**
     * \brief Get the minimum valid address.
     *
     * \return The minimum valid address.
     */
    static constexpr auto min() noexcept -> Device_Address_Transmitted
    {
        return Device_Address_Transmitted{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, MIN };
    }

    /**
     * \brief Get the maximum valid address.
     *
     * \return The maximum valid address.
     */
    static constexpr auto max() noexcept -> Device_Address_Transmitted
    {
        return Device_Address_Transmitted{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, MAX };
    }

    /**
     * \brief Constructor.
     */
    constexpr Device_Address_Transmitted() noexcept :
        Address_Transmitted{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, MIN }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] address The address.
     *
     * \pre address >= picolibrary::I2C::Device_Address_Transmitted::min().as_unsigned_integer()
     * \pre address <= picolibrary::I2C::Device_Address_Transmitted::max().as_unsigned_integer()
     * \pre not( address & 0b1 )
     */
    constexpr Device_Address_Transmitted( Unsigned_Integer address ) noexcept :
        Address_Transmitted{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, address }
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
    constexpr Device_Address_Transmitted( Bypass_Precondition_Expectation_Checks, Unsigned_Integer address ) noexcept
        :
        Address_Transmitted{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, address }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] address The address.
     */
    constexpr Device_Address_Transmitted( Device_Address_Numeric<( MIN >> 1 ), ( MAX >> 1 )> address ) noexcept :
        Address_Transmitted{ address }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Device_Address_Transmitted( Device_Address_Transmitted && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Device_Address_Transmitted( Device_Address_Transmitted const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Device_Address_Transmitted() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Device_Address_Transmitted && expression ) noexcept
        -> Device_Address_Transmitted & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Device_Address_Transmitted const & expression ) noexcept
        -> Device_Address_Transmitted & = default;
};

/**
 * \brief Bus multiplexer aligner for buses with no multiplexers.
 */
class Bus_Multiplexer_Aligner {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Bus_Multiplexer_Aligner() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Bus_Multiplexer_Aligner( Bus_Multiplexer_Aligner && source ) noexcept = default;

    Bus_Multiplexer_Aligner( Bus_Multiplexer_Aligner const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Bus_Multiplexer_Aligner() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator          =( Bus_Multiplexer_Aligner && expression ) noexcept
        -> Bus_Multiplexer_Aligner & = default;

    auto operator=( Bus_Multiplexer_Aligner const & ) = delete;

    /**
     * \brief Align the bus's nonexistent multiplexers to enable communication with a
     *        specific device.
     */
    constexpr void operator()() const noexcept
    {
    }
};

/**
 * \brief Device.
 *
 * \tparam Bus_Multiplexer_Aligner A nullary functor. The functor must be default
 *         constructable, move constructable, and move assignable. When called, the
 *         functor should align the bus's multiplexer(s) (if any) to enable communication
 *         with the device.
 * \tparam Controller The type of controller used to communicate with the device.
 */
template<typename Bus_Multiplexer_Aligner, typename Controller>
class Device {
  public:
    Device( Device const & ) = delete;

    auto operator=( Device const & ) = delete;

    /**
     * \brief Get the device's address.
     *
     * \return The device's address.
     */
    constexpr auto address() const noexcept -> Address_Transmitted
    {
        return m_address;
    }

    /**
     * \brief Get the fatal error that occurs if the device does not respond when
     *        addressed or does not acknowledge a write.
     *
     * \return The fatal error that occurs if the device does not respond when addressed
     *         or does not acknowledge a write.
     */
    constexpr auto nonresponsive_device_error() const noexcept -> Error_Code const &
    {
        return m_nonresponsive_device_error;
    }

    /**
     * \brief Check if the device is responsive.
     *
     * \param[in] operation The operation to request when addressing the device.
     *
     * \return picolibrary::I2C::Response::ACK if the device is responsive.
     * \return picolibrary::I2C::Response::NACK if the device is not responsive.
     */
    auto ping( Operation operation ) const noexcept -> Response
    {
        align_bus_multiplexer();

        return ::picolibrary::I2C::ping( *m_controller, m_address, operation );
    }

    /**
     * \brief Check if the device is responsive.
     *
     * \return picolibrary::I2C::Response::ACK if the device is responsive.
     * \return picolibrary::I2C::Response::NACK if the device is not responsive.
     */
    auto ping() const noexcept -> Response
    {
        align_bus_multiplexer();

        return ::picolibrary::I2C::ping( *m_controller, m_address );
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
     * \param[in] controller The controller used to communicate with the device.
     * \param[in] address The device's address.
     * \param[in] nonresponsive_device_error The fatal error that occurs if the device
     *            does not respond when addressed or does not acknowledge a write.
     */
    constexpr Device(
        Bus_Multiplexer_Aligner bus_multiplexer_aligner,
        Controller &            controller,
        Address_Transmitted     address,
        Error_Code const &      nonresponsive_device_error ) noexcept :
        m_align_bus_multiplexer{ std::move( bus_multiplexer_aligner ) },
        m_controller{ &controller },
        m_address{ std::move( address ) },
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
        m_address{ std::move( source.m_address ) },
        m_nonresponsive_device_error{ std::move( source.m_nonresponsive_device_error ) }
    {
        source.m_controller = nullptr;
    }

    /**
     * \brief Destructor.
     */
    ~Device() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Device && expression ) noexcept -> Device &
    {
        if ( &expression != this ) {
            m_align_bus_multiplexer = std::move( expression.m_align_bus_multiplexer );
            m_controller            = expression.m_controller;
            m_address               = std::move( expression.m_address );
            m_nonresponsive_device_error = std::move( expression.m_nonresponsive_device_error );

            expression.m_controller = nullptr;
        } // if

        return *this;
    }

    /**
     * \brief Align the bus's multiplexer(s) (if any) to enable communication with the
     *        device.
     */
    constexpr void align_bus_multiplexer() const noexcept
    {
        m_align_bus_multiplexer();
    }

    /**
     * \brief Get the controller used to communicate with the device.
     *
     * \return The controller used to communicate with the device.
     */
    constexpr auto controller() const noexcept -> Controller &
    {
        return *m_controller;
    }

    /**
     * \brief Read a register.
     *
     * \pre the device is responsive
     *
     * \param[in] register_address The address of the register to read.
     *
     * \return The data read from the register.
     */
    auto read( std::uint8_t register_address ) const noexcept -> std::uint8_t
    {
        m_align_bus_multiplexer();

        auto const guard = Bus_Control_Guard{ *m_controller };

        PICOLIBRARY_EXPECT(
            m_controller->address( m_address, Operation::WRITE ) == Response::ACK,
            m_nonresponsive_device_error );
        PICOLIBRARY_EXPECT( m_controller->write( register_address ) == Response::ACK, m_nonresponsive_device_error );

        m_controller->repeated_start();

        PICOLIBRARY_EXPECT(
            m_controller->address( m_address, Operation::READ ) == Response::ACK,
            m_nonresponsive_device_error );
        return m_controller->read( Response::NACK );
    }

    /**
     * \brief Read a block of registers.
     *
     * \pre the device is responsive
     *
     * \param[in] register_address The address of the block of registers to read.
     * \param[out] begin The beginning of the data read from the block of registers.
     * \param[out] end The end of the data read from the block of registers.
     *
     * \warning This function does not verify that the register block size is non-zero. If
     *          the register block size is zero, a NACK terminated read will never be
     *          performed which results in the device retaining control of the SDA signal,
     *          locking up the bus.
     */
    void read( std::uint8_t register_address, std::uint8_t * begin, std::uint8_t * end ) const noexcept
    {
        m_align_bus_multiplexer();

        auto const guard = Bus_Control_Guard{ *m_controller };

        PICOLIBRARY_EXPECT(
            m_controller->address( m_address, Operation::WRITE ) == Response::ACK,
            m_nonresponsive_device_error );
        PICOLIBRARY_EXPECT( m_controller->write( register_address ) == Response::ACK, m_nonresponsive_device_error );

        m_controller->repeated_start();

        PICOLIBRARY_EXPECT(
            m_controller->address( m_address, Operation::READ ) == Response::ACK,
            m_nonresponsive_device_error );
        m_controller->read( begin, end, Response::NACK );
    }

    /**
     * \brief Write to a register.
     *
     * \pre the device is responsive
     *
     * \param[in] register_address The address of the register to write to.
     * \param[in] data The data to write to the register.
     */
    void write( std::uint8_t register_address, std::uint8_t data ) noexcept
    {
        m_align_bus_multiplexer();

        auto const guard = Bus_Control_Guard{ *m_controller };

        PICOLIBRARY_EXPECT(
            m_controller->address( m_address, Operation::WRITE ) == Response::ACK,
            m_nonresponsive_device_error );
        PICOLIBRARY_EXPECT( m_controller->write( register_address ) == Response::ACK, m_nonresponsive_device_error );
        PICOLIBRARY_EXPECT( m_controller->write( data ) == Response::ACK, m_nonresponsive_device_error );
    }

    /**
     * \brief Write to a block of registers.
     *
     * \pre the device is responsive
     *
     * \param[in] register_address The address of the block of registers to write to.
     * \param[in] begin, The beginning of the data to write to the block of registers.
     * \param[in] end, The end of the data to write to the block of registers.
     */
    void write( std::uint8_t register_address, std::uint8_t const * begin, std::uint8_t const * end ) noexcept
    {
        m_align_bus_multiplexer();

        auto const guard = Bus_Control_Guard{ *m_controller };

        PICOLIBRARY_EXPECT(
            m_controller->address( m_address, Operation::WRITE ) == Response::ACK,
            m_nonresponsive_device_error );
        PICOLIBRARY_EXPECT( m_controller->write( register_address ) == Response::ACK, m_nonresponsive_device_error );
        PICOLIBRARY_EXPECT( m_controller->write( begin, end ) == Response::ACK, m_nonresponsive_device_error );
    }

  private:
    /**
     * \brief Align the bus's multiplexer(s) (if any) to enable communication with the
     *        device.
     */
    Bus_Multiplexer_Aligner m_align_bus_multiplexer{};

    /**
     * \brief The controller used to communicate with the device.
     */
    Controller * m_controller{};

    /**
     * \brief The device's address.
     */
    Address_Transmitted m_address{};

    /**
     * \brief The fatal error that occurs if the device does not respond when addressed or
     *        does not acknowledge a write.
     */
    Error_Code m_nonresponsive_device_error{};
};

} // namespace picolibrary::I2C

#endif // PICOLIBRARY_I2C_H

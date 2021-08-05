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
 * \brief picolibrary::Testing::Unit::I2C interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_I2C_H
#define PICOLIBRARY_TESTING_UNIT_I2C_H

#include <algorithm>
#include <cstdint>
#include <functional>
#include <vector>

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/i2c.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/void.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Generate a pseudo-random I2C device address within the specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random I2C device address in the range [min,max].
 */
template<>
inline auto random<I2C::Address>( I2C::Address min, I2C::Address max )
{
    return I2C::Address{ I2C::Address::NUMERIC,
                         random<std::uint_fast8_t>( min.numeric(), max.numeric() ) };
}

/**
 * \brief Generate a pseudo-random I2C device address greater than or equal to a minimum
 *        address.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random I2C device address in the range
 *         [min,picolibrary::I2C::Address::max()].
 */
template<>
inline auto random<I2C::Address>( I2C::Address min )
{
    return random<I2C::Address>( min, I2C::Address::max() );
}

/**
 * \brief Generate a pseudo-random I2C device address.
 *
 * \return A pseudo-random I2C device address in the range
 *         [picolibrary::I2C::Address::min(),picolibrary::I2C::Address::max()].
 */
template<>
inline auto random<I2C::Address>()
{
    return random<I2C::Address>( I2C::Address::min(), I2C::Address::max() );
}

/**
 * \brief Generate a pseudo-random I2C operation.
 *
 * \return A pseudo-random I2C operation.
 */
template<>
inline auto random<I2C::Operation>()
{
    return static_cast<I2C::Operation>( random<std::uint_fast8_t>(
        static_cast<std::uint_fast8_t>( I2C::Operation::WRITE ),
        static_cast<std::uint_fast8_t>( I2C::Operation::READ ) ) );
}

/**
 * \brief Generate a pseudo-random I2C response.
 *
 * \return A pseudo-random I2C response.
 */
template<>
inline auto random<I2C::Response>()
{
    return static_cast<I2C::Response>( random<std::uint_fast8_t>(
        static_cast<std::uint_fast8_t>( I2C::Response::ACK ),
        static_cast<std::uint_fast8_t>( I2C::Response::NACK ) ) );
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief Inter-Integrated Circuit (I2C) unit testing facilities.
 */
namespace picolibrary::Testing::Unit::I2C {

/**
 * \copydoc picolibrary::I2C::Address
 */
using Address = ::picolibrary::I2C::Address;

/**
 * \copydoc picolibrary::I2C::Operation
 */
using Operation = ::picolibrary::I2C::Operation;

/**
 * \copydoc picolibrary::I2C::Response
 */
using Response = ::picolibrary::I2C::Response;

/**
 * \brief Mock I2C basic controller.
 */
class Mock_Basic_Controller {
  public:
    /**
     * \brief Movable mock basic controller handle.
     */
    class Handle {
      public:
        /**
         * \brief Constructor.
         */
        Handle() noexcept = default;

        /**
         * \brief Constructor.
         *
         * \param[in] mock_basic_controller The mock basic controller.
         */
        Handle( Mock_Basic_Controller & mock_basic_controller ) noexcept :
            m_mock_basic_controller{ &mock_basic_controller }
        {
        }

        /**
         * \brief Constructor.
         *
         * \param[in] source The source of the move.
         */
        Handle( Handle && source ) noexcept :
            m_mock_basic_controller{ source.m_mock_basic_controller }
        {
            source.m_mock_basic_controller = nullptr;
        }

        Handle( Handle const & ) = delete;

        /**
         * \brief Destructor.
         */
        ~Handle() noexcept = default;

        /**
         * \brief Assignment operator.
         *
         * \param[in] expression The expression to be assigned.
         *
         * \return The assigned to object.
         */
        auto & operator=( Handle && expression ) noexcept
        {
            if ( &expression != this ) {
                m_mock_basic_controller = expression.m_mock_basic_controller;

                expression.m_mock_basic_controller = nullptr;
            } // if

            return *this;
        }

        auto operator=( Handle const & ) = delete;

        /**
         * \brief Get the mock basic controller.
         *
         * \return The mock basic controller.
         */
        auto & mock() noexcept
        {
            return *m_mock_basic_controller;
        }

        /**
         * \brief Initialize the controller's hardware.
         *
         * \return Nothing if controller hardware initialization succeeded.
         * \return An error code if controller hardware initialization failed.
         */
        auto initialize()
        {
            return m_mock_basic_controller->initialize();
        }

        /**
         * \brief Transmit a start condition.
         *
         * \return Nothing if start condition transmission succeeded.
         * \return An error code if start condition transmission failed.
         */
        auto start()
        {
            return m_mock_basic_controller->start();
        }

        /**
         * \brief Transmit a repeated start condition.
         *
         * \return Nothing if repeated start condition transmission succeeded.
         * \return An error code if repeated start condition transmission failed.
         */
        auto repeated_start()
        {
            return m_mock_basic_controller->repeated_start();
        }

        /**
         * \brief Transmit a stop condition.
         *
         * \return Nothing if stop condition transmission succeeded.
         * \return An error code if stop condition transmission failed.
         */
        auto stop()
        {
            return m_mock_basic_controller->stop();
        }

        /**
         * \brief Address a device.
         *
         * \param[in] address The address of the device to address.
         * \param[in] operation The operation that will be performed once the device has
         *            been addressed.
         *
         * \return Nothing if addressing the device succeeded.
         * \return An error code if addressing the device failed.
         */
        auto address( Address address, Operation operation )
        {
            return m_mock_basic_controller->address( address, operation );
        }

        /**
         * \brief Read data from a device.
         *
         * \param[in] response The response to send after the data is read.
         *
         * \return The read data if the read succeeded.
         * \return An error code if the read failed.
         */
        auto read( Response response )
        {
            return m_mock_basic_controller->read( response );
        }

        /**
         * \brief Write data to the device.
         *
         * \param[in] data The data to write.
         *
         * \return Nothing if the write succeeded.
         * \return An error code if the write failed.
         */
        auto write( std::uint8_t data )
        {
            return m_mock_basic_controller->write( data );
        }

      private:
        /**
         * \brief The mock basic controller.
         */
        Mock_Basic_Controller * m_mock_basic_controller{};
    };

    /**
     * \brief Constructor.
     */
    Mock_Basic_Controller();

    Mock_Basic_Controller( Mock_Basic_Controller && ) = delete;

    Mock_Basic_Controller( Mock_Basic_Controller const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Basic_Controller() noexcept;

    auto operator=( Mock_Basic_Controller && ) = delete;

    auto operator=( Mock_Basic_Controller const & ) = delete;

    /**
     * \brief Get a movable handle to the mock basic controller.
     *
     * \return A movable handle to the mock basic controller.
     */
    auto handle() noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );

    MOCK_METHOD( (Result<Void, Error_Code>), start, () );

    MOCK_METHOD( (Result<Void, Error_Code>), repeated_start, () );

    MOCK_METHOD( (Result<Void, Error_Code>), stop, () );

    MOCK_METHOD( (Result<Void, Error_Code>), address, ( Address, Operation ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read, ( Response ) );

    MOCK_METHOD( (Result<Void, Error_Code>), write, ( std::uint8_t ) );
};

/**
 * \brief Mock I2C controller.
 */
class Mock_Controller {
  public:
    /**
     * \brief Movable mock controller handle.
     */
    class Handle {
      public:
        /**
         * \brief Constructor.
         */
        Handle() noexcept = default;

        /**
         * \brief Constructor.
         *
         * \param[in] mock_controller The mock controller.
         */
        Handle( Mock_Controller & mock_controller ) noexcept :
            m_mock_controller{ &mock_controller }
        {
        }

        /**
         * \brief Constructor.
         *
         * \param[in] source The source of the move.
         */
        Handle( Handle && source ) noexcept :
            m_mock_controller{ source.m_mock_controller }
        {
            source.m_mock_controller = nullptr;
        }

        Handle( Handle const & ) = delete;

        /**
         * \brief Destructor.
         */
        ~Handle() noexcept = default;

        /**
         * \brief Assignment operator.
         *
         * \param[in] expression The expression to be assigned.
         *
         * \return The assigned to object.
         */
        auto & operator=( Handle && expression ) noexcept
        {
            if ( &expression != this ) {
                m_mock_controller = expression.m_mock_controller;

                expression.m_mock_controller = nullptr;
            } // if

            return *this;
        }

        auto operator=( Handle const & ) = delete;

        /**
         * \brief Get the mock basic controller.
         *
         * \return The mock basic controller.
         */
        auto & mock() noexcept
        {
            return *m_mock_controller;
        }

        /**
         * \brief Initialize the controller's hardware.
         *
         * \return Nothing if controller hardware initialization succeeded.
         * \return An error code if controller hardware initialization failed.
         */
        auto initialize()
        {
            return m_mock_controller->initialize();
        }

        /**
         * \brief Transmit a start condition.
         *
         * \return Nothing if start condition transmission succeeded.
         * \return An error code if start condition transmission failed.
         */
        auto start()
        {
            return m_mock_controller->start();
        }

        /**
         * \brief Transmit a repeated start condition.
         *
         * \return Nothing if repeated start condition transmission succeeded.
         * \return An error code if repeated start condition transmission failed.
         */
        auto repeated_start()
        {
            return m_mock_controller->repeated_start();
        }

        /**
         * \brief Transmit a stop condition.
         *
         * \return Nothing if stop condition transmission succeeded.
         * \return An error code if stop condition transmission failed.
         */
        auto stop()
        {
            return m_mock_controller->stop();
        }

        /**
         * \brief Address a device.
         *
         * \param[in] address The address of the device to address.
         * \param[in] operation The operation that will be performed once the device has
         *            been addressed.
         *
         * \return Nothing if addressing the device succeeded.
         * \return An error code if addressing the device failed.
         */
        auto address( Address address, Operation operation )
        {
            return m_mock_controller->address( address, operation );
        }

        /**
         * \brief Read data from a device.
         *
         * \param[in] response The response to send after the data is read.
         *
         * \return The read data if the read succeeded.
         * \return An error code if the read failed.
         */
        auto read( Response response )
        {
            return m_mock_controller->read( response );
        }

        /**
         * \brief Read a block of data from a device.
         *
         * \param[out] begin The beginning of the block of read data.
         * \param[out] end The end of the block of read data.
         * \param[in] response The response to send after the last byte of the block is
         *            read.
         *
         * \return Nothing if the read succeeded.
         * \return An error code if the read failed.
         */
        auto read( std::uint8_t * begin, std::uint8_t * end, Response response )
        {
            return m_mock_controller->read( begin, end, response );
        }

        /**
         * \brief Write data to the device.
         *
         * \param[in] data The data to write.
         *
         * \return Nothing if the write succeeded.
         * \return An error code if the write failed.
         */
        auto write( std::uint8_t data )
        {
            return m_mock_controller->write( data );
        }

        /**
         * \brief Write a block of data to a device.
         *
         * \param[in] begin The beginning of the block of data to write.
         * \param[in] end The end of the block of data to write.
         *
         * \return Nothing if the write succeeded.
         * \return An error code if the write failed.
         */
        auto write( std::uint8_t const * begin, std::uint8_t const * end )
        {
            return m_mock_controller->write( begin, end );
        }

      private:
        /**
         * \brief The mock basic controller.
         */
        Mock_Controller * m_mock_controller{};
    };

    /**
     * \brief Constructor.
     */
    Mock_Controller();

    Mock_Controller( Mock_Controller && ) = delete;

    Mock_Controller( Mock_Controller const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Controller() noexcept;

    auto operator=( Mock_Controller && ) = delete;

    auto operator=( Mock_Controller const & ) = delete;

    /**
     * \brief Get a movable handle to the mock basic controller.
     *
     * \return A movable handle to the mock basic controller.
     */
    auto handle() noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );

    MOCK_METHOD( (Result<Void, Error_Code>), start, () );

    MOCK_METHOD( (Result<Void, Error_Code>), repeated_start, () );

    MOCK_METHOD( (Result<Void, Error_Code>), stop, () );

    MOCK_METHOD( (Result<Void, Error_Code>), address, ( Address, Operation ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read, ( Response ) );

    MOCK_METHOD( (Result<std::vector<std::uint8_t>, Error_Code>), read, ( std::vector<std::uint8_t>, Response ) );

    /**
     * \brief Read a block of data from a device.
     *
     * \param[out] begin The beginning of the block of read data.
     * \param[out] end The end of the block of read data.
     *
     * \return Nothing if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read( std::uint8_t * begin, std::uint8_t * end, Response response ) -> Result<Void, Error_Code>
    {
        static_cast<void>( end );

        auto const result = read( std::vector<std::uint8_t>{}, response );

        if ( result.is_error() ) {
            return result.error();
        } // if

        std::for_each( result.value().begin(), result.value().end(), [ &begin ]( auto data ) {
            *begin = data;

            ++begin;
        } );

        return {};
    }

    MOCK_METHOD( (Result<Void, Error_Code>), write, ( std::uint8_t ) );

    MOCK_METHOD( (Result<Void, Error_Code>), write, (std::vector<std::uint8_t>));

    /**
     * \brief Write a block of data to a device.
     *
     * \param[in] begin The beginning of the block of data to write.
     * \param[in] end The end of the block of data to write.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write( std::uint8_t const * begin, std::uint8_t const * end ) -> Result<Void, Error_Code>
    {
        return write( std::vector<std::uint8_t>{ begin, end } );
    }
};

/**
 * \brief Mock I2C device.
 *
 * \tparam Register_Address The integral type used to hold device register addresses.
 */
template<typename Register_Address>
class Mock_Device;

/**
 * \brief Mock 8-bit register address space I2C device.
 */
template<>
class Mock_Device<std::uint8_t> {
  public:
    /**
     * \brief Constructor.
     */
    Mock_Device();

    /**
     * \brief Constructor.
     */
    Mock_Device( std::function<Result<Void, Error_Code>()>, Mock_Controller &, Address, Error_Code )
    {
    }

    Mock_Device( Mock_Device && ) = delete;

    Mock_Device( Mock_Device const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Device() noexcept;

    auto operator=( Mock_Device && ) = delete;

    auto operator=( Mock_Device const & ) = delete;

    MOCK_METHOD( Address, address, (), ( const ) );

    MOCK_METHOD( void, change_address, ( Address ) );

    MOCK_METHOD( Error_Code, nonresponsive_device_error, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), align_bus_multiplexer, (), ( const ) );

    MOCK_METHOD( Mock_Controller &, controller, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), ping, ( Operation ), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), ping, (), ( const ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read, ( std::uint8_t ), ( const ) );

    MOCK_METHOD(
        (Result<std::vector<std::uint8_t>, Error_Code>),
        read,
        (std::uint8_t, std::vector<std::uint8_t>),
        ( const ) );

    /**
     * \brief Read a block of registers.
     *
     * \param[in] register_address The address of the block of registers to read.
     * \param[in] begin The beginning of the data read from the block of registers.
     * \param[in] end The end of the data read from the block of registers.
     *
     * \return Nothing if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read( std::uint8_t register_address, std::uint8_t * begin, std::uint8_t * end ) const
        -> Result<Void, Error_Code>
    {
        static_cast<void>( end );

        auto const result = read( register_address, std::vector<std::uint8_t>{} );

        if ( result.is_error() ) {
            return result.error();
        } // if

        std::for_each( result.value().begin(), result.value().end(), [ &begin ]( auto data ) {
            *begin = data;

            ++begin;
        } );

        return {};
    }

    MOCK_METHOD( (Result<Void, Error_Code>), write, ( std::uint8_t, std::uint8_t ) );

    MOCK_METHOD( (Result<Void, Error_Code>), write, (std::uint8_t, std::vector<std::uint8_t>));

    /**
     * \brief Write to a block of registers.
     *
     * \param[in] register_address The address of the block of registers to write to.
     * \param[in] begin The beginning of the data to write to the block of registers.
     * \param[in] end The end of the data to write to the block of registers.
     */
    auto write( std::uint8_t register_address, std::uint8_t const * begin, std::uint8_t const * end )
    {
        return write( register_address, std::vector<std::uint8_t>{ begin, end } );
    }
};

} // namespace picolibrary::Testing::Unit::I2C

#endif // PICOLIBRARY_TESTING_UNIT_I2C_H

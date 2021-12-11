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
#include "picolibrary/testing/unit/mock_handle.h"
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
    return random<bool>() ? I2C::Operation::WRITE : I2C::Operation::READ;
}

/**
 * \brief Generate a pseudo-random I2C response.
 *
 * \return A pseudo-random I2C response.
 */
template<>
inline auto random<I2C::Response>()
{
    return random<bool>() ? I2C::Response::ACK : I2C::Response::NACK;
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief Inter-Integrated Circuit (I2C) unit testing facilities.
 */
namespace picolibrary::Testing::Unit::I2C {

/**
 * \brief Mock I2C basic controller.
 */
class Mock_Basic_Controller {
  public:
    class Handle : public Mock_Handle<Mock_Basic_Controller> {
      public:
        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Basic_Controller & mock ) noexcept :
            Mock_Handle<Mock_Basic_Controller>{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        auto initialize()
        {
            return mock().initialize();
        }

        auto start()
        {
            return mock().start();
        }

        auto repeated_start()
        {
            return mock().repeated_start();
        }

        auto stop()
        {
            return mock().stop();
        }

        auto address( ::picolibrary::I2C::Address address, ::picolibrary::I2C::Operation operation )
        {
            return mock().address( address, operation );
        }

        auto read( ::picolibrary::I2C::Response response )
        {
            return mock().read( response );
        }

        auto write( std::uint8_t data )
        {
            return mock().write( data );
        }
    };

    Mock_Basic_Controller() = default;

    Mock_Basic_Controller( Mock_Basic_Controller && ) = delete;

    Mock_Basic_Controller( Mock_Basic_Controller const & ) = delete;

    ~Mock_Basic_Controller() noexcept = default;

    auto operator=( Mock_Basic_Controller && ) = delete;

    auto operator=( Mock_Basic_Controller const & ) = delete;

    auto handle() noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );

    MOCK_METHOD( (Result<Void, Error_Code>), start, () );
    MOCK_METHOD( (Result<Void, Error_Code>), repeated_start, () );
    MOCK_METHOD( (Result<Void, Error_Code>), stop, () );

    MOCK_METHOD( (Result<Void, Error_Code>), address, ( ::picolibrary::I2C::Address, ::picolibrary::I2C::Operation ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read, ( ::picolibrary::I2C::Response ) );

    MOCK_METHOD( (Result<Void, Error_Code>), write, ( std::uint8_t ) );
};

/**
 * \brief Mock I2C controller.
 */
class Mock_Controller : public Mock_Basic_Controller {
  public:
    class Handle : public Mock_Basic_Controller::Handle {
      public:
        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Controller & mock ) noexcept :
            Mock_Basic_Controller::Handle{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        auto & mock() noexcept
        {
            return static_cast<Mock_Controller &>( Mock_Basic_Controller::Handle::mock() );
        }

        auto const & mock() const noexcept
        {
            return static_cast<Mock_Controller const &>( Mock_Basic_Controller::Handle::mock() );
        }

        using Mock_Basic_Controller::Handle::read;

        auto read( std::uint8_t * begin, std::uint8_t * end, ::picolibrary::I2C::Response response )
        {
            return mock().read( begin, end, response );
        }

        using Mock_Basic_Controller::Handle::write;

        auto write( std::uint8_t const * begin, std::uint8_t const * end )
        {
            return mock().write( begin, end );
        }
    };

    Mock_Controller() = default;

    Mock_Controller( Mock_Controller && ) = delete;

    Mock_Controller( Mock_Controller const & ) = delete;

    ~Mock_Controller() noexcept = default;

    auto operator=( Mock_Controller && ) = delete;

    auto operator=( Mock_Controller const & ) = delete;

    auto handle() noexcept
    {
        return Handle{ *this };
    }

    using Mock_Basic_Controller::gmock_read;
    using Mock_Basic_Controller::read;

    MOCK_METHOD(
        (Result<std::vector<std::uint8_t>, Error_Code>),
        read,
        ( std::vector<std::uint8_t>, ::picolibrary::I2C::Response ) );

    auto read( std::uint8_t * begin, std::uint8_t * end, ::picolibrary::I2C::Response response )
        -> Result<Void, Error_Code>
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

    using Mock_Basic_Controller::gmock_write;
    using Mock_Basic_Controller::write;

    MOCK_METHOD( (Result<Void, Error_Code>), write, (std::vector<std::uint8_t>));

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
    Mock_Device() = default;

    Mock_Device( std::function<Result<Void, Error_Code>()>, Mock_Controller &, ::picolibrary::I2C::Address, Error_Code )
    {
    }

    Mock_Device( Mock_Device && ) = delete;

    Mock_Device( Mock_Device const & ) = delete;

    ~Mock_Device() noexcept = default;

    auto operator=( Mock_Device && ) = delete;

    auto operator=( Mock_Device const & ) = delete;

    MOCK_METHOD( ::picolibrary::I2C::Address, address, (), ( const ) );

    MOCK_METHOD( void, change_address, ( ::picolibrary::I2C::Address ) );

    MOCK_METHOD( Error_Code, nonresponsive_device_error, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), align_bus_multiplexer, (), ( const ) );

    MOCK_METHOD( Mock_Controller &, controller, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), ping, ( ::picolibrary::I2C::Operation ), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), ping, (), ( const ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read, ( std::uint8_t ), ( const ) );
    MOCK_METHOD(
        (Result<std::vector<std::uint8_t>, Error_Code>),
        read,
        (std::uint8_t, std::vector<std::uint8_t>),
        ( const ) );

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

    auto write( std::uint8_t register_address, std::uint8_t const * begin, std::uint8_t const * end )
    {
        return write( register_address, std::vector<std::uint8_t>{ begin, end } );
    }
};

} // namespace picolibrary::Testing::Unit::I2C

#endif // PICOLIBRARY_TESTING_UNIT_I2C_H

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
#include "picolibrary/testing/unit/mock_handle.h"
#include "picolibrary/testing/unit/random.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Generate a pseudo-random picolibrary::I2C::Address_Numeric within the specified
 *        range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::I2C::Address_Numeric in the range [min,max].
 */
template<>
inline auto random<I2C::Address_Numeric>( I2C::Address_Numeric min, I2C::Address_Numeric max )
    -> I2C::Address_Numeric
{
    return I2C::Address_Numeric{ random<I2C::Address_Numeric::Unsigned_Integer>(
        min.as_unsigned_integer(), max.as_unsigned_integer() ) };
}

/**
 * \brief Generate a pseudo-random picolibrary::I2C::Address_Numeric greater than or equal
 *        to a minimum picolibrary::I2C::Address_Numeric.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::I2C::Address_Numeric in the range
 *         [min,picolibrary::I2C::Address_Numeric::max()].
 */
template<>
inline auto random<I2C::Address_Numeric>( I2C::Address_Numeric min ) -> I2C::Address_Numeric
{
    return random<I2C::Address_Numeric>( min, I2C::Address_Numeric::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::I2C::Address_Numeric.
 *
 * \return A pseudo-random picolibrary::I2C::Address_Numeric in the range
 *         [picolibrary::I2C::Address_Numeric::min(),picolibrary::I2C::Address_Numeric::max()].
 */
template<>
inline auto random<I2C::Address_Numeric>() -> I2C::Address_Numeric
{
    return random<I2C::Address_Numeric>( I2C::Address_Numeric::min(), I2C::Address_Numeric::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::I2C::Address_Transmitted within the
 *        specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::I2C::Address_Transmitted in the range [min,max].
 */
template<>
inline auto random<I2C::Address_Transmitted>( I2C::Address_Transmitted min, I2C::Address_Transmitted max )
    -> I2C::Address_Transmitted
{
    return I2C::Address_Transmitted{ static_cast<I2C::Address_Transmitted::Unsigned_Integer>(
        random<I2C::Address_Transmitted::Unsigned_Integer>( min.as_unsigned_integer(), max.as_unsigned_integer() )
        & 0b1111111'0 ) };
}

/**
 * \brief Generate a pseudo-random picolibrary::I2C::Address_Transmitted greater than or
 *        equal to a minimum picolibrary::I2C::Address_Transmitted.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::I2C::Address_Transmitted in the range
 *         [min,picolibrary::I2C::Address_Transmitted::max()].
 */
template<>
inline auto random<I2C::Address_Transmitted>( I2C::Address_Transmitted min ) -> I2C::Address_Transmitted
{
    return random<I2C::Address_Transmitted>( min, I2C::Address_Transmitted::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::I2C::Address_Transmitted.
 *
 * \return A pseudo-random picolibrary::I2C::Address_Transmitted in the range
 *         [picolibrary::I2C::Address_Transmitted::min(),picolibrary::I2C::Address_Transmitted::max()].
 */
template<>
inline auto random<I2C::Address_Transmitted>() -> I2C::Address_Transmitted
{
    return random<I2C::Address_Transmitted>(
        I2C::Address_Transmitted::min(), I2C::Address_Transmitted::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::I2C::Operation.
 *
 * \return A pseudo-randomly generated picolibrary::I2C::Operation.
 */
template<>
inline auto random<I2C::Operation>() -> I2C::Operation
{
    return random<bool>() ? I2C::Operation::READ : I2C::Operation::WRITE;
}

/**
 * \brief Generate a pseudo-random picolibrary::I2C::Response.
 *
 * \return A pseudo-randomly generated picolibrary::I2C::Response.
 */
template<>
inline auto random<I2C::Response>() -> I2C::Response
{
    return random<bool>() ? I2C::Response::ACK : I2C::Response::NACK;
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief Inter-Integrated Circuit (I2C) unit testing facilities.
 */
namespace picolibrary::Testing::Unit::I2C {

/**
 * \brief Mock basic controller.
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

        void initialize()
        {
            Mock_Handle<Mock_Basic_Controller>::mock().initialize();
        }

        auto bus_error_present() const -> bool
        {
            return Mock_Handle<Mock_Basic_Controller>::mock().bus_error_present();
        }

        void start()
        {
            Mock_Handle<Mock_Basic_Controller>::mock().start();
        }

        void repeated_start()
        {
            Mock_Handle<Mock_Basic_Controller>::mock().repeated_start();
        }

        void stop()
        {
            Mock_Handle<Mock_Basic_Controller>::mock().stop();
        }

        auto address( ::picolibrary::I2C::Address_Transmitted address, ::picolibrary::I2C::Operation operation )
            -> ::picolibrary::I2C::Response
        {
            return Mock_Handle<Mock_Basic_Controller>::mock().address( address, operation );
        }

        auto read( ::picolibrary::I2C::Response response ) -> std::uint8_t
        {
            return Mock_Handle<Mock_Basic_Controller>::mock().read( response );
        }

        auto write( std::uint8_t data ) -> ::picolibrary::I2C::Response
        {
            return Mock_Handle<Mock_Basic_Controller>::mock().write( data );
        }
    };

    Mock_Basic_Controller() = default;

    Mock_Basic_Controller( Mock_Basic_Controller && ) = delete;

    Mock_Basic_Controller( Mock_Basic_Controller const & ) = delete;

    ~Mock_Basic_Controller() noexcept = default;

    auto operator=( Mock_Basic_Controller && ) = delete;

    auto operator=( Mock_Basic_Controller const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    MOCK_METHOD( void, initialize, () );

    MOCK_METHOD( bool, bus_error_present, (), ( const ) );

    MOCK_METHOD( void, start, () );

    MOCK_METHOD( void, repeated_start, () );

    MOCK_METHOD( void, stop, () );

    MOCK_METHOD(
        ::picolibrary::I2C::Response,
        address,
        ( ::picolibrary::I2C::Address_Transmitted, ::picolibrary::I2C::Operation ) );

    MOCK_METHOD( std::uint8_t, read, ( ::picolibrary::I2C::Response ) );

    MOCK_METHOD( ::picolibrary::I2C::Response, write, ( std::uint8_t ) );
};

/**
 * \brief Mock controller.
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

        auto mock() noexcept -> Mock_Controller &
        {
            return static_cast<Mock_Controller &>( Mock_Basic_Controller::Handle::mock() );
        }

        auto mock() const noexcept -> Mock_Controller const &
        {
            return static_cast<Mock_Controller const &>( Mock_Basic_Controller::Handle::mock() );
        }

        using Mock_Basic_Controller::Handle::read;

        void read( std::uint8_t * begin, std::uint8_t * end, ::picolibrary::I2C::Response response )
        {
            mock().read( begin, end, response );
        }

        using Mock_Basic_Controller::Handle::write;

        auto write( std::uint8_t const * begin, std::uint8_t const * end ) -> ::picolibrary::I2C::Response
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

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    using Mock_Basic_Controller::gmock_read;
    using Mock_Basic_Controller::read;

    MOCK_METHOD( std::vector<std::uint8_t>, read, ( std::vector<std::uint8_t>, ::picolibrary::I2C::Response ) );

    void read( std::uint8_t * begin, std::uint8_t * end, ::picolibrary::I2C::Response response )
    {
        static_cast<void>( end );

        auto const data = read( std::vector<std::uint8_t>{}, response );

        std::copy( data.begin(), data.end(), begin );
    }

    using Mock_Basic_Controller::gmock_write;
    using Mock_Basic_Controller::write;

    MOCK_METHOD( ::picolibrary::I2C::Response, write, (std::vector<std::uint8_t>));

    auto write( std::uint8_t const * begin, std::uint8_t const * end ) -> ::picolibrary::I2C::Response
    {
        return write( std::vector<std::uint8_t>{ begin, end } );
    }
};

/**
 * \brief Mock device.
 */
class Mock_Device {
  public:
    Mock_Device() = default;

    Mock_Device( std::function<void()>, Mock_Controller &, ::picolibrary::I2C::Address_Transmitted, Error_Code const & )
    {
    }

    Mock_Device( Mock_Device && ) = delete;

    Mock_Device( Mock_Device const & ) = delete;

    ~Mock_Device() noexcept = default;

    auto operator=( Mock_Device && ) = delete;

    auto operator=( Mock_Device const & ) = delete;

    MOCK_METHOD( ::picolibrary::I2C::Address_Transmitted, address, (), ( const ) );

    MOCK_METHOD( Error_Code, nonresponsive_device_error, (), ( const ) );

    MOCK_METHOD( ::picolibrary::I2C::Response, ping, ( ::picolibrary::I2C::Operation ), ( const ) );
    MOCK_METHOD( ::picolibrary::I2C::Response, ping, (), ( const ) );

    MOCK_METHOD( void, align_bus_multiplexer, (), ( const ) );

    MOCK_METHOD( Mock_Controller &, controller, (), ( const ) );

    MOCK_METHOD( std::uint8_t, read, ( std::uint8_t ), ( const ) );
    MOCK_METHOD( std::vector<std::uint8_t>, read, (std::uint8_t, std::vector<std::uint8_t>), ( const ) );

    void read( std::uint8_t register_address, std::uint8_t * begin, std::uint8_t * end ) const
    {
        static_cast<void>( end );

        auto const data = read( register_address, std::vector<std::uint8_t>{} );

        std::copy( data.begin(), data.end(), begin );
    }

    MOCK_METHOD( void, write, ( std::uint8_t, std::uint8_t ) );
    MOCK_METHOD( void, write, (std::uint8_t, std::vector<std::uint8_t>));

    void write( std::uint8_t register_address, std::uint8_t const * begin, std::uint8_t const * end )
    {
        write( register_address, std::vector<std::uint8_t>{ begin, end } );
    }
};

} // namespace picolibrary::Testing::Unit::I2C

#endif // PICOLIBRARY_TESTING_UNIT_I2C_H

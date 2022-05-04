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
 * \brief picolibrary::Testing::Automated::Microchip::MCP23S08 interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_MICROCHIP_MCP23S08_H
#define PICOLIBRARY_TESTING_AUTOMATED_MICROCHIP_MCP23S08_H

#include <cstdint>

#include "gmock/gmock.h"
#include "picolibrary/microchip/mcp23s08.h"
#include "picolibrary/testing/automated/microchip/mcp23x08.h"
#include "picolibrary/testing/automated/random.h"
#include "picolibrary/testing/automated/spi.h"

namespace picolibrary::Testing::Automated {

/**
 * \brief Generate a pseudo-random picolibrary::Microchip::MCP23S08::Address_Numeric
 *        within the specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::Microchip::MCP23S08::Address_Numeric in the range
 *         [min,max].
 */
template<>
inline auto random<::picolibrary::Microchip::MCP23S08::Address_Numeric>(
    ::picolibrary::Microchip::MCP23S08::Address_Numeric min,
    ::picolibrary::Microchip::MCP23S08::Address_Numeric max )
    -> ::picolibrary::Microchip::MCP23S08::Address_Numeric
{
    return ::picolibrary::Microchip::MCP23S08::Address_Numeric{
        random<::picolibrary::Microchip::MCP23S08::Address_Numeric::Unsigned_Integer>(
            min.as_unsigned_integer(), max.as_unsigned_integer() )
    };
}

/**
 * \brief Generate a pseudo-random picolibrary::Microchip::MCP23S08::Address_Numeric
 *        greater than or equal to a minimum
 *        picolibrary::Microchip::MCP23S08::Address_Numeric.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::Microchip::MCP23S08::Address_Numeric in the range
 *         [min,picolibrary::Microchip::MCP23S08::Address_Numeric::max()].
 */
template<>
inline auto random<::picolibrary::Microchip::MCP23S08::Address_Numeric>(
    ::picolibrary::Microchip::MCP23S08::Address_Numeric min )
    -> ::picolibrary::Microchip::MCP23S08::Address_Numeric
{
    return random<::picolibrary::Microchip::MCP23S08::Address_Numeric>(
        min, ::picolibrary::Microchip::MCP23S08::Address_Numeric::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::Microchip::MCP23S08::Address_Numeric.
 *
 * \return A pseudo-random picolibrary::Microchip::MCP23S08::Address_Numeric in the range
 *         [picolibrary::Microchip::MCP23S08::Address_Numeric::min(),picolibrary::Microchip::MCP23S08::Address_Numeric::max()].
 */
template<>
inline auto random<::picolibrary::Microchip::MCP23S08::Address_Numeric>()
    -> ::picolibrary::Microchip::MCP23S08::Address_Numeric
{
    return random<::picolibrary::Microchip::MCP23S08::Address_Numeric>(
        ::picolibrary::Microchip::MCP23S08::Address_Numeric::min(),
        ::picolibrary::Microchip::MCP23S08::Address_Numeric::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::Microchip::MCP23S08::Address_Transmitted
 *        within the specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::Microchip::MCP23S08::Address_Transmitted in the
 *         range [min,max].
 */
template<>
inline auto random<::picolibrary::Microchip::MCP23S08::Address_Transmitted>(
    ::picolibrary::Microchip::MCP23S08::Address_Transmitted min,
    ::picolibrary::Microchip::MCP23S08::Address_Transmitted max )
    -> ::picolibrary::Microchip::MCP23S08::Address_Transmitted
{
    return ::picolibrary::Microchip::MCP23S08::Address_Transmitted{
        static_cast<::picolibrary::Microchip::MCP23S08::Address_Transmitted::Unsigned_Integer>(
            random<::picolibrary::Microchip::MCP23S08::Address_Transmitted::Unsigned_Integer>(
                min.as_unsigned_integer(), max.as_unsigned_integer() )
            & 0b11111'11'0 )
    };
}

/**
 * \brief Generate a pseudo-random picolibrary::Microchip::MCP23S08::Address_Transmitted
 *        greater than or equal to a minimum
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::Microchip::MCP23S08::Address_Transmitted in the
 *         range [min,picolibrary::Microchip::MCP23S08::Address_Transmitted::max()].
 */
template<>
inline auto random<::picolibrary::Microchip::MCP23S08::Address_Transmitted>(
    ::picolibrary::Microchip::MCP23S08::Address_Transmitted min )
    -> ::picolibrary::Microchip::MCP23S08::Address_Transmitted
{
    return random<::picolibrary::Microchip::MCP23S08::Address_Transmitted>(
        min, ::picolibrary::Microchip::MCP23S08::Address_Transmitted::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::Microchip::MCP23S08::Address_Transmitted.
 *
 * \return A pseudo-random picolibrary::Microchip::MCP23S08::Address_Transmitted in the
 *         range
 *         [picolibrary::Microchip::MCP23S08::Address_Transmitted::min(),picolibrary::Microchip::MCP23S08::Address_Transmitted::max()].
 */
template<>
inline auto random<::picolibrary::Microchip::MCP23S08::Address_Transmitted>()
    -> ::picolibrary::Microchip::MCP23S08::Address_Transmitted
{
    return random<::picolibrary::Microchip::MCP23S08::Address_Transmitted>(
        ::picolibrary::Microchip::MCP23S08::Address_Transmitted::min(),
        ::picolibrary::Microchip::MCP23S08::Address_Transmitted::max() );
}

} // namespace picolibrary::Testing::Automated

/**
 * \brief Microchip MCP23S08 automated testing facilities.
 */
namespace picolibrary::Testing::Automated::Microchip::MCP23S08 {

/**
 * \brief Mock communication controller.
 */
class Mock_Communication_Controller : public SPI::Mock_Device {
  public:
    Mock_Communication_Controller() = default;

    Mock_Communication_Controller(
        SPI::Mock_Controller &,
        SPI::Mock_Controller::Configuration const &,
        SPI::Mock_Device_Selector::Handle,
        ::picolibrary::Microchip::MCP23S08::Address_Transmitted )
    {
    }

    Mock_Communication_Controller( Mock_Communication_Controller && ) = delete;

    Mock_Communication_Controller( Mock_Communication_Controller const & ) = delete;

    ~Mock_Communication_Controller() noexcept = default;

    auto operator=( Mock_Communication_Controller && ) = delete;

    auto operator=( Mock_Communication_Controller const & ) = delete;

    MOCK_METHOD( ::picolibrary::Microchip::MCP23S08::Address_Transmitted, address, (), ( const ) );

    MOCK_METHOD( std::uint8_t, read, ( std::uint8_t ), ( const ) );

    MOCK_METHOD( void, write, ( std::uint8_t, std::uint8_t ) );
};

/**
 * \brief Mock driver.
 */
class Mock_Driver : public Mock_Communication_Controller, public MCP23X08::Mock_Driver {
  public:
    Mock_Driver() = default;

    Mock_Driver(
        SPI::Mock_Controller &,
        SPI::Mock_Controller::Configuration const &,
        SPI::Mock_Device_Selector::Handle,
        ::picolibrary::Microchip::MCP23S08::Address_Transmitted )
    {
    }

    Mock_Driver( Mock_Driver && ) = delete;

    Mock_Driver( Mock_Driver const & ) = delete;

    ~Mock_Driver() noexcept = default;

    auto operator=( Mock_Driver && ) = delete;

    auto operator=( Mock_Driver const & ) = delete;
};

} // namespace picolibrary::Testing::Automated::Microchip::MCP23S08

#endif // PICOLIBRARY_TESTING_AUTOMATED_MICROCHIP_MCP23S08_H

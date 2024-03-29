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
 * \brief picolibrary::Testing::Automated::Microchip::MCP23S08 interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_MICROCHIP_MCP23S08_H
#define PICOLIBRARY_TESTING_AUTOMATED_MICROCHIP_MCP23S08_H

#include <cstdint>
#include <iomanip>
#include <ios>
#include <limits>
#include <ostream>

#include "gmock/gmock.h"
#include "picolibrary/microchip/mcp23s08.h"
#include "picolibrary/testing/automated/microchip/mcp23x08.h"
#include "picolibrary/testing/automated/spi.h"

namespace picolibrary::Microchip::MCP23S08 {

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the
 *            picolibrary::Microchip::MCP23S08::Address_Numeric to.
 * \param[in] address The picolibrary::Microchip::MCP23S08::Address_Numeric to write to
 *            the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Address_Numeric address ) -> std::ostream &
{
    return stream << "0x" << std::hex << std::uppercase
                  << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 )
                  << std::setfill( '0' )
                  << static_cast<std::uint_fast16_t>( address.as_unsigned_integer() );
}

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the
 *            picolibrary::Microchip::MCP23S08::Address_Transmitted to.
 * \param[in] address The picolibrary::Microchip::MCP23S08::Address_Transmitted to write
 *            to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Address_Transmitted address ) -> std::ostream &
{
    return stream << "0x" << std::hex << std::uppercase
                  << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 )
                  << std::setfill( '0' )
                  << static_cast<std::uint_fast16_t>( address.as_unsigned_integer() );
}

} // namespace picolibrary::Microchip::MCP23S08

/**
 * \brief Microchip MCP23S08 automated testing facilities.
 */
namespace picolibrary::Testing::Automated::Microchip::MCP23S08 {

/**
 * \brief Mock communication controller.
 */
class Mock_Communication_Controller : public SPI::Mock_Device {
  public:
    Mock_Communication_Controller() noexcept
    {
    }

    Mock_Communication_Controller(
        SPI::Mock_Controller &,
        SPI::Mock_Controller::Configuration const &,
        SPI::Mock_Device_Selector::Handle,
        ::picolibrary::Microchip::MCP23S08::Address_Transmitted ) noexcept
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

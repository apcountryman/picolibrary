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
 * \brief picolibrary::Testing::Unit::Microchip::MCP23X08 interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_MICROCHIP_MCP23X08_H
#define PICOLIBRARY_TESTING_UNIT_MICROCHIP_MCP23X08_H

#include <cstdint>

#include "gmock/gmock.h"

/**
 * \brief Microchip MCP23008/MCP23S08 unit testing facilities.
 */
namespace picolibrary::Testing::Unit::Microchip::MCP23X08 {

/**
 * \brief Mock driver.
 */
class Mock_Driver {
  public:
    Mock_Driver() = default;

    Mock_Driver( Mock_Driver && ) = delete;

    Mock_Driver( Mock_Driver const & ) = delete;

    ~Mock_Driver() noexcept = default;

    auto operator=( Mock_Driver && ) = delete;

    auto operator=( Mock_Driver const & ) = delete;

    MOCK_METHOD( std::uint8_t, read_iodir, (), ( const ) );
    MOCK_METHOD( void, write_iodir, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, read_ipol, (), ( const ) );
    MOCK_METHOD( void, write_ipol, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, read_gpinten, (), ( const ) );
    MOCK_METHOD( void, write_gpinten, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, read_defval, (), ( const ) );
    MOCK_METHOD( void, write_defval, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, read_intcon, (), ( const ) );
    MOCK_METHOD( void, write_intcon, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, read_iocon, (), ( const ) );
    MOCK_METHOD( void, write_iocon, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, read_gppu, (), ( const ) );
    MOCK_METHOD( void, write_gppu, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, read_intf, (), ( const ) );

    MOCK_METHOD( std::uint8_t, read_intcap, (), ( const ) );

    MOCK_METHOD( std::uint8_t, read_gpio, (), ( const ) );
    MOCK_METHOD( void, write_gpio, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, read_olat, (), ( const ) );
    MOCK_METHOD( void, write_olat, ( std::uint8_t ) );
};

/**
 * \brief Mock caching driver.
 */
class Mock_Caching_Driver : public Mock_Driver {
  public:
    Mock_Caching_Driver() = default;

    Mock_Caching_Driver( Mock_Caching_Driver && ) = delete;

    Mock_Caching_Driver( Mock_Caching_Driver const & ) = delete;

    ~Mock_Caching_Driver() noexcept = default;

    auto operator=( Mock_Caching_Driver && ) = delete;

    auto operator=( Mock_Caching_Driver const & ) = delete;

    MOCK_METHOD( void, reset_cache, () );

    MOCK_METHOD( std::uint8_t, iodir, (), ( const ) );

    MOCK_METHOD( std::uint8_t, ipol, (), ( const ) );

    MOCK_METHOD( std::uint8_t, gpinten, (), ( const ) );

    MOCK_METHOD( std::uint8_t, defval, (), ( const ) );

    MOCK_METHOD( std::uint8_t, intcon, (), ( const ) );

    MOCK_METHOD( std::uint8_t, iocon, (), ( const ) );

    MOCK_METHOD( std::uint8_t, gppu, (), ( const ) );

    MOCK_METHOD( std::uint8_t, olat, (), ( const ) );
};

} // namespace picolibrary::Testing::Unit::Microchip::MCP23X08

/**
 * \brief Mock pin.
 */
class Mock_Pin {
  public:
    Mock_Pin() = default;

    Mock_Pin( Mock_Pin && ) = delete;

    Mock_Pin( Mock_Pin const & ) = delete;

    ~Mock_Pin() noexcept = default;

    auto operator=( Mock_Pin && ) = delete;

    auto operator=( Mock_Pin const & ) = delete;

    MOCK_METHOD( bool, is_associated, (), ( const ) );

    explicit operator bool() const
    {
        return is_associated();
    }

    MOCK_METHOD( void, configure_pin_as_internally_pulled_up_input, () );
    MOCK_METHOD( void, configure_pin_as_open_drain_io, () );
    MOCK_METHOD( void, configure_pin_as_push_pull_io, () );

    MOCK_METHOD( bool, pull_up_is_disabled, (), ( const ) );
    MOCK_METHOD( bool, pull_up_is_enabled, (), ( const ) );

    MOCK_METHOD( void, disable_pull_up, () );
    MOCK_METHOD( void, enable_pull_up, () );

    MOCK_METHOD( bool, is_low, (), ( const ) );
    MOCK_METHOD( bool, is_high, (), ( const ) );

    MOCK_METHOD( void, transition_open_drain_io_to_low, () );
    MOCK_METHOD( void, transition_push_pull_io_to_low, () );

    MOCK_METHOD( void, transition_open_drain_io_to_high, () );
    MOCK_METHOD( void, transition_push_pull_io_to_high, () );

    MOCK_METHOD( void, toggle_open_drain_io, () );
    MOCK_METHOD( void, toggle_push_pull_io, () );
};

#endif // PICOLIBRARY_TESTING_UNIT_MICROCHIP_MCP23X08_H

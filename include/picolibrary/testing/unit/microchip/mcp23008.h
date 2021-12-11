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
 * \brief picolibrary::Testing::Unit::Microchip::MCP23008 interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_MICROCHIP_MCP23008_H
#define PICOLIBRARY_TESTING_UNIT_MICROCHIP_MCP23008_H

#include <cstdint>

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/microchip/mcp23008.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/i2c.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/void.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Generate a pseudo-random Microchip MCP23008 sequential operation mode.
 *
 * \return A pseudo-randomly generated Microchip MCP23008 sequential operation mode.
 */
template<>
inline auto random<Microchip::MCP23008::Sequential_Operation_Mode>()
{
    return random<bool>() ? Microchip::MCP23008::Sequential_Operation_Mode::DISABLED
                          : Microchip::MCP23008::Sequential_Operation_Mode::ENABLED;
}

/**
 * \brief Generate a pseudo-random Microchip MCP23008 SDA slew rate control configuration.
 *
 * \return A pseudo-randomly generated Microchip MCP23008 SDA slew rate control
 *         configuration.
 */
template<>
inline auto random<Microchip::MCP23008::SDA_Slew_Rate_Control>()
{
    return random<bool>() ? Microchip::MCP23008::SDA_Slew_Rate_Control::DISABLED
                          : Microchip::MCP23008::SDA_Slew_Rate_Control::ENABLED;
}

/**
 * \brief Generate a pseudo-random Microchip MCP23008 interrupt mode.
 *
 * \return A pseudo-randomly generated Microchip MCP23008 interrupt mode.
 */
template<>
inline auto random<Microchip::MCP23008::Interrupt_Mode>()
{
    return static_cast<Microchip::MCP23008::Interrupt_Mode>( random<std::uint8_t>( 0b00, 0b10 ) << 1 );
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief Microchip MCP23008 unit testing facilities.
 */
namespace picolibrary::Testing::Unit::Microchip::MCP23008 {

/**
 * \brief Microchip MCP23008 mock register cache.
 */
class Mock_Register_Cache {
  public:
    Mock_Register_Cache() = default;

    Mock_Register_Cache( Mock_Register_Cache && ) = delete;

    Mock_Register_Cache( Mock_Register_Cache const & ) = delete;

    ~Mock_Register_Cache() noexcept = default;

    auto operator=( Mock_Register_Cache && ) = delete;

    auto operator=( Mock_Register_Cache const & ) = delete;

    MOCK_METHOD( void, initialize, () );

    MOCK_METHOD( std::uint8_t, iodir, (), ( const ) );
    MOCK_METHOD( void, cache_iodir, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, ipol, (), ( const ) );
    MOCK_METHOD( void, cache_ipol, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, gpinten, (), ( const ) );
    MOCK_METHOD( void, cache_gpinten, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, defval, (), ( const ) );
    MOCK_METHOD( void, cache_defval, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, intcon, (), ( const ) );
    MOCK_METHOD( void, cache_intcon, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, iocon, (), ( const ) );
    MOCK_METHOD( void, cache_iocon, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, gppu, (), ( const ) );
    MOCK_METHOD( void, cache_gppu, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, gpio, (), ( const ) );
    MOCK_METHOD( void, cache_gpio, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, olat, (), ( const ) );
    MOCK_METHOD( void, cache_olat, ( std::uint8_t ) );
};

/**
 * \brief Microchip MCP23008 mock driver.
 */
class Mock_Driver : public I2C::Mock_Device<std::uint8_t>, public Mock_Register_Cache {
  public:
    Mock_Driver() = default;

    Mock_Driver( Mock_Driver && ) = delete;

    Mock_Driver( Mock_Driver const & ) = delete;

    ~Mock_Driver() noexcept = default;

    auto operator=( Mock_Driver && ) = delete;

    auto operator=( Mock_Driver const & ) = delete;

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_iodir, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_iodir, ( std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_ipol, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_ipol, ( std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_gpinten, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_gpinten, ( std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_defval, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_defval, ( std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_intcon, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_intcon, ( std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_iocon, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_iocon, ( std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_gppu, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_gppu, ( std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_intf, (), ( const ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_intcap, (), ( const ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_gpio, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_gpio, ( std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_olat, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_olat, ( std::uint8_t ) );

    MOCK_METHOD(
        (Result<::picolibrary::Microchip::MCP23008::Interrupt_Context, Error_Code>),
        read_interrupt_context,
        (),
        ( const ) );

    MOCK_METHOD(
        (Result<Void, Error_Code>),
        configure,
        ( ::picolibrary::Microchip::MCP23008::Sequential_Operation_Mode,
          ::picolibrary::Microchip::MCP23008::SDA_Slew_Rate_Control,
          ::picolibrary::Microchip::MCP23008::Interrupt_Mode ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_pin_as_internally_pulled_up_input, ( std::uint8_t ) );

    MOCK_METHOD( (Result<Void, Error_Code>), enable_pull_up, ( std::uint8_t ) );
    MOCK_METHOD( (Result<Void, Error_Code>), disable_pull_up, ( std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), state, ( std::uint8_t ), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_pin_as_open_drain_output, ( std::uint8_t ) );

    MOCK_METHOD( (Result<Void, Error_Code>), transition_open_drain_output_to_low, ( std::uint8_t ) );
    MOCK_METHOD( (Result<Void, Error_Code>), transition_open_drain_output_to_high, ( std::uint8_t ) );
    MOCK_METHOD( (Result<Void, Error_Code>), toggle_open_drain_output, ( std::uint8_t ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_pin_as_push_pull_output, ( std::uint8_t ) );

    MOCK_METHOD( (Result<Void, Error_Code>), transition_push_pull_output_to_high, ( std::uint8_t ) );
    MOCK_METHOD( (Result<Void, Error_Code>), transition_push_pull_output_to_low, ( std::uint8_t ) );
    MOCK_METHOD( (Result<Void, Error_Code>), toggle_push_pull_output, ( std::uint8_t ) );
};

} // namespace picolibrary::Testing::Unit::Microchip::MCP23008

#endif // PICOLIBRARY_TESTING_UNIT_MICROCHIP_MCP23008_H

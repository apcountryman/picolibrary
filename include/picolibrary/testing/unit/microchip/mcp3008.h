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
 * \brief picolibrary::Testing::Unit::Microchip::MCP3008 interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_MICROCHIP_MCP3008_H
#define PICOLIBRARY_TESTING_UNIT_MICROCHIP_MCP3008_H

#include <cstdint>

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/microchip/mcp3008.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/spi.h"
#include "picolibrary/void.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Generate a pseudo-random Microchip MCP3008 single-ended input channel.
 *
 * \return A pseudo-random Microchip MCP3008 single-ended input channel.
 */
template<>
inline auto random<Microchip::MCP3008::Channel>()
{
    return static_cast<Microchip::MCP3008::Channel>(
        ( 0b1'000 | random<std::uint8_t>( 0b000, 0b111 ) ) << 4 );
}

/**
 * \brief Generate a pseudo-random Microchip MCP3008 differential input channel pair.
 *
 * \return A pseudo-random Microchip MCP3008 differential input channel pair.
 */
template<>
inline auto random<Microchip::MCP3008::Channel_Pair>()
{
    return static_cast<Microchip::MCP3008::Channel_Pair>( random<std::uint8_t>( 0b000, 0b111 ) << 4 );
}

/**
 * \brief Generate a pseudo-random Microchip MCP3008 input mode/channel(s).
 *
 * \return A pseudo-random Microchip MCP3008 input mode/channel(s).
 */
template<>
inline auto random<Microchip::MCP3008::Input>()
{
    return random<bool>()
               ? Microchip::MCP3008::Input{ random<Microchip::MCP3008::Channel>() }
               : Microchip::MCP3008::Input{ random<Microchip::MCP3008::Channel_Pair>() };
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief Microchip MCP3008 unit testing facilities.
 */
namespace picolibrary::Testing::Unit::Microchip::MCP3008 {

/**
 * \brief Mock Microchip MCP3008 driver.
 */
class Mock_Driver {
  public:
    using Controller = SPI::Mock_Controller;

    using Device_Selector = SPI::Mock_Device_Selector::Handle;

    Mock_Driver() = default;

    Mock_Driver( Controller &, Controller::Configuration, Device_Selector, Error_Code )
    {
    }

    Mock_Driver( Mock_Driver && ) = delete;

    Mock_Driver( Mock_Driver const & ) = delete;

    ~Mock_Driver() noexcept = default;

    auto operator=( Mock_Driver && ) = delete;

    auto operator=( Mock_Driver const & ) = delete;

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );

    MOCK_METHOD(
        (Result<::picolibrary::Microchip::MCP3008::Sample, Error_Code>),
        sample,
        ( ::picolibrary::Microchip::MCP3008::Input ) );
};

} // namespace picolibrary::Testing::Unit::Microchip::MCP3008

#endif // PICOLIBRARY_TESTING_UNIT_MICROCHIP_MCP3008_H

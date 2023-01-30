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
 * \brief picolibrary::Testing::Automated::Microchip::MCP3008 interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_MICROCHIP_MCP3008_H
#define PICOLIBRARY_TESTING_AUTOMATED_MICROCHIP_MCP3008_H

#include <cstdint>

#include "picolibrary/microchip/mcp3008.h"
#include "picolibrary/testing/automated/random.h"
#include "picolibrary/testing/automated/spi.h"

namespace picolibrary::Testing::Automated {

/**
 * \brief Generate a pseudo-random picolibrary::Microchip::MCP3008::Input.
 *
 * \return A pseudo-randomly generated picolibrary::Microchip::MCP3008::Input.
 */
template<>
inline auto random<Microchip::MCP3008::Input>() -> Microchip::MCP3008::Input
{
    return static_cast<Microchip::MCP3008::Input>( random<std::uint_fast8_t>( 0b0'000, 0b1'111 ) << 4 );
}

} // namespace picolibrary::Testing::Automated

/**
 * \brief Microchip MCP3008 automated testing facilities.
 */
namespace picolibrary::Testing::Automated::Microchip::MCP3008 {

/**
 * \brief Mock driver.
 */
class Mock_Driver : public SPI::Mock_Device {
  public:
    Mock_Driver() = default;

    Mock_Driver( SPI::Mock_Controller &, SPI::Mock_Controller::Configuration const &, SPI::Mock_Device_Selector::Handle )
    {
    }

    Mock_Driver( Mock_Driver && ) = delete;

    Mock_Driver( Mock_Driver const & ) = delete;

    ~Mock_Driver() noexcept = default;

    auto operator=( Mock_Driver && ) = delete;

    auto operator=( Mock_Driver const & ) = delete;

    MOCK_METHOD( ::picolibrary::Microchip::MCP3008::Sample, sample, ( ::picolibrary::Microchip::MCP3008::Input ) );
};

} // namespace picolibrary::Testing::Automated::Microchip::MCP3008

#endif // PICOLIBRARY_TESTING_AUTOMATED_MICROCHIP_MCP3008_H

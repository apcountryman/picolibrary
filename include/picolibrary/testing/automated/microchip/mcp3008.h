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

#include <ostream>
#include <stdexcept>

#include "picolibrary/microchip/mcp3008.h"
#include "picolibrary/testing/automated/spi.h"

namespace picolibrary::Microchip::MCP3008 {

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::Microchip::MCP3008::Input to.
 * \param[in] input The picolibrary::Microchip::MCP3008::Input to write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Input input ) -> std::ostream &
{
    // #lizard forgives the length

    switch ( input ) {
            // clang-format off

        case Input::CH0: return stream << "::picolibrary::Microchip::MCP3008::Input::CH0";
        case Input::CH1: return stream << "::picolibrary::Microchip::MCP3008::Input::CH1";
        case Input::CH2: return stream << "::picolibrary::Microchip::MCP3008::Input::CH2";
        case Input::CH3: return stream << "::picolibrary::Microchip::MCP3008::Input::CH3";
        case Input::CH4: return stream << "::picolibrary::Microchip::MCP3008::Input::CH4";
        case Input::CH5: return stream << "::picolibrary::Microchip::MCP3008::Input::CH5";
        case Input::CH6: return stream << "::picolibrary::Microchip::MCP3008::Input::CH6";
        case Input::CH7: return stream << "::picolibrary::Microchip::MCP3008::Input::CH7";

        case Input::CH0_RELATIVE_TO_CH1: return stream << "::picolibrary::Microchip::MCP3008::Input::CH0_RELATIVE_TO_CH1";
        case Input::CH1_RELATIVE_TO_CH0: return stream << "::picolibrary::Microchip::MCP3008::Input::CH1_RELATIVE_TO_CH0";
        case Input::CH2_RELATIVE_TO_CH3: return stream << "::picolibrary::Microchip::MCP3008::Input::CH2_RELATIVE_TO_CH3";
        case Input::CH3_RELATIVE_TO_CH2: return stream << "::picolibrary::Microchip::MCP3008::Input::CH3_RELATIVE_TO_CH2";
        case Input::CH4_RELATIVE_TO_CH5: return stream << "::picolibrary::Microchip::MCP3008::Input::CH4_RELATIVE_TO_CH5";
        case Input::CH5_RELATIVE_TO_CH4: return stream << "::picolibrary::Microchip::MCP3008::Input::CH5_RELATIVE_TO_CH4";
        case Input::CH6_RELATIVE_TO_CH7: return stream << "::picolibrary::Microchip::MCP3008::Input::CH6_RELATIVE_TO_CH7";
        case Input::CH7_RELATIVE_TO_CH6: return stream << "::picolibrary::Microchip::MCP3008::Input::CH7_RELATIVE_TO_CH6";

            // clang-format on
    } // switch

    throw std::invalid_argument{
        "input is not a valid ::picolibrary::Microchip::MCP3008::Input"
    };
}

} // namespace picolibrary::Microchip::MCP3008

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

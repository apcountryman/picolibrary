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
 * \brief picolibrary::Testing::Interactive::Microchip::MCP23S08 interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_MICROCHIP_MCP23S08_H
#define PICOLIBRARY_TESTING_INTERACTIVE_MICROCHIP_MCP23S08_H

#include <cstdint>
#include <utility>

#include "picolibrary/microchip/mcp23s08.h"
#include "picolibrary/microchip/mcp23x08.h"
#include "picolibrary/stream.h"
#include "picolibrary/testing/interactive/gpio.h"

/**
 * \brief Microchip MCP23S08 interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::Microchip::MCP23S08 {

/**
 * \brief Internally pulled-up input pin state interactive test helper.
 *
 * \tparam Controller The type of controller used to communicate with the MCP23S08.
 * \tparam Device_Selector The type of device selector used to select and deselect the
 *         MCP23S08.
 * \tparam Delayer A nullary functor called to introduce a delay each time the pin's state
 *         is gotten.
 *
 * \param[in] stream The output stream to write the pin state to.
 * \param[in] controller The controller used to communicate with the MCP23S08.
 * \param[in] configuration The controller clock and data exchange bit order configuration
 *            that meets the MCP23S08's communication requirements.
 * \param[in] device_selector The device selector used to select and deselect the
 *            MCP23S08.
 * \param[in] address The MCP23S08's address.
 * \param[in] mask The mask identifying the pin.
 * \param[in] delay The nullary functor to call to introduce a delay each time the pin's
 *            state is gotten.
 *
 * \pre writing to the stream succeeds
 */
template<typename Controller, typename Device_Selector, typename Delayer>
void state(
    Output_Stream &                                         stream,
    Controller                                              controller,
    typename Controller::Configuration                      configuration,
    Device_Selector                                         device_selector,
    ::picolibrary::Microchip::MCP23S08::Address_Transmitted address,
    std::uint8_t                                            mask,
    Delayer                                                 delay ) noexcept
{
    controller.initialize();

    auto mcp23s08 = ::picolibrary::Microchip::MCP23X08::Caching_Driver<::picolibrary::Microchip::MCP23S08::Driver<Controller, Device_Selector>>{
        controller, configuration, std::move( device_selector ), std::move( address )
    };

    mcp23s08.initialize();

    ::picolibrary::Testing::Interactive::GPIO::state(
        stream,
        ::picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin{ mcp23s08, mask },
        std::move( delay ) );
}

} // namespace picolibrary::Testing::Interactive::Microchip::MCP23S08

#endif // PICOLIBRARY_TESTING_INTERACTIVE_MICROCHIP_MCP23S08_H

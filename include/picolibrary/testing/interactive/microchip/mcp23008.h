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
 * \brief picolibrary::Testing::Interactive::Microchip::MCP23008 interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_MICROCHIP_MCP23008_H
#define PICOLIBRARY_TESTING_INTERACTIVE_MICROCHIP_MCP23008_H

#include <cstdint>
#include <utility>

#include "picolibrary/error.h"
#include "picolibrary/i2c.h"
#include "picolibrary/microchip/mcp23008.h"
#include "picolibrary/microchip/mcp23x08.h"
#include "picolibrary/stream.h"
#include "picolibrary/testing/interactive/gpio.h"

/**
 * \brief Microchip MCP23008 interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::Microchip::MCP23008 {

/**
 * \brief Internally pulled-up input pin state interactive test helper.
 *
 * \tparam Controller The type of controller used to communicate with the MCP23008.
 * \tparam Delayer A nullary functor called to introduce a delay each time the pin's state
 *         is gotten.
 *
 * \param[in] stream The output stream to write the pin state to.
 * \param[in] controller The controller used to communicate with the MCP23008.
 * \param[in] address The MCP23008's address.
 * \param[in] mask The mask identifying the pin.
 * \param[in] delay The nullary functor to call to introduce a delay each time the pin's
 *            state is gotten.
 *
 * \pre writing to the stream succeeds
 */
template<typename Controller, typename Delayer>
void state(
    Output_Stream &                                         stream,
    Controller                                              controller,
    ::picolibrary::Microchip::MCP23008::Address_Transmitted address,
    std::uint8_t                                            mask,
    Delayer                                                 delay ) noexcept
{
    controller.initialize();

    auto mcp23008 = ::picolibrary::Microchip::MCP23X08::Caching_Driver<
        ::picolibrary::Microchip::MCP23008::Driver<::picolibrary::I2C::Bus_Multiplexer_Aligner, Controller>>{
        {}, controller, std::move( address ), Generic_Error::NONRESPONSIVE_DEVICE
    };

    ::picolibrary::Testing::Interactive::GPIO::state(
        stream,
        ::picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin{ mcp23008, mask },
        std::move( delay ) );
}

/**
 * \brief Output pin toggle interactive test helper.
 *
 * \tparam Output_Pin The type of output pin to toggle.
 * \tparam Controller The type of controller used to communicate with the MCP23008.
 * \tparam Delayer A nullary functor called to introduce a delay each time the pin is
 *         toggled.
 *
 * \param[in] controller The controller used to communicate with the MCP23008.
 * \param[in] address The MCP23008's address.
 * \param[in] mask The mask identifying the pin.
 * \param[in] delay The nullary functor to call to introduce a delay each time the pin is
 *            toggled.
 */
template<template<typename> typename Output_Pin, typename Controller, typename Delayer>
void toggle(
    Controller                                              controller,
    ::picolibrary::Microchip::MCP23008::Address_Transmitted address,
    std::uint8_t                                            mask,
    Delayer                                                 delay ) noexcept
{
    controller.initialize();

    auto mcp23008 = ::picolibrary::Microchip::MCP23X08::Caching_Driver<
        ::picolibrary::Microchip::MCP23008::Driver<::picolibrary::I2C::Bus_Multiplexer_Aligner, Controller>>{
        {}, controller, std::move( address ), Generic_Error::NONRESPONSIVE_DEVICE
    };

    ::picolibrary::Testing::Interactive::GPIO::toggle(
        Output_Pin{ mcp23008, mask }, std::move( delay ) );
}

} // namespace picolibrary::Testing::Interactive::Microchip::MCP23008

#endif // PICOLIBRARY_TESTING_INTERACTIVE_MICROCHIP_MCP23008_H

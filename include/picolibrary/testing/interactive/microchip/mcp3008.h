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
 * \brief picolibrary::Testing::Interactive::Microchip::MCP3008 interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_MICROCHIP_MCP3008_H
#define PICOLIBRARY_TESTING_INTERACTIVE_MICROCHIP_MCP3008_H

#include <utility>

#include "picolibrary/error.h"
#include "picolibrary/microchip/mcp3008.h"
#include "picolibrary/testing/interactive/adc.h"

/**
 * \brief Microchip MCP3008 interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::Microchip::MCP3008 {

/**
 * \brief Microchip MCP3008 sample interactive test helper.
 *
 * \tparam Output_Stream The type of asynchronous serial output stream to use to output
 *         information to the user.
 * \tparam Transmitter The type of asynchronous serial transmitter to use to transmit
 *         information to the user.
 * \tparam Controller The type of SPI controller to use to communicate with the MCP3008.
 * \tparam Device_Selector The type of SPI device selector to use to select the Microchip
 *         MCP3008.
 * \tparam Delayer A nullary functor called to introduce a delay each time a Microchip
 *         MCP3008 input sample is gotten.
 *
 * \param[in] transmitter The asynchronous serial transmitter to use to transmit
 *            information to the user.
 * \param[in] controller The SPI controller to use to communicate with the Microchip
 *            MCP3008.
 * \param[in] configuration The SPI controller configuration to use to communicate with
 *            the Microchip MCP3008.
 * \param[in] device_selector The device selector to use to select the Microchip MCP3008.
 * \param[in] input The Microchip MCP3008 input mode/channel(s) to use when getting a
 *            sample.
 * \param[in] delay The nullary functor to called to introduce a delay each time a
 *            Microchip MCP3008 input sample is gotten.
 */
template<template<typename> typename Output_Stream, typename Transmitter, typename Controller, typename Device_Selector, typename Delayer>
void sample(
    Transmitter                              transmitter,
    Controller                               controller,
    typename Controller::Configuration       configuration,
    Device_Selector                          device_selector,
    ::picolibrary::Microchip::MCP3008::Input input,
    Delayer                                  delay ) noexcept
{
    // #lizard forgives the PARAM

    if ( controller.initialize().is_error() ) {
        return;
    } // if

    auto mcp3008 = ::picolibrary::Microchip::MCP3008::Driver{
        controller, configuration, std::move( device_selector ), Generic_Error::NONRESPONSIVE_DEVICE
    };

    if ( mcp3008.initialize().is_error() ) {
        return;
    } // if

    ADC::sample_blocking_single_sample_converter<Output_Stream>(
        std::move( transmitter ),
        ::picolibrary::Microchip::MCP3008::Blocking_Single_Sample_Converter{ mcp3008, input },
        std::move( delay ) );
}

} // namespace picolibrary::Testing::Interactive::Microchip::MCP3008

#endif // PICOLIBRARY_TESTING_INTERACTIVE_MICROCHIP_MCP3008_H

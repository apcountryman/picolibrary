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
 * \brief picolibrary::Testing::Interactive::Microchip::MCP3008 interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_MICROCHIP_MCP3008_H
#define PICOLIBRARY_TESTING_INTERACTIVE_MICROCHIP_MCP3008_H

#include <utility>

#include "picolibrary/microchip/mcp3008.h"
#include "picolibrary/stream.h"
#include "picolibrary/testing/interactive/adc.h"

/**
 * \brief Microchip MCP3008 interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::Microchip::MCP3008 {

/**
 * \brief Blocking, single sample ADC sample interactive test helper.
 *
 * \tparam Controller The type of controller used to communicate with the MCP3008.
 * \tparam Device_Selector The type of device selector used to select and deselect the
 *         MCP3008.
 * \tparam Delayer A nullary functor called to introduce a delay each time a sample is
 *         gotten.
 *
 * \param[in] stream The output stream to write the samples to.
 * \param[in] controller The controller used to communicate with the MCP3008.
 * \param[in] configuration The controller clock and data exchange bit order configuration
 *            that meets the MCP3008's communication requirements.
 * \param[in] device_selector The device selector used to select and deselect the MCP3008.
 * \param[in] input The input to sample.
 * \param[in] delay The nullary functor to call to introduce a delay each time a sample is
 *            gotten.
 */
template<typename Controller, typename Device_Selector, typename Delayer>
// NOLINTNEXTLINE(readability-function-size)
void sample(
    Reliable_Output_Stream &                 stream,
    Controller                               controller,
    typename Controller::Configuration       configuration,
    Device_Selector                          device_selector,
    ::picolibrary::Microchip::MCP3008::Input input,
    Delayer                                  delay ) noexcept
{
    // #lizard forgives the parameter count

    controller.initialize();

    auto mcp3008 = ::picolibrary::Microchip::MCP3008::Driver{ controller,
                                                              configuration,
                                                              std::move( device_selector ) };

    mcp3008.initialize();

    ::picolibrary::Testing::Interactive::ADC::sample_blocking_single_sample_converter(
        stream,
        ::picolibrary::Microchip::MCP3008::Blocking_Single_Sample_Converter{ mcp3008, input },
        std::move( delay ) );
}

} // namespace picolibrary::Testing::Interactive::Microchip::MCP3008

#endif // PICOLIBRARY_TESTING_INTERACTIVE_MICROCHIP_MCP3008_H

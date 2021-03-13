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
 * \brief picolibrary::Testing::Interactive::Microchip::MCP23008 interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_MICROCHIP_MCP23008_H
#define PICOLIBRARY_TESTING_INTERACTIVE_MICROCHIP_MCP23008_H

#include <cstdint>

#include "picolibrary/error.h"
#include "picolibrary/i2c.h"
#include "picolibrary/microchip/mcp23008.h"
#include "picolibrary/testing/interactive/gpio.h"
#include "picolibrary/utility.h"

/**
 * \brief Microchip MCP23008 interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::Microchip::MCP23008 {

/**
 * \brief Microchip MCP23008 internally pulled-up input pin state interactive test helper.
 *
 * \tparam Output_Stream The type of asynchronous serial output stream to use to output
 *         information to the user.
 * \tparam Transmitter The type of asynchronous serial transmitter to use to transmit
 *         information to the user.
 * \tparam Controller The type of I2C controller used to interact with the bus the
 *         Microchip MCP23008 is attached to.
 * \tparam Delayer A nullary functor called to introduce a delay each time the pins' state
 *         is gotten.
 *
 * \param[in] transmitter The asynchronous serial transmitter to use to transmit
 *            information to the user.
 * \param[in] controller The I2C controller used to interact with the bus the Microchip
 *            MCP23008 is attached to.
 * \param[in] address The Microchip MCP23008's address.
 * \param[in] mask The mask identifying the Microchip MCP23008 internally pulled-up input
 *            pin.
 */
template<template<typename> typename Output_Stream, typename Transmitter, typename Controller, typename Delayer>
void state( Transmitter transmitter, Controller controller, I2C::Address address, std::uint8_t mask, Delayer delay ) noexcept
{
    auto stream = Output_Stream{ std::move( transmitter ) };

    if ( stream.initialize().is_error() ) {
        return;
    } // if

    {
        auto const result = controller.initialize();
        if ( result.is_error() ) {
            static_cast<void>(
                stream.print( "controller initialization error: {}\n", result.error() ) );

            return;
        } // if
    }

    auto driver = ::picolibrary::Microchip::MCP23008::Driver<decltype( nop ), Controller>{};

    {
        auto result = ::picolibrary::Microchip::MCP23008::make_driver(
            nop, controller, address, Generic_Error::NONRESPONSIVE_DEVICE );
        if ( result.is_error() ) {
            static_cast<void>( stream.print( "driver construction error: {}\n", result.error() ) );

            return;
        } // if

        driver = std::move( result ).value();
    }

    driver.initialize();

    GPIO::state(
        stream,
        ::picolibrary::Microchip::MCP23008::Internally_Pulled_Up_Input_Pin{ driver, mask },
        std::move( delay ) );
}

} // namespace picolibrary::Testing::Interactive::Microchip::MCP23008

#endif // PICOLIBRARY_TESTING_INTERACTIVE_MICROCHIP_MCP23008_H

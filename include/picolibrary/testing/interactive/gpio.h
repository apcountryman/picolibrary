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
 * \brief picolibrary::Testing::Interactive::GPIO interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_GPIO_H
#define PICOLIBRARY_TESTING_INTERACTIVE_GPIO_H

#include <utility>

/**
 * \brief General Purpose Input/Output (GPIO) interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::GPIO {

/**
 * \brief GPIO output pin toggle interactive test helper.
 *
 * \tparam Output_Stream The type of asynchronous serial output stream to use to output
 *         information to the user.
 * \tparam Transmitter The type of asynchronous serial transmitter to use to transmit
 *         information to the user.
 * \tparam Output_Pin The type of output pin to toggle.
 * \tparam Delayer A nullary functor called to introduce a delay each time the pin is
 *         toggled.
 *
 * \param[in] transmitter The asynchronous serial transmitter to use to transmit
 *            information to the user.
 * \param[in] pin The pin to toggle.
 * \param[in] delay The nullary functor called to introduce a delay each time the pin is
 *            toggled.
 */
template<template<typename> typename Output_Stream, typename Transmitter, typename Output_Pin, typename Delayer>
void toggle( Transmitter transmitter, Output_Pin pin, Delayer delay ) noexcept
{
    auto stream = Output_Stream{ std::move( transmitter ) };

    if ( stream.initialize().is_error() ) {
        return;
    } // if

    {
        auto const result = pin.initialize();
        if ( result.is_error() ) {
            static_cast<void>( stream.print( "pin initialization error: {}\n", result.error() ) );

            return;
        } // if
    }

    for ( ;; ) {
        delay();

        auto const result = pin.toggle();
        if ( result.is_error() ) {
            static_cast<void>( stream.print( "pin toggle error: {}\n", result.error() ) );

            return;
        } // if
    }     // for
}

} // namespace picolibrary::Testing::Interactive::GPIO

#endif // PICOLIBRARY_TESTING_INTERACTIVE_GPIO_H

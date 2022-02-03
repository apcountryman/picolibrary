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
 * \brief picolibrary::Testing::Interactive::GPIO interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_GPIO_H
#define PICOLIBRARY_TESTING_INTERACTIVE_GPIO_H

#include "picolibrary/precondition.h"
#include "picolibrary/stream.h"

/**
 * \brief General Purpose Input/Output (GPIO) interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::GPIO {

/**
 * \brief GPIO input pin state interactive test helper.
 *
 * \tparam Input_Pin The type of input pin to get the state of.
 * \tparam Delayer A nullary functor called to introduce a delay each time the pin's state
 *         is gotten.
 *
 * \param[in] stream The output stream to write the pin state to.
 * \param[in] pin The pin to get the state of.
 * \param[in] delay The nullary functor to call to introduce a delay each time the pin's
 *            state is gotten.
 *
 * \pre Writing to stream succeeds.
 */
template<typename Input_Pin, typename Delayer>
void state( Output_Stream & stream, Input_Pin pin, Delayer delay ) noexcept
{
    pin.initialize();

    for ( ;; ) {
        delay();

        auto const result = stream.put( pin.is_high() ? "high\n" : "low\n" );
        expect( not result.is_error(), result.error() );
    } // for
}

/**
 * \brief GPIO output pin toggle interactive test helper.
 *
 * \tparam Output_Pin The type of output pin to toggle.
 * \tparam Delayer A nullary functor called to introduce a delay each time the pin is
 *         toggled.
 *
 * \param[in] pin The pin to toggle.
 * \param[in] delay The nullary functor to call to introduce a delay each time the pin is
 *            toggled.
 */
template<typename Output_Pin, typename Delayer>
void toggle( Output_Pin pin, Delayer delay ) noexcept
{
    pin.initialize();

    for ( ;; ) {
        delay();

        pin.toggle();
    } // for
}

} // namespace picolibrary::Testing::Interactive::GPIO

#endif // PICOLIBRARY_TESTING_INTERACTIVE_GPIO_H

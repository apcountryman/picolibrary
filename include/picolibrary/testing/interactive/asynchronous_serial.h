/**
 * picolibrary
 *
 * Copyright 2020 Andrew Countryman <apcountryman@gmail.com>
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
 * \brief picolibrary::Testing::Interactive::Asynchronous_Serial interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_ASYNCHRONOUS_SERIAL_H
#define PICOLIBRARY_TESTING_INTERACTIVE_ASYNCHRONOUS_SERIAL_H

/**
 * \brief Asynchronous serial interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::Asynchronous_Serial {

/**
 * \brief Initialize an asynchronous serial transmitter, and transmit "Hello, world!\n".
 *
 * \tparam Transmitter The type of transmitter to use.
 *
 * \param[in] transmitter The transmitter to use.
 */
template<typename Transmitter>
void hello_world( Transmitter transmitter ) noexcept
{
    if ( transmitter.initialize().is_error() ) { return; }

    auto const * string = "Hello, world!\n";

    while ( auto const character = *string++ ) {
        if ( transmitter.transmit( character ).is_error() ) { return; } // if
    }                                                                   // while
}

} // namespace picolibrary::Testing::Interactive::Asynchronous_Serial

#endif // PICOLIBRARY_TESTING_INTERACTIVE_ASYNCHRONOUS_SERIAL_H

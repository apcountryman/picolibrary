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
 * \brief picolibrary::Testing::Interactive::Asynchronous_Serial interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_ASYNCHRONOUS_SERIAL_H
#define PICOLIBRARY_TESTING_INTERACTIVE_ASYNCHRONOUS_SERIAL_H

#include "picolibrary/rom.h"

/**
 * \brief Asynchronous serial interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::Asynchronous_Serial {

/**
 * \brief Asynchronous serial transmitter hello world interactive test helper.
 *
 * \tparam Transmitter The type of transmitter to use.
 *
 * \param[in] transmitter The transmitter to use.
 */
template<typename Transmitter>
void hello_world( Transmitter transmitter ) noexcept
{
    transmitter.initialize();

    for ( auto       string    = PICOLIBRARY_ROM_STRING( "Hello, world!\n" );
          auto const character = *string++; ) {
        transmitter.transmit( character );
    } // for
}

} // namespace picolibrary::Testing::Interactive::Asynchronous_Serial

#endif // PICOLIBRARY_TESTING_INTERACTIVE_ASYNCHRONOUS_SERIAL_H

/**
 * picolibrary
 *
 * Copyright 2020-2024, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::Testing::Interactive::I2C interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_I2C_H
#define PICOLIBRARY_TESTING_INTERACTIVE_I2C_H

#include <cstdint>

#include "picolibrary/format.h"
#include "picolibrary/i2c.h"
#include "picolibrary/rom.h"
#include "picolibrary/stream.h"

/**
 * \brief I2C interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::I2C {

/**
 * \brief Controller bus scan interactive test helper.
 *
 * \tparam Controller The type of controller used to communicate with devices on the bus.
 *
 * \param[in] stream The output stream to write the scan results to.
 * \param[in] controller The controller used to communicate with devices on the bus.
 */
template<typename Controller>
// NOLINTNEXTLINE(readability-function-size)
void scan( Reliable_Output_Stream & stream, Controller controller ) noexcept
{
    // #lizard forgives the length

    controller.initialize();

    auto devices_found = false;

    ::picolibrary::I2C::scan(
        controller,
        [ &stream, &devices_found ](
            ::picolibrary::I2C::Address_Numeric address, auto operation, auto response ) noexcept {
            if ( response == ::picolibrary::I2C::Response::ACK ) {
                devices_found = true;

                stream.print(
                    PICOLIBRARY_ROM_STRING( "device found: " ),
                    Format::Hex{ static_cast<std::uint8_t>( address.as_unsigned_integer() ) },
                    PICOLIBRARY_ROM_STRING( " (" ),
                    operation == ::picolibrary::I2C::Operation::READ ? 'R' : 'W',
                    PICOLIBRARY_ROM_STRING( ")\n" ) );
            } // if
        } );

    if ( not devices_found ) {
        stream.put( PICOLIBRARY_ROM_STRING( "no devices found\n" ) );
    } // if

    stream.flush();
}

} // namespace picolibrary::Testing::Interactive::I2C

#endif // PICOLIBRARY_TESTING_INTERACTIVE_I2C_H

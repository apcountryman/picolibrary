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
 * \brief picolibrary::Testing::Interactive::Texas_Instruments::TCA9548A interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_TEXAS_INSTRUMENTS_TCA9548A_H
#define PICOLIBRARY_TESTING_INTERACTIVE_TEXAS_INSTRUMENTS_TCA9548A_H

#include <cstdint>
#include <limits>
#include <utility>

#include "picolibrary/error.h"
#include "picolibrary/format.h"
#include "picolibrary/i2c.h"
#include "picolibrary/rom.h"
#include "picolibrary/stream.h"
#include "picolibrary/texas_instruments/tca9548a.h"

/**
 * \brief Texas Instruments TCA9548A interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::Texas_Instruments::TCA9548A {

/**
 * \brief Driver scan interactive test helper.
 *
 * \tparam Controller The type of controller used to communicate with devices on the bus.
 *
 * \param[in] stream The output stream to write the scan results to.
 * \param[in] controller The controller used to communicate with devices on the bus.
 * \param[in] tca9548a_address the TCA9548A's address.
 */
template<typename Controller>
void scan(
    Reliable_Output_Stream &                                        stream,
    Controller                                                      controller,
    ::picolibrary::Texas_Instruments::TCA9548A::Address_Transmitted tca9548a_address ) noexcept
{
    controller.initialize();

    auto tca9548a = ::picolibrary::Texas_Instruments::TCA9548A::Driver{
        I2C::Bus_Multiplexer_Aligner{}, controller, std::move( tca9548a_address ), Generic_Error::NONRESPONSIVE_DEVICE
    };

    for ( auto channel = std::uint_fast8_t{ 0 }; channel < std::numeric_limits<std::uint8_t>::digits;
          ++channel ) {
        tca9548a.write_control( 1 << channel );

        stream.print(
            PICOLIBRARY_ROM_STRING( "channel " ), Format::Dec{ channel }, ":\n" );

        I2C::scan( controller, [ &stream ]( I2C::Address_Numeric address, auto operation, auto response ) noexcept {
            if ( response == I2C::Response::ACK ) {
                stream.print(
                    Format::Hex{ static_cast<std::uint8_t>( address.as_unsigned_integer() ) },
                    PICOLIBRARY_ROM_STRING( " (" ),
                    operation == I2C::Operation::READ ? 'R' : 'W',
                    PICOLIBRARY_ROM_STRING( ")\n" ) );
            } // if
        } );
    } // for

    stream.flush();
}

} // namespace picolibrary::Testing::Interactive::Texas_Instruments::TCA9548A

#endif // PICOLIBRARY_TESTING_INTERACTIVE_TEXAS_INSTRUMENTS_TCA9548A_H

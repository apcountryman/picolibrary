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
 * \brief picolibrary::Testing::Interactive::I2C interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_I2C_H
#define PICOLIBRARY_TESTING_INTERACTIVE_I2C_H

#include <utility>

#include "picolibrary/fatal_error.h"
#include "picolibrary/format.h"
#include "picolibrary/i2c.h"

/**
 * \brief Inter-Integrated Circuit (I2C) interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::I2C {

/**
 * \brief I2C bus scan interactive test helper.
 *
 * \tparam Output_Stream The type of asynchronous serial output stream to use to output
 *         information to the user.
 * \tparam Transmitter The type of asynchronous serial transmitter to use to transmit
 *         information to the user.
 * \tparam Controller The type if I2C controller used to interact with the bus.
 *
 * \param[in] transmitter The asynchronous serial transmitter to use to transmit
 *            information to the user.
 * \param[in] controller The I2C controller for the bus to be scanned.
 */
template<template<typename> typename Output_Stream, typename Transmitter, typename Controller>
void scan( Transmitter transmitter, Controller controller ) noexcept
{
    // #lizard forgives the length

    auto stream = Output_Stream{ std::move( transmitter ) };

    {
        auto const result = stream.initialize();
        if ( result.is_error() ) {
            trap_fatal_error( result.error() );
        } // if
    }

    {
        auto const result = controller.initialize();
        if ( result.is_error() ) {
            auto const print_result = stream.print(
                "controller initialization error: {}\n", result.error() );
            if ( print_result.is_error() ) {
                trap_fatal_error( print_result.error() );
            } // if

            return;
        } // if
    }

    {
        auto devices_found = false;

        auto const result = ::picolibrary::I2C::scan(
            controller, [ &stream, &devices_found ]( auto address, auto operation ) noexcept {
                devices_found = true;

                return stream.print(
                    "device found: {} ({})\n",
                    Format::Hexadecimal{ address.numeric() },
                    operation == ::picolibrary::I2C::Operation::READ ? 'R' : 'W' );
            } );
        if ( result.is_error() ) {
            auto const print_result = stream.print( "scan error: {}\n", result.error() );
            if ( print_result.is_error() ) {
                trap_fatal_error( print_result.error() );
            } // if

            return;
        } // if

        if ( not devices_found ) {
            auto const put_result = stream.put( "no devices found\n" );
            if ( put_result.is_error() ) {
                trap_fatal_error( put_result.error() );
            } // if
        }     // if
    }
}

} // namespace picolibrary::Testing::Interactive::I2C

#endif // PICOLIBRARY_TESTING_INTERACTIVE_I2C_H

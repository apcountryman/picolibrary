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
 * \brief picolibrary::Testing::Interactive::SPI interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_SPI_H
#define PICOLIBRARY_TESTING_INTERACTIVE_SPI_H

#include <cstdint>
#include <utility>

#include "picolibrary/fatal_error.h"
#include "picolibrary/format.h"

/**
 * \brief Serial Peripheral Interface (SPI) interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::SPI {

/**
 * \brief SPI controller echo interactive test helper.
 *
 * \tparam Output_Stream The type of asynchronous serial output stream to use to output
 *         information to the user.
 * \tparam Transmitter The type of asynchronous serial transmitter to use to transmit
 *         information to the user.
 * \tparam Controller The type of SPI controller to test.
 * \tparam Delayer A nullary functor called to introduce a delay each time data is
 *         exchanged.
 *
 * \param[in] transmitter The asynchronous serial transmitter to use to transmit
 *            information to the user.
 * \param[in] controller The SPI controller to test.
 * \param[in] configuration The SPI controller clock, and data exchange bit order
 *            configuration to use for the test.
 * \param[in] delay The nullary functor called to introduce a delay each time data is
 *            exchanged.
 */
template<template<typename> typename Output_Stream, typename Transmitter, typename Controller, typename Delayer>
void echo( Transmitter transmitter, Controller controller, typename Controller::Configuration configuration, Delayer delay ) noexcept
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
        auto const result = controller.configure( configuration );
        if ( result.is_error() ) {
            auto const print_result = stream.print(
                "controller configuration error: {}\n", result.error() );
            if ( print_result.is_error() ) {
                trap_fatal_error( print_result.error() );
            } // if

            return;
        } // if
    }

    for ( auto value = std::uint8_t{};; ++value ) {
        delay();

        auto const result = controller.exchange( value );
        if ( result.is_error() ) {
            auto const print_result = stream.print( "data exchange error: {}\n", result.error() );
            if ( print_result.is_error() ) {
                trap_fatal_error( print_result.error() );
            } // if

            return;
        } // if

        auto const print_result = stream.print(
            "exchange( {} ) -> {}\n",
            Format::Hexadecimal{ value },
            Format::Hexadecimal{ result.value() } );
        if ( print_result.is_error() ) {
            trap_fatal_error( print_result.error() );
        } // if

        if ( result.value() != value ) {
            auto const put_result = stream.put( "echo error\n" );
            if ( put_result.is_error() ) {
                trap_fatal_error( put_result.error() );
            } // if

            return;
        } // if
    }     // for
}

} // namespace picolibrary::Testing::Interactive::SPI

#endif // PICOLIBRARY_TESTING_INTERACTIVE_SPI_H

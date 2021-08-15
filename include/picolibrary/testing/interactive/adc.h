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
 * \brief picolibrary::Testing::Interactive::ADC interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_ADC_H
#define PICOLIBRARY_TESTING_INTERACTIVE_ADC_H

#include <utility>

#include "picolibrary/fatal_error.h"
#include "picolibrary/format.h"
#include "picolibrary/stream.h"

/**
 * \brief Analog-to-Digital Converter (ADC) interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::ADC {

/**
 * \brief Blocking, single sample ADC sample interactive test helper.
 *
 * \tparam Blocking_Single_Sample_Converter The type of blocking, single sample ADC to
 *         test.
 * \tparam Delayer A nullary functor called to introduce a delay each time an ADC sample
 *         is gotten.
 *
 * \param[in] stream The output stream to use to output information to the user.
 * \param[in] adc The blocking, single sample ADC to test.
 * \param[in] delay The nullary functor called to introduce a delay each time an ADC
 *            sample is gotten.
 */
template<typename Blocking_Single_Sample_Converter, typename Delayer>
void sample_blocking_single_sample_converter( Output_Stream & stream, Blocking_Single_Sample_Converter adc, Delayer delay ) noexcept
{
    // #lizard forgives the length

    {
        auto const result = adc.initialize();
        if ( result.is_error() ) {
            auto const print_result = stream.print(
                "ADC initialization error: {}\n", result.error() );
            if ( print_result.is_error() ) {
                trap_fatal_error( print_result.error() );
            } // if

            return;
        } // if
    }

    {
        auto const print_result = stream.print(
            "ADC sample range: [ {}, {} ]\n",
            Format::Decimal{ Blocking_Single_Sample_Converter::Sample::MIN },
            Format::Decimal{ Blocking_Single_Sample_Converter::Sample::MAX } );
        if ( print_result.is_error() ) {
            trap_fatal_error( print_result.error() );
        } // if
    }

    for ( ;; ) {
        delay();

        auto const result = adc.sample();
        if ( result.is_error() ) {
            auto const print_result = stream.print( "ADC sampling error: {}\n", result.error() );
            if ( print_result.is_error() ) {
                trap_fatal_error( print_result.error() );
            } // if

            return;
        } // if

        auto const sample       = result.value();
        auto const print_result = stream.print( "{}\n", Format::Decimal{ sample.value() } );
        if ( print_result.is_error() ) {
            trap_fatal_error( print_result.error() );
        } // if
    }     // for
}

/**
 * \brief Blocking, single sample ADC sample interactive test helper.
 *
 * \tparam Output_Stream The type of asynchronous serial output stream to use to output
 *         information to the user.
 * \tparam Transmitter The type of asynchronous serial transmitter to use to transmit
 *         information to the user.
 * \tparam Blocking_Single_Sample_Converter The type of blocking, single sample ADC to
 *         test.
 * \tparam Delayer A nullary functor called to introduce a delay each time an ADC sample
 *         is gotten.
 *
 * \param[in] transmitter The asynchronous serial transmitter to use to transmit
 *            information to the user.
 * \param[in] adc The blocking, single sample ADC to test.
 * \param[in] delay The nullary functor called to introduce a delay each time an ADC
 *            sample is gotten.
 */
template<template<typename> typename Output_Stream, typename Transmitter, typename Blocking_Single_Sample_Converter, typename Delayer>
void sample_blocking_single_sample_converter( Transmitter transmitter, Blocking_Single_Sample_Converter adc, Delayer delay ) noexcept
{
    auto stream = Output_Stream{ std::move( transmitter ) };

    {
        auto const result = stream.initialize();
        if ( result.is_error() ) {
            trap_fatal_error( result.error() );
        } // if
    }

    sample_blocking_single_sample_converter( stream, std::move( adc ), std::move( delay ) );
}

} // namespace picolibrary::Testing::Interactive::ADC

#endif // PICOLIBRARY_TESTING_INTERACTIVE_ADC_H

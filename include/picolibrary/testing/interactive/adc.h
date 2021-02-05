/**
 * picolibrary
 *
 * Copyright 2021 Andrew Countryman <apcountryman@gmail.com>
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

#include "picolibrary/format.h"

/**
 * \brief Analog-to-Digital Converter (ADC) interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::ADC {

/**
 * \brief Blocking, single sample ADC sample interactive test helper.
 *
 * \tparam Output_Stream The type of asynchronous serial output stream to use.
 * \tparam Transmitter The type of asynchronous serial transmitter to use.
 * \tparam Blocking_Single_Sample_Converter The type of blocking, single sample ADC to
 *         test.
 * \tparam Delayer A nullary functor called to introduce a delay each time an ADC sample
 *         is gotten.
 *
 * \param[in] transmitter The transmitter to use.
 * \param[in] adc The blocking, single sample ADC to test.
 * \param[in] delay The nullary functor called to introduce a delay each time an ADC
 *            sample is gotten.
 */
template<template<typename> typename Output_Stream, typename Transmitter, typename Blocking_Single_Sample_Converter, typename Delayer>
void sample_blocking_single_sample_converter( Transmitter transmitter, Blocking_Single_Sample_Converter adc, Delayer delay ) noexcept
{
    // #lizard forgives the length

    auto stream = Output_Stream{ std::move( transmitter ) };

    if ( stream.initialize().is_error() ) { return; } // if

    {
        auto const result = adc.initialize();
        if ( result.is_error() ) {
            static_cast<void>( stream.print( "ADC initialization error: {}\n", result.error() ) );

            return;
        } // if
    }

    if ( stream
             .print(
                 "ADC sample range: [ {}, {} ]\n",
                 Format::Decimal{ Blocking_Single_Sample_Converter::Sample::MIN },
                 Format::Decimal{ Blocking_Single_Sample_Converter::Sample::MAX } )
             .is_error() ) {
        return;
    } // if

    for ( ;; ) {
        delay();

        auto const result = adc.sample();
        if ( result.is_error() ) {
            static_cast<void>( stream.print( "ADC sampling error: {}\n", result.error() ) );

            return;
        } // if

        if ( stream.print( "{}\n", Format::Decimal{ result.value() } ).is_error() ) {
            return;
        } // if
    }     // for
}

} // namespace picolibrary::Testing::Interactive::ADC

#endif // PICOLIBRARY_TESTING_INTERACTIVE_ADC_H

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
 * \brief picolibrary::Testing::Interactive::ADC interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_ADC_H
#define PICOLIBRARY_TESTING_INTERACTIVE_ADC_H

#include "picolibrary/format.h"
#include "picolibrary/stream.h"

/**
 * \brief Analog-to-Digital Converter (ADC) interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::ADC {

/**
 * \brief Blocking, single sample ADC sample interactive test helper.
 *
 * \tparam Blocking_Single_Sample_Converter The type of blocking, single sample ADC to use
 *         to get the samples.
 * \tparam Delayer A nullary functor called to introduce a delay each time a sample is
 *         gotten.
 *
 * \param[in] stream The output stream to write the samples to.
 * \param[in] adc The ADC to use to get the samples.
 * \param[in] delay The nullary functor to call to introduce a delay each time a sample is
 *            gotten.
 */
template<typename Blocking_Single_Sample_Converter, typename Delayer>
void sample_blocking_single_sample_converter( Reliable_Output_Stream & stream, Blocking_Single_Sample_Converter adc, Delayer delay ) noexcept
{
    adc.initialize();

    stream.print(
        "ADC sample range: [",
        Format::Dec{ Blocking_Single_Sample_Converter::Sample::min().as_unsigned_integer() },
        ',',
        Format::Dec{ Blocking_Single_Sample_Converter::Sample::max().as_unsigned_integer() },
        "]\n" );

    for ( ;; ) {
        delay();

        stream.print( Format::Dec{ adc.sample().as_unsigned_integer() }, '\n' );

        stream.flush();
    } // for
}

} // namespace picolibrary::Testing::Interactive::ADC

#endif // PICOLIBRARY_TESTING_INTERACTIVE_ADC_H

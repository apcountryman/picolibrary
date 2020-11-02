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
 * \brief picolibrary::ADC interface.
 */

#ifndef PICOLIBRARY_ADC_H
#define PICOLIBRARY_ADC_H

#include <cstdint>

#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/utility.h"

/**
 * \brief Analog-to-Digital Converter (ADC) facilities.
 */
namespace picolibrary::ADC {

/**
 * \brief Blocking, single sample ADC concept.
 */
class Blocking_Single_Sample_Converter_Concept {
  public:
    /**
     * \brief The integral type used to hold a sample.
     */
    using Sample = std::uint16_t;

    Blocking_Single_Sample_Converter_Concept() = delete;

    /**
     * \todo #29
     */
    Blocking_Single_Sample_Converter_Concept( Blocking_Single_Sample_Converter_Concept && ) = delete;

    /**
     * \todo #29
     */
    Blocking_Single_Sample_Converter_Concept( Blocking_Single_Sample_Converter_Concept const & ) = delete;

    ~Blocking_Single_Sample_Converter_Concept() = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Blocking_Single_Sample_Converter_Concept && ) = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Blocking_Single_Sample_Converter_Concept const & ) = delete;

    /**
     * \brief Initialize the ADC's hardware.
     *
     * \return Nothing if initializing the ADC's hardware succeeded.
     * \return An error code if initializing the ADC's hardware failed. If initializing
     *         the ADC's hardware cannot fail, return
     *         picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto initialize() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Get the minimum sample value.
     *
     * \return The minimum sample value.
     */
    auto min() const noexcept -> Sample;

    /**
     * \brief Get the maximum sample value.
     *
     * \return The maximum sample value.
     */
    auto max() const noexcept -> Sample;

    /**
     * \brief Get a sample.
     *
     * \return A sample if getting the sample succeeded.
     * \return An error code if getting the sample failed. If getting the sample cannot
     *         fail, return picolibrary::Result<Sample, picolibrary::Void>.
     */
    auto sample() noexcept -> Result<Sample, Error_Code>;
};

} // namespace picolibrary::ADC

#endif // PICOLIBRARY_ADC_H

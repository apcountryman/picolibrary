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
 * \brief picolibrary::Testing::Unit::ADC interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_ADC_H
#define PICOLIBRARY_TESTING_UNIT_ADC_H

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/utility.h"

/**
 * \brief Analog-to-Digital Converter (ADC) unit testing facilities.
 */
namespace picolibrary::Testing::Unit::ADC {

/**
 * \brief Mock blocking, single sample ADC.
 *
 * \tparam Sample_Type The integral type used to hold a sample.
 */
template<typename Sample_Type>
class Mock_Blocking_Single_Sample_Converter {
  public:
    /**
     * \copydoc picolibrary::ADC::Blocking_Single_Sample_Converter_Concept::Sample
     */
    using Sample = Sample_Type;

    /**
     * \brief Movable mock blocking, single sample ADC handle.
     */
    class Handle {
      public:
        /**
         * \copydoc picolibrary::ADC::Blocking_Single_Sample_Converter_Concept::Sample
         */
        using Sample = Sample_Type;

        /**
         * \brief Constructor.
         */
        Handle() noexcept = default;

        /**
         * \brief Constructor.
         *
         * \param[in] mock_blocking_single_sample_converter The mock blocking, single
         *            sample ADC.
         */
        Handle( Mock_Blocking_Single_Sample_Converter & mock_blocking_single_sample_converter ) noexcept :
            m_mock_blocking_single_sample_converter{ &mock_blocking_single_sample_converter }
        {
        }

        /**
         * \brief Constructor.
         *
         * \param[in] source The source of the move.
         */
        Handle( Handle && source ) noexcept = default;

        /**
         * \todo #29
         */
        Handle( Handle const & ) = delete;

        /**
         * \brief Destructor.
         */
        ~Handle() noexcept = default;

        /**
         * \brief Assignment operator.
         *
         * \param[in] expression The expression to be assigned.
         *
         * \return The assigned to object.
         */
        auto operator=( Handle && expression ) noexcept -> Handle & = default;

        /**
         * \todo #29
         *
         * \return
         */
        auto operator=( Handle const & ) = delete;

        /**
         * \brief Initialize the ADC's hardware.
         *
         * \return Nothing if initializing the ADC's hardware succeeded.
         * \return An error code if initializing the ADC's hardware failed.
         */
        auto initialize()
        {
            return m_mock_blocking_single_sample_converter->initialize();
        }

        /**
         * \copydoc picolibrary::ADC::Blocking_Single_Sample_Converter_Concept::min()
         */
        auto min() const noexcept
        {
            return m_mock_blocking_single_sample_converter->min();
        }

        /**
         * \copydoc picolibrary::ADC::Blocking_Single_Sample_Converter_Concept::max()
         */
        auto max() const noexcept
        {
            return m_mock_blocking_single_sample_converter->max();
        }

        /**
         * \brief Get a sample.
         *
         * \return A sample if getting the sample succeeded.
         * \return An error code if getting the sample failed.
         */
        auto sample() noexcept
        {
            return m_mock_blocking_single_sample_converter->sample();
        }

      private:
        /**
         * \brief The mock blocking, single sample ADC.
         */
        Mock_Blocking_Single_Sample_Converter * m_mock_blocking_single_sample_converter{};
    };

    /**
     * \brief Constructor.
     */
    Mock_Blocking_Single_Sample_Converter() = default;

    /**
     * \todo #29
     */
    Mock_Blocking_Single_Sample_Converter( Mock_Blocking_Single_Sample_Converter && ) = default;

    /**
     * \todo #29
     */
    Mock_Blocking_Single_Sample_Converter( Mock_Blocking_Single_Sample_Converter const & ) = default;

    /**
     * \brief Destructor.
     */
    ~Mock_Blocking_Single_Sample_Converter() = default;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Mock_Blocking_Single_Sample_Converter && ) = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Mock_Blocking_Single_Sample_Converter const & ) = delete;

    /**
     * \brief Get a movable handle to the mock blocking, single sample ADC.
     *
     * \return A movable handle to the mock blocking, single sample ADC.
     */
    auto handle() noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );

    MOCK_METHOD( Sample, min, (), ( const ) );

    MOCK_METHOD( Sample, max, (), ( const ) );

    MOCK_METHOD( (Result<Sample, Error_Code>), sample, () );
};

} // namespace picolibrary::Testing::Unit::ADC

#endif // PICOLIBRARY_TESTING_UNIT_ADC_H

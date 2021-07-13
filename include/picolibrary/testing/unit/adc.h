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
 * \brief picolibrary::Testing::Unit::ADC interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_ADC_H
#define PICOLIBRARY_TESTING_UNIT_ADC_H

#include "gmock/gmock.h"
#include "picolibrary/adc.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/void.h"

/**
 * \brief Analog-to-Digital Converter (ADC) unit testing facilities.
 */
namespace picolibrary::Testing::Unit::ADC {

/**
 * \brief Mock blocking, single sample ADC.
 *
 * \tparam Value_Type The integral type used to hold a sample value.
 * \tparam MIN_SAMPLE The minimum possible sample value.
 * \tparam MAX_SAMPLE The maximum possible sample value.
 */
template<typename Value_Type, Value_Type MIN_SAMPLE, Value_Type MAX_SAMPLE>
class Mock_Blocking_Single_Sample_Converter {
  public:
    /**
     * \copydoc picolibrary::ADC::Blocking_Single_Sample_Converter_Concept::Sample
     */
    using Sample = ::picolibrary::ADC::Sample<Value_Type, MIN_SAMPLE, MAX_SAMPLE>;

    /**
     * \brief Movable mock blocking, single sample ADC handle.
     */
    class Handle {
      public:
        /**
         * \copydoc picolibrary::ADC::Blocking_Single_Sample_Converter_Concept::Sample
         */
        using Sample = ::picolibrary::ADC::Sample<Value_Type, MIN_SAMPLE, MAX_SAMPLE>;

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
        Handle( Handle && source ) noexcept :
            m_mock_blocking_single_sample_converter{ source.m_mock_blocking_single_sample_converter }
        {
            source.m_mock_blocking_single_sample_converter = nullptr;
        }

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
        auto & operator=( Handle && expression ) noexcept
        {
            if ( &expression != this ) {
                m_mock_blocking_single_sample_converter = expression.m_mock_blocking_single_sample_converter;

                expression.m_mock_blocking_isngle_sample_converter = nullptr;
            } // if

            return *this;
        }

        auto operator=( Handle const & ) = delete;

        /**
         * \brief Get the mock blocking single sample converter.
         *
         * \return The mock blocking single sample converter.
         */
        auto & mock() noexcept
        {
            return *m_mock_blocking_single_sample_converter;
        }

        /**
         * \brief Initialize the ADC's hardware.
         *
         * \return Nothing if ADC hardware initialization succeeded.
         * \return An error code if ADC hardware initialization failed.
         */
        auto initialize()
        {
            return m_mock_blocking_single_sample_converter->initialize();
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

    Mock_Blocking_Single_Sample_Converter( Mock_Blocking_Single_Sample_Converter && ) = delete;

    Mock_Blocking_Single_Sample_Converter( Mock_Blocking_Single_Sample_Converter const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Blocking_Single_Sample_Converter() noexcept = default;

    auto operator=( Mock_Blocking_Single_Sample_Converter && ) = delete;

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

    MOCK_METHOD( (Result<Sample, Error_Code>), sample, () );
};

/**
 * \brief Mock non-blocking, single sample ADC.
 *
 * \tparam Value_Type The integral type used to hold a sample value.
 * \tparam MIN_SAMPLE The minimum possible sample value.
 * \tparam MAX_SAMPLE The maximum possible sample value.
 */
template<typename Value_Type, Value_Type MIN_SAMPLE, Value_Type MAX_SAMPLE>
class Mock_Non_Blocking_Single_Sample_Converter {
  public:
    /**
     * \copydoc picolibrary::ADC::Non_Blocking_Single_Sample_Converter_Concept::Sample
     */
    using Sample = ::picolibrary::ADC::Sample<Value_Type, MIN_SAMPLE, MAX_SAMPLE>;

    /**
     * \brief Movable mock non-blocking, single sample ADC handle.
     */
    class Handle {
      public:
        /**
         * \copydoc picolibrary::ADC::Non_Blocking_Single_Sample_Converter_Concept::Sample
         */
        using Sample = ::picolibrary::ADC::Sample<Value_Type, MIN_SAMPLE, MAX_SAMPLE>;

        /**
         * \brief Constructor.
         */
        Handle() noexcept = default;

        /**
         * \brief Constructor.
         *
         * \param[in] mock_non_blocking_single_sample_converter The mock non-blocking,
         *            single sample ADC.
         */
        Handle( Mock_Non_Blocking_Single_Sample_Converter & mock_non_blocking_single_sample_converter ) noexcept
            :
            m_mock_non_blocking_single_sample_converter{ &mock_non_blocking_single_sample_converter }
        {
        }

        /**
         * \brief Constructor.
         *
         * \param[in] source The source of the move.
         */
        Handle( Handle && source ) noexcept :
            m_mock_non_blocking_single_sample_converter{ source.m_mock_non_blocking_single_sample_converter }
        {
            source.m_mock_non_blocking_single_sample_converter = nullptr;
        }

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
        auto & operator=( Handle && expression ) noexcept
        {
            if ( &expression != this ) {
                m_mock_non_blocking_single_sample_converter = expression.m_mock_non_blocking_single_sample_converter;

                expression.m_mock_non_blocking_isngle_sample_converter = nullptr;
            } // if

            return *this;
        }

        auto operator=( Handle const & ) = delete;

        /**
         * \brief Get the mock non-blocking single sample converter.
         *
         * \return The mock non-blocking single sample converter.
         */
        auto & mock() noexcept
        {
            return *m_mock_non_blocking_single_sample_converter;
        }

        /**
         * \brief Initialize the ADC's hardware.
         *
         * \return Nothing if ADC hardware initialization succeeded.
         * \return An error code if ADC hardware initialization failed.
         */
        auto initialize()
        {
            return m_mock_non_blocking_single_sample_converter->initialize();
        }

        /**
         * \brief Initiate a conversion.
         *
         * \return Nothing if initiation of the conversion succeeded.
         * \return An error code if initiation of the conversion failed.
         */
        auto initiate_conversion() noexcept
        {
            return m_mock_non_blocking_single_sample_converter->initiate_conversion();
        }

        /**
         * \brief Check if a sample is available.
         *
         * \return true if a sample is available.
         * \return false if a sample is not available.
         * \return An error code if the check failed.
         */
        auto sample_available() const noexcept
        {
            return m_mock_non_blocking_single_sample_converter->sample_available();
        }

        /**
         * \brief Get a sample.
         *
         * \return A sample if getting the sample succeeded.
         * \return An error code if getting the sample failed.
         */
        auto sample() noexcept
        {
            return m_mock_non_blocking_single_sample_converter->sample();
        }

      private:
        /**
         * \brief The mock non-blocking, single sample ADC.
         */
        Mock_Non_Blocking_Single_Sample_Converter * m_mock_non_blocking_single_sample_converter{};
    };

    /**
     * \brief Constructor.
     */
    Mock_Non_Blocking_Single_Sample_Converter() = default;

    Mock_Non_Blocking_Single_Sample_Converter( Mock_Non_Blocking_Single_Sample_Converter && ) = delete;

    Mock_Non_Blocking_Single_Sample_Converter( Mock_Non_Blocking_Single_Sample_Converter const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Non_Blocking_Single_Sample_Converter() noexcept = default;

    auto operator=( Mock_Non_Blocking_Single_Sample_Converter && ) = delete;

    auto operator=( Mock_Non_Blocking_Single_Sample_Converter const & ) = delete;

    /**
     * \brief Get a movable handle to the mock non-blocking, single sample ADC.
     *
     * \return A movable handle to the mock non-blocking, single sample ADC.
     */
    auto handle() noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );

    MOCK_METHOD( (Result<Void, Error_Code>), initiate_conversion, () );

    MOCK_METHOD( (Result<bool, Error_Code>), sample_available, (), ( const ) );

    MOCK_METHOD( (Result<Sample, Error_Code>), sample, () );
};

/**
 * \brief Mock blocking, free running ADC.
 *
 * \tparam Value_Type The integral type used to hold a sample value.
 * \tparam MIN_SAMPLE The minimum possible sample value.
 * \tparam MAX_SAMPLE The maximum possible sample value.
 */
template<typename Value_Type, Value_Type MIN_SAMPLE, Value_Type MAX_SAMPLE>
class Mock_Blocking_Free_Running_Converter {
  public:
    /**
     * \copydoc picolibrary::ADC::Blocking_Free_Running_Converter_Concept::Sample
     */
    using Sample = ::picolibrary::ADC::Sample<Value_Type, MIN_SAMPLE, MAX_SAMPLE>;

    /**
     * \brief Movable mock blocking, free running ADC handle.
     */
    class Handle {
      public:
        /**
         * \copydoc picolibrary::ADC::Blocking_Free_Running_Converter_Concept::Sample
         */
        using Sample = ::picolibrary::ADC::Sample<Value_Type, MIN_SAMPLE, MAX_SAMPLE>;

        /**
         * \brief Constructor.
         */
        Handle() noexcept = default;

        /**
         * \brief Constructor.
         *
         * \param[in] mock_blocking_free_running_converter The mock blocking, free running
         *            ADC.
         */
        Handle( Mock_Blocking_Free_Running_Converter & mock_blocking_free_running_converter ) noexcept :
            m_mock_blocking_free_running_converter{ &mock_blocking_free_running_converter }
        {
        }

        /**
         * \brief Constructor.
         *
         * \param[in] source The source of the move.
         */
        Handle( Handle && source ) noexcept :
            m_mock_blocking_free_running_converter{ source.m_mock_blocking_free_running_converter }
        {
            source.m_mock_blocking_free_running_converter = nullptr;
        }

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
        auto & operator=( Handle && expression ) noexcept
        {
            if ( &expression != this ) {
                m_mock_blocking_free_running_converter = expression.m_mock_blocking_free_running_converter;

                expression.m_mock_blocking_isngle_sample_converter = nullptr;
            } // if

            return *this;
        }

        auto operator=( Handle const & ) = delete;

        /**
         * \brief Get the mock blocking, free running converter.
         *
         * \return The mock blocking, free running converter.
         */
        auto & mock() noexcept
        {
            return *m_mock_blocking_free_running_converter;
        }

        /**
         * \brief Initialize the ADC's hardware.
         *
         * \return Nothing if ADC hardware initialization succeeded.
         * \return An error code if ADC hardware initialization failed.
         */
        auto initialize()
        {
            return m_mock_blocking_free_running_converter->initialize();
        }

        /**
         * \brief Start the ADC.
         *
         * \return Nothing if starting the ADC succeeded.
         * \return An error code if starting the ADC failed.
         */
        auto start_converter()
        {
            return m_mock_blocking_free_running_converter->start_converter();
        }

        /**
         * \brief Stop the ADC.
         *
         * \return Nothing if stopping the ADC succeeded.
         * \return An error code if stopping the ADC failed.
         */
        auto stop_converter()
        {
            return m_mock_blocking_free_running_converter->stop_converter();
        }

        /**
         * \brief Get a sample.
         *
         * \return A sample if getting the sample succeeded.
         * \return An error code if getting the sample failed.
         */
        auto sample() noexcept
        {
            return m_mock_blocking_free_running_converter->sample();
        }

      private:
        /**
         * \brief The mock blocking, free running ADC.
         */
        Mock_Blocking_Free_Running_Converter * m_mock_blocking_free_running_converter{};
    };

    /**
     * \brief Constructor.
     */
    Mock_Blocking_Free_Running_Converter() = default;

    Mock_Blocking_Free_Running_Converter( Mock_Blocking_Free_Running_Converter && ) = delete;

    Mock_Blocking_Free_Running_Converter( Mock_Blocking_Free_Running_Converter const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Blocking_Free_Running_Converter() noexcept = default;

    auto operator=( Mock_Blocking_Free_Running_Converter && ) = delete;

    auto operator=( Mock_Blocking_Free_Running_Converter const & ) = delete;

    /**
     * \brief Get a movable handle to the mock blocking, free running ADC.
     *
     * \return A movable handle to the mock blocking, free running ADC.
     */
    auto handle() noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );

    MOCK_METHOD( (Result<Void, Error_Code>), start_converter, () );

    MOCK_METHOD( (Result<Void, Error_Code>), stop_converter, () );

    MOCK_METHOD( (Result<Sample, Error_Code>), sample, () );
};

} // namespace picolibrary::Testing::Unit::ADC

#endif // PICOLIBRARY_TESTING_UNIT_ADC_H

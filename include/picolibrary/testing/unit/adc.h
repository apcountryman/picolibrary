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
#include "picolibrary/testing/unit/mock_handle.h"
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
    using Sample = ::picolibrary::ADC::Sample<Value_Type, MIN_SAMPLE, MAX_SAMPLE>;

    class Handle : public Mock_Handle<Mock_Blocking_Single_Sample_Converter> {
      public:
        using Sample = Mock_Blocking_Single_Sample_Converter::Sample;

        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Blocking_Single_Sample_Converter & mock ) noexcept :
            Mock_Handle<Mock_Blocking_Single_Sample_Converter>{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        auto initialize()
        {
            return Mock_Handle<Mock_Blocking_Single_Sample_Converter>::mock().initialize();
        }

        auto sample()
        {
            return Mock_Handle<Mock_Blocking_Single_Sample_Converter>::mock().sample();
        }
    };

    Mock_Blocking_Single_Sample_Converter() = default;

    Mock_Blocking_Single_Sample_Converter( Mock_Blocking_Single_Sample_Converter && ) = delete;

    Mock_Blocking_Single_Sample_Converter( Mock_Blocking_Single_Sample_Converter const & ) = delete;

    ~Mock_Blocking_Single_Sample_Converter() noexcept = default;

    auto operator=( Mock_Blocking_Single_Sample_Converter && ) = delete;

    auto operator=( Mock_Blocking_Single_Sample_Converter const & ) = delete;

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
    using Sample = ::picolibrary::ADC::Sample<Value_Type, MIN_SAMPLE, MAX_SAMPLE>;

    class Handle : public Mock_Handle<Mock_Non_Blocking_Single_Sample_Converter> {
      public:
        using Sample = Mock_Non_Blocking_Single_Sample_Converter::Sample;

        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Non_Blocking_Single_Sample_Converter & mock ) noexcept :
            Mock_Handle<Mock_Non_Blocking_Single_Sample_Converter>{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        auto initialize()
        {
            return Mock_Handle<Mock_Non_Blocking_Single_Sample_Converter>::mock().initialize();
        }

        auto initiate_conversion()
        {
            return Mock_Handle<Mock_Non_Blocking_Single_Sample_Converter>::mock().initiate_conversion();
        }

        auto sample_available() const
        {
            return Mock_Handle<Mock_Non_Blocking_Single_Sample_Converter>::mock().sample_available();
        }

        auto sample()
        {
            return Mock_Handle<Mock_Non_Blocking_Single_Sample_Converter>::mock().sample();
        }
    };

    Mock_Non_Blocking_Single_Sample_Converter() = default;

    Mock_Non_Blocking_Single_Sample_Converter( Mock_Non_Blocking_Single_Sample_Converter && ) = delete;

    Mock_Non_Blocking_Single_Sample_Converter( Mock_Non_Blocking_Single_Sample_Converter const & ) = delete;

    ~Mock_Non_Blocking_Single_Sample_Converter() noexcept = default;

    auto operator=( Mock_Non_Blocking_Single_Sample_Converter && ) = delete;

    auto operator=( Mock_Non_Blocking_Single_Sample_Converter const & ) = delete;

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
    using Sample = ::picolibrary::ADC::Sample<Value_Type, MIN_SAMPLE, MAX_SAMPLE>;

    class Handle : public Mock_Handle<Mock_Blocking_Free_Running_Converter> {
      public:
        using Sample = Mock_Blocking_Free_Running_Converter::Sample;

        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Blocking_Free_Running_Converter & mock ) noexcept :
            Mock_Handle<Mock_Blocking_Free_Running_Converter>{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        auto initialize()
        {
            return Mock_Handle<Mock_Blocking_Free_Running_Converter>::mock().initialize();
        }

        auto start_converter()
        {
            return Mock_Handle<Mock_Blocking_Free_Running_Converter>::mock().start_converter();
        }

        auto stop_converter()
        {
            return Mock_Handle<Mock_Blocking_Free_Running_Converter>::mock().stop_converter();
        }

        auto sample()
        {
            return Mock_Handle<Mock_Blocking_Free_Running_Converter>::mock().sample();
        }
    };

    Mock_Blocking_Free_Running_Converter() = default;

    Mock_Blocking_Free_Running_Converter( Mock_Blocking_Free_Running_Converter && ) = delete;

    Mock_Blocking_Free_Running_Converter( Mock_Blocking_Free_Running_Converter const & ) = delete;

    ~Mock_Blocking_Free_Running_Converter() noexcept = default;

    auto operator=( Mock_Blocking_Free_Running_Converter && ) = delete;

    auto operator=( Mock_Blocking_Free_Running_Converter const & ) = delete;

    auto handle() noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );

    MOCK_METHOD( (Result<Void, Error_Code>), start_converter, () );
    MOCK_METHOD( (Result<Void, Error_Code>), stop_converter, () );
    MOCK_METHOD( (Result<Sample, Error_Code>), sample, () );
};

/**
 * \brief Mock non-blocking, free running ADC.
 *
 * \tparam Value_Type The integral type used to hold a sample value.
 * \tparam MIN_SAMPLE The minimum possible sample value.
 * \tparam MAX_SAMPLE The maximum possible sample value.
 */
template<typename Value_Type, Value_Type MIN_SAMPLE, Value_Type MAX_SAMPLE>
class Mock_Non_Blocking_Free_Running_Converter {
  public:
    using Sample = ::picolibrary::ADC::Sample<Value_Type, MIN_SAMPLE, MAX_SAMPLE>;

    class Handle : public Mock_Handle<Mock_Non_Blocking_Free_Running_Converter> {
      public:
        using Sample = Mock_Non_Blocking_Free_Running_Converter::Sample;

        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Non_Blocking_Free_Running_Converter & mock ) noexcept :
            Mock_Handle<Mock_Non_Blocking_Free_Running_Converter>{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        auto initialize()
        {
            return Mock_Handle<Mock_Non_Blocking_Free_Running_Converter>::mock().initialize();
        }

        auto start_converter()
        {
            return Mock_Handle<Mock_Non_Blocking_Free_Running_Converter>::mock().start_converter();
        }

        auto stop_converter()
        {
            return Mock_Handle<Mock_Non_Blocking_Free_Running_Converter>::mock().stop_converter();
        }

        auto sample_available() const
        {
            return Mock_Handle<Mock_Non_Blocking_Free_Running_Converter>::mock().sample_available();
        }

        auto sample()
        {
            return Mock_Handle<Mock_Non_Blocking_Free_Running_Converter>::mock().sample();
        }
    };

    Mock_Non_Blocking_Free_Running_Converter() = default;

    Mock_Non_Blocking_Free_Running_Converter( Mock_Non_Blocking_Free_Running_Converter && ) = delete;

    Mock_Non_Blocking_Free_Running_Converter( Mock_Non_Blocking_Free_Running_Converter const & ) = delete;

    ~Mock_Non_Blocking_Free_Running_Converter() noexcept = default;

    auto operator=( Mock_Non_Blocking_Free_Running_Converter && ) = delete;

    auto operator=( Mock_Non_Blocking_Free_Running_Converter const & ) = delete;

    auto handle() noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );

    MOCK_METHOD( (Result<Void, Error_Code>), start_converter, () );
    MOCK_METHOD( (Result<Void, Error_Code>), stop_converter, () );
    MOCK_METHOD( (Result<bool, Error_Code>), sample_available, (), ( const ) );
    MOCK_METHOD( (Result<Sample, Error_Code>), sample, () );
};

} // namespace picolibrary::Testing::Unit::ADC

#endif // PICOLIBRARY_TESTING_UNIT_ADC_H

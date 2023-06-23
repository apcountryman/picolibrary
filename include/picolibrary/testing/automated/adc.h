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
 * \brief picolibrary::Testing::Automated::ADC interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_ADC_H
#define PICOLIBRARY_TESTING_AUTOMATED_ADC_H

#include <cstdint>
#include <ios>
#include <limits>
#include <ostream>

#include "gmock/gmock.h"
#include "picolibrary/adc.h"
#include "picolibrary/testing/automated/mock_handle.h"

namespace picolibrary::ADC {

/**
 * \brief Insertion operator.
 *
 * \tparam T The sample unsigned integer representation.
 * \tparam N The number of bits in the sample.
 *
 * \param[in] stream The stream to write the picolibrary::ADC::Sample to.
 * \param[in] sample The picolibrary::ADC::Sample to write to the stream.
 */
template<typename T, std::uint_fast8_t N>
auto operator<<( std::ostream & stream, Sample<T, N> sample ) -> std::ostream &
{
    static_assert( N <= std::numeric_limits<std::uint_fast32_t>::digits );

    return stream << std::dec << static_cast<std::uint_fast32_t>( sample.as_unsigned_integer() );
}

} // namespace picolibrary::ADC

/**
 * \brief ADC automated testing facilities.
 */
namespace picolibrary::Testing::Automated::ADC {

/**
 * \brief Mock blocking, single sample ADC.
 *
 * \tparam T The sample unsigned integer representation.
 * \tparam N The number of bits in the sample.
 */
template<typename T, std::uint_fast8_t N>
class Mock_Blocking_Single_Sample_Converter {
  public:
    using Sample = ::picolibrary::ADC::Sample<T, N>;

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

        void initialize()
        {
            Mock_Handle<Mock_Blocking_Single_Sample_Converter>::mock().initialize();
        }

        auto sample() -> Sample
        {
            return Mock_Handle<Mock_Blocking_Single_Sample_Converter>::mock.sample();
        }
    };

    Mock_Blocking_Single_Sample_Converter() = default;

    Mock_Blocking_Single_Sample_Converter( Mock_Blocking_Single_Sample_Converter && ) = delete;

    Mock_Blocking_Single_Sample_Converter( Mock_Blocking_Single_Sample_Converter const & ) = delete;

    ~Mock_Blocking_Single_Sample_Converter() noexcept = default;

    auto operator=( Mock_Blocking_Single_Sample_Converter && ) = delete;

    auto operator=( Mock_Blocking_Single_Sample_Converter const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    MOCK_METHOD( void, initialize, () );

    MOCK_METHOD( Sample, sample, () );
};

/**
 * \brief Mock non-blocking, single sample ADC.
 *
 * \tparam T The sample unsigned integer representation.
 * \tparam N The number of bits in the sample.
 */
template<typename T, std::uint_fast8_t N>
class Mock_Non_Blocking_Single_Sample_Converter {
  public:
    using Sample = ::picolibrary::ADC::Sample<T, N>;

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

        void initialize()
        {
            Mock_Handle<Mock_Non_Blocking_Single_Sample_Converter>::mock().initialize();
        }

        void initiate_conversion()
        {
            Mock_Handle<Mock_Non_Blocking_Single_Sample_Converter>::mock().initiate_conversion();
        }

        auto sample_is_available() const -> bool
        {
            return Mock_Handle<Mock_Non_Blocking_Single_Sample_Converter>::mock.sample_is_available();
        }

        auto sample() -> Sample
        {
            return Mock_Handle<Mock_Non_Blocking_Single_Sample_Converter>::mock.sample();
        }
    };

    Mock_Non_Blocking_Single_Sample_Converter() = default;

    Mock_Non_Blocking_Single_Sample_Converter( Mock_Non_Blocking_Single_Sample_Converter && ) = delete;

    Mock_Non_Blocking_Single_Sample_Converter( Mock_Non_Blocking_Single_Sample_Converter const & ) = delete;

    ~Mock_Non_Blocking_Single_Sample_Converter() noexcept = default;

    auto operator=( Mock_Non_Blocking_Single_Sample_Converter && ) = delete;

    auto operator=( Mock_Non_Blocking_Single_Sample_Converter const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    MOCK_METHOD( void, initialize, () );

    MOCK_METHOD( void, initiate_conversion, () );
    MOCK_METHOD( bool, sample_is_available, (), ( const ) );
    MOCK_METHOD( Sample, sample, () );
};

/**
 * \brief Mock blocking, free running ADC.
 *
 * \tparam T The sample unsigned integer representation.
 * \tparam N The number of bits in the sample.
 */
template<typename T, std::uint_fast8_t N>
class Mock_Blocking_Free_Running_Converter {
  public:
    using Sample = ::picolibrary::ADC::Sample<T, N>;

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

        void initialize()
        {
            Mock_Handle<Mock_Blocking_Free_Running_Converter>::mock().initialize();
        }

        void start_converter()
        {
            Mock_Handle<Mock_Blocking_Free_Running_Converter>::mock().start_converter();
        }

        void stop_converter()
        {
            Mock_Handle<Mock_Blocking_Free_Running_Converter>::mock().stop_converter();
        }

        auto sample() -> Sample
        {
            return Mock_Handle<Mock_Blocking_Free_Running_Converter>::mock.sample();
        }
    };

    Mock_Blocking_Free_Running_Converter() = default;

    Mock_Blocking_Free_Running_Converter( Mock_Blocking_Free_Running_Converter && ) = delete;

    Mock_Blocking_Free_Running_Converter( Mock_Blocking_Free_Running_Converter const & ) = delete;

    ~Mock_Blocking_Free_Running_Converter() noexcept = default;

    auto operator=( Mock_Blocking_Free_Running_Converter && ) = delete;

    auto operator=( Mock_Blocking_Free_Running_Converter const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    MOCK_METHOD( void, initialize, () );

    MOCK_METHOD( void, start_converter, () );
    MOCK_METHOD( void, stop_converter, () );
    MOCK_METHOD( Sample, sample, () );
};

/**
 * \brief Mock non-blocking, free running ADC.
 *
 * \tparam T The sample unsigned integer representation.
 * \tparam N The number of bits in the sample.
 */
template<typename T, std::uint_fast8_t N>
class Mock_Non_Blocking_Free_Running_Converter {
  public:
    using Sample = ::picolibrary::ADC::Sample<T, N>;

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

        void initialize()
        {
            Mock_Handle<Mock_Non_Blocking_Free_Running_Converter>::mock().initialize();
        }

        void start_converter()
        {
            Mock_Handle<Mock_Non_Blocking_Free_Running_Converter>::mock().start_converter();
        }

        void stop_converter()
        {
            Mock_Handle<Mock_Non_Blocking_Free_Running_Converter>::mock().stop_converter();
        }

        auto sample_is_available() const -> bool
        {
            return Mock_Handle<Mock_Non_Blocking_Free_Running_Converter>::mock.sample_is_available();
        }

        auto sample() -> Sample
        {
            return Mock_Handle<Mock_Non_Blocking_Free_Running_Converter>::mock.sample();
        }
    };

    Mock_Non_Blocking_Free_Running_Converter() = default;

    Mock_Non_Blocking_Free_Running_Converter( Mock_Non_Blocking_Free_Running_Converter && ) = delete;

    Mock_Non_Blocking_Free_Running_Converter( Mock_Non_Blocking_Free_Running_Converter const & ) = delete;

    ~Mock_Non_Blocking_Free_Running_Converter() noexcept = default;

    auto operator=( Mock_Non_Blocking_Free_Running_Converter && ) = delete;

    auto operator=( Mock_Non_Blocking_Free_Running_Converter const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    MOCK_METHOD( void, initialize, () );

    MOCK_METHOD( void, start_converter, () );
    MOCK_METHOD( void, stop_converter, () );
    MOCK_METHOD( bool, sample_is_available, (), ( const ) );
    MOCK_METHOD( Sample, sample, () );
};

} // namespace picolibrary::Testing::Automated::ADC

#endif // PICOLIBRARY_TESTING_AUTOMATED_ADC_H

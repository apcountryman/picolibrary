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
 * \brief picolibrary::Testing::Automated::Indicator interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_INDICATOR_H
#define PICOLIBRARY_TESTING_AUTOMATED_INDICATOR_H

#include "gmock/gmock.h"
#include "picolibrary/indicator.h"
#include "picolibrary/testing/automated/mock_handle.h"
#include "picolibrary/testing/automated/random.h"

namespace picolibrary::Testing::Automated {

/**
 * \brief Generate a pseudo-random picolibrary::Indicator::Initial_Indicator_State.
 *
 * \return A pseudo-randomly generated picolibrary::Indicator::Initial_Indicator_State.
 */
template<>
inline auto random<Indicator::Initial_Indicator_State>() -> Indicator::Initial_Indicator_State
{
    return random<bool>() ? Indicator::Initial_Indicator_State::EXTINGUISHED
                          : Indicator::Initial_Indicator_State::ILLUMINATED;
}

} // namespace picolibrary::Testing::Automated

/**
 * \brief Indicator automated testing facilities.
 */
namespace picolibrary::Testing::Automated::Indicator {

/**
 * \brief Mock fixed intensity indicator.
 */
class Mock_Fixed_Intensity_Indicator {
  public:
    class Handle : public Mock_Handle<Mock_Fixed_Intensity_Indicator> {
      public:
        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Fixed_Intensity_Indicator & mock ) noexcept :
            Mock_Handle<Mock_Fixed_Intensity_Indicator>{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        void initialize()
        {
            mock().initialize();
        }

        void initialize( ::picolibrary::Indicator::Initial_Indicator_State initial_indicator_state )
        {
            mock().initialize( initial_indicator_state );
        }

        void extinguish()
        {
            mock().extinguish();
        }

        void illuminate()
        {
            mock().illuminate();
        }

        void toggle()
        {
            mock().toggle();
        }
    };

    Mock_Fixed_Intensity_Indicator() = default;

    Mock_Fixed_Intensity_Indicator( Mock_Fixed_Intensity_Indicator && ) = delete;

    Mock_Fixed_Intensity_Indicator( Mock_Fixed_Intensity_Indicator const & ) = delete;

    ~Mock_Fixed_Intensity_Indicator() noexcept = default;

    auto operator=( Mock_Fixed_Intensity_Indicator && ) = delete;

    auto operator=( Mock_Fixed_Intensity_Indicator const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    MOCK_METHOD( void, initialize, () );
    MOCK_METHOD( void, initialize, ( ::picolibrary::Indicator::Initial_Indicator_State ) );

    MOCK_METHOD( void, extinguish, () );
    MOCK_METHOD( void, illuminate, () );
    MOCK_METHOD( void, toggle, () );
};

} // namespace picolibrary::Testing::Automated::Indicator

#endif // PICOLIBRARY_TESTING_AUTOMATED_INDICATOR_H

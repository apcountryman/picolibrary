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
 * \brief picolibrary::Testing::Unit::Indicator interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_INDICATOR_H
#define PICOLIBRARY_TESTING_UNIT_INDICATOR_H

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/indicator.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/mock_handle.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/void.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Generate a pseudo-random picolibrary::Indicator::Initial_Indicator_State.
 *
 * \return A pseudo-randomly generated picolibrary::Indicator::Initial_Indicator_State.
 */
template<>
inline auto random<Indicator::Initial_Indicator_State>()
{
    return random<bool>() ? Indicator::Initial_Indicator_State::ILLUMINATED
                          : Indicator::Initial_Indicator_State::EXTINGUISHED;
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief Indicator unit testing facilities.
 */
namespace picolibrary::Testing::Unit::Indicator {

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

        auto initialize()
        {
            return mock().initialize();
        }

        auto initialize( ::picolibrary::Indicator::Initial_Indicator_State initial_indicator_state )
        {
            return mock().initialize( initial_indicator_state );
        }

        auto illuminate()
        {
            return mock().illuminate();
        }

        auto extinguish()
        {
            return mock().extinguish();
        }

        auto toggle()
        {
            return mock().toggle();
        }
    };

    Mock_Fixed_Intensity_Indicator() = default;

    Mock_Fixed_Intensity_Indicator( Mock_Fixed_Intensity_Indicator && ) = delete;

    Mock_Fixed_Intensity_Indicator( Mock_Fixed_Intensity_Indicator const & ) = delete;

    ~Mock_Fixed_Intensity_Indicator() noexcept = default;

    auto operator=( Mock_Fixed_Intensity_Indicator && ) = delete;

    auto operator=( Mock_Fixed_Intensity_Indicator const & ) = delete;

    auto handle() noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );
    MOCK_METHOD( (Result<Void, Error_Code>), initialize, ( ::picolibrary::Indicator::Initial_Indicator_State ) );

    MOCK_METHOD( (Result<Void, Error_Code>), illuminate, () );
    MOCK_METHOD( (Result<Void, Error_Code>), extinguish, () );
    MOCK_METHOD( (Result<Void, Error_Code>), toggle, () );
};

} // namespace picolibrary::Testing::Unit::Indicator

#endif // PICOLIBRARY_TESTING_UNIT_INDICATOR_H

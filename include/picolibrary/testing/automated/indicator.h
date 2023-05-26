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
 * \brief picolibrary::Testing::Automated::Indicator interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_INDICATOR_H
#define PICOLIBRARY_TESTING_AUTOMATED_INDICATOR_H

#include <ostream>
#include <stdexcept>

#include "gmock/gmock.h"
#include "picolibrary/indicator.h"
#include "picolibrary/testing/automated/mock_handle.h"

namespace picolibrary::Indicator {

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the
 *            picolibrary::Indicator::Initial_Indicator_State to.
 * \param[in] initial_indicator_state The picolibrary::Indicator::Initial_Indicator_State
 *            to write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Initial_Indicator_State initial_indicator_state )
    -> std::ostream &
{
    switch ( initial_indicator_state ) {
            // clang-format off

        case Initial_Indicator_State::EXTINGUISHED: return stream << "::picolibrary::Initial_Indicator_State::EXTINGUISHED";
        case Initial_Indicator_State::ILLUMINATED:  return stream << "::picolibrary::Initial_Indicator_State::ILLUMINATED";

            // clang-format on
    } // switch

    throw std::invalid_argument{
        "initial_indicator_state is not a valid "
        "::picolibrary::Indicator::Initial_Indicator_State"
    };
}

} // namespace picolibrary::Indicator

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

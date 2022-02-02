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
 * \brief picolibrary::Testing::Unit::GPIO interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_GPIO_H
#define PICOLIBRARY_TESTING_UNIT_GPIO_H

#include "gmock/gmock.h"
#include "picolibrary/gpio.h"
#include "picolibrary/testing/unit/mock_handle.h"
#include "picolibrary/testing/unit/random.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Generate a pseudo-random picolibrary::GPIO::Initial_Pull_Up_State.
 *
 * \return A pseudo-randomly generated picolibrary::GPIO::Initial_Pull_Up_State.
 */
template<>
inline auto random<GPIO::Initial_Pull_Up_State>()
{
    return random<bool>() ? GPIO::Initial_Pull_Up_State::DISABLED
                          : GPIO::Initial_Pull_Up_State::ENABLED;
}

/**
 * \brief Generate a pseudo-random picolibrary::GPIO::Initial_Pin_State.
 *
 * \return A pseudo-randomly generated picolibrary::GPIO::Initial_Pin_State.
 */
template<>
inline auto random<GPIO::Initial_Pin_State>()
{
    return random<bool>() ? GPIO::Initial_Pin_State::LOW : GPIO::Initial_Pin_State::HIGH;
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief General Purpose Input/Output (GPIO) unit testing facilities.
 */
namespace picolibrary::Testing::Unit::GPIO {

/**
 * \brief Mock input pin.
 */
class Mock_Input_Pin {
  public:
    class Handle : public Mock_Handle<Mock_Input_Pin> {
      public:
        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Input_Pin & mock ) noexcept :
            Mock_Handle<Mock_Input_Pin>{ mock }
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

        auto is_low() const
        {
            return mock().is_low();
        }

        auto is_high() const
        {
            return mock().is_high();
        }
    };

    Mock_Input_Pin() = default;

    Mock_Input_Pin( Mock_Input_Pin && ) = delete;

    Mock_Input_Pin( Mock_Input_Pin const & ) = delete;

    ~Mock_Input_Pin() noexcept = default;

    auto operator=( Mock_Input_Pin && ) = delete;

    auto operator=( Mock_Input_Pin const & ) = delete;

    auto handle() noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( void, initialize, () );

    MOCK_METHOD( bool, is_low, (), ( const ) );
    MOCK_METHOD( bool, is_high, (), ( const ) );
};

} // namespace picolibrary::Testing::Unit::GPIO

#endif // PICOLIBRARY_TESTING_UNIT_GPIO_H

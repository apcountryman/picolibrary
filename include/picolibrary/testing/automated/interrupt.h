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
 * \brief picolibrary::Testing::Automated::Interrupt interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_INTERRUPT_H
#define PICOLIBRARY_TESTING_AUTOMATED_INTERRUPT_H

#include <cstdint>

#include "gmock/gmock.h"
#include "picolibrary/testing/automated/mock_handle.h"

/**
 * \brief Interrupt automated testing facilities.
 */
namespace picolibrary::Testing::Automated::Interrupt {

/**
 * \brief Mock controller.
 */
class Mock_Controller {
  public:
    using Interrupt_Enable_State = std::uint_fast8_t;

    class Handle : public Mock_Handle<Mock_Controller> {
      public:
        using Interrupt_Enable_State = Mock_Controller::Interrupt_Enable_State;

        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Controller & mock ) noexcept :
            Mock_Handle<Mock_Controller>{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        void disable_interrupt()
        {
            mock().disable_interrupt();
        }

        void enable_interrupt()
        {
            mock().enable_interrupt();
        }

        auto save_interrupt_enable_state() const noexcept -> Interrupt_Enable_State
        {
            return mock().save_interrupt_enable_state();
        }

        void restore_interrupt_enable_state( Interrupt_Enable_State interrupt_enable_state ) noexcept
        {
            mock().restore_interrupt_enable_state( interrupt_enable_state );
        }
    };

    Mock_Controller() = default;

    Mock_Controller( Mock_Controller && ) = delete;

    Mock_Controller( Mock_Controller const & ) = delete;

    ~Mock_Controller() noexcept = default;

    auto operator=( Mock_Controller && ) = delete;

    auto operator=( Mock_Controller const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    MOCK_METHOD( void, disable_interrupt, () );
    MOCK_METHOD( void, enable_interrupt, () );

    MOCK_METHOD( Interrupt_Enable_State, save_interrupt_enable_state, (), ( const ) );
    MOCK_METHOD( void, restore_interrupt_enable_state, ( Interrupt_Enable_State ) );
};

} // namespace picolibrary::Testing::Automated::Interrupt

#endif // PICOLIBRARY_TESTING_AUTOMATED_INTERRUPT_H

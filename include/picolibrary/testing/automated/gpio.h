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
 * \brief picolibrary::Testing::Automated::GPIO interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_GPIO_H
#define PICOLIBRARY_TESTING_AUTOMATED_GPIO_H

#include "gmock/gmock.h"
#include "picolibrary/gpio.h"
#include "picolibrary/testing/automated/mock_handle.h"
#include "picolibrary/testing/automated/random.h"

namespace picolibrary::Testing::Automated {

/**
 * \brief Generate a pseudo-random picolibrary::GPIO::Initial_Pull_Up_State.
 *
 * \return A pseudo-randomly generated picolibrary::GPIO::Initial_Pull_Up_State.
 */
template<>
inline auto random<GPIO::Initial_Pull_Up_State>() -> GPIO::Initial_Pull_Up_State
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
inline auto random<GPIO::Initial_Pin_State>() -> GPIO::Initial_Pin_State
{
    return random<bool>() ? GPIO::Initial_Pin_State::LOW : GPIO::Initial_Pin_State::HIGH;
}

} // namespace picolibrary::Testing::Automated

/**
 * \brief General Purpose Input/Output (GPIO) automated testing facilities.
 */
namespace picolibrary::Testing::Automated::GPIO {

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

        auto is_low() const -> bool
        {
            return mock().is_low();
        }

        auto is_high() const -> bool
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

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    MOCK_METHOD( void, initialize, () );

    MOCK_METHOD( bool, is_low, (), ( const ) );
    MOCK_METHOD( bool, is_high, (), ( const ) );
};

/**
 * \brief Mock internally pull-up input pin.
 */
class Mock_Internally_Pulled_Up_Input_Pin : public Mock_Input_Pin {
  public:
    class Handle : public Mock_Input_Pin::Handle {
      public:
        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Internally_Pulled_Up_Input_Pin & mock ) noexcept :
            Mock_Input_Pin::Handle{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        auto mock() noexcept -> Mock_Internally_Pulled_Up_Input_Pin &
        {
            return static_cast<Mock_Internally_Pulled_Up_Input_Pin &>( Mock_Input_Pin::Handle::mock() );
        }

        auto mock() const noexcept -> Mock_Internally_Pulled_Up_Input_Pin const &
        {
            return static_cast<Mock_Internally_Pulled_Up_Input_Pin const &>(
                Mock_Input_Pin::Handle::mock() );
        }

        using Mock_Input_Pin::Handle::initialize;

        void initialize( ::picolibrary::GPIO::Initial_Pull_Up_State initial_pull_up_state )
        {
            mock().initialize( initial_pull_up_state );
        }

        auto pull_up_is_disabled() const -> bool
        {
            return mock().pull_up_is_disabled();
        }

        auto pull_up_is_enabled() const -> bool
        {
            return mock().pull_up_is_enabled();
        }

        void disable_pull_up()
        {
            mock().disable_pull_up();
        }

        void enable_pull_up()
        {
            mock().enable_pull_up();
        }
    };

    Mock_Internally_Pulled_Up_Input_Pin() = default;

    Mock_Internally_Pulled_Up_Input_Pin( Mock_Internally_Pulled_Up_Input_Pin && ) = delete;

    Mock_Internally_Pulled_Up_Input_Pin( Mock_Internally_Pulled_Up_Input_Pin const & ) = delete;

    ~Mock_Internally_Pulled_Up_Input_Pin() noexcept = default;

    auto operator=( Mock_Internally_Pulled_Up_Input_Pin && ) = delete;

    auto operator=( Mock_Internally_Pulled_Up_Input_Pin const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    using Mock_Input_Pin::gmock_initialize;
    using Mock_Input_Pin::initialize;

    MOCK_METHOD( void, initialize, ( ::picolibrary::GPIO::Initial_Pull_Up_State ) );

    MOCK_METHOD( bool, pull_up_is_disabled, (), ( const ) );
    MOCK_METHOD( bool, pull_up_is_enabled, (), ( const ) );

    MOCK_METHOD( void, disable_pull_up, () );
    MOCK_METHOD( void, enable_pull_up, () );
};

/**
 * \brief Mock output pin.
 */
class Mock_Output_Pin {
  public:
    class Handle : public Mock_Handle<Mock_Output_Pin> {
      public:
        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Output_Pin & mock ) noexcept :
            Mock_Handle<Mock_Output_Pin>{ mock }
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

        void initialize( ::picolibrary::GPIO::Initial_Pin_State initial_pin_state )
        {
            mock().initialize( initial_pin_state );
        }

        void transition_to_low()
        {
            mock().transition_to_low();
        }

        void transition_to_high()
        {
            mock().transition_to_high();
        }

        void toggle()
        {
            mock().toggle();
        }
    };

    Mock_Output_Pin() = default;

    Mock_Output_Pin( Mock_Output_Pin && ) = delete;

    Mock_Output_Pin( Mock_Output_Pin const & ) = delete;

    ~Mock_Output_Pin() noexcept = default;

    auto operator=( Mock_Output_Pin && ) = delete;

    auto operator=( Mock_Output_Pin const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    MOCK_METHOD( void, initialize, () );
    MOCK_METHOD( void, initialize, ( ::picolibrary::GPIO::Initial_Pin_State ) );

    MOCK_METHOD( void, transition_to_low, () );
    MOCK_METHOD( void, transition_to_high, () );
    MOCK_METHOD( void, toggle, () );
};

/**
 * \brief Mock Input/Output (I/O) pin.
 */
class Mock_IO_Pin {
  public:
    class Handle : public Mock_Handle<Mock_IO_Pin> {
      public:
        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_IO_Pin & mock ) noexcept : Mock_Handle<Mock_IO_Pin>{ mock }
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

        void initialize( ::picolibrary::GPIO::Initial_Pin_State initial_pin_state )
        {
            mock().initialize( initial_pin_state );
        }

        auto is_low() const -> bool
        {
            return mock().is_low();
        }

        auto is_high() const -> bool
        {
            return mock().is_high();
        }

        void transition_to_low()
        {
            mock().transition_to_low();
        }

        void transition_to_high()
        {
            mock().transition_to_high();
        }

        void toggle()
        {
            mock().toggle();
        }
    };

    Mock_IO_Pin() = default;

    Mock_IO_Pin( Mock_IO_Pin && ) = delete;

    Mock_IO_Pin( Mock_IO_Pin const & ) = delete;

    ~Mock_IO_Pin() noexcept = default;

    auto operator=( Mock_IO_Pin && ) = delete;

    auto operator=( Mock_IO_Pin const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    MOCK_METHOD( void, initialize, () );
    MOCK_METHOD( void, initialize, ( ::picolibrary::GPIO::Initial_Pin_State ) );

    MOCK_METHOD( bool, is_low, (), ( const ) );
    MOCK_METHOD( bool, is_high, (), ( const ) );

    MOCK_METHOD( void, transition_to_low, () );
    MOCK_METHOD( void, transition_to_high, () );
    MOCK_METHOD( void, toggle, () );
};

} // namespace picolibrary::Testing::Automated::GPIO

#endif // PICOLIBRARY_TESTING_AUTOMATED_GPIO_H

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
 * \brief picolibrary::Testing::Unit::GPIO interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_GPIO_H
#define PICOLIBRARY_TESTING_UNIT_GPIO_H

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/gpio.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/mock_handle.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/void.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Generate a pseudo-random picolibrary::GPIO::Initial_Pin_State.
 *
 * \return A pseudo-randomly generated picolibrary::GPIO::Initial_Pin_State.
 */
template<>
inline auto random<GPIO::Initial_Pin_State>()
{
    return random<bool>() ? GPIO::Initial_Pin_State::HIGH : GPIO::Initial_Pin_State::LOW;
}

/**
 * \brief Generate a pseudo-random picolibrary::GPIO::Initial_Pull_Up_State.
 *
 * \return A pseudo-randomly generated picolibrary::GPIO::Initial_Pull_Up_State.
 */
template<>
inline auto random<GPIO::Initial_Pull_Up_State>()
{
    return random<bool>() ? GPIO::Initial_Pull_Up_State::ENABLED
                          : GPIO::Initial_Pull_Up_State::DISABLED;
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

        auto initialize()
        {
            return mock().initialize();
        }

        auto state() const
        {
            return mock().state();
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

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );

    MOCK_METHOD( (Result<::picolibrary::GPIO::Pin_State, Error_Code>), state, (), ( const ) );
};

/**
 * \brief Mock internally pulled-up input pin.
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

        auto & mock() noexcept
        {
            return static_cast<Mock_Internally_Pulled_Up_Input_Pin &>( Mock_Input_Pin::Handle::mock() );
        }

        auto const & mock() const noexcept
        {
            return static_cast<Mock_Internally_Pulled_Up_Input_Pin const &>(
                Mock_Input_Pin::Handle::mock() );
        }

        using Mock_Input_Pin::Handle::initialize;

        auto initialize( ::picolibrary::GPIO::Initial_Pull_Up_State initial_pull_up_state )
        {
            return mock().initialize( initial_pull_up_state );
        }

        auto enable_pull_up()
        {
            return mock().enable_pull_up();
        }

        auto disable_pull_up()
        {
            return mock().disable_pull_up();
        }
    };

    Mock_Internally_Pulled_Up_Input_Pin() = default;

    Mock_Internally_Pulled_Up_Input_Pin( Mock_Internally_Pulled_Up_Input_Pin && ) = delete;

    Mock_Internally_Pulled_Up_Input_Pin( Mock_Internally_Pulled_Up_Input_Pin const & ) = delete;

    ~Mock_Internally_Pulled_Up_Input_Pin() noexcept = default;

    auto operator=( Mock_Internally_Pulled_Up_Input_Pin && ) = delete;

    auto operator=( Mock_Internally_Pulled_Up_Input_Pin const & ) = delete;

    auto handle() noexcept
    {
        return Handle{ *this };
    }

    using Mock_Input_Pin::gmock_initialize;
    using Mock_Input_Pin::initialize;

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, ( ::picolibrary::GPIO::Initial_Pull_Up_State ) );

    MOCK_METHOD( (Result<Void, Error_Code>), enable_pull_up, () );

    MOCK_METHOD( (Result<Void, Error_Code>), disable_pull_up, () );
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

        auto initialize()
        {
            return mock().initialize();
        }

        auto initialize( ::picolibrary::GPIO::Initial_Pin_State initial_pin_state )
        {
            return mock().initialize( initial_pin_state );
        }

        auto transition_to_high()
        {
            return mock().transition_to_high();
        }

        auto transition_to_low()
        {
            return mock().transition_to_low();
        }

        auto toggle()
        {
            return mock().toggle();
        }
    };

    Mock_Output_Pin() = default;

    Mock_Output_Pin( Mock_Output_Pin && ) = delete;

    Mock_Output_Pin( Mock_Output_Pin const & ) = delete;

    ~Mock_Output_Pin() noexcept = default;

    auto operator=( Mock_Output_Pin && ) = delete;

    auto operator=( Mock_Output_Pin const & ) = delete;

    auto handle() noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );
    MOCK_METHOD( (Result<Void, Error_Code>), initialize, ( ::picolibrary::GPIO::Initial_Pin_State ) );

    MOCK_METHOD( (Result<Void, Error_Code>), transition_to_high, () );
    MOCK_METHOD( (Result<Void, Error_Code>), transition_to_low, () );
    MOCK_METHOD( (Result<Void, Error_Code>), toggle, () );
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

        auto initialize()
        {
            return mock().initialize();
        }

        auto initialize( ::picolibrary::GPIO::Initial_Pin_State initial_pin_state )
        {
            return mock().initialize( initial_pin_state );
        }

        auto state() const
        {
            return mock().state();
        }

        auto transition_to_high()
        {
            return mock().transition_to_high();
        }

        auto transition_to_low()
        {
            return mock().transition_to_low();
        }

        auto toggle()
        {
            return mock().toggle();
        }
    };

    Mock_IO_Pin() = default;

    Mock_IO_Pin( Mock_IO_Pin && ) = delete;

    Mock_IO_Pin( Mock_IO_Pin const & ) = delete;

    ~Mock_IO_Pin() noexcept = default;

    auto operator=( Mock_IO_Pin && ) = delete;

    auto operator=( Mock_IO_Pin const & ) = delete;

    auto handle() noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );
    MOCK_METHOD( (Result<Void, Error_Code>), initialize, ( ::picolibrary::GPIO::Initial_Pin_State ) );

    MOCK_METHOD( (Result<::picolibrary::GPIO::Pin_State, Error_Code>), state, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), transition_to_high, () );
    MOCK_METHOD( (Result<Void, Error_Code>), transition_to_low, () );
    MOCK_METHOD( (Result<Void, Error_Code>), toggle, () );
};

} // namespace picolibrary::Testing::Unit::GPIO

#endif // PICOLIBRARY_TESTING_UNIT_GPIO_H

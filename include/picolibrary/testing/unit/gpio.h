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
    /**
     * \brief Movable mock input pin handle.
     */
    class Handle {
      public:
        /**
         * \brief Constructor.
         */
        Handle() noexcept = default;

        /**
         * \brief Constructor.
         *
         * \param[in] mock_input_pin The mock input pin.
         */
        Handle( Mock_Input_Pin & mock_input_pin ) noexcept :
            m_mock_input_pin{ &mock_input_pin }
        {
        }

        /**
         * \brief Constructor.
         *
         * \param[in] source The source of the move.
         */
        Handle( Handle && source ) noexcept : m_mock_input_pin{ source.m_mock_input_pin }
        {
            source.m_mock_input_pin = nullptr;
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
                m_mock_input_pin = expression.m_mock_input_pin;

                expression.m_mock_input_pin = nullptr;
            } // if

            return *this;
        }

        auto operator=( Handle const & ) = delete;

        /**
         * \brief Get the mock input pin.
         *
         * \return The mock input pin.
         */
        auto & mock() noexcept
        {
            return *m_mock_input_pin;
        }

        /**
         * \brief Initialize the pin's hardware.
         *
         * \return Nothing if pin hardware initialization succeeded.
         * \return An error code if pin hardware initialization failed.
         */
        auto initialize()
        {
            return m_mock_input_pin->initialize();
        }

        /**
         * \brief Get the state of the pin.
         *
         * \return High if the pin is high.
         * \return Low if the pin is low.
         * \return An error code if getting the state of the pin failed.
         */
        auto state() const
        {
            return m_mock_input_pin->state();
        }

      private:
        /**
         * \brief The mock input pin.
         */
        Mock_Input_Pin * m_mock_input_pin{};
    };

    /**
     * \brief Constructor.
     */
    Mock_Input_Pin() = default;

    Mock_Input_Pin( Mock_Input_Pin && ) = delete;

    Mock_Input_Pin( Mock_Input_Pin const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Input_Pin() noexcept = default;

    auto operator=( Mock_Input_Pin && ) = delete;

    auto operator=( Mock_Input_Pin const & ) = delete;

    /**
     * \brief Get a movable handle to the mock input pin.
     *
     * \return A movable handle to the mock input pin.
     */
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
class Mock_Internally_Pulled_Up_Input_Pin {
  public:
    /**
     * \brief Movable mock internally pulled-up input pin handle.
     */
    class Handle {
      public:
        /**
         * \brief Constructor.
         */
        Handle() noexcept = default;

        /**
         * \brief Constructor.
         *
         * \param[in] mock_internally_pulled_up_input_pin The mock internally pulled-up
         *            input pin.
         */
        Handle( Mock_Internally_Pulled_Up_Input_Pin & mock_internally_pulled_up_input_pin ) noexcept :
            m_mock_internally_pulled_up_input_pin{ &mock_internally_pulled_up_input_pin }
        {
        }

        /**
         * \brief Constructor.
         *
         * \param[in] source The source of the move.
         */
        Handle( Handle && source ) noexcept :
            m_mock_internally_pulled_up_input_pin{ source.m_mock_internally_pulled_up_input_pin }
        {
            source.m_mock_internally_pulled_up_input_pin = nullptr;
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
                m_mock_internally_pulled_up_input_pin = expression.m_mock_internally_pulled_up_input_pin;

                expression.m_mock_internally_pulled_up_input_pin = nullptr;
            } // if

            return *this;
        }

        auto operator=( Handle const & ) = delete;

        /**
         * \brief Get the mock internally pulled-up input pin.
         *
         * \return The mock internally pulled-up input pin.
         */
        auto & mock() noexcept
        {
            return *m_mock_internally_pulled_up_input_pin;
        }

        /**
         * \brief Initialize the pin's hardware.
         *
         * \param[in] initial_pull_up_state The initial state of the pin's internal
         *            pull-up resistor.
         *
         * \return Nothing if pin hardware initialization succeeded.
         * \return An error code if pin hardware initialization failed.
         */
        auto initialize( ::picolibrary::GPIO::Initial_Pull_Up_State initial_pull_up_state = ::picolibrary::GPIO::Initial_Pull_Up_State::DISABLED )
        {
            return m_mock_internally_pulled_up_input_pin->initialize( initial_pull_up_state );
        }

        /**
         * \brief Enable the pin's internal pull-up resistor.
         *
         * \return Nothing if enabling the pin's internal pull-up resistor succeeded.
         * \return An error code if enabling the pin's internal pull-up resistor failed.
         */
        auto enable_pull_up()
        {
            return m_mock_internally_pulled_up_input_pin->enable_pull_up();
        }

        /**
         * \brief Disable the pin's internal pull-up resistor.
         *
         * \return Nothing if disabling the pin's internal pull-up resistor succeeded.
         * \return An error code if disabling the pin's internal pull-up resistor failed.
         */
        auto disable_pull_up()
        {
            return m_mock_internally_pulled_up_input_pin->disable_pull_up();
        }

        /**
         * \brief Get the state of the pin.
         *
         * \return High if the pin is high.
         * \return Low if the pin is low.
         * \return An error code if getting the state of the pin failed.
         */
        auto state() const
        {
            return m_mock_internally_pulled_up_input_pin->state();
        }

      private:
        /**
         * \brief The mock internally pulled-up input pin.
         */
        Mock_Internally_Pulled_Up_Input_Pin * m_mock_internally_pulled_up_input_pin{};
    };

    /**
     * \brief Constructor.
     */
    Mock_Internally_Pulled_Up_Input_Pin() = default;

    Mock_Internally_Pulled_Up_Input_Pin( Mock_Internally_Pulled_Up_Input_Pin && ) = delete;

    Mock_Internally_Pulled_Up_Input_Pin( Mock_Internally_Pulled_Up_Input_Pin const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Internally_Pulled_Up_Input_Pin() noexcept = default;

    auto operator=( Mock_Internally_Pulled_Up_Input_Pin && ) = delete;

    auto operator=( Mock_Internally_Pulled_Up_Input_Pin const & ) = delete;

    /**
     * \brief Get a movable handle to the mock internally pulled-up input pin.
     *
     * \return A movable handle to the mock internally pulled-up input pin.
     */
    auto handle() noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, ( ::picolibrary::GPIO::Initial_Pull_Up_State ) );

    MOCK_METHOD( (Result<Void, Error_Code>), enable_pull_up, () );

    MOCK_METHOD( (Result<Void, Error_Code>), disable_pull_up, () );

    MOCK_METHOD( (Result<::picolibrary::GPIO::Pin_State, Error_Code>), state, (), ( const ) );
};

/**
 * \brief Mock output pin.
 */
class Mock_Output_Pin {
  public:
    /**
     * \brief Movable mock output pin handle.
     */
    class Handle {
      public:
        /**
         * \brief Constructor.
         */
        Handle() noexcept = default;

        /**
         * \brief Constructor.
         *
         * \param[in] mock_output_pin The mock output pin.
         */
        Handle( Mock_Output_Pin & mock_output_pin ) noexcept :
            m_mock_output_pin{ &mock_output_pin }
        {
        }

        /**
         * \brief Constructor.
         *
         * \param[in] source The source of the move.
         */
        Handle( Handle && source ) noexcept :
            m_mock_output_pin{ source.m_mock_output_pin }
        {
            source.m_mock_output_pin = nullptr;
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
                m_mock_output_pin = expression.m_mock_output_pin;

                expression.m_mock_output_pin = nullptr;
            } // if

            return *this;
        }

        auto operator=( Handle const & ) = delete;

        /**
         * \brief Get the mock output pin.
         *
         * \return The mock output pin.
         */
        auto & mock() noexcept
        {
            return *m_mock_output_pin;
        }

        /**
         * \brief Initialize the pin's hardware.
         *
         * \param[in] initial_pin_state The initial state of the pin.
         *
         * \return Nothing if pin hardware initialization succeeded.
         * \return An error code if pin hardware initialization failed.
         */
        auto initialize( ::picolibrary::GPIO::Initial_Pin_State initial_pin_state = ::picolibrary::GPIO::Initial_Pin_State::LOW )
        {
            return m_mock_output_pin->initialize( initial_pin_state );
        }

        /**
         * \brief Transition the pin to the high state.
         *
         * \return Nothing if transitioning the pin to the high state succeeded.
         * \return An error code if transitioning the pin to the high state failed.
         */
        auto transition_to_high()
        {
            return m_mock_output_pin->transition_to_high();
        }

        /**
         * \brief Transition the pin to the low state.
         *
         * \return Nothing if transitioning the pin to the low state succeeded.
         * \return An error code if transitioning the pin to the low state failed.
         */
        auto transition_to_low()
        {
            return m_mock_output_pin->transition_to_low();
        }

        /**
         * \brief Toggle the pin state.
         *
         * \return Nothing if toggling the pin state succeeded.
         * \return An error code if toggling the pin state failed.
         */
        auto toggle()
        {
            return m_mock_output_pin->toggle();
        }

      private:
        /**
         * \brief The mock output pin.
         */
        Mock_Output_Pin * m_mock_output_pin{};
    };

    /**
     * \brief Constructor.
     */
    Mock_Output_Pin() = default;

    Mock_Output_Pin( Mock_Output_Pin && ) = delete;

    Mock_Output_Pin( Mock_Output_Pin const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Output_Pin() noexcept = default;

    auto operator=( Mock_Output_Pin && ) = delete;

    auto operator=( Mock_Output_Pin const & ) = delete;

    /**
     * \brief Get a movable handle to the mock output pin.
     *
     * \return A movable handle to the mock output pin.
     */
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
    /**
     * \brief Movable mock I/O pin handle.
     */
    class Handle {
      public:
        /**
         * \brief Constructor.
         */
        Handle() noexcept = default;

        /**
         * \brief Constructor.
         *
         * \param[in] mock_io_pin The mock I/O pin.
         */
        Handle( Mock_IO_Pin & mock_io_pin ) noexcept : m_mock_io_pin{ &mock_io_pin }
        {
        }

        /**
         * \brief Constructor.
         *
         * \param[in] source The source of the move.
         */
        Handle( Handle && source ) noexcept : m_mock_io_pin{ source.m_mock_io_pin }
        {
            source.m_mock_io_pin = nullptr;
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
                m_mock_io_pin = expression.m_mock_io_pin;

                expression.m_mock_io_pin = nullptr;
            } // if

            return *this;
        }

        auto operator=( Handle const & ) = delete;

        /**
         * \brief Get the mock I/O pin.
         *
         * \return The mock I/O pin.
         */
        auto & mock() noexcept
        {
            return *m_mock_io_pin;
        }

        /**
         * \brief Initialize the pin's hardware.
         *
         * \param[in] initial_pin_state The initial state of the pin.
         *
         * \return Nothing if pin hardware initialization succeeded.
         * \return An error code if pin hardware initialization failed.
         */
        auto initialize( ::picolibrary::GPIO::Initial_Pin_State initial_pin_state = ::picolibrary::GPIO::Initial_Pin_State::LOW )
        {
            return m_mock_io_pin->initialize( initial_pin_state );
        }

        /**
         * \brief Get the state of the pin.
         *
         * \return High if the pin is high.
         * \return Low if the pin is low.
         * \return An error code if getting the state of the pin failed.
         */
        auto state() const
        {
            return m_mock_io_pin->state();
        }

        /**
         * \brief Transition the pin to the high state.
         *
         * \return Nothing if transitioning the pin to the high state succeeded.
         * \return An error code if transitioning the pin to the high state failed.
         */
        auto transition_to_high()
        {
            return m_mock_io_pin->transition_to_high();
        }

        /**
         * \brief Transition the pin to the low state.
         *
         * \return Nothing if transitioning the pin to the low state succeeded.
         * \return An error code if transitioning the pin to the low state failed.
         */
        auto transition_to_low()
        {
            return m_mock_io_pin->transition_to_low();
        }

        /**
         * \brief Toggle the pin state.
         *
         * \return Nothing if toggling the pin state succeeded.
         * \return An error code if toggling the pin state failed.
         */
        auto toggle()
        {
            return m_mock_io_pin->toggle();
        }

      private:
        /**
         * \brief The mock I/O pin.
         */
        Mock_IO_Pin * m_mock_io_pin{};
    };

    /**
     * \brief Constructor.
     */
    Mock_IO_Pin() = default;

    Mock_IO_Pin( Mock_IO_Pin && ) = delete;

    Mock_IO_Pin( Mock_IO_Pin const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_IO_Pin() noexcept = default;

    auto operator=( Mock_IO_Pin && ) = delete;

    auto operator=( Mock_IO_Pin const & ) = delete;

    /**
     * \brief Get a movable handle to the mock I/O pin.
     *
     * \return A movable handle to the mock I/O pin.
     */
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

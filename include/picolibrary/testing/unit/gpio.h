/**
 * picolibrary
 *
 * Copyright 2020 Andrew Countryman <apcountryman@gmail.com>
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
#include "picolibrary/utility.h"

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
        Handle( Handle && source ) noexcept = default;

        /**
         * \todo #29
         */
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
        auto operator=( Handle && expression ) noexcept -> Handle & = default;

        /**
         * \todo #29
         *
         * \return
         */
        auto operator=( Handle const & ) = delete;

        /**
         * \brief Initialize the pin's hardware.
         *
         * \return Nothing if initializing the pin's hardware succeeded.
         * \return An error code if initializing the pin's hardware failed.
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

    /**
     * \todo #29
     */
    Mock_Input_Pin( Mock_Input_Pin && ) = delete;

    /**
     * \todo #29
     */
    Mock_Input_Pin( Mock_Input_Pin const & ) = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Mock_Input_Pin && ) = delete;

    /**
     * \todo #29
     *
     * \return
     */
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
        Handle( Handle && source ) noexcept = default;

        /**
         * \todo #29
         */
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
        auto operator=( Handle && expression ) noexcept -> Handle & = default;

        /**
         * \todo #29
         *
         * \return
         */
        auto operator=( Handle const & ) = delete;

        /**
         * \brief Initialize the pin's hardware.
         *
         * \return Nothing if initializing the pin's hardware succeeded.
         * \return An error code if initializing the pin's hardware failed.
         */
        auto initialize()
        {
            return m_mock_internally_pulled_up_input_pin->initialize();
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

    /**
     * \todo #29
     */
    Mock_Internally_Pulled_Up_Input_Pin( Mock_Input_Pin && ) = delete;

    /**
     * \todo #29
     */
    Mock_Internally_Pulled_Up_Input_Pin( Mock_Input_Pin const & ) = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Mock_Internally_Pulled_Up_Input_Pin && ) = delete;

    /**
     * \todo #29
     *
     * \return
     */
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

} // namespace picolibrary::Testing::Unit::GPIO

#endif // PICOLIBRARY_TESTING_UNIT_GPIO_H

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
    /**
     * \brief Movable mock fixed intensity indicator handle.
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
         * \param[in] mock_fixed_intensity_indicator The mock fixed intensity indicator.
         */
        Handle( Mock_Fixed_Intensity_Indicator & mock_fixed_intensity_indicator ) noexcept :
            m_mock_fixed_intensity_indicator{ &mock_fixed_intensity_indicator }
        {
        }

        /**
         * \brief Constructor.
         *
         * \param[in] source The source of the move.
         */
        Handle( Handle && source ) noexcept :
            m_mock_fixed_intensity_indicator{ source.m_mock_fixed_intensity_indicator }
        {
            source.m_mock_fixed_intensity_indicator = nullptr;
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
                m_mock_fixed_intensity_indicator = expression.m_mock_fixed_intensity_indicator;

                expression.m_mock_fixed_intensity_indicator = nullptr;
            } // if

            return *this;
        }

        auto operator=( Handle const & ) = delete;

        /**
         * \brief Get the mock fixed intensity indicator.
         *
         * \return The mock fixed intensity indicator.
         */
        auto & mock() noexcept
        {
            return *m_mock_fixed_intensity_indicator;
        }

        /**
         * \brief Initialize the indicator's hardware.
         *
         * \param[in] initial_indicator_state The initial state of the indicator.
         *
         * \return Nothing if indicator hardware initialization succeeded.
         * \return An error code if indicator hardware initialization failed.
         */
        auto initialize(
            ::picolibrary::Indicator::Initial_Indicator_State initial_indicator_state =
                ::picolibrary::Indicator::Initial_Indicator_State::EXTINGUISHED )
        {
            return m_mock_fixed_intensity_indicator->initialize( initial_indicator_state );
        }

        /**
         * \brief Illuminate the indicator.
         *
         * \return Nothing if illuminating the indicator succeeded.
         * \return An error code if illuminating the indicator failed.
         */
        auto illuminate()
        {
            return m_mock_fixed_intensity_indicator->illuminate();
        }

        /**
         * \brief Extinguish the indicator.
         *
         * \return Nothing if extinguishing the indicator succeeded.
         * \return An error code if extinguishing the indicator failed.
         */
        auto extinguish()
        {
            return m_mock_fixed_intensity_indicator->extinguish();
        }

        /**
         * \brief Toggle the indicator.
         *
         * \return Nothing if toggling the indicator succeeded.
         * \return An error code if toggling the indicator failed.
         */
        auto toggle()
        {
            return m_mock_fixed_intensity_indicator->toggle();
        }

      private:
        /**
         * \brief The mock fixed intensity indicator.
         */
        Mock_Fixed_Intensity_Indicator * m_mock_fixed_intensity_indicator{};
    };

    /**
     * \brief Constructor.
     */
    Mock_Fixed_Intensity_Indicator() = default;

    Mock_Fixed_Intensity_Indicator( Mock_Fixed_Intensity_Indicator && ) = delete;

    Mock_Fixed_Intensity_Indicator( Mock_Fixed_Intensity_Indicator const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Fixed_Intensity_Indicator() noexcept = default;

    auto operator=( Mock_Fixed_Intensity_Indicator && ) = delete;

    auto operator=( Mock_Fixed_Intensity_Indicator const & ) = delete;

    /**
     * \brief Get a movable handle to the mock fixed intensity indicator.
     *
     * \return A movable handle to the mock fixed intensity indicator.
     */
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

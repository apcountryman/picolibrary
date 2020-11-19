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
 * \brief picolibrary::Testing::Unit::SPI interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_SPI_H
#define PICOLIBRARY_TESTING_UNIT_SPI_H

#include <cstdint>

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/utility.h"

/**
 * \brief Serial Peripheral Interface (SPI) unit testing facilities.
 */
namespace picolibrary::Testing::Unit::SPI {

/**
 * \brief Mock SPI basic controller.
 */
class Mock_Basic_Controller {
  public:
    /**
     * \copydoc picolibrary::SPI::Basic_Controller_Concept::Configuration
     */
    using Configuration = std::uint_fast16_t;

    /**
     * \brief Movable mock basic controller handle.
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
         * \param[in] mock_basic_controller The mock basic controller.
         */
        Handle( Mock_Basic_Controller & mock_basic_controller ) noexcept :
            m_mock_basic_controller{ &mock_basic_controller }
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
         * \brief Get the mock basic controller.
         *
         * \return The mock basic controller.
         */
        auto & mock() noexcept
        {
            return *m_mock_basic_controller;
        }

        /**
         * \brief Initialize the controller's hardware.
         *
         * \return Nothing if initializing the controller's hardware succeeded.
         * \return An error code if initializing the controller's hardware failed.
         */
        auto initialize()
        {
            return m_mock_basic_controller->initialize();
        }

        /**
         * \brief Configure the controller's clock to meet a specific device's
         *        communication requirements.
         *
         * \param[in] configuration The clock configuration that meets the device's
         *            communication requirements.
         *
         * \return Nothing if configuring the controller's clock succeeded.
         * \return An error code if configuring the controller's clock failed.
         */
        auto configure( Configuration configuration )
        {
            return m_mock_basic_controller->configure( configuration );
        }

        /**
         * \brief Exchange data with a device.
         *
         * \param[in] data The data to transmit.
         *
         * \return The received data if data exchange succeeded.
         * \return An error code if data exchange failed.
         */
        auto exchange( std::uint8_t data )
        {
            return m_mock_basic_controller->exchange( data );
        }

      private:
        /**
         * \brief The mock basic controller.
         */
        Mock_Basic_Controller * m_mock_basic_controller{};
    };

    /**
     * \brief Constructor.
     */
    Mock_Basic_Controller() = default;

    /**
     * \todo #29
     */
    Mock_Basic_Controller( Mock_Basic_Controller && ) = delete;

    /**
     * \todo #29
     */
    Mock_Basic_Controller( Mock_Basic_Controller const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Basic_Controller() noexcept = default;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Mock_Basic_Controller && ) = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Mock_Basic_Controller const & ) = delete;

    /**
     * \brief Get a movable handle to the mock basic controller.
     *
     * \return A movable handle to the mock basic controller.
     */
    auto handle() noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );

    MOCK_METHOD( (Result<Void, Error_Code>), configure, ( Configuration ) );

    MOCK_METHOD( (Result<Void, Error_Code>), exchange, ( std::uint8_t ) );
};

} // namespace picolibrary::Testing::Unit::SPI

#endif // PICOLIBRARY_TESTING_UNIT_SPI_H

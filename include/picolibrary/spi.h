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
 * \brief picolibrary::SPI interface.
 */

#ifndef PICOLIBRARY_SPI_H
#define PICOLIBRARY_SPI_H

#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/utility.h"

/**
 * \brief Serial Peripheral Interface (SPI) facilities.
 */
namespace picolibrary::SPI {

/**
 * \brief SPI basic controller concept.
 */
class Basic_Controller_Concept {
  public:
    /**
     * \brief Clock configuration (frequency, polarity, and phase).
     */
    struct Configuration {
    };

    Basic_Controller_Concept() = delete;

    /**
     * \todo #29
     */
    Basic_Controller_Concept( Basic_Controller_Concept && ) = delete;

    /**
     * \todo #29
     */
    Basic_Controller_Concept( Basic_Controller_Concept const & ) = delete;

    ~Basic_Controller_Concept() = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Basic_Controller_Concept && ) = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Basic_Controller_Concept const & ) = delete;

    /**
     * \brief Initialize the controller's hardware.
     *
     * \return Nothing if initializing the controller's hardware succeeded.
     * \return An error code if initializing the controller's hardware failed. If
     *      initializing the controller's hardware cannot fail, return
     *      picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto initialize() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Configure the controller's clock to meet a specific device's communication
     *        requirements.
     *
     * \param[in] configuration The clock configuration that meets the device's
     *            communication requirements.
     *
     * \return Nothing if configuring the controller's clock succeeded.
     * \return An error code if configuring the controller's clock failed. If configuring
     *         the controller's clock cannot fail, return
     *         picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto configure( Configuration configuration ) noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Exchange data with a device.
     *
     * \param[in] data The data to transmit.
     *
     * \return The received data if data exchange succeeded.
     * \return An error code if data exchange failed. If data exchange cannot fail, return
     *         picolibrary::Result<std::uint8_t, picolibrary::Void>.
     */
    auto exchange( std::uint8_t data ) noexcept -> Result<std::uint8_t, Error_Code>;
};

} // namespace picolibrary::SPI

#endif // PICOLIBRARY_SPI_H

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
 * \brief picolibrary::SPI interface.
 */

#ifndef PICOLIBRARY_SPI_H
#define PICOLIBRARY_SPI_H

#include <cstdint>

/**
 * \brief Serial Peripheral Interface (SPI) facilities.
 */
namespace picolibrary::SPI {

/**
 * \brief Basic controller concept.
 */
class Basic_Controller_Concept {
  public:
    /**
     * \brief Clock (frequency, polarity, and phase), and data exchange bit order
     *        configuration.
     */
    struct Configuration {
    };

    /**
     * \brief Constructor.
     */
    Basic_Controller_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Basic_Controller_Concept( Basic_Controller_Concept && source ) noexcept;

    Basic_Controller_Concept( Basic_Controller_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Basic_Controller_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Basic_Controller_Concept && expression ) noexcept -> Basic_Controller_Concept &;

    auto operator=( Basic_Controller_Concept const & ) = delete;

    /**
     * \brief Initialize the controller's hardware.
     */
    void initialize() noexcept;

    /**
     * \brief Configure the controller's clock, and data exchange bit order to meet a
     *        specific device's communication requirements.
     *
     * \param[in] configuration The clock, and data exchange bit order configuration that
     *            meets the device's communication requirements.
     */
    void configure( Configuration const & configuration ) noexcept;

    /**
     * \brief Exchange data with a device.
     *
     * \param[in] data The data to transmit to the device.
     *
     * \return The data received from the device.
     */
    auto exchange( std::uint8_t data ) noexcept -> std::uint8_t;
};

} // namespace picolibrary::SPI

#endif // PICOLIBRARY_SPI_H

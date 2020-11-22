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

#include <cstdint>

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

/**
 * \brief SPI controller concept.
 */
class Controller_Concept {
  public:
    /**
     * \brief Clock configuration (frequency, polarity, and phase).
     */
    struct Configuration {
    };

    Controller_Concept() = delete;

    /**
     * \todo #29
     */
    Controller_Concept( Controller_Concept && ) = delete;

    /**
     * \todo #29
     */
    Controller_Concept( Controller_Concept const & ) = delete;

    ~Controller_Concept() = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Controller_Concept && ) = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Controller_Concept const & ) = delete;

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

    /**
     * \brief Exchange a block of data with a device.
     *
     * \param[in] tx_begin The beginning of the block of data to transmit.
     * \param[in] tx_end The end of the block of data to transmit.
     * \param[in] rx_begin The beginning of the block of received data.
     * \param[in] rx_end The end of the block of received data.
     *
     * \warning This function may not verify that the transmit and receive data blocks are
     *          the same size.
     *
     * \return Nothing if data exchange succeeded.
     * \return An error code if data exchange failed. If data exchange cannot fail, return
     *         picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto exchange( std::uint8_t const * tx_begin, std::uint8_t const * tx_end, std::uint8_t * rx_begin, std::uint8_t * rx_end ) noexcept
        -> Result<Void, Error_Code>;

    /**
     * \brief Receive data from a device.
     *
     * \return The received data if data reception succeeded.
     * \return An error code if data reception failed. If data reception cannot fail,
     *         return picolibrary::Result<std::uint8_t, picolibrary::Void>.
     */
    auto receive() noexcept -> Result<std::uint8_t, ::picolibrary::Error_Code>;

    /**
     * \brief Receive a block of data from a device.
     *
     * \param[in] begin The beginning of the block of received data.
     * \param[in] end The end of the block of received data.
     *
     * \return Nothing if data reception succeeded.
     * \return An error code if data reception failed. If data reception cannot fail,
     *         return picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto receive( std::uint8_t * begin, std::uint8_t * end ) noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Transmit data to a device.
     *
     * \param[in] data The data to transmit.
     *
     * \return Nothing if data transmission succeeded.
     * \return An error code if data transmission failed. If data transmission cannot
     *         fail, return picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto transmit( std::uint8_t data ) noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Transmit a block of data to a device.
     *
     * \param[in] begin The beginning of the block of data to transmit.
     * \param[in] end The end of the block of data to transmit.
     *
     * \return Nothing if data transmission succeeded.
     * \return An error code if data transmission failed. If data transmission cannot
     *         fail, return picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto transmit( std::uint8_t const * begin, std::uint8_t const * end ) noexcept
        -> Result<Void, Error_Code>;
};

/**
 * \brief SPI controller.
 *
 * \tparam Basic_Controller The SPI basic controller to add SPI controller functionality
 *         to.
 */
template<typename Basic_Controller>
class Controller : public Basic_Controller {
  public:
    using Basic_Controller::Basic_Controller;

    using Basic_Controller::exchange;

    /**
     * \brief Receive data from a device.
     *
     * \return The received data if data reception succeeded.
     * \return An error code if data reception failed.
     */
    auto receive() noexcept
    {
        return exchange( 0x00 );
    }
};

} // namespace picolibrary::SPI

#endif // PICOLIBRARY_SPI_H
